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
#ifndef GWIDGETSTOOLTIP_H
#define GWIDGETSTOOLTIP_H

#include <QtGui/QLabel>

namespace GWidgets
{

/**
 * @short Class for showing text in a bubble like widget.
 * @author Gregor KaliÅ¡nik <gregor@unimatrix-one.org>
 */
class ToolTip
{
  public:
    /**
     * A constructor.
     */
    ToolTip();

    /**
     * A static method for showing a bubble text.
     *
     * @param text Text to show.
     * @param parent Parent of this tip.
     * @param pos Position where to show this bubble.
     */
    static void showMessage(const QString &text, QWidget *parent = 0, const QPoint &pos = QPoint(0, 0));
};

}

#endif
