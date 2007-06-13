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
#include "photoitem.h"

#include <QtCore/QTimeLine>

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItemAnimation>
#include <QtGui/QTextDocument>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QColor>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QScrollBar>

#include "core/imagemodel.h"
#include "core/imageitem.h"
#include "core/data.h"

#include "widgets/photoview.h"

#include "widgets/photowidgets/photoname.h"
#include "widgets/photowidgets/photodescription.h"
#include "widgets/photowidgets/photorect.h"
#include "widgets/photowidgets/photopixmap.h"

namespace GWidgets
{

namespace GPhotoWidgets
{

const QPoint PhotoItem::PixmapPosition = QPoint(30, 15);

PhotoItem::PhotoItem(PhotoView *view, const QModelIndex &index)
    : QObject(0),
    QGraphicsItemGroup(0, view->scene()),
    m_view(view),
    m_scaleMultiplier(1),
    m_fullsizePixmap(0),
    m_fullsize(false),
    m_editMode(false),
    m_zooming(false),
    m_pendingDoom(false),
    m_new(0),
    m_hide(false),
    m_rotation(0),
    m_index(index)
{
  if (!index.isValid())
    return;

  m_item = static_cast<GCore::ImageItem*>(m_index.internalPointer());

  setHandlesChildEvents(false);
  setAcceptsHoverEvents(true);
  setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

  m_itemTimeLine = new QTimeLine(1000, this);
  m_itemTimeLine->setUpdateInterval(10);

  m_animation = new QGraphicsItemAnimation(m_view);
  m_animation->setItem(this);
  m_animation->setTimeLine(m_itemTimeLine);
  m_animation->setPosAt(1, QPointF(-170, -170));
  QGraphicsItemGroup::setPos(-170, -170);

  // Setup other items in the group
  setupUi();

  // Connect the elements
  connect(m_itemTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(slotSetFullsizePixmap(qreal)));
  connect(view, SIGNAL(signalEditMode(bool)), this, SLOT(slotEdit(bool)));
  connect(view, SIGNAL(signalEditMode(bool)), static_cast<GCore::ImageItem*>(index.internalPointer()), SLOT(prepareForEdit(bool)));
  connect(m_text, SIGNAL(editingFinished(const QString&)), this, SLOT(slotSaveName(const QString&)));
  connect(m_description, SIGNAL(editingFinished(const QString&)), this, SLOT(slotSaveDescription(const QString&)));

  connect(m_item, SIGNAL(imageChanged(const QImage&)), this, SLOT(changeImage(const QImage&)));
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

  QLinearGradient normalGradient(QPoint(0, 0), QPoint(0, ItemHeight));
  normalGradient.setColorAt(0, QColor(228, 241, 247));
  normalGradient.setColorAt(0.5, QColor(240, 249, 254));
  normalGradient.setColorAt(1, QColor(228, 241, 247));
  m_normalGradient = normalGradient;
  m_normalBorder = QPen(QColor(134, 201, 239));

  QLinearGradient hoverGradient(QPoint(0, 0), QPoint(0, ItemHeight));
  hoverGradient.setColorAt(0, QColor(202, 232, 245));
  hoverGradient.setColorAt(0.5, QColor(213, 241, 252));
  hoverGradient.setColorAt(1, QColor(202, 232, 245));
  m_hoverGradient = hoverGradient;
  m_hoverBorder = QPen(QColor(159, 211, 249));

  m_rect->setBrush(QBrush(m_normalGradient));
  m_rect->setPen(m_normalBorder);

  m_pixmap = new PhotoPixmap(this, m_view->scene());
  m_pixmap->setAcceptsHoverEvents(true);
  m_pixmap->setPos(PixmapPosition);
  m_pixmap->setZValue(2);
  m_pixmap->setTransformationMode(Qt::SmoothTransformation);
  m_pixmap->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

  // Get the thumbnail
  setPixmap(m_index.data(GCore::ImageModel::ImageThumbnailRole).value<QIcon>().pixmap(128, 128));

  m_text = new PhotoName(this, m_view->scene(), m_view);
  m_text->setPos(15, 140);
  m_text->setZValue(2);

  // Get the name
  m_text->setText(m_index.data(GCore::ImageModel::ImageNameRole).toString());

  m_description = new PhotoDescription(this, m_view->scene(), m_view);
  m_description->setPos(15, 160);
  m_description->setZValue(2);

  // Get the description
  m_description->setText(m_index.data(GCore::ImageModel::ImageDescriptionRole).toString());

  // Get the tooltip
  setToolTip(m_index.data(GCore::ImageModel::ImageTooltipRole).toString());

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
  m_rotation = 0;

  m_pixmap->setPos(((PixmapPosition.x() + 128) / 2) - (width / 2), ((PixmapPosition.y() + 128) / 2) - (height / 2));

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

  m_item = 0;
  m_index = QModelIndex();

  return output;
}

void PhotoItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{}

void PhotoItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
  Q_UNUSED(event)

