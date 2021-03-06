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
#include "photoitem.h"
#include "photoview.h"
#include "photoname.h"
#include "photodescription.h"

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItemAnimation>
#include <QtCore/QTimeLine>
#include <QtGui/QTextDocument>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QColor>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QScrollBar>

#include <QtCore/QtDebug>

#include "core/imagemodel.h"
#include "core/data.h"

namespace GWidgets
{

// PhotoRect
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

// PhotoPixmap
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

// PhotoItem

PhotoItem::PhotoItem(PhotoView *view)
    : QObject(0),
    QGraphicsItemGroup(0, view->scene()),
    m_view(view),
    m_scaleMultiplier(1),
    m_fullsizePixmap(0),
    m_fullsize(false),
    m_editMode(false),
    m_pendingDoom(false)
{
  setHandlesChildEvents(false);
  setAcceptsHoverEvents(true);
  setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

  m_itemTimeLine = new QTimeLine(1000, this);

  m_animation = new QGraphicsItemAnimation(m_view);
  m_animation->setItem(this);
  m_animation->setTimeLine(m_itemTimeLine);

  m_oldPos.setX(-170);
  m_oldPos.setY(-170);

  // Setup other items in the group
  setupUi();

  connect(m_itemTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(slotSetFullsizePixmap(qreal)));
  connect(view, SIGNAL(signalEditMode(bool)), this, SLOT(slotEdit(bool)));
}

PhotoItem::~PhotoItem()
{
  delete m_animation;
}

void PhotoItem::setupUi()
{
  // Setup other items
  m_rect = new PhotoRect(this, m_view->scene());
  m_rect->setAcceptsHoverEvents(true);
  m_rect->setRect(0, 0, ItemWidth, ItemHeight);
  m_rect->setZValue(1);

  m_rect->setBrush(QBrush(QColor(231, 231, 231)));
  m_rect->setPen(QPen(QColor(231, 231, 231)));

  m_pixmap = new PhotoPixmap(this, m_view->scene());
  m_pixmap->setAcceptsHoverEvents(true);
  m_pixmap->setPos(20, 5);
  m_pixmap->setZValue(2);
  m_pixmap->setTransformationMode(Qt::SmoothTransformation);
  m_pixmap->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);



  m_text = new PhotoName(this, m_view->scene(), m_view);
  m_text->setPos(15, 140);
  m_text->setZValue(2);

  m_description = new PhotoDescription(this, m_view->scene(), m_view);
  m_description->setPos(15, 160);
  m_description->setZValue(2);

  // Add items to this group
  addToGroup(m_rect);
  addToGroup(m_pixmap);
  addToGroup(m_text);
  addToGroup(m_description);
}

void PhotoItem::setPixmap(const QPixmap &pixmap)
{
  if (pixmap.width() > 128 || pixmap.height() > 128)
    m_pixmap->setPixmap(pixmap.scaled(QSize(128, 128), Qt::KeepAspectRatio));
  else
    m_pixmap->setPixmap(pixmap);

  qreal width = m_pixmap->pixmap().width();
  qreal height = m_pixmap->pixmap().height();

  m_pixmap->setPos(((30 + 128) / 2) - (width / 2), ((5 + 128) / 2) - (height / 2));

  delete m_fullsizePixmap;
  m_fullsizePixmap = 0;
}

void PhotoItem::setText(const QString &text, const QString &description)
{
  disconnect(m_text, SIGNAL(editingFinished(const QString&)), this, SLOT(slotSaveName(const QString&)));
  disconnect(m_description, SIGNAL(editingFinished(const QString&)), this, SLOT(slotSaveDescription(const QString&)));

  m_text->setText(text);
  m_description->setText("<i>" + description + "</i>");

  connect(m_text, SIGNAL(editingFinished(const QString&)), this, SLOT(slotSaveName(const QString&)));
  connect(m_description, SIGNAL(editingFinished(const QString&)), this, SLOT(slotSaveDescription(const QString&)));
}

void PhotoItem::rename()
{
  m_text->setEdit();
}

void PhotoItem::describe()
{
  m_description->setEdit();
}

bool PhotoItem::deleteItself()
{
  bool output = m_pendingDoom;
  setPos(500, -300);
  m_pendingDoom = true;
  return output;
}

void PhotoItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{}

void PhotoItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  Q_UNUSED(event)

  m_rect->setBrush(QBrush(QColor(204, 204, 204)));
  m_rect->setPen(QPen(QColor(0, 0, 0)));
}

void PhotoItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  Q_UNUSED(event)

  m_rect->setBrush(QBrush(QColor(231, 231, 231)));
  m_rect->setPen(QPen(QColor(231, 231, 231)));
}

void PhotoItem::keyPressEvent(QKeyEvent *event)
{
  //if (event->key() == Qt::Key_F2) {
  //  rename();
  //  event->ignore();
  //  return;
  //}

  QGraphicsItem::keyPressEvent(event);
}

void PhotoItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  switch (event->button()) {
    case (Qt::LeftButton) : {
        QGraphicsItemGroup::mousePressEvent(event);
        break;
      }
    case (Qt::MidButton) : {
        mouseDoubleClickEvent(event);
        break;
      }
    default : {
      return;
    }
  }
}

void PhotoItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  // Only left and middle button initiates the double click action
  if (event->button() == Qt::LeftButton || event->button() == Qt::MidButton) {
    m_view->setEditMode(true, this);
    qDebug("double clicking");
  }
}

void PhotoItem::setPos(qreal x, qreal y)
{
  if (m_itemTimeLine->state() != QTimeLine::Running)
    m_itemTimeLine->start();
  else {
    m_itemTimeLine->setPaused(true);
    QPointF currentPos = m_animation->posAt(m_itemTimeLine->currentValue());
    m_animation->clear();
    m_animation->setPosAt(0, m_oldPos);
    m_animation->setPosAt(m_itemTimeLine->currentValue(), currentPos);
    m_animation->setPosAt(1, QPointF(x, y));
    m_oldPos.setX(x);
    m_oldPos.setY(y);
    m_itemTimeLine->setPaused(false);

    return;
  }

  m_animation->clear();
  m_animation->setPosAt(0, m_oldPos);
  m_animation->setPosAt(1, QPointF(x, y));
  m_oldPos.setX(x);
  m_oldPos.setY(y);

}

QString PhotoItem::getText()
{
  return m_text->text();
}

QString PhotoItem::toolTip()
{
  return m_view->model()->data(m_view->indexForItem(const_cast<PhotoItem*>(this)), GCore::ImageModel::ImageTooltipRole).toString();
}

QPointF PhotoItem::getScaledSize()
{
  if (!m_editMode || !m_fullsizePixmap)
    return QPointF(m_view->width(), m_view->height());

  qreal scaledWidth;
  qreal scaledHeight;

  if (!m_fullsize) {
    qreal scaleFullscreenMultiplier;

    if (m_fullsizePixmap->width() > m_fullsizePixmap->height())
      scaleFullscreenMultiplier = ((qreal) m_view->width() - 20) / (qreal) m_fullsizePixmap->width();
    else
      scaleFullscreenMultiplier = ((qreal) m_view->height() - 20) / (qreal) m_fullsizePixmap->height();

    scaledWidth = m_fullsizePixmap->width() * scaleFullscreenMultiplier;
    scaledHeight = m_fullsizePixmap->height() * scaleFullscreenMultiplier;
  } else {
    scaledWidth = m_fullsizePixmap->width() * m_scaleMultiplier;
    scaledHeight = m_fullsizePixmap->height() * m_scaleMultiplier;
  }

  return QPointF(scaledWidth, scaledHeight);
}

void PhotoItem::fullSizePixmap()
{
  if (!m_editMode)
    return;

  if (!m_fullsizePixmap) {
    m_view->showLoading(true);
    QPixmap pixmap = QPixmap::fromImage(m_view->model()->data(m_view->indexForItem(this), GCore::ImageModel::ImagePictureRole).value<QImage>());

    m_fullsizePixmap = new QPixmap(pixmap);
  }
}

void PhotoItem::zoomIn()
{
  if (m_itemTimeLine->state() != QTimeLine::Running)
    m_itemTimeLine->start();

  // Calculate new scale
  QPointF scaledSize = getScaledSize();
  qreal newScale = (scaledSize.x() + (qreal) ItemZoomStep) / (qreal) m_fullsizePixmap->width();

  m_animation->clear();
  m_itemTimeLine->setCurrentTime(0);
  m_animation->setScaleAt(0, m_scaleMultiplier, m_scaleMultiplier);
  m_scaleMultiplier = newScale;
  m_animation->setScaleAt(0.1, m_scaleMultiplier, m_scaleMultiplier);
}

void PhotoItem::zoomOut()
{
  if (m_itemTimeLine->state() != QTimeLine::Running)
    m_itemTimeLine->start();

  // Calculate new scale
  QPointF scaledSize = getScaledSize();
  qreal newScale = (scaledSize.x() - (qreal) ItemZoomStep) / (qreal) m_fullsizePixmap->width();

  m_animation->clear();
  m_itemTimeLine->setCurrentTime(0);
  m_animation->setScaleAt(0, m_scaleMultiplier, m_scaleMultiplier);
  m_scaleMultiplier = newScale;
  m_animation->setScaleAt(0.1, m_scaleMultiplier, m_scaleMultiplier);
}

void PhotoItem::setZoom(float zoomLevel)
{
  if (m_itemTimeLine->state() != QTimeLine::Running)
    m_itemTimeLine->start();

  m_animation->clear();
  m_itemTimeLine->setCurrentTime(0);
  m_animation->setScaleAt(0, m_scaleMultiplier, m_scaleMultiplier);
  m_scaleMultiplier = zoomLevel;
  m_animation->setScaleAt(0.1, m_scaleMultiplier, m_scaleMultiplier);
}

