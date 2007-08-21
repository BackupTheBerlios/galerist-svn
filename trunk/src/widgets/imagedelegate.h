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
#ifndef GWIDGETSIMAGEDELEGATE_H_
#define GWIDGETSIMAGEDELEGATE_H_

#include <QtGui/QItemDelegate>
#include <QtGui/QPen>

class QPainter;
class QStyleOptionViewItem;
class QModelIndex;

namespace GWidgets
{

class ImageDelegate : public QItemDelegate
{
    Q_OBJECT
  public:
    ImageDelegate(QObject *parent = 0);

    ~ImageDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

  private:
    QLinearGradient m_normalGradient;
    QLinearGradient m_hoverGradient;
    QPen m_normalBorder;
    QPen m_hoverBorder;

    QLinearGradient m_normalGradientGallery;
    QLinearGradient m_hoverGradientGallery;
    QPen m_normalBorderGallery;
    QPen m_hoverBorderGallery;

    void drawBackground(QPainter *painter, const QStyleOptionViewItem &option, int type) const;
    void drawPixmap(QPainter *painter, const QStyleOptionViewItem &option, const QPixmap &pixmap) const;
    void drawMask(QPainter *painter, const QRect &rect) const;

    QRect textRect(const QString &text, const QRect &position) const;
};

}

#endif
