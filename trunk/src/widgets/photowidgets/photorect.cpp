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
#include "photorect.h"

#include <QtGui/QPainter>

#include "core/data.h"

namespace GWidgets
{

namespace GPhotoWidgets
{

PhotoRect::PhotoRect(QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsRectItem(parent, scene)
{}

void PhotoRect::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  painter->save();
  painter->setPen(pen());
  painter->setBrush(brush());

  switch (GCore::Data::self()->getBackgroundType()) {
    case (GCore::Data::Round) : {
      painter->drawRoundRect(rect());

      if (isSelected()) {
        painter->setBrush(QColor(0, 0, 250, 50));
        painter->drawRoundRect(rect());
      }
      break;
    }
    default: {
      painter->drawRect(rect());

      if (isSelected()) {
        painter->setBrush(QColor(0, 0, 250, 50));
        painter->drawRect(rect());
      }
      break;
    }
  }
  painter->restore();
}

}

}
