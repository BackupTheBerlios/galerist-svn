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
#include "photoview.h"
#include "photoitem.h"
#include "photocontrol.h"

#include "core/data.h"
#include "core/imagemodel.h"
#include "core/imageitem.h"

#include <QtGui/QApplication>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItem>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QScrollBar>
#include <QtCore/QModelIndex>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QTimeLine>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtCore/QTimer>
#include <QtGui/QGraphicsItem>
#include <QtGui/QRubberBand>
#include <QtGui/QMouseEvent>
#include <QtCore/QProcess>
#include <QtGui/QMessageBox>
#include <QtCore/QDir>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtCore/QUrl>
#include <QtGui/QSortFilterProxyModel>

#include <QtCore/QtDebug>

namespace GWidgets
{

PhotoView::PhotoView(QWidget *parent)
    : QGraphicsView(parent),
    m_timerId(0),
    m_editMode(false),
    m_currentEdited(0),
    m_model(0),
    m_rubberBand(0)
{
  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setBackgroundBrush(Qt::white);
  setScene(scene);

  // Set some graphics view options
  setSpacing(10);
  setRenderHint(QPainter::Antialiasing);
  setInteractive(true);

  if (GCore::Data::self()->getOpengl() && QGLFormat::hasOpenGL())
    setViewport(new QGLWidget(QGLFormat(QGL::DirectRendering | QGL::SampleBuffers)));
  //setDragMode(QGraphicsView::RubberBandDrag);

  // Loading item
  m_loading = new QGraphicsPixmapItem(QPixmap(":/images/loading.png"), 0, this->scene());
  m_loading->setZValue(3.5);
  m_loading->hide();

  m_oldHorizontalScrollMaximum = horizontalScrollBar()->maximum();
  m_oldVerticalScrollMaximum = verticalScrollBar()->maximum();

  // Connect signals
  connect(scene, SIGNAL(changed(const QList<QRectF>&)), this, SLOT(slotSceneChanged()));

  QTimer::singleShot(1, this, SLOT(slotConnectNavButtons()));
}

void PhotoView::setSpacing(int spacing)
{
  m_spacing = spacing;
}

void PhotoView::setModel(QAbstractItemModel *model)
{
  if (m_model == model)
    return;

  if (m_model) {
    // Disconnect old model
    disconnect(m_model, SIGNAL(layoutChanged()), this, SLOT(slotModelLayoutChanged()));
    disconnect(m_model, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(slotModelRowsInserted(const QModelIndex&, int, int)));
    disconnect(m_model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(slotModelRowsRemoved(const QModelIndex&, int, int)));
    disconnect(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slotModelDataChanged(const QModelIndex&, const QModelIndex&)));
    disconnect(m_model, SIGNAL(modelReset()), this, SLOT(slotModelReset()));
  }

  m_model = model;

  // Connect the new model
  connect(m_model, SIGNAL(layoutChanged()), this, SLOT(slotModelLayoutChanged()));
  connect(m_model, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(slotModelRowsInserted(const QModelIndex&, int, int)));
  connect(m_model, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(slotModelRowsRemoved(const QModelIndex&, int, int)));
  connect(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slotModelDataChanged(const QModelIndex&, const QModelIndex&)));
  connect(m_model, SIGNAL(modelReset()), this, SLOT(slotModelReset()));
}

void PhotoView::setRootIndex(const QModelIndex &index)
{
  QModelIndex mappedIndex = GCore::Data::self()->getModelProxy()->mapToSource(index);
  
  if (!mappedIndex.isValid() || mappedIndex.model() != m_model || mappedIndex == m_rootIndex)
    return;

  setFocus();

  m_rootIndex = mappedIndex;
  setEditMode(false);
  m_currentEdited = 0;

  // Read items
  readModel();
}

void PhotoView::slotEdit()
{
  setEditMode(true, getSelectedPhoto());
}

void PhotoView::slotRename()
{
  PhotoItem *selectedPhoto = getSelectedPhoto();
  if (selectedPhoto)
    selectedPhoto->rename();
}

void PhotoView::slotDescribe()
{
  PhotoItem *selectedPhoto = getSelectedPhoto();
  if (selectedPhoto)
    selectedPhoto->describe();
}