  m_rect->setBrush(QBrush(m_hoverGradient));
  m_rect->setPen(m_hoverBorder);
}

void PhotoItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
  Q_UNUSED(event)

  m_rect->setBrush(QBrush(m_normalGradient));
  m_rect->setPen(m_normalBorder);
}

void PhotoItem::keyPressEvent(QKeyEvent *event)
{
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
  if (GCore::Data::self()->isVisualEffectsDisabled()) {
    QGraphicsItemGroup::setPos(x, y);
    return;
  }

  initialiseTimer();

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
  return m_index.data(GCore::ImageModel::ImageTooltipRole).toString();
}

QModelIndex PhotoItem::getIndex()
{
  return m_index;
}

QSizeF PhotoItem::getScaledSize()
{
  if (!m_editMode || !m_fullsizePixmap)
    return QSizeF(m_view->viewport()->width(), m_view->viewport()->height());

  qreal scaledWidth;
  qreal scaledHeight;

  scaledWidth = m_fullsizePixmap->width() * m_scaleMultiplier;
  scaledHeight = m_fullsizePixmap->height() * m_scaleMultiplier;

  return QSizeF(scaledWidth, scaledHeight);
}

void PhotoItem::fullSizePixmap()
{
  if (!m_fullsizePixmap) {
    if (!GCore::Data::self()->isVisualEffectsDisabled())
      m_view->showLoading(true, tr("Sharpening image, please wait"));

    QPixmap pixmap = QPixmap::fromImage(m_index.data(GCore::ImageModel::ImagePictureRole).value<QImage>());

    m_fullsizePixmap = new QPixmap(pixmap);

    if (GCore::Data::self()->isVisualEffectsDisabled()) {
      qreal scaleFactor = calculateScale(*m_fullsizePixmap, m_view->viewport()->size());
      m_scaleMultiplier = scaleFactor;
      scale(scaleFactor, scaleFactor);
      m_pixmap->setPixmap(*m_fullsizePixmap);
      m_fullsize = true;
    }
  }
}

void PhotoItem::zoomIn()
{
  // Calculate new scale
  QSizeF scaledSize = getScaledSize();
  qreal newScale = calculateScale(*m_fullsizePixmap, scaledSize + QSizeF(ItemZoomStep, ItemZoomStep));

  m_scaleMultiplier = newScale;

  if (GCore::Data::self()->isVisualEffectsDisabled()) {
    resetTransform();
    scale(m_scaleMultiplier, m_scaleMultiplier);
  } else {
    initialiseTimer();
    m_animation->setScaleAt(0.1, m_scaleMultiplier, m_scaleMultiplier);
    m_animation->setScaleAt(1, m_scaleMultiplier, m_scaleMultiplier);
  }
}

void PhotoItem::zoomOut()
{
  // Calculate new scale
  QSizeF scaledSize = getScaledSize();
  qreal newScale = calculateScale(*m_fullsizePixmap, scaledSize - QSizeF(ItemZoomStep, ItemZoomStep));

  m_scaleMultiplier = newScale;

  if (GCore::Data::self()->isVisualEffectsDisabled()) {
    resetTransform();
    scale(m_scaleMultiplier, m_scaleMultiplier);
  } else {
    initialiseTimer();
    m_animation->setScaleAt(0.1, m_scaleMultiplier, m_scaleMultiplier);
    m_animation->setScaleAt(1, m_scaleMultiplier, m_scaleMultiplier);
  }
}

void PhotoItem::setZoom(float zoomLevel)
{
  m_scaleMultiplier = zoomLevel;

  if (GCore::Data::self()->isVisualEffectsDisabled()) {
    resetTransform();
    scale(m_scaleMultiplier, m_scaleMultiplier);
  } else {
    initialiseTimer();
    m_animation->setScaleAt(0.1, m_scaleMultiplier, m_scaleMultiplier);
    m_animation->setScaleAt(1, m_scaleMultiplier, m_scaleMultiplier);
  }
}

