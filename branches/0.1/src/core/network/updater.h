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

#include <QtCore/QObject>

#ifdef WANT_UPDATER

class QHttp;
class QProgressDialog;
class QTemporaryFile;

namespace GCore
{

namespace GNetwork
{

class Updater : public QObject
{
    Q_OBJECT
  public:
    /**
     * A constructor.
     *
     * @param parent Parent of this updater object.
     */
    Updater(QObject *parent = 0);

    /**
     * A quite update checking.
     *
     * @see slotCheckUpdate
     */
    void checkUpdate();

    ~Updater();

  private slots:
    /**
     * Process QHttp response.
     *
     * @param error If there was an error.
     */
    void slotProcess(int requestId, bool error);

    /**
     * Used for showing progress.
     */
    void slotShowProgress(int done, int total);

    /**
    * Checks for an update. Call this slot before any other!
    * It uses its own notification system (that's why it isn't threaded...).
    */
    void slotCheckUpdate();

  private:
    QHttp *m_httpClient;
    QString m_latestVersion;
    int m_versionRequestId;
    int m_versionQuiteRequestId;
    int m_changeLogRequestId;
    int m_downloadRequestId;
    QProgressDialog *m_progressDialog;
    QTemporaryFile *m_temp;

    /**
    * Downloads and returns the changelog of the new version.
    */
    void showChangeLog();
    /**
    * Downloads the update. Installation is queued.
    */
    void downloadUpdate();

    /**
     * Convenience method for checking versions.
     *
     * @param newVersion The new version string.
     * @param quite Be more quite or not.
     */
    void checkVersions(const QString &newVersion, bool quite = false);
};

}

}

#endif