void PhotoView::slotSelectAll()
{
  QHash<QModelIndex, PhotoItem*>::const_iterator end = m_itemHash.constEnd();
  for (QHash<QModelIndex, PhotoItem*>::const_iterator count = m_itemHash.constBegin(); count != end; count++)
    count.value()->setSelected(true);
}

void PhotoView::slotDeselectAll()
{
  QHash<QModelIndex, PhotoItem*>::const_iterator end = m_itemHash.constEnd();
  for (QHash<QModelIndex, PhotoItem*>::const_iterator count = m_itemHash.constBegin(); count != end; count++)
    count.value()->setSelected(false);
}

void PhotoView::slotInvertSelection()
{
  QHash<QModelIndex, PhotoItem*>::const_iterator end = m_itemHash.constEnd();
  for (QHash<QModelIndex, PhotoItem*>::const_iterator count = m_itemHash.constBegin(); count != end; count++)
    count.value()->setSelected(!(*count)->isSelected());
}

PhotoItem *PhotoView::itemForIndex(const QModelIndex &index)
{
  // !index.isValid() || m_itemVector.count() < index.row()
  if (index.isValid())
    return 0;

  return m_itemHash.value(index);
}

void PhotoView::readModel()
{
  // Clear selection and focus. If we don't do this, it crashes
  scene()->clearFocus();
  scene()->clearSelection();

  // Remove all current items
  foreach(PhotoItem *item, m_itemHash) {
    if (!item->deleteItself())
      m_removeList << item;
    //scene()->removeItem(item);
    //delete item;
  }

  m_itemHash.clear();
  m_itemVector.clear();

  int rowCount = m_model->rowCount(m_rootIndex);

  for (int i = 0; i < rowCount; i++) {
    QModelIndex row = m_model->index(i, 0, m_rootIndex);

    // We don't want to show directories(galleries), don't we?
    if (row.data(GCore::ImageModel::ImageTypeRole).toInt() == GCore::ImageItem::Gallery)
      continue;

    PhotoItem *item = new PhotoItem(this);
    item->setText(row.data(Qt::DisplayRole).toString(), row.data(GCore::ImageModel::ImageDescriptionRole).toString());
    item->setPixmap(row.data(Qt::DecorationRole).value<QIcon>().pixmap(128, 128));
    item->setToolTip(row.data(Qt::ToolTipRole).toString());

    // Add to item vector
    m_itemHash.insert(row, item);
    m_itemVector.append(item);
  }

  updateScene();
}

void PhotoView::slotModelReset()
{
  readModel();
  qDebug("model reset requested.");
}

void PhotoView::slotModelLayoutChanged()
{
  readModel();
  qDebug("layout has changed.");
}

void PhotoView::slotModelRowsInserted(const QModelIndex &parent, int start, int end)
{
  QModelIndex index = parent;
  if (!(parent.isValid() && m_rootIndex == parent))
    return;

  for (int count = start; count < end; count++) {
    QModelIndex row = m_model->index(count, 0, index);
    if (row.data(GCore::ImageModel::ImageTypeRole) != GCore::ImageItem::Image)
      continue;

    PhotoItem *item = new PhotoItem(this);
    item->setText(row.data(Qt::DisplayRole).toString(), row.data(GCore::ImageModel::ImageDescriptionRole).toString());
    item->setPixmap(row.data(Qt::DecorationRole).value<QIcon>().pixmap(128, 128));
    item->setToolTip(row.data(Qt::ToolTipRole).toString());

    // Add to item vector
    m_itemHash.insert(row, item);
    m_itemVector.append(item);
  }
  updateScene();

  qDebug("rows inserted");
}

void PhotoView::slotModelRowsRemoved(const QModelIndex &parent, int start, int end)
{
  if (!(parent.isValid() && m_rootIndex == parent))
    return;

  for (int count = end; count >= start; count--) {
    PhotoItem *picture = m_itemVector.value(count);
    QModelIndex index = indexForItem(picture);
    if (picture) {
      m_removeList << picture;
      m_itemHash.remove(index);
      m_itemVector.remove(count);
    }
  }
  updateScene();

  qDebug("rows removed");
}

