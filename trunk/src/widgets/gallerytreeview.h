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
#ifndef GWIDGETSGALLERYTREEVIEW_H_
#define GWIDGETSGALLERYTREEVIEW_H_

#include <QtGui/QTreeView>

class QModelIndex;
class QPaintEvent;

namespace GWidgets
{

/**
 * Class that shows all the galleries.
 * @short Tree-like list for galleries.
 * @author Gregor KaliÅ¡nik <gregor@podnapisi.net>
 */
class GalleryTreeView : public QTreeView
{
    Q_OBJECT
  signals:
    /**
     * Signal that report that a gallery has been selected or not.
     *
     * @param selected Self explanatory.
     */
    void signalSelected(bool selected);

  public:
    /**
     * A default constructor.
     *
     * @param parent Pointer to the parent widget.
     */
    GalleryTreeView(QWidget *parent);

    /**
     * A default destructor.
     */
    ~GalleryTreeView();

  protected:
    /**
     * Overloaded method to defining context menu (right mouse click ;)).
     *
     * @param event Event itself, to get out the coordinates where to paint the menu.
     */
    void contextMenuEvent(QContextMenuEvent *event);

  private:

  private slots:
    /**
     * Slot for deleting the gallery.
     */
    void slotDelete();
    /**
     * Slot that checks if a gallery has been selected and sends a signal.
     *
     * @param selected Index, that represents an item in the Model, of selected gallery.
     */
    void slotCheckSelection(const QModelIndex &selected);
};

}

#endif
