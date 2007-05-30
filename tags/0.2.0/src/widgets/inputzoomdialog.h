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
#ifndef GWIDGETSINPUTZOOMDIALOG_H
#define GWIDGETSINPUTZOOMDIALOG_H

#include <QtGui/QDialog>

class QPushButton;
class QComboBox;

namespace GWidgets
{

/**
 * @short Dialog for getting zoom level.
 * @author Gregor KaliÅ¡nik <gregor@unimatrix-one.org>
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
     * @return @c -1 User canceled the zoom selection.
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