void PhotoView::slotModelDataChanged(const QModelIndex &start, const QModelIndex &end)
{
  if (start.model() != end.model())
    return;

  if (start.isValid() && end.isValid()) {
    int beginRow = start.row();
    int endRow = end.row();

    for (int count = beginRow; count <= endRow; count++) {
      QModelIndex picture = start.sibling(count, 0);
      if (!picture.isValid())
        continue;
      PhotoItem *item;
      int scrollbarPosition = verticalScrollBar()->value();
      if ((item = itemForIndex(picture))) {
        item->setText(picture.data(GCore::ImageModel::ImageNameRole).toString(), picture.data(GCore::ImageModel::ImageDescriptionRole).toString());
        item->setPixmap(picture.data(GCore::ImageModel::ImageThumbnailRole).value<QIcon>().pixmap(128, 128));
        item->setToolTip(picture.data(Qt::ToolTipRole).toString());
      }
      verticalScrollBar()->setValue(scrollbarPosition);
    }
  }

  qDebug("data changed");
}

int PhotoView::rearrangeItems(bool update)
{
  bool change = false;

  int rows = 1;
  int count = 0;

  qreal y = m_zero.y() + m_spacing;
  qreal x = m_zero.x() + m_spacing;

  if (!m_zero.x()) {
    m_zero = mapToScene(0, 0);
  }

  if (m_editMode && m_currentEdited) {
    x = - (10000) * m_itemVector.indexOf(m_currentEdited);
  }

  int itemsPerRow = width() / (PhotoItem::ItemWidth + m_spacing);

  QList<PhotoItem*> items = m_itemVector.toList();
  QList<PhotoItem*>::iterator end = items.end();
  for (QList<PhotoItem*>::iterator i = items.begin(); i != end; i++) {

    if (m_removeList.contains(*i))
      continue;

    PhotoItem *item = *i;

    if (item->getText() == tr("Unavailable")) {
      item->setText(indexForItem(item).data(GCore::ImageModel::ImageNameRole).toString(), indexForItem(item).data(GCore::ImageModel::ImageDescriptionRole).toString());
      m_timerId = startTimer(500);
    }

    if (!item->group()) {
      if (item->pos() != QPointF(x, y))
        change = true;

      if (m_currentEdited == item)
        item->fullSizePixmap();

      item->setPos(x, y);

      if (m_editMode)
        x += 10000;
      else
        x += PhotoItem::ItemWidth + m_spacing;

      if (++count >= itemsPerRow && !m_editMode) {
        x = m_zero.x() + m_spacing;
        y += PhotoItem::ItemHeight + m_spacing;
        count = 0;
        rows++;
      }
    }
  }

  if (m_editMode)
    updateScrollBars();

  if (update)
    QGraphicsView::update();

  foreach(PhotoItem *picture, m_removeList) {
    picture->deleteItself();
    if (!scene()->items().contains(picture)) {
      //m_itemVector.remove(m_itemVector.indexOf(picture));
      m_removeList.removeAt(m_removeList.lastIndexOf(picture));
      if (picture)
        delete picture;
    }
  }

  return rows;
}

void PhotoView::resizeEvent(QResizeEvent *event)
{
  Q_UNUSED(event)

  m_loading->setPos(mapToScene((width() / 2) - (m_loading->pixmap().width() / 2), (height() / 2) - (m_loading->pixmap().height() / 2)));

  updateScene();
}

void PhotoView::mousePressEvent(QMouseEvent *event)
{
  // We store our currently selected items if the user uses control modifier (to expand our selection, not replace)
  if (event->modifiers() == Qt::ControlModifier)
    m_oldSelectedItems = scene()->selectedItems();

  // We store the position of this event and create the rubberband
  if (dragMode() == QGraphicsView::NoDrag) {
    m_rubberStartPos = event->pos();
    m_rubberScrollValue = verticalScrollBar()->value();
    m_rubberBand = new QRubberBand(QRubberBand::Rectangle, viewport());
    //m_rubberBand->show();
  }

  // If the event does nothing with zooming operation, we handle it in non-edit fashion
  switch (event->button()) {
    case (Qt::RightButton) : {
        // Right button is for context menu only unless nothing is selected
        if (scene()->selectedItems().isEmpty()) {
          QPainterPath selectionArea(mapToScene(event->pos()));
          selectionArea.lineTo(mapToScene(event->pos()).x() + 1, mapToScene(event->pos()).y() + 1);
          scene()->setSelectionArea(selectionArea);
        }
        break;
      }
    case (Qt::MidButton) : {
        // Middle button intiates double click, and that's all
        QGraphicsView::mouseDoubleClickEvent(event);
        break;
      }
    default : {
      // All other buttons are not defined... Yet!
      QGraphicsView::mousePressEvent(event);
      break;
    }
  }
}

