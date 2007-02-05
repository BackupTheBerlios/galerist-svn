/***************************************************************************
 *   Copyright (C) 2006 by Gregor Kalisnik                                 *
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

#include <QtNetwork/QHttp>
#include <QtGui/QMessageBox>
#include <QtGui/QProgressDialog>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtGui/QApplication>

#include <QtCore/QtDebug>

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
  m_httpClient = new QHttp("www.vegovc.si", 80, this);
  m_progressDialog = new QProgressDialog(QString(), tr("&Cancel"), 0, 0, GCore::Data::self()->getMainWindow());
  m_progressDialog->hide();

  connect(m_httpClient, SIGNAL(requestFinished(int, bool)), this, SLOT(slotProcess(int, bool)));
  connect(m_progressDialog, SIGNAL(canceled()), m_httpClient, SLOT(abort()));
}

void Updater::checkUpdate()
{
  m_versionQuiteRequestId = m_httpClient->get("/~mastermind/goya/index.html");
}

Updater::~Updater()
{
  //m_progressDialog->close();
}

void Updater::slotProcess(int requestId, bool error)
{
  m_progressDialog->hide();

  // Check if there was an error
  if (!error) {
    // Check if we are just checking if there is a new version
    QString response = m_httpClient->readAll();
    if (requestId == m_versionRequestId) {
      checkVersions(response);
      m_versionRequestId = -1;

      // Need to be quite?
    } else if (requestId == m_versionQuiteRequestId) {
      checkVersions(response, true);
      m_versionQuiteRequestId = -1;

      // Get the changelog
    } else if (requestId == m_changeLogRequestId) {
      if (QMessageBox::question(GCore::Data::self()->getMainWindow(), tr("Changelog"), tr("The changelog:\n\n%1").arg(response), tr("&Update"), tr("Do &not update"), QString(), 0, 1) == 0)
        downloadUpdate();
      m_changeLogRequestId = -1;

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

  } else {
    qDebug() << "ERROR";
  }
}

void Updater::slotShowProgress(int done, int total)
{
  m_progressDialog->setMaximum(total);
  m_progressDialog->setValue(done);
}

void Updater::slotCheckUpdate()
{
  m_versionRequestId = m_httpClient->get("/~mastermind/goya/index.html");

  // set up the Progress dialog
  m_progressDialog->setLabelText(tr("Checking for updates..."));
  m_progressDialog->setMaximum(0);
  m_progressDialog->setValue(1);
  m_progressDialog->show();
}

void Updater::showChangeLog()
{
  m_changeLogRequestId = m_httpClient->get("/~mastermind/goya/changelog-" + m_latestVersion);

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
  m_downloadRequestId = m_httpClient->get("/~mastermind/goya/goya-patch-" + m_latestVersion + ".exe", m_temp);

  // Set up the progress dialog
  m_progressDialog->setLabelText(tr("Downloading the update..."));
  connect(m_httpClient, SIGNAL(dataReadProgress(int, int)), this, SLOT(slotShowProgress(int, int)));
  m_progressDialog->show();
}

void Updater::checkVersions(const QString &newVersion, bool quite)
{
  QString version = newVersion;
  version.remove("\n").replace(" ", "_");
  // Is it newer?
  if (version > GCore::Data::self()->getAppVersion().replace(" ", "_")) {
    // We save the new version tag
    m_latestVersion = version.replace(" ", "_");

    // Ask if we download the update
    switch (QMessageBox::question(GCore::Data::self()->getMainWindow(), tr("Update available"), tr("A new version of %1 is available. Your version is %2 and the latest is %3.\n\nDo you want to install it?").arg(GCore::Data::self()->getAppName()).arg(GCore::Data::self()->getAppVersion()).arg(version), tr("&Update"), tr("Do &not update"), tr("Show &changelog"), 0, 1)) {
      case (0) : {
        downloadUpdate();
        break;
      }
      case (2) : {
        showChangeLog();
        break;
      }
    }
  } else if (!quite) {
    // Just notify that it's the newest version
    QMessageBox::information(GCore::Data::self()->getMainWindow(), tr("No update available"), tr("You already have the latest %1 installed.").arg(GCore::Data::self()->getAppName()));
  }
}

}

}

#endif
