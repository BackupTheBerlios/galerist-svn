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

class QStackedWidget;
class QPushButton;

namespace GWidgets
{

/**
 * @short A control panel for PhotoView.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class PhotoControl : public QWidget
{
    Q_OBJECT
  public:
    /**
     * Just a constructor.
     */
    PhotoControl(QWidget *parent = 0);

    /**
     * A destructor.
     */
    ~PhotoControl();

    // Buttons for the MainWindow
    
    // The main page
    // First group box
    QPushButton *rotateCCWButton;
    QPushButton *rotateCWButton;
    QPushButton *editButton;
    QPushButton *cropButton;

    //Second group box
    QPushButton *zoomOutButton;
    QPushButton *zoomInButton;
    QPushButton *zoomInputButton;
    QPushButton *zoomScreenButton;
    QPushButton *actualSizeButton;

    // Control button
    QPushButton *nextButton;
    QPushButton *backButton;
    QPushButton *closeButton;
    QPushButton *saveButton;

  private:
    QStackedWidget *m_controlPanel;


};

}

#endif