void PhotoView::mouseMoveEvent(QMouseEvent *event)
{
  // Now we show the rubber band
  if (dragMode() == QGraphicsView::NoDrag) {
    if (m_rubberBand) {
      // Define the selection rectangle
      QRect selectionRect = QRect(m_rubberStartPos, event->pos());

      // Rubberband should expand with scroll
      selectionRect.setTop(selectionRect.top() - (verticalScrollBar()->value() - m_rubberScrollValue));
      selectionRect = selectionRect.normalized();

      // Sets the selection area
      QPainterPath selectionArea;
      selectionArea.addPolygon(mapToScene(selectionRect));
      scene()->setSelectionArea(selectionArea);

      // Now we reselect our previous selection
      QList<QGraphicsItem*>::const_iterator end = m_oldSelectedItems.constEnd();
      for (QList<QGraphicsItem*>::const_iterator count = m_oldSelectedItems.constBegin(); count != end; count++)
        (*count)->setSelected(true);

      // Set rubberbands geometry and show it
      m_rubberBand->setGeometry(selectionRect);
      m_rubberBand->show();

      return;
    }
  }

  // If it's not our call, we pass it to QGraphicsView
  QGraphicsView::mouseMoveEvent(event);

}

void PhotoView::mouseReleaseEvent(QMouseEvent *event)
{
  // Now we hide and delete the rubber band
  if (dragMode() == QGraphicsView::NoDrag)
    if (m_rubberBand) {
      m_rubberBand->hide();
      delete m_rubberBand;
      m_rubberBand = 0;
    }

  // Our selection has ended. We can now empty our list
  m_oldSelectedItems.clear();

  // If it's not our call, we pass it to QGraphicsView
  QGraphicsView::mouseReleaseEvent(event);
}

void PhotoView::contextMenuEvent(QContextMenuEvent *event)
{
  if (!m_editMode)
    GCore::Data::self()->getPhotoContextMenu()->exec(event->globalPos());
  else
    GCore::Data::self()->getPhotoEditingContextMenu()->exec(event->globalPos());
}

void PhotoView::dragEnterEvent(QDragEnterEvent *event)
{
  if (m_rootIndex.data(GCore::ImageModel::ImageTypeRole).toInt() == GCore::ImageItem::Gallery)
    event->acceptProposedAction();
}

void PhotoView::dragMoveEvent(QDragMoveEvent *event)
{
  if (m_rootIndex.data(GCore::ImageModel::ImageTypeRole).toInt() == GCore::ImageItem::Gallery)
    event->acceptProposedAction();
}

void PhotoView::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

void PhotoView::dropEvent(QDropEvent *event)
{
  if (m_rootIndex.data(GCore::ImageModel::ImageTypeRole).toInt() != GCore::ImageItem::Gallery)
    return;

  const QMimeData *mimeData = event->mimeData();

  QStringList imagePaths;
  QList<QUrl> imageUrls = mimeData->urls();
  QList<QUrl>::const_iterator end = imageUrls.constEnd();

  for (QList<QUrl>::const_iterator count = imageUrls.constBegin(); count != end; count++) {
    QString path = (*count).toLocalFile();
    if (!path.isEmpty())
      imagePaths << path;
  }

  QString image = imagePaths.first();
  image.remove(QRegExp("^.+/"));
  QString path = imagePaths.first();
  path.remove(image);

  QStringList pictures = imagePaths;
  pictures.replaceInStrings(path, QString());

  qRegisterMetaType<QImage>("QImage");
  connect(GCore::Data::self()->getImageModel()->addImages(m_rootIndex, path, pictures), SIGNAL(signalProgress(int, int, const QString&, const QImage&)), GCore::Data::self()->getMainWindow(), SLOT(slotStatusProgress(int, int, const QString&, const QImage&)));

  event->acceptProposedAction();
}

