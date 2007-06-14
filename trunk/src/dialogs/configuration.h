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
#ifndef GDIALOGSCONFIGURATIONDIALOG_H
#define GDIALOGSCONFIGURATIONDIALOG_H

#include <QtGui/QDialog>
#include "ui_configuration.h"

class QProcess;

namespace GDialogs
{

/**
 * Class that represents a graphical fronted to the configuration.
 * @short Class for configuration
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
*/
class Configuration : public QDialog, private Ui::Configuration
{
    Q_OBJECT
  signals:
    /**
     * Signal that reports the operation has failed.
     *
     * @param errorMessage The error message.
     * @param errorType The type of the error. You enter GCore::ErrorHandler::ErrorType enum.
     *
     * @see GCore::ErrorHandler#ErrorType
     */
    void signalFailed(const QString &errorMessage, int errorType);
  public:
    /**
     * The default constructor.
     *
     * @param parent Pointer to the parent widget of this Configuration dialog.
     */
    Configuration(QWidget *parent = 0);

    /**
     * A default destructor.
     */
    ~Configuration();

  protected slots:
    /**
     * Overloaded slot for accepting configuration.
     */
    void accept();

  private:

  private slots:
    /**
     * Tests if the executable works.
     */
    void slotTest();

    /**
     * Opens a browse dialog for selecting an external image editor.
     */
    void slotBrowse();

    /**
     * Updates the progressbar for current gallery.
     *
     * @param done Images already moved.
     * @param total Total images to move.
     * @param name Curent gallery name.
     */
    void updateGalleryProgress(int done, int total, const QString &name);
    /**
     * Updates the prograssbar for total progress.
     *
     * @param done Images already moved.
     * @param total Total number of images.
     * @param name Name of the current image.
     */
    void updateImagesProgress(int done, int total, const QString &name, const QImage&);
    /**
     * Set of actions to execute at finish.
     *
     * @param succesful The state of the job.
     */
    void finish(bool successful);

};

}

#endif
