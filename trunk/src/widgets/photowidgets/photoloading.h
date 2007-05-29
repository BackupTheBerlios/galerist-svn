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
#ifndef GWIDGETS_GPHOTOWIDGETSPHOTOLOADING_H
#define GWIDGETS_GPHOTOWIDGETSPHOTOLOADING_H

#include <QtCore/QObject>

#include <QtGui/QGraphicsItemGroup>

namespace GWidgets
{

namespace GPhotoWidgets
{

/**
 * @short For showing loading "dialog" in PhotoView.
 * @author Gregor KaliÅ¡nik <gregor@unimatrix-one.org>
 */
class PhotoLoading : public QGraphicsItemGroup, QObject
{
  public:
    /**
     * A constructor.
     */
    PhotoLoading(QGraphicsScene *scene);

    /**
     * A destructor.
     */
    ~PhotoLoading();

    /**
     * Sets text.
     */
    void setText(const QString &text);

    /**
     * Reimplemented method.
     * Returns the real bounding rectangle for this item.
     */
    QRectF boundingRect() const;

  protected:
    /**
     * Reimplemeted method for defining its own painting operation.
     *
     * @param painter Reference to the painter.
     * @param option Options of this photo.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);

    /**
     * Reimplemented method.
     */
    void timerEvent(QTimerEvent *event);

  private:
    QGraphicsTextItem *m_text;
    QString m_rawText;
    short m_dots;

};

}

}

#endif