void PhotoItem::zoomActual()
{
  m_scaleMultiplier = 1;

  if (GCore::Data::self()->isVisualEffectsDisabled()) {
    resetTransform();
    scale(m_scaleMultiplier, m_scaleMultiplier);
  } else {
    initialiseTimer();
    m_animation->setScaleAt(1, m_scaleMultiplier, m_scaleMultiplier);
  }
}

void PhotoItem::zoomScreen()
{
  // Calculate new scale
  qreal newScale = calculateScale(*m_fullsizePixmap, m_view->viewport()->size());

  m_scaleMultiplier = newScale;

  if (GCore::Data::self()->isVisualEffectsDisabled()) {
    resetTransform();
    scale(m_scaleMultiplier, m_scaleMultiplier);
  } else {
    initialiseTimer();
    m_animation->setScaleAt(1, m_scaleMultiplier, m_scaleMultiplier);
  }
}

void PhotoItem::rotateCW()
{
  m_item->rotateCW();
  rotate(90);
}

void PhotoItem::rotateCCW()
{
  m_item->rotateCCW();
  rotate(-90);
}

void PhotoItem::crop(const QRect &area)
{
  QRect mappedArea(mapToItem(m_pixmap, area.topLeft()).toPoint() / m_scaleMultiplier, area.size() / m_scaleMultiplier);

  if (m_new) {
    m_pixmap->setPixmap(QPixmap::fromImage(m_index.data(GCore::ImageModel::ImagePictureRole).value<QImage>()));
    delete m_new;
  }
  mappedArea = m_pixmap->pixmap().rect().intersected(mappedArea);

  m_new = new PhotoPixmap(this, m_view->scene());
  m_new->setPixmap(m_pixmap->pixmap().copy(mappedArea));
  m_new->setPos(mappedArea.topLeft());
  m_new->setZValue(5);

  QImage inverted(m_pixmap->pixmap().toImage());
  inverted.invertPixels();

  m_pixmap->setPixmap(QPixmap::fromImage(inverted));
}

void PhotoItem::closeTransformations()
{
  if (m_new) {
    m_pixmap->setPixmap(QPixmap::fromImage(m_index.data(GCore::ImageModel::ImagePictureRole).value<QImage>()));
    delete m_new;
    m_new = 0;
  }
}

void PhotoItem::saveCrop()
{
  if (!m_item)
    return;

  if (!m_new) {
    qDebug("BUG! m_new premeturely deleted!");
    return;
  }

  QRect area(m_new->pos().toPoint(), m_new->pixmap().rect().size());

  m_item->crop(area);
}

void PhotoItem::blurPreview(int blurFilters)
{
  if (!m_item)
    return;

  m_item->blur(blurFilters);

  m_view->showLoading(true, tr("Generating blur preview, please wait"));
}

void PhotoItem::saveBlur()
{
  if (!m_item)
    return;

  m_item->saveImage();

  closeTransformations();

  m_view->showLoading(true, tr("Saving image with blur filters, please wait"));
}

void PhotoItem::sharpenPreview(int sharpenFilters)
{
  if (!m_item)
    return;

  m_item->sharpen(sharpenFilters);

  m_view->showLoading(true, tr("Generating sharpen preview, please wait"));
}

void PhotoItem::saveSharpen()
{
  if (!m_item)
    return;

  m_item->saveImage();

  closeTransformations();

  m_view->showLoading(true, tr("Saving image with blur filters, please wait"));
}

void PhotoItem::resizePreview(const QSize &size)
{
  if (!m_item)
    return;

  m_item->resize(size);

  m_view->showLoading(true, tr("Generating resize preview, please wait"));
}

void PhotoItem::saveResize()
{
  if (!m_item)
    return;

  m_item->saveImage();

  closeTransformations();

  m_view->showLoading(true, tr("Saving resized image, please wait"));
}

void PhotoItem::cancelTransformations()
{
  if (!m_item)
    return;

  closeTransformations();

  m_pixmap->setPixmap(QPixmap::fromImage(m_index.data(GCore::ImageModel::ImagePictureRole).value<QImage>()));

  m_item->cancelTransformations();
}

