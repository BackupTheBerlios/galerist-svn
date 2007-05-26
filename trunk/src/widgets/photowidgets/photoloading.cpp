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
#include "photoloading.h"

#include <QtGui/QPainter>
#include <QtGui/QGraphicsTextItem>
#include <QtGui/QGraphicsPixmapItem>

#include <QtCore/QtDebug>

namespace GWidgets {

namespace GPhotoWidgets {

PhotoLoading::PhotoLoading(QGraphicsScene *scene)
 : QGraphicsItemGroup(0, scene),
   m_dots(0)
{
  m_text = new QGraphicsTextItem(this);
  m_text->setFont(QFont("", 20));
  m_text->setPos(m_text->pos() + QPoint(40, 0));
  addToGroup(m_text);
  startTimer(500);
}

PhotoLoading::~PhotoLoading()
{
}

void PhotoLoading::setText(const QString &text)
{
  removeFromGroup(m_text);
  m_text->setPlainText(text + "...");
  m_dots = 3;
  m_rawText = text;
  addToGroup(m_text);
}

QRectF PhotoLoading::boundingRect() const
{
  QRectF rect = QGraphicsItemGroup::boundingRect();

  rect.setLeft(rect.left() - 10);
  rect.setTop(rect.top() - 10);
  rect.setWidth(rect.width() + 10);
  rect.setHeight(rect.height() + 10);

  return rect;
}

void PhotoLoading::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);

  QLinearGradient normalGradient(boundingRect().topLeft(), boundingRect().bottomLeft());
  normalGradient.setColorAt(0, QColor(228, 241, 247));
  normalGradient.setColorAt(0.5, QColor(240, 249, 254));
  normalGradient.setColorAt(1, QColor(228, 241, 247));

  painter->setPen(QColor(134, 201, 239));
  painter->setBrush(QBrush(normalGradient));
  painter->drawRoundRect(boundingRect(), 10, 70);

  painter->restore();
}

void PhotoLoading::timerEvent(QTimerEvent *event)
{
  if (m_dots < 3) {
    m_text->setPlainText(m_text->toPlainText() + ".");
    m_dots++;
  } else {
    m_text->setPlainText(m_rawText);
    m_dots = 0;
  }
}

}

}
