/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÃÂ¡nik                                 *
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
#ifndef GCORE_GNETWORKUNIUPDATEMANAGER_H
#define GCORE_GNETWORKUNIUPDATEMANAGER_H

#ifdef WANT_UPDATER

#include <QtCore/QObject>
#include <QtCore/QUrl>

class QHttp;
class QBuffer;
class QTemporaryFile;

namespace GCore
{

namespace GNetwork
{

/**
 * @short Manager for UniUpdate service.
 * @author Gregor KaliÃÂ¡nik <gregor@unimatrix-one.org>
 */
class UniUpdateManager : public QObject
{
    Q_OBJECT
  signals:
    /**
     * Emitted when the manager finishes checking for updates and reports the state of availability.
     *
     * @param availability Enum type.
     * @param changelog The complete changelog for the new version, if available.
     *
     * @see GCore#GNetwork#UniUpdateManager#Availability
     * @see GCore#GNetwork#UniUpdateManager#checkUpdate
     */
    void updateAvailability(int availability, const QString &changelog);

    /**
     * Reports how much of the download is complete. Used only when downloading the patch.
     *
     * @see GCore#GNetwork#UniUpdateManager#checkUpdate
     */
    void downloadProgress(int done, int total);

    /**
     * Sends the path to the patch.
     */
    void patchReady(const QString &filename);

  public:
    /**
     * Defines the states of the update availability.
     */
    enum Availability
    {
      /** Update is available. */
      Available,
      /** Program is already latest. */
      Latest,
      /** UniUpdate service is unavailable. */
      CannotConnect,
      /** Applicaton is unsupported by the UniUpdate service. */
      ApplicationUnsupported,
      /** Platform is unsupported by UniUpdate service. */
      PlatformUnsupported,
      /** Specified branch is unsupported. */
      BranchUnsupported
  };

    /**
     * A default constructor.
     */
    UniUpdateManager(QObject *parent = 0);

    /**
     * A destructor.
     */
    ~UniUpdateManager();

    /**
     * Checks for updates and reports the availability through updateAvailability signal.
     *
     * @param appName Name of the application.
     * @param platform Short platform name (basicaly it's Operating System).
     * @param selectedBranch Look for the selected branch.
     * @param version The current version.
     * @param currentBranch Current application's branch.
     *
     * @see GCore#GNetwork#UniUpdateManager#updateAvailability
     */
    void checkUpdate(const QString &appName, const QString &platform, const QString &selectedBranch, const QString &version, const QString &currentBranch);

  public slots:
    /**
     * Initiates the patch download and reports the progress through downloadProgress signal.
     *
     * @see GCore#GNetwork#UniUpdateManager#downloadProgress
     */
    void downloadUpdate();

  private:
    QHttp *m_httpClient;
    QUrl m_patchUrl;
    QTemporaryFile *m_patch;
    QBuffer *m_changelogBuffer;
    int m_changelogId;
    int m_patchId;

  private slots:
    /**
     * Processes he HTTP clients output.
     */
    void processRequest(int id, bool error);

};

}

}

#endif

#endif
