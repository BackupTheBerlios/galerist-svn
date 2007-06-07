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

#ifndef GWIDGETSPHOTODESCRIPTION_H
#define GWIDGETSPHOTODESCRIPTION_H

#include <QtGui/QGraphicsItem>
#include <QtGui/QTextEdit>

class QGraphicsScene;
class QGraphicsView;
class QStyleOptionGraphicsItem;
class QGraphicsSceneMouseEvent;
class QWidget;
class QPainter;

namespace GWidgets
{

/**
 * Class for text edits used in PhotoView.
 * @short Text edit just for PhotoView.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class TextEdit : public QTextEdit
{
    Q_OBJECT
  signals:
    /**
     * Signals taht the editing has been finished.
     *
     * @param text The new text.
     */
    void editingFinished(const QString &text);
    /**
     * Signals that the editing has been canceled.
     */
    void editingCanceled();

  public:
    /**
     * Default constructor.
     */
    TextEdit(QWidget *parent = 0);

  protected:
    /**
     * Reimplemented method for defining what to do when it gets pressed.
     *
     * @param event The event :).
     */
    void keyPressEvent(QKeyEvent *event);
    /**
     * Reimplemented method for defining what to do when focus goes away.
     */
    void focusOutEvent(QFocusEvent*);

  private:
    QString m_previous;
};

namespace GPhotoWidgets
{

/**
 * Class that represents the description of the photo.
 * @short Photos description.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
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
