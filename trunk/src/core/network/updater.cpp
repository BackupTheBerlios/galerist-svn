/***************************************************************************
 *   Copyright (C) 2006 by Gregor Kališnik                                 *
 *   Copyright (C) 2006 by Jernej Kos                                      *
 *   Copyright (C) 2006 by Unimatrix-One                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "updater.h"

#include "core/data.h"

#include <QtCore/QTemporaryFile>
#include <QtCore/QDir>
#include <QtCore/QProcess>

#include <QtNetwork/QHttp>

#include <QtGui/QMessageBox>
#include <QtGui/QProgressDialog>
#include <QtGui/QApplication>

#include <XmlRpc.h>

#include "core/errorhandler.h"


#ifdef WANT_UPDATER

namespace GCore
{

namespace GNetwork
{

Updater::Updater(QObject *parent)
    : QObject(parent),
    m_latestVersion(GCore::Data::self()->getAppVersion().replace(" ", "_")),
    m_versionRequestId(-1),
    m_versionQuiteRequestId(-1),
    m_changeLogRequestId(-1),
    m_downloadRequestId(-1)
{
  m_httpClient = new QHttp(this);
  m_progressDialog = new QProgressDialog(QString(), tr("&Cancel"), 0, 0, GCore::Data::self()->getMainWindow(), Qt::WindowStaysOnTopHint);
  m_progressDialog->hide();

  m_rpcClient = new XmlRpc::XmlRpcClient("francl", 8000);

  connect(m_httpClient, SIGNAL(requestFinished(int, bool)), this, SLOT(slotProcess(int, bool)));
  connect(m_progressDialog, SIGNAL(canceled()), m_httpClient, SLOT(abort()));
}

void Updater::checkUpdate()
{
  getLatestVersion(true);
}

Updater::~Updater()
{
}

void Updater::slotProcess(int requestId, bool error)
{
  // Check if there was an error
  if (!error) {
    QString response = m_httpClient->readAll();
    // Get the changelog
    if (requestId == m_changeLogRequestId) {
      m_progressDialog->hide();
      m_changeLogRequestId = -1;
      if (QMessageBox::question(GCore::Data::self()->getMainWindow(), tr("Changelog"), tr("The changelog:\n\n%1").arg(response), tr("&Update"), tr("Do &not update"), QString(), 0, 1) == 0)
        downloadUpdate();

      // Install the update?
    } else if (requestId == m_downloadRequestId) {
      // Hide the progress dialog
      m_downloadRequestId = -1;
      m_progressDialog->hide();
      disconnect(m_httpClient, SIGNAL(dataReadProgress(int, int)), this, SLOT(slotShowProgress(int, int)));

      // Notify the user about our intentions
      QMessageBox::information(GCore::Data::self()->getMainWindow(), tr("Download complete"), tr("The update has been successfully downloaded. %1 is now going to exit so the update can install.").arg(GCore::Data::self()->getAppName()), tr("Quit and install"));

      // Rename the file (stupid windows)
      QString path = QDir::toNativeSeparators(m_temp->fileName());
      m_temp->close();
      QFile::rename(path, path + ".exe");

      // Execute the installer and quit
      if (QProcess::startDetached(path + ".exe"))
        qApp->quit();
    }

  }
}

void Updater::slotShowProgress(int done, int total)
{
  m_progressDialog->setMaximum(total);
  m_progressDialog->setValue(done);
}

void Updater::slotCheckUpdate()
{
  getLatestVersion(false);
}

void Updater::showChangeLog()
{
  m_httpClient->setHost(m_changelog.host());
  m_changeLogRequestId = m_httpClient->get(m_changelog.path());

  // set up the Progress dialog
  m_progressDialog->setLabelText(tr("Downloading the changelog..."));
  m_progressDialog->setMaximum(0);
  m_progressDialog->setValue(1);
  m_progressDialog->show();
}

void Updater::downloadUpdate()
{
  m_temp = new QTemporaryFile(this);
  m_temp->open();
  m_httpClient->setHost(m_patch.host());
  m_downloadRequestId = m_httpClient->get(m_patch.path(), m_temp);

  // Set up the progress dialog
  m_progressDialog->setLabelText(tr("Downloading the update..."));
  connect(m_httpClient, SIGNAL(dataReadProgress(int, int)), this, SLOT(slotShowProgress(int, int)));
  m_progressDialog->show();
}

void Updater::getLatestVersion(bool quite)
{
  XmlRpc::XmlRpcValue parameters, results;
  parameters[0] = GCore::Data::self()->getAppName().toLower().toStdString();
  // For now we have only Windows platform
  parameters[1] = "windows";
  // There is no stable branch at the moment
  parameters[2] = "unstable";
  // App version number
  parameters[3] = GCore::Data::self()->getAppVersion().toStdString();
  // App branch
  parameters[4] = GCore::Data::self()->getBranch().toLower().toStdString();
  
  if (!m_rpcClient->execute("check", parameters, results)) {
    qDebug("Updater: Cannot check version!");
    GCore::ErrorHandler::reportMessage("Cannot contact UniUpdate server.", GCore::ErrorHandler::Critical);
    return;
  }

  if (!results["available"]) {
    int errcode = results["errcode"];
    //if (!quite)
      switch (errcode) {
        case (0) : {
          GCore::ErrorHandler::reportMessage("This application is not supported by the UniUpdate service.", GCore::ErrorHandler::Critical);
          break;
        }
        case (1) : {
          GCore::ErrorHandler::reportMessage("Selected branch does not exist on UniUpdate server.", GCore::ErrorHandler::Critical);
          break;
        }
        case (2) : {
          GCore::ErrorHandler::reportMessage("This platform is not supported by the UniUpdate service.", GCore::ErrorHandler::Critical);
          break;
        }
        case (3) : {
          QMessageBox::information(GCore::Data::self()->getMainWindow(), tr("No update available"), tr("You already have the latest %1 installed.").arg(GCore::Data::self()->getAppName()));
          break;
        }
    }
    return;
  }

  QString version = QString::fromStdString(results["version"]);
  QString branch = QString::fromStdString(results["branch"]);
  m_changelog.setUrl(QString::fromStdString(results["changelog"]));
  m_patch.setUrl(QString::fromStdString(results["patch"]));

  // Ask if we download the update
  switch (QMessageBox::question(GCore::Data::self()->getMainWindow(), tr("Update available"), tr("A new version of %1 is available. Your version is %2 %3 and the latest is %4 %5.\n\nDo you want to install it?").arg(GCore::Data::self()->getAppName()).arg(GCore::Data::self()->getAppVersion()).arg(GCore::Data::self()->getBranch()).arg(version).arg(branch), tr("&Update"), tr("Do &not update"), tr("Show &changelog"), 0, 1)) {
    case (0) : {
      downloadUpdate();
      break;
    }
    case (2) : {
      showChangeLog();
      break;
    }
  }


}

}

}

#endif