void PhotoView::keyPressEvent(QKeyEvent *event)
{
  if (!scene()->focusItem() && !m_itemHash.isEmpty())
    scene()->setFocusItem(static_cast<QGraphicsItem*>(m_itemVector.at(0)));

  PhotoItem *focusedItem = static_cast<PhotoItem*>(scene()->focusItem());

  int itemsPerRow = width() / (PhotoItem::ItemWidth + m_spacing);

  switch (event->key()) {
    case(Qt::Key_Left): {
        int index = m_itemVector.indexOf(focusedItem);
        if ((index - 1) >= 0) {
          if (event->modifiers() != Qt::ShiftModifier)
            scene()->clearSelection();
          else if (m_itemVector.at(index - 1)->isSelected())
            m_itemVector.at(index)->setSelected(false);
          m_itemVector.at(index - 1)->setSelected(true);
          scene()->setFocusItem(m_itemVector.at(index - 1));
          centerOn(scene()->focusItem());
        }
        break;
      }
    case(Qt::Key_Right): {
        int index = m_itemVector.indexOf(focusedItem);
        if ((index + 1) < m_itemVector.size()) {
          if (event->modifiers() != Qt::ShiftModifier)
            scene()->clearSelection();
          else if (m_itemVector.at(index + 1)->isSelected())
            m_itemVector.at(index)->setSelected(false);
          m_itemVector.at(index + 1)->setSelected(true);
          scene()->setFocusItem(m_itemVector.at(index + 1));
          centerOn(scene()->focusItem());
        }
        break;
      }
    case(Qt::Key_Up): {
        int index = m_itemVector.indexOf(focusedItem);
        if ((index - itemsPerRow) >= 0) {
          if (event->modifiers() != Qt::ShiftModifier)
            scene()->clearSelection();
          else if (m_itemVector.at(index - itemsPerRow)->isSelected())
            m_itemVector.at(index)->setSelected(false);
          m_itemVector.at(index - itemsPerRow)->setSelected(true);
          scene()->setFocusItem(m_itemVector.at(index - itemsPerRow));
          centerOn(scene()->focusItem());
        }
        break;
      }
    case(Qt::Key_Down): {
        int index = m_itemVector.indexOf(focusedItem);
        if ((index + itemsPerRow) < m_itemVector.size()) {
          if (event->modifiers() != Qt::ShiftModifier)
            scene()->clearSelection();
          else if (m_itemVector.at(index + itemsPerRow)->isSelected())
            m_itemVector.at(index)->setSelected(false);
          m_itemVector.at(index + itemsPerRow)->setSelected(true);
          scene()->setFocusItem(m_itemVector.at(index + itemsPerRow));
          centerOn(scene()->focusItem());
        }
        break;
      }
    default: {
      QGraphicsView::keyPressEvent(event);
    }
  }
}

void PhotoView::timerEvent(QTimerEvent *event)
{
  Q_UNUSED(event);

  if (m_timerId) {
    killTimer(m_timerId);
    m_timerId = 0;
  }

  rearrangeItems();
}

void PhotoView::slotSceneChanged()
{
  emit signalSelected(!scene()->selectedItems().isEmpty());
  emit signalOneSelected(scene()->selectedItems().count() == 1);
}

void PhotoView::updateScene()
{
  if (!m_editMode) {
    int rows = (m_itemVector.count() / (width() / (PhotoItem::ItemWidth + m_spacing))) + 1;

    // Properly resize the scene
    int scrollbarWidth = verticalScrollBar()->width() * 2;
    qreal sceneWidth = width() - scrollbarWidth;

    int scrollbarHeight = horizontalScrollBar()->height() * 2;
    qreal sceneHeight = rows * (PhotoItem::ItemHeight + m_spacing);

    if (sceneHeight < height())
      sceneHeight = height() - scrollbarHeight;

    setSceneRect(0, 0, sceneWidth, sceneHeight);
  }

  // Let the animation to do it's job
  if (!m_timerId)
    m_timerId = startTimer(1000 / 25);
}

void PhotoView::checkNavigationEdges()
{
  int index = m_itemVector.indexOf(m_currentEdited);

  if (m_itemVector.count() == 1) {
    GCore::Data::self()->getPhotoControl()->getBackButton()->setEnabled(false);
    GCore::Data::self()->getPhotoControl()->getNextButton()->setEnabled(false);
  } else if (m_itemVector.count() <= index + 1) {
    GCore::Data::self()->getPhotoControl()->getBackButton()->setEnabled(true);
    GCore::Data::self()->getPhotoControl()->getNextButton()->setEnabled(false);
  } else if (index == 0) {
    GCore::Data::self()->getPhotoControl()->getBackButton()->setEnabled(false);
    GCore::Data::self()->getPhotoControl()->getNextButton()->setEnabled(true);
  } else {
    GCore::Data::self()->getPhotoControl()->getBackButton()->setEnabled(true);
    GCore::Data::self()->getPhotoControl()->getNextButton()->setEnabled(true);
  }
}

