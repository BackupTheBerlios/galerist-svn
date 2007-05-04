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
#ifndef GWIDGETSPHOTOITEM_H
#define GWIDGETSPHOTOITEM_H

#include <QtCore/QObject>
#include <QtCore/QModelIndex>

#include <QtGui/QGraphicsItem>


class QGraphicsItemAnimation;
class QTimeLine;

namespace GCore {
class ImageItem;
}

namespace GWidgets
{

class PhotoView;

namespace GPhotoWidgets
{

class PhotoName;
class PhotoDescription;
class PhotoRect;
class PhotoPixmap;

/**
 * This class represents a single view item. It can be assigned a pixmap and
 * a name and also a short description.
 * @short A photo in a PhotoView.
 *
 * @author Jernej Kos <kostko@unimatrix-one.org>
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class PhotoItem : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
  public:
    /**
     * Item width.
     */
    static const int ItemWidth = 160;

    /**
     * Item height.
     */
    static const int ItemHeight = 200;

    /**
     * Zooming step.
     */
    static const int ItemZoomStep = 50;

    /**
     * Class constructor.
     *
     * @param view The parent view
     * @param timeline The QTimeLine for animations
     */
    PhotoItem(PhotoView *view, const QModelIndex &index);

    /**
     * Default destructor.
     */
    ~PhotoItem();

    /**
     * Set item's pixmap.
     *
     * @param pixmap The new pixmap
     */
    void setPixmap(const QPixmap &pixmap);

    /**
     * Set item's text.
     *
     * @param text The new text
     */
    void setText(const QString &text, const QString &description);

    /**
     * Initiate the renaming sequence. A wrapper method.
     */
    void rename();

    /**
     * Initiate the describing sequence. A wrapper method.
     */
    void describe();

    /**
     * Notify the photo of it's pending doom. If it is allready domed, returnes true.
     *
     * @return Is it allready doomed?
     */
    bool deleteItself();

    /**
     * Sets the new position and sets the editing mode.
     *
     * @param x The x coordinate.
     * @param y The y coordinate.
     * @param editMode Go to edit mode or not.
     */
    void setPos(qreal x, qreal y);

    /**
     * Returns the name of the photo.
     *
     * @return Name of the photo.
     */
    QString getText();

    /**
     * Returns the tooltip of the photo.
     *
     * @return The tooltip
     */
    QString toolTip();

    /**
     * Gets the index of this item.
     */
    QModelIndex getIndex();

    /**
     * Returns the actual scaled size.
     *
     * @return Absolute size. X is width and y is height.
     */
    QPointF getScaledSize();

    /**
     * Returns the scale multiplier.
     */
    qreal getScaleMultiplier()
    {
      return m_scaleMultiplier;
    };

    /**
     * Load the fullsize pixmap.
     */
    void fullSizePixmap();

    /**
     * Zoom in.
     */
    void zoomIn();
    /**
     * Zoom out.
     */
    void zoomOut();
    /**
     * Zoom into defined zoom level.
     */
    void setZoom(float zoomLevel);
    /**
     * Zoom to actual size.
     */
    void zoomActual();
    /**
     * Zoom to fit screen.
     */
    void zoomScreen();

    /**
     * Rotates clock-wise.
     */
    void rotateCW();
    /**
     * Rotates counter clock-wise
     */
    void rotateCCW();

    /**
     * Crop image.
     */
    void crop(const QRect &area);

    /**
     * Cancel the transformation changes.
     */
    void closeTransformations();

    /**
     * Save the crop operation result.
     */
    void saveCrop();

    /**
     * Shows the blur preview.
     *
     * @param blurFilters Number of blur filters.
     */
    void blurPreview(int blurFilters);
    /**
     * Saves the blur effects.
     *
     * @param blurFilters Number of blur filters.
     */
    void saveBlur(int blurFilters);

    /**
     * Shows the sharpen preview.
     *
     * @param sharpenFilters Number of sharpen filters.
     */
    void sharpenPreview(int sharpenFilters);
    /**
     * Saves the sharpen effects.
     *
     * @param sharpenFilters Number of sharpen filters.
     */
    void saveSharpen(int sharpenFilters);

    /**
     * Shows the resize preview.
     *
     * @param size New size of the image.
     */
    void resizePreview(const QSize &size);
    /**
     * Saves the resize effects.
     */
    void saveResize();

  public slots:
    /**
     * Changes the image.
     *
     * @param image The new image.
     */
    void changeImage(const QImage &image);

  protected:
    /**
     * Setup the item's UI.
     */
    void setupUi();

    /**
     * Overloaded method for defining its own painting operation.
     *
     * @param painter Reference to the painter.
     * @param option Options of this photo.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*);
    /**
     * Defines what to do on hover mouse enter.
     *
     * @param event Reference to the event itself.
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    /**
     * Defines what to do on hover mouse leave.
     *
     * @param event Reference to the event itself.
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    /**
     * Defines what to do on key press.
     *
     * @param event Reference to the event itself.
     */
    void keyPressEvent(QKeyEvent *event);
    /**
     * Defines what to do when a mouse press event happens.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    /**
     * Defines what to do on mouse double click.
     *
     * @param event Reference to the event itself.
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

  private:
    PhotoView *m_view;
    QPointF m_zero;
    qreal m_scaleMultiplier;
    QPixmap *m_fullsizePixmap;
    bool m_fullsize;

    // Animation variables
    QPointF m_oldPos;
    QGraphicsItemAnimation *m_animation;
    bool m_editMode;
    bool m_zooming;
    QTimeLine *m_itemTimeLine;
    bool m_pendingDoom;

    PhotoRect *m_rect;
    PhotoPixmap *m_pixmap;
    PhotoPixmap *m_new;
    PhotoName *m_text;
    PhotoDescription *m_description;

    bool m_hide;

    int m_rotation;

    // Connection with the model
    //GCore::ImageItem *m_item;
    QModelIndex m_index;

    /**
     * Rotates the image.
     */
    void rotate(int rotation);

    /**
     * Initialise timer.
     */
    void initialiseTimer();

  private slots:
    /**
     * Saves the name to the model.
     *
     * @param name New name.
     */
    void slotSaveName(const QString &name);
    /**
     * Saves description to the model.
     *
     * @param description The new description
     */
    void slotSaveDescription(const QString &description);
    /**
     * Sets the fullsize pixmap.
     *
     * @param step Step of the animation.
     */
    void slotSetFullsizePixmap(qreal step);

    /**
     * Defines the m_edit variable. Used for an early warning.
     *
     * @param edit Goes in or out of editing mode.
     */
    void slotEdit(bool edit);
};

}

}

#endif
