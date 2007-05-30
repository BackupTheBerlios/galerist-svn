/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÃÂÃÂ¡nik                                 *
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

#ifdef WANT_UPDATER

#include "uniupdatemanager.h"

#include <QtCore/QBuffer>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDir>

#include <QtNetwork/QHttp>

#include <XmlRpc.h>

namespace GCore
{

namespace GNetwork
{

UniUpdateManager::UniUpdateManager(QObject *parent)
    : QObject(parent),
    m_changelogId(-1),
    m_patchId(-1)
{
  m_httpClient = new QHttp(this);

  connect(m_httpClient, SIGNAL(requestFinished(int, bool)), this, SLOT(processRequest(int, bool)));
  connect(m_httpClient, SIGNAL(dataReadProgress(int, int)), this, SIGNAL(downloadProgress(int, int)));
}

UniUpdateManager::~UniUpdateManager()
{}

void UniUpdateManager::checkUpdate(const QString &appName, const QString &platform, const QString &selectedBranch, const QString &version, const QString &currentBranch)
{
  XmlRpc::XmlRpcClient uniUpdate("francl", 8000);

  XmlRpc::XmlRpcValue parameters, results;
  parameters[0] = appName.toLower().toStdString();
  // For now we have only Windows platform
  parameters[1] = platform.toLower().toStdString();
  // There is no stable branch at the moment
  parameters[2] = selectedBranch.toLower().toStdString();
  // App version number
  parameters[3] = version.toStdString();
  // App branch
  parameters[4] = currentBranch.toLower().toStdString();

  if (!uniUpdate.execute("check", parameters, results)) {
    emit updateAvailability(CannotConnect, QString());
    return;
  }

  if (!results["available"]) {
    int errcode = results["errcode"];
    switch (errcode) {
      case (0) : {
        emit updateAvailability(ApplicationUnsupported, QString());
        break;
      }
      case (1) : {
        emit updateAvailability(BranchUnsupported, QString());
        break;
      }
      case (2) : {
        emit updateAvailability(PlatformUnsupported, QString());
        break;
      }
      case (3) : {
        emit updateAvailability(Latest, QString());
        break;
      }
    }
    return;
  }

  QUrl changelog(QString::fromStdString(results["changelog"]));
  m_patchUrl = QUrl(QString::fromStdString(results["patch"]));

  m_changelogBuffer = new QBuffer(this);

  m_httpClient->setHost(changelog.host());
  m_changelogId = m_httpClient->get(changelog.path(), m_changelogBuffer);
}

void UniUpdateManager::downloadUpdate()
{
  m_patch = new QTemporaryFile(this);
  m_patch->open();

  m_httpClient->setHost(m_patchUrl.host());
  m_patchId = m_httpClient->get(m_patchUrl.path(), m_patch);
}

void UniUpdateManager::processRequest(int id, bool error)
{
  if (error)
    emit updateAvailability(CannotConnect, QString());

  // We send the changelog
  if (m_changelogId == id) {
    emit updateAvailability(Available, m_changelogBuffer->buffer());
    m_changelogId = -1;
  } else if (m_patchId == id) {
    // We have to rename the file so Windows can execute it (dumb system of extensions!!!)
    QString patchFileName = QDir::toNativeSeparators(m_patch->fileName());
    m_patch->close();
    QFile::rename(patchFileName, patchFileName + ".exe");

    emit patchReady(patchFileName + ".exe");
    m_patchId = -1;
  }
}

}

}

#endif