void PhotoView::updateScrollBars()
{

  if (m_currentEdited->getScaledSize().x() > width() && m_currentEdited->getScaledSize().y() > height())
    setSceneRect(-m_spacing, -m_spacing, m_currentEdited->getScaledSize().x(), m_currentEdited->getScaledSize().y());
  else if (m_currentEdited->getScaledSize().x() > width())
    setSceneRect(-m_spacing, -m_spacing, m_currentEdited->getScaledSize().x(), height());
  else if (m_currentEdited->getScaledSize().y() > height())
    setSceneRect(-m_spacing, -m_spacing, width(), m_currentEdited->getScaledSize().y());
  else
    setSceneRect(-m_spacing, -m_spacing, width(), height());

  // To stay in center of zoom
  verticalScrollBar()->setSliderPosition(((verticalScrollBar()->maximum() - m_oldVerticalScrollMaximum) / 2) + verticalScrollBar()->sliderPosition());
  horizontalScrollBar()->setSliderPosition(((horizontalScrollBar()->maximum() - m_oldHorizontalScrollMaximum) / 2) + horizontalScrollBar()->sliderPosition());

  m_oldHorizontalScrollMaximum = horizontalScrollBar()->maximum();
  m_oldVerticalScrollMaximum = verticalScrollBar()->maximum();
}

QModelIndex PhotoView::rootIndex()
{
  return m_rootIndex;
}

void PhotoView::slotRemove()
{
  QList<QGraphicsItem*> selectedPictures = scene()->selectedItems();
  QList<QGraphicsItem*>::const_iterator end = selectedPictures.constEnd();
  QModelIndexList selectedIndexes;

  if (selectedPictures.isEmpty())
    return;

  if (QMessageBox::question(0, tr("Confirm remove"), tr("Do you want to remove %1 pictures?").arg(selectedPictures.count()), tr("Delete"), tr("Keep"), QString(), 1 , 1) == 1)
    return;

  scene()->clearFocus();
  scene()->clearSelection();

  for (QList<QGraphicsItem*>::const_iterator count = selectedPictures.constBegin(); count != end; count++) {
    PhotoItem *picture = static_cast<PhotoItem*>(*count);

    if (!picture->group()) {
      QModelIndex index = indexForItem(picture);
      if (index.isValid()) {
        selectedIndexes << index;

        static_cast<GCore::ImageModel*>(m_model)->removeImages(selectedIndexes);
        selectedIndexes.clear();
        //picture->deleteItself();
      }
    }
  }
}

void PhotoView::slotNextPhoto()
{
  // !m_editMode || !m_currentEdited
  if (!(m_editMode && m_currentEdited))
    return;

  int newIndex = m_itemVector.indexOf(m_currentEdited) + 1;

  if (m_itemVector.count() <= newIndex) {
    GCore::Data::self()->getPhotoControl()->getNextButton()->setEnabled(false);
    return;
  }

  m_currentEdited = m_itemVector.at(newIndex);

  checkNavigationEdges();

  updateScene();
}

void PhotoView::slotPreviousPhoto()
{
  // !m_editMode || !m_currentEdited
  if (!(m_editMode && m_currentEdited))
    return;

  int newIndex = m_itemVector.indexOf(m_currentEdited) - 1;

  if (newIndex < 0) {
    GCore::Data::self()->getPhotoControl()->getBackButton()->setEnabled(false);
    return;
  }

  m_currentEdited = m_itemVector.at(newIndex);

  checkNavigationEdges();

  updateScene();
}

void PhotoView::slotZoomInPhoto()
{
  if (m_currentEdited) {
    m_currentEdited->zoomIn();
    updateScrollBars();
  }
}

void PhotoView::slotZoomOutPhoto()
{
  if (m_currentEdited) {
    m_currentEdited->zoomOut();
    updateScrollBars();
  }
}