void PhotoItem::zoomActual()
{
  if (m_itemTimeLine->state() != QTimeLine::Running)
    m_itemTimeLine->start();

  m_animation->clear();
  m_itemTimeLine->setCurrentTime(0);
  m_animation->setScaleAt(0, m_scaleMultiplier, m_scaleMultiplier);
  m_scaleMultiplier = 1;
  m_animation->setScaleAt(1, m_scaleMultiplier, m_scaleMultiplier);
}

void PhotoItem::zoomScreen()
{
  if (m_itemTimeLine->state() != QTimeLine::Running)
    m_itemTimeLine->start();

  // Calculate new scale
  qreal newScale;
  if (m_view->width() < m_view->height())
    newScale = ((qreal) m_view->width() - 20) / (qreal) m_fullsizePixmap->width();
  else
    newScale = ((qreal) m_view->height() - 20) / (qreal) m_fullsizePixmap->height();

  m_animation->clear();
  m_itemTimeLine->setCurrentTime(0);
  m_animation->setScaleAt(0, m_scaleMultiplier, m_scaleMultiplier);
  m_scaleMultiplier = newScale;
  m_animation->setScaleAt(1, m_scaleMultiplier, m_scaleMultiplier);
}

void PhotoItem::slotEdit(bool edit)
{
  m_editMode = edit;

  // Make changes to go to or out of edit
  if (m_editMode) {
    // Item isn't selectable nor focusable
    setFlags(0);

    // We move the picture to a new location
    m_pixmap->setPos(0, 0);

    // We hide the boxes
    m_text->hide();
    m_description->hide();
    m_rect->hide();
  } else {
    // Item is selectable and focusable again
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    // Go back to default value
    setZValue(1);
  }
}

void PhotoItem::slotSaveName(const QString &name)
{
  static_cast<GCore::ImageModel*>(m_view->model())->setName(name, m_view->indexForItem(this));
  setToolTip(m_view->model()->data(m_view->indexForItem(this), GCore::ImageModel::ImageTooltipRole).toString());
}

void PhotoItem::slotSaveDescription(const QString &description)
{
  static_cast<GCore::ImageModel*>(m_view->model())->setDescription(description, m_view->indexForItem(this));
  setToolTip(m_view->model()->data(m_view->indexForItem(this), GCore::ImageModel::ImageTooltipRole).toString());
}

void PhotoItem::slotSetFullsizePixmap(qreal step)
{
  if (!m_editMode) {
    qreal oldScaleMultiplier;

    oldScaleMultiplier = m_scaleMultiplier;

    // At the last step we change the thumbnail and show the borders
    if (step > 0.9999) {
      setPixmap(m_view->model()->data(m_view->indexForItem(this), GCore::ImageModel::ImageThumbnailRole).value<QIcon>().pixmap(128, 128));

      // Show the borders and other elements
      m_text->show();
      m_description->show();
      m_rect->show();
    }

    // We set the scaling if the Item is still in fullsize mode (This created that eye candish zoom out)
    if (m_fullsize) {
      m_animation->setScaleAt(0, oldScaleMultiplier, oldScaleMultiplier);
      m_animation->setScaleAt(0.9999, (qreal) 128 / (qreal) m_fullsizePixmap->height(), (qreal) 128 / (qreal) m_fullsizePixmap->width());
      m_fullsize = false;
    }

    // The default scaling at the end
    m_animation->setScaleAt(1, 1, 1);
  } else if (!m_fullsize) {
    // Calculate the new scaling for the upcoming full sized photo
    qreal scaleMultiplier;
    qreal oldScaleMultiplier;
    qreal scaleFullscreenMultiplier;

    oldScaleMultiplier = m_scaleMultiplier;

    // We calculate how much spreading is needed for photo to fit the view
    if (m_view->width() < m_view->height())
      scaleMultiplier = ((qreal) m_view->width() - 20) / (qreal) m_pixmap->pixmap().width();
    else
      scaleMultiplier = ((qreal) m_view->height() - 20) / (qreal) m_pixmap->pixmap().height();

    // And calculation for fullsized photo to fit the view
    if (m_fullsizePixmap) {
      if (m_view->width() < m_view->height())
        scaleFullscreenMultiplier = ((qreal) m_view->width() - 20) / (qreal) m_fullsizePixmap->width();
      else
        scaleFullscreenMultiplier = ((qreal) m_view->height() - 20) / (qreal) m_fullsizePixmap->height();

      m_fullsize = true;


    } else {
      scaleFullscreenMultiplier = scaleMultiplier;
    }

    m_scaleMultiplier = scaleFullscreenMultiplier;

    // Scaling ratios for fully sized photo
    m_animation->setScaleAt(0.9999999999, scaleMultiplier, scaleMultiplier);
    m_animation->setScaleAt(1, scaleFullscreenMultiplier, scaleFullscreenMultiplier);
  }

  if (m_pendingDoom && step > 0.99999999) {
    m_view->scene()->removeItem(this);
    m_pendingDoom = false;
  }

  if (!m_editMode || step != 1)
    return;

  // We use the new fully sized photo
  if (m_fullsizePixmap) {
    m_view->showLoading(false);
    m_pixmap->setPixmap(*m_fullsizePixmap);
  }
}

}



