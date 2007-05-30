/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÅ¡nik                                 *
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
#ifndef GDIALOGSUNIUPDATE_H
#define GDIALOGSUNIUPDATE_H

#ifdef WANT_UPDATER

#include <QtGui/QDialog>

#include "ui_uniupdate.h"

namespace GCore
{
namespace GNetwork
{
class UniUpdateManager;
}
}

namespace GDialogs
{

/**
 * @short Frontend class for UniUpdate services.
 * @author Gregor KaliÅ¡nik <gregor@unimatrix-one.org>
 */
class UniUpdate : public QDialog, private Ui::UniUpdate
{
    Q_OBJECT
  public:
    /**
     * A constructor.
     */
    UniUpdate(QWidget *parent = 0);

    /**
     * A destructor.
     */
    ~UniUpdate();

    /**
     * A static method for initiating version checking.
     */
    static void checkUpdates(QWidget *parent = 0);

    /**
     * A conveniance method for checking version at start time.
     */
    static void checkUpdatesStartup(QWidget *parent = 0);

  private:
    GCore::GNetwork::UniUpdateManager *m_updateManager;
    QString m_patchFilename;

  private slots:
    /**
     * Reports the availability of the update.
     */
    void updateProcess(int availability, const QString &changelog);

    /**
     * Begins the download.
     */
    void beginDownload();

    /**
     * Prepares for the execution of the patch.
     */
    void preparePatch(const QString &filename);

    /**
     * Executes the patch and exits the application.
     */
    void executePatch();

    /**
     * Updates the progress bar.
     */
    void updateDownloadProgress(int done, int total);

};

}

#endif

#endif