void PhotoView::slotZoomActualPhoto()
{
  if (m_currentEdited) {
    m_currentEdited->zoomActual();
    updateScrollBars();
  }
}

void PhotoView::slotZoomScreenPhoto()
{
  if (m_currentEdited) {
    m_currentEdited->zoomScreen();
    updateScrollBars();
  }
}

void PhotoView::slotZoomInputPhoto()
{
  // We ask for the new zoom level
  float newZoom = InputZoomDialog::getZoomLevel(m_currentEdited->getScaleMultiplier() * 100, this);
  if (newZoom == m_currentEdited->getScaleMultiplier() * 100)
    return;

  m_currentEdited->setZoom(newZoom);
  updateScrollBars();
}

void PhotoView::slotEditPhoto()
{
  QString photo = m_model->data(indexForItem(m_currentEdited), GCore::ImageModel::ImageFilepathRole).toString();

  QStringList arguments;
  arguments << QDir::toNativeSeparators(photo);

  if (!QProcess::startDetached(GCore::Data::self()->getPhotoEditor(), arguments))
    QMessageBox::critical(this, tr("External edit failed"), tr("The external editor, which was specified in configuration, couldn't be run."));
}

void PhotoView::slotExitEdit()
{
  if (m_currentEdited) {
    setEditMode(false, m_currentEdited);
  }
}

void PhotoView::slotConnectNavButtons()
{
  connect(GCore::Data::self()->getPhotoControl()->getNextButton(), SIGNAL(clicked()), this, SLOT(slotNextPhoto()));
  connect(GCore::Data::self()->getPhotoControl()->getBackButton(), SIGNAL(clicked()), this, SLOT(slotPreviousPhoto()));
  connect(GCore::Data::self()->getPhotoControl()->getZoomInButton(), SIGNAL(clicked()), this, SLOT(slotZoomInPhoto()));
  connect(GCore::Data::self()->getPhotoControl()->getZoomOutButton(), SIGNAL(clicked()), this, SLOT(slotZoomOutPhoto()));
  connect(GCore::Data::self()->getPhotoControl()->getActualSizeButton(), SIGNAL(clicked()), this, SLOT(slotZoomActualPhoto()));
  connect(GCore::Data::self()->getPhotoControl()->getZoomScreenButton(), SIGNAL(clicked()), this, SLOT(slotZoomScreenPhoto()));
  connect(GCore::Data::self()->getPhotoControl()->getZoomInputButton(), SIGNAL(clicked()), this, SLOT(slotZoomInputPhoto()));
  connect(GCore::Data::self()->getPhotoControl()->getEditButton(), SIGNAL(clicked()), this, SLOT(slotEditPhoto()));
  connect(GCore::Data::self()->getPhotoControl()->getExitButton(), SIGNAL(clicked()), this, SLOT(slotExitEdit()));
}

QModelIndex PhotoView::indexForItem(PhotoItem *item)
{
  return m_itemHash.key(item);
}

QAbstractItemModel *PhotoView::model()
{
  return m_model;
}

void PhotoView::setEditMode(bool editMode, GWidgets::PhotoItem *selectedItem)
{
  if (editMode && !selectedItem) {
    qDebug("Illeagal entry into edit mode, without selected item!");
    return;
  }

  m_currentEdited = selectedItem;

  m_editMode = editMode;

  // Notify all components of our intentions
  emit signalEditMode(editMode);

  if (editMode) {
    checkNavigationEdges();
    setDragMode(QGraphicsView::ScrollHandDrag);
    viewport()->setCursor(Qt::OpenHandCursor);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  } else {
    // We have our own Rubber band defined!
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_currentEdited = 0;
    viewport()->setCursor(Qt::ArrowCursor);

    verticalScrollBar()->setValue(0);
    horizontalScrollBar()->setValue(0);
  }

  updateScene();
}

void PhotoView::showLoading(bool show)
{
  // Set loading image
  m_loading->setVisible(show);
}

PhotoItem *PhotoView::getSelectedPhoto()
{
  if (scene()->selectedItems().count() == 1) {
    return static_cast<PhotoItem*>(scene()->selectedItems().at(0));
  } else {
    qDebug("Wrong entry to editing mode! None or too many items selected!");
    return 0;
  }
}

PhotoView::~PhotoView()
{
  delete m_loading;
}

}
