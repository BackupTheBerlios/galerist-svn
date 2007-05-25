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
#include "photopixmap.h"

#include <QtGui/QPainter>
#include <QtGui/QStyleOptionGraphicsItem>

namespace GWidgets
{

namespace GPhotoWidgets
{

PhotoPixmap::PhotoPixmap(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsPixmapItem(parent, scene)
{}

void PhotoPixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*)
{
  painter->setRenderHint(QPainter::SmoothPixmapTransform, (transformationMode() == Qt::SmoothTransformation));

  QRectF exposed = option->exposedRect.adjusted(-1, -1, 1, 1);
  exposed &= QRectF(offset().x(), offset().y(), pixmap().width(), pixmap().height());
  exposed.translate(offset());
  painter->drawPixmap(exposed, pixmap(), exposed);

  if (isSelected()) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 250, 100));
    painter->drawRect(exposed);
  }
}

}

}
