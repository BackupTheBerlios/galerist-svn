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
#ifndef GWIDGETSLISTVIEW_H
#define GWIDGETSLISTVIEW_H

#include <QtGui/QListView>

namespace GWidgets
{

/**
 * @short Modified QListView. (Drop in support)
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class ListView : public QListView
{
    Q_OBJECT
  public:
    ListView(QWidget *parent = 0);

    ~ListView();

  protected:
    /**
    * Method that allows Drag in images.
    *
    * @param event The event.
     */
    void dragEnterEvent(QDragEnterEvent *event);
    /**
     * Method that allows Drag in images.
     *
     * @param event The event.
     */
    void dragMoveEvent(QDragMoveEvent *event);
    /**
     * Method that allows Drag in images.
     *
     * @param event The event.
     */
    void dragLeaveEvent(QDragLeaveEvent *event);
    /**
     * The implemetation of drop in action. It parses the links and passes it to ImageModel and reports to MainWindow.
     *
     * @param event Event itself.
     */
    void dropEvent(QDropEvent *event);

};

}

#endif
