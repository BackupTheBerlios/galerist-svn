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
#ifndef GWIDGETSPHOTOCONTROL_H
#define GWIDGETSPHOTOCONTROL_H

#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include "ui_photocontrol.h"

class QPushButton;
class QComboBox;

namespace GWidgets
{

/**
 * Class that represents the control buttons for controlling photos.
 * @short Control buttons.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class PhotoControl : public QWidget, private Ui::PhotoControl
{
    Q_OBJECT
  public:
    /**
     * Default constructor.
     */
    PhotoControl(QWidget *parent = 0);

    /**
     * Gets the reference to the back button.
     *
     * @return Reference of the back button.
     */
    QPushButton *getBackButton();
    /**
     * Gets the reference to the next button.
     *
     * @return Reference of the next button.
     */
    QPushButton *getNextButton();
    /**
     * Gets the reference to the zoom in button.
     *
     * @return Reference of the zoom in button.
     */
    QPushButton *getZoomInButton();
    /**
     * Gets the reference to the zoom out button.
     *
     * @return Reference of the zoom out button.
     */
    QPushButton *getZoomOutButton();
    /**
    * Gets the reference to the zoom input button.
    *
    * @return Reference of the zoom input button.
    */
    QPushButton *getZoomInputButton();
    /**
     * Gets the reference to the actual size button.
     *
     * @return Reference of the actual size button.
     */
    QPushButton *getActualSizeButton();
    /**
     * Gets the reference to the zoom to screen button.
     *
     * @return Reference of the zoom to screen button.
     */
    QPushButton *getZoomScreenButton();
    /**
     * Gets the reference to the edit photo button.
     *
     * @return Reference of the edit photo button.
     */
    QPushButton *getEditButton();
    /**
     * Gets the reference to the exit button.
     *
     * @return Reference of the exit button.
     */
    QPushButton *getExitButton();

    /**
     * Default destructor.
     */
    ~PhotoControl();

};

/**
 * Dialog for getting zoom level.
 * @author Gregor KaliÅ¡nik <gregor@podnapisi.net>
 */
class InputZoomDialog : public QDialog
{
    Q_OBJECT
  public:
    /**
     * Default constructor.
     *
     * @param currentZoom The current zoom level.
     * @param parent Parent widget.
     */
    InputZoomDialog(float currentZoom, QWidget *parent = 0);

    /**
     * Static method for obtaining a new zoom level.
     *
     * @param currentZoom The current zoom level. Returns this value if user canceles.
     * @param parent Parent of this dialog.
     *
     * @return The new zoom level.
     * @return @c currentZoom User canceled the zoom selection.
     */
    static float getZoomLevel(float currentZoom, QWidget *parent = 0);

  public slots:
    /**
     * Overloaded method for convinence. Writes the result to a static variable.
     */
    void accept();

  private:
    QComboBox *m_zoomLevel;
    QPushButton *m_accept;
    QPushButton *m_reject;
    static QString m_result;
};

}

#endif
