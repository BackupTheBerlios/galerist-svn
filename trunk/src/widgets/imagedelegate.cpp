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

#include "imagedelegate.h"

#include <QtGui/QPainter>
#include <QtGui/QItemEditorCreator>

#include "core/imageitem.h"
#include "core/imagemodel.h"
#include "core/data.h"

#include "widgets/lineedit.h"

#include <QtDebug>

using namespace GCore;

namespace GWidgets
{

ImageDelegate::ImageDelegate(QObject *parent)
    : QItemDelegate(parent)
{
  QLinearGradient normalGradient(QPoint(0, 0), QPoint(0, 160));
  normalGradient.setColorAt(0, QColor(228, 241, 247));
  normalGradient.setColorAt(0.5, QColor(240, 249, 254));
  normalGradient.setColorAt(1, QColor(228, 241, 247));
  m_normalGradient = normalGradient;
  m_normalBorder = QPen(QColor(134, 201, 239));

  QLinearGradient hoverGradient(QPoint(0, 0), QPoint(0, 160));
  hoverGradient.setColorAt(0, QColor(202, 232, 245));
  hoverGradient.setColorAt(0.5, QColor(213, 241, 252));
  hoverGradient.setColorAt(1, QColor(202, 232, 245));
  m_hoverGradient = hoverGradient;
  m_hoverBorder = QPen(QColor(159, 211, 249));
}

ImageDelegate::~ImageDelegate()
{}

void ImageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  //We don't want to show galleries in our main window, do we?
  if (index.data(GCore::ImageModel::ImageTypeRole).toInt() == ImageItem::Gallery)
    return;

  painter->save();
  painter->setClipRect(option.rect, Qt::NoClip);

  // Display background
  drawBackground(painter, option);

  // Display thumbnail
  drawPixmap(painter, option, index.data(ImageModel::ImageThumbnailRole).value<QIcon>().pixmap(QSize(128, 128)));

  // Display text
  QString name = index.data(ImageModel::ImageNameRole).toString();
  QRect nameRect = textRect(name, option.rect);

  drawDisplay(painter, option, nameRect, name);

  painter->restore();
}

QSize ImageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  return QSize(150, 160);
}


void ImageDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  editor->setGeometry(textRect(index.data(ImageModel::ImageNameRole).toString(), option.rect));
}

void ImageDelegate::drawBackground(QPainter *painter, const QStyleOptionViewItem &option) const
{
  painter->save();

  if (option.state & QStyle::State_MouseOver) {
    painter->setBrush(m_hoverGradient);
    painter->setPen(m_hoverBorder);
  } else {
    painter->setBrush(m_normalGradient);
    painter->setPen(m_normalBorder);
  }

  QPainterPath rect;
  rect.addRoundRect(option.rect, 25);

  painter->drawPath(rect);

  painter->restore();
}

void ImageDelegate::drawPixmap(QPainter *painter, const QStyleOptionViewItem &option, const QPixmap &pixmap) const
{
  painter->save();

  QPoint pos;
  pos.setX(option.rect.left() + ((option.rect.width() / 2) - (pixmap.width() / 2)));
  pos.setY(option.rect.top() + (((128 + 20) / 2) - (pixmap.height() / 2)));

  painter->drawPixmap(pos, pixmap);

  painter->restore();
}

QRect ImageDelegate::textRect(const QString &text, const QRect &position) const
{
  QFont textFont("", 10);
  QFontMetrics textMetrics(textFont);

  int textHeight = textMetrics.height();
  int textWidth = textMetrics.width(text) + 10;
  textWidth = textWidth > (position.width() - 10) ? position.width() - 10 : textWidth;

  QRect textRect;
  textRect.setTop(position.bottom() - textHeight);
  textRect.setLeft(position.left() + ((position.width() / 2) - (textWidth / 2)));
  textRect.setHeight(textHeight);
  textRect.setWidth(textWidth);

  return textRect;
}

}