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
#ifndef GWIDGETS_GPHOTOWIDGETSPHOTOPIXMAP_H
#define GWIDGETS_GPHOTOWIDGETSPHOTOPIXMAP_H

#include <QtGui/QGraphicsPixmapItem>

namespace GWidgets
{

namespace GPhotoWidgets
{

/**
 * Class that represents the thumbnail of the photo.
 * @short Thumbnail of the photo.
 * @author Gregor KaliÅ¡nik <gregor@unimatrix-one.org>
 */
class PhotoPixmap : public QGraphicsPixmapItem
{
  public:
    /**
     * Default constructor.
     *
     * @param parent Parent of this item.
     * @param scene The scene in which this item is.
     */
    PhotoPixmap(QGraphicsItem *parent, QGraphicsScene *scene);

  protected:
    /**
     * Overloaded method for defining its own painting operation.
     *
     * @param painter Reference to the used painter.
     * @param option Option of this item.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*);
};

}

}

#endif
