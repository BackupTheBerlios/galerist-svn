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

#ifndef GWIDGETSPHOTODESCRIPTION_H
#define GWIDGETSPHOTODESCRIPTION_H

#include <QtGui/QGraphicsItem>

class QGraphicsScene;
class QGraphicsView;
class QStyleOptionGraphicsItem;
class QGraphicsSceneMouseEvent;
class QWidget;
class QPainter;

namespace GWidgets
{

class TextEdit;

namespace GPhotoWidgets
{

/**
 * Class that represents the description of the photo.
 * @short Photos description.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class PhotoDescription : public QGraphicsTextItem
{
    Q_OBJECT
  signals:
    /**
     * Signals finish of edit.
     *
     * @param text The new text.
     */
    void editingFinished(const QString &text);

  public:
    /**
     * Default constructor.
     */
    PhotoDescription(QGraphicsItem *parent, QGraphicsScene *scene, QGraphicsView *view);

    /**
     * Set the text to show.
     *
     * @param text Text to be shown.
     */
    void setText(const QString &text);
    /**
     * Go to editing mode.
     */
    void setEdit();

    /**
     * Default destructor.
     */
    ~PhotoDescription();

  protected:
    /**
     * Overloaded method for defining its own painting operation.
     *
     * @param painter Reference to the painter.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
    /**
     * Overloaded method for defining the behaviour on mouse press event.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent*);

  private:
    QString m_text;
    TextEdit *m_editor;
    QGraphicsView *m_view;

  private slots:
    /**
     * Stops the editing mode.
     *
     * @param description The new description which must be updated.
     */
    void slotHide(const QString &description);
    /**
     * Cancels the editing mode.
     */
    void slotCancelEditing();
};

}

}

#endif
