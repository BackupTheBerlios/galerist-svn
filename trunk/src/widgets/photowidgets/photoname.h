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

#ifndef GWIDGETSPHOTONAME_H
#define GWIDGETSPHOTONAME_H

#include <QtGui/QGraphicsItem>
#include <QtCore/QObject>
#include <QtGui/QTextLayout>

class QGraphicsScene;
class QGraphicsView;
class QStyleOptionGraphicsItem;
class QGraphicsSceneMouseEvent;
class QWidget;
class QPainter;

namespace GWidgets
{

class LineEdit;

namespace GPhotoWidgets
{

/**
 * Class that represents the name of the Photo.
 * @short Name of the photo.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class PhotoName : public QObject, public QGraphicsSimpleTextItem
{
    Q_OBJECT
  signals:
    /**
     * Signal for reporting the editing has finished.
     *
     * @param text The new text.
     */
    void editingFinished(const QString &text);

  public:
    /**
     * Default constructor.
     */
    PhotoName(QGraphicsItem *parent, QGraphicsScene *scene, QGraphicsView *view);

    /**
     * Sets the text.
     *
     * @param text The text to show up.
     */
    void setText(const QString &text);
    /**
     * Set the editing mode on.
     */
    void setEdit();

    /**
     * Default destructor.
     */
    ~PhotoName();

  protected:
    /**
     * Overloaded method for painting operations.
     *
     * @param painter The painter we are using.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*);
    /**
     * Overloaded method for defining what to do on mouse press.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent*);

  private:
    QString m_text;
    QTextLayout m_layout;
    LineEdit *m_editor;
    QGraphicsView *m_view;
    bool m_editing;

  private slots:
    /**
     * Checks if the name allready exists.
     *
     * @param name The name to check.
     */
    void slotCheck(const QString &name);
    /**
     * Hide the line edit and updates it into the new name.
     *
     * @param name Name to set.
     */
    void slotHide(const QString &name);
    /**
     * Canceles the editing.
     */
    void slotCancel();
};

}

}

#endif