void PhotoItem::rotate(int rotation)
{
  Q_UNUSED(rotation)
  m_view->showLoading(true, tr("Rotating image, please wait"));

  /*initialiseTimer();
  m_rotation += rotation;
  m_animation->setRotationAt(1, m_rotation);

  if (qAbs(m_rotation) == 360)
    m_rotation = 0;

  int absoluteRotation = 0;

  if (m_rotation == -90)
    absoluteRotation = 270;
  else if (m_rotation == -270)
    absoluteRotation = 90;
  else
    absoluteRotation = qAbs(m_rotation);


  switch (absoluteRotation) {
    case 0 : {
      m_animation->setPosAt(0, m_oldPos);
      m_oldPos.setX(0);
      m_oldPos.setY(0);
      m_animation->setPosAt(1, m_oldPos);
      break;
    }
    case 90 : {
      m_animation->setPosAt(0, m_oldPos);
      m_oldPos.setX(m_pixmap->pixmap().height() * m_scaleMultiplier);
      m_oldPos.setY(0);
      m_animation->setPosAt(1, m_oldPos);
      break;
    }
    case 180 : {
      m_animation->setPosAt(0, m_oldPos);
      m_oldPos.setX(m_pixmap->pixmap().width() * m_scaleMultiplier);
      m_oldPos.setY(m_pixmap->pixmap().height() * m_scaleMultiplier);
      m_animation->setPosAt(1, m_oldPos);
      break;
    }
    case 270 : {
      m_animation->setPosAt(0, m_oldPos);
      m_oldPos.setX(0);
      m_oldPos.setY(m_pixmap->pixmap().width() * m_scaleMultiplier);
      m_animation->setPosAt(1, m_oldPos);
      break;
    }
  }*/
}

void PhotoItem::initialiseTimer()
{
  if (m_itemTimeLine->state() == QTimeLine::Running) {
    m_itemTimeLine->setPaused(true);

    // Saving initial values and current values

    QPointF position = m_animation->posAt(0);
    QPair<qreal, qreal> shear(m_animation->horizontalShearAt(1), m_animation->verticalShearAt(0));
    QPair<qreal, qreal> scale(m_animation->horizontalScaleAt(1), m_animation->verticalScaleAt(0));
    qreal rotation = m_animation->rotationAt(0);

    qreal currentStep = m_itemTimeLine->currentValue();
    QPointF currentPos = m_animation->posAt(currentStep);
    QPair<qreal, qreal> currentShear(m_animation->horizontalShearAt(currentStep), m_animation->verticalShearAt(currentStep));
    QPair<qreal, qreal> currentScale(m_animation->horizontalScaleAt(currentStep), m_animation->verticalScaleAt(currentStep));
    qreal currentRotation = m_animation->rotationAt(currentStep);

    m_animation->clear();

    m_animation->setPosAt(0, position);
    m_animation->setShearAt(0, shear.first, shear.second);
    m_animation->setScaleAt(0, scale.first, scale.second);
    m_animation->setRotationAt(0, rotation);

    m_animation->setPosAt(currentStep, currentPos);
    m_animation->setShearAt(currentStep, currentShear.first, currentShear.second);
    m_animation->setScaleAt(currentStep, currentScale.first, currentScale.second);
    m_animation->setRotationAt(currentStep, currentRotation);

  } else {
    // Setting finishing values to initial values

    QPointF position = m_animation->posAt(1);
    QPair<qreal, qreal> shear(m_animation->horizontalShearAt(1), m_animation->verticalShearAt(1));
    QPair<qreal, qreal> scale(m_animation->horizontalScaleAt(1), m_animation->verticalScaleAt(1));
    qreal rotation = m_animation->rotationAt(1);

    m_animation->clear();

    m_animation->setPosAt(0, position);
    m_animation->setShearAt(0, shear.first, shear.second);
    m_animation->setScaleAt(0, scale.first, scale.second);
    m_animation->setRotationAt(0, rotation);
  }

  if (m_itemTimeLine->state() != QTimeLine::Running)
    m_itemTimeLine->start();
  else
    m_itemTimeLine->setPaused(false);
}

void PhotoItem::slotEdit(bool edit)
{
  if (!m_item)
    return;

  m_zooming = !GCore::Data::self()->isVisualEffectsDisabled();
  m_editMode = edit;

  // Make changes to go to or out of edit
  if (edit) {
    // Item isn't selectable nor focusable
    setFlags(0);

    // We move the picture to a new location
    m_pixmap->setPos(0, 0);

    // We hide the boxes
    m_text->hide();
    m_description->hide();
    m_rect->hide();

    setToolTip("");

    connect(m_item, SIGNAL(imageChanged(const QImage&)), this, SLOT(changeImage(const QImage&)));
  } else {
    // Item is selectable and focusable again
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    if (GCore::Data::self()->isVisualEffectsDisabled()) {
      setPixmap(m_index.data(GCore::ImageModel::ImageThumbnailRole).value<QIcon>().pixmap(128, 128));
      resetTransform();
      m_text->show();
      m_description->show();
      m_rect->show();
    }

    if (m_new) {
      delete m_new;
      m_new = 0;
    }

    // Go back to default value
    setZValue(1);

    setToolTip(m_index.data(GCore::ImageModel::ImageTooltipRole).toString());
  }
}

