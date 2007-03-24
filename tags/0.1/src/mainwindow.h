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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "widgets/photoview.h"
#include "widgets/gallerytreeview.h"
#include "ui_mainwindow.h"

class QProgressBar;

namespace GCore
{
class GalleryHandler;
class ImageModel;
}

namespace GWidgets
{
class ImageAddProgress;
}

/**
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 * @short Main window
 * Main window for Goya.
 */
class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
  public:
    /**
     * A constructor.
     */
    MainWindow();

    /**
     * A destructor.
     */
    ~MainWindow();

    void addImages(const QStringList &images);

  public slots:
    /**
     * Slot that defines the Goya went into editing mode (or is leaving it).
     *
     * @param edit Is it in editing mode?
     */
    void setEditMode(bool edit);

    /**
     * Slot that sets the progress bar. Should be in private zone?
     *
     * @param finished How many units has finished.
     * @param total Total number of units.
     * @param current Name of the current image.
     * @param currentImage The reference to the current thumbnail image.
     *
     * @see MainWindow#slotProgressHide
     * @see MainWindow#slotProgressShow
     * @see MainWindow#slotHideProgress
     */
    void slotStatusProgress(int finished, int total, const QString &current, const QImage &currentImage);

  private:
    /**
     * Method for initialising action buttons.
     */
    void initActionButtons();

    /**
     * Method for initialising tool bar.
     */
    void initToolbar();

    /**
     * Method for initialising docks.
     */
    void initDocks();

    QProgressBar *m_progressBar;
    GWidgets::ImageAddProgress *m_imageProgress;

  private slots:
    /**
     * Slot for showing about Goya dialog.
     */
    void about();

    /**
     * Slot for creating New gallery wizard.
     */
    void slotNew();

    /**
     * Slot that sets the background to default (square rectangles).
     *
     * @param checked Sets it if true, if false cheks what is selected.
     *
     * @see MainWindow#slotRoundBackground
     */
    void slotDefaultBackground(bool checked);

    /**
     * Slots that sets the background to rounded rectangles.
     *
     * @param checked Sets it if true, if false cheks what is selected.
     *
     * @see MainWindow#slotDefaultBackground
     */
    void slotRoundBackground(bool checked);

    /**
     * Slot that sets to show the progress popup.
     *
     * @param checked Sets it if true, if false cheks what is selected.
     *
     * @see MainWindow#slotProgressHide
     * @see MainWindow#slotStatusProgress
     */
    void slotProgressShow(bool checked);

    /**
     * Slot that sets to show the progress in the status bar.
     *
     * @param checked Sets it if true, if false cheks what is selected.
     *
     * @see MainWindow#slotProgressShow
     * @see MainWindow#slotStatusProgress
     */
    void slotProgressHide(bool checked);

    /**
     * Slot that hides the progress bar/dialog.
     *
     * @see MainWindow#slotStatusProgress
     */
    void slotHideProgress();

    /**
     * Slot that enables or disables buttons.
     *
     * @param selected If an item is selected or not.
     */
    void slotUpdateEditButtons(bool selected);

    /**
     * Slot that created the configuration dialog.
     */
    void slotConfiguration();

    /**
     * Slot that quits the Goya.
     */
    void slotQuit();

    /**
     * Slot that popups the open images dialog.
     */
    void slotAddImages();

};

#endif