void PhotoItem::slotSaveName(const QString &name)
{
  if (!m_item)
    return;

  m_item->setName(name);
  setToolTip(m_index.data(GCore::ImageModel::ImageTooltipRole).toString());
}

void PhotoItem::slotSaveDescription(const QString &description)
{
  if (!m_item)
    return;

  m_item->setDescription(description);
  setToolTip(m_index.data(GCore::ImageModel::ImageTooltipRole).toString());
}

void PhotoItem::slotSetFullsizePixmap(qreal step)
{
  if (m_hide && step > 0.9) {
    QGraphicsItem::hide();
    return;
  }

  if (!m_hide && step < 0.1) {
    QGraphicsItem::show();
  }

  if (m_zooming)
    if (!m_editMode) {
      qreal oldScaleMultiplier;

      oldScaleMultiplier = m_scaleMultiplier;

      // At the last step we change the thumbnail and show the borders
      if (step > 0.9999) {
        setPixmap(m_index.data(GCore::ImageModel::ImageThumbnailRole).value<QIcon>().pixmap(128, 128));

        // Show the borders and other elements
        m_text->show();
        m_description->show();
        m_rect->show();
        m_zooming = false;
      }

      // We set the scaling if the Item is still in fullsize mode (This creates that eye candish zoom out)
      if (m_fullsize) {
        // Making sure that we are going to have the picture with changes applied
        delete m_fullsizePixmap;
        m_fullsizePixmap = 0;
        fullSizePixmap();
        m_view->showLoading(false);
        m_pixmap->setPixmap(*m_fullsizePixmap);
        m_animation->setRotationAt(0, 0);
        m_animation->setRotationAt(1, 0);

        m_animation->setScaleAt(0, oldScaleMultiplier, oldScaleMultiplier);
        m_animation->setScaleAt(0.9999, (qreal) 128 / (qreal) m_fullsizePixmap->height(), (qreal) 128 / (qreal) m_fullsizePixmap->width());
        m_fullsize = false;
      }

      // The default scaling at the end
      m_animation->setScaleAt(1, 1, 1);
    } else {
      // Calculate the new scaling for the upcoming full sized photo
      qreal scaleMultiplier;
      qreal oldScaleMultiplier;
      qreal scaleFullscreenMultiplier;

      oldScaleMultiplier = m_scaleMultiplier;

      // We calculate how much spreading is needed for photo to fit the view
      scaleMultiplier = calculateScale(m_pixmap->pixmap(), m_view->viewport()->size());

      // And calculation for fullsized photo to fit the view
      if (m_fullsizePixmap) {
        scaleFullscreenMultiplier = calculateScale(*m_fullsizePixmap, m_view->viewport()->size());

        m_fullsize = true;


      } else {
        scaleFullscreenMultiplier = scaleMultiplier;
      }

      m_scaleMultiplier = scaleFullscreenMultiplier;

      // Scaling ratios for fully sized photo
      m_animation->setScaleAt(0.9999, scaleMultiplier, scaleMultiplier);
      m_animation->setScaleAt(1, scaleFullscreenMultiplier, scaleFullscreenMultiplier);
    }

  if (m_pendingDoom && step > 0.9999) {
    m_view->scene()->removeItem(this);
    m_pendingDoom = false;
  }

  if (!m_editMode || step != 1)
    return;

  // We use the new fully sized photo
  if (m_fullsizePixmap && m_zooming) {
    m_view->showLoading(false);
    m_pixmap->setPixmap(*m_fullsizePixmap);
    m_zooming = false;
  }
}

qreal PhotoItem::calculateScale(const QPixmap &image, const QSizeF &size) const
{
  qreal scale;

  if (size.width() < size.height())
    scale = static_cast<qreal>(size.width() - 20) / static_cast<qreal>(image.width());
  else
    scale = static_cast<qreal>(size.height() - 20) / static_cast<qreal>(image.height());

  return scale;
}

void PhotoItem::changeImage(const QImage &image)
{
  if (m_new) {
    delete m_new;
    m_new = 0;
  }

  m_fullsizePixmap = new QPixmap(QPixmap::fromImage(image));
  m_pixmap->setPixmap(*m_fullsizePixmap);
  m_view->showLoading(false);
}

}

}
