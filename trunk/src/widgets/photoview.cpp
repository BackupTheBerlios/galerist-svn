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
#include "photoview.h"

#include <QtCore/QModelIndex>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtCore/QDir>
#include <QtCore/QUrl>

#include <QtGui/QApplication>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItem>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QScrollBar>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QGraphicsItem>
#include <QtGui/QRubberBand>
#include <QtGui/QMouseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QSortFilterProxyModel>

#include <QtOpenGL/QGLWidget>

#include <math.h>

#include "core/data.h"
#include "core/imagemodel.h"
#include "core/imageitem.h"

#include "core/jobs/transformationjob.h"

#include "widgets/inputzoomdialog.h"
#include "widgets/searchbar.h"
#include "widgets/photocontrol.h"

#include "widgets/photowidgets/photoitem.h"
#include "widgets/photowidgets/photoloading.h"

#include "dialogs/newgallerywizard.h"

using namespace GCore;

namespace GWidgets
{

PhotoView::PhotoView(QWidget *parent)
    : QGraphicsView(parent),
    m_timerId(0),
    m_editMode(false),
    m_currentEdited(0),
    m_model(0),
    m_rubberBand(0),
    m_needRubberBand(false)
{
  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setBackgroundBrush(Qt::white);
  setScene(scene);

  // Set some graphics view options
  setSpacing(10);
  setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
  setInteractive(true);

  if (Data::self()->value(Data::OpenGL).toBool() && QGLFormat::hasOpenGL())
    setViewport(new QGLWidget(QGLFormat(QGL::DirectRendering | QGL::SampleBuffers | QGL::DoubleBuffer)));

  // Loading item
  m_loading = new GPhotoWidgets::PhotoLoading(this->scene());
  m_loading->setZValue(3.5);
  m_loading->hide();

  m_oldHorizontalScrollMaximum = horizontalScrollBar()->maximum();
  m_oldVerticalScrollMaximum = verticalScrollBar()->maximum();

  // Connect signals
  connect(scene, SIGNAL(changed(const QList<QRectF>&)), this, SLOT(slotSceneChanged()));

  setDragMode(QGraphicsView::NoDrag);

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
    disconnect(m_model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(slotModelRowsRemoved(const QModelIndex&, int, int)));
    disconnect(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slotModelDataChanged(const QModelIndex&, const QModelIndex&)));
    disconnect(m_model, SIGNAL(modelReset()), this, SLOT(slotModelReset()));
  }

  m_model = model;

  // Connect the new model
  connect(m_model, SIGNAL(layoutChanged()), this, SLOT(slotModelLayoutChanged()));
  connect(m_model, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(slotModelRowsInserted(const QModelIndex&, int, int)));
  connect(m_model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), this, SLOT(slotModelRowsRemoved(const QModelIndex&, int, int)));
  connect(m_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slotModelDataChanged(const QModelIndex&, const QModelIndex&)));
  connect(m_model, SIGNAL(modelReset()), this, SLOT(slotModelReset()));
}

void PhotoView::setRootIndex(const QModelIndex &index)
{
  QModelIndex mappedIndex = static_cast<QSortFilterProxyModel*>(Data::self()->value(Data::ModelProxy).value<QObject*>())->mapToSource(index);

  if (!mappedIndex.isValid() || mappedIndex.model() != m_model || mappedIndex == m_rootIndex)
    return;

  setFocus();

  m_rootIndex = mappedIndex;
  //setEditMode(false);
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
  GPhotoWidgets::PhotoItem *selectedPhoto = getSelectedPhoto();
  if (selectedPhoto)
    selectedPhoto->rename();
}

void PhotoView::slotDescribe()
{
  GPhotoWidgets::PhotoItem *selectedPhoto = getSelectedPhoto();
  if (selectedPhoto)
    selectedPhoto->describe();
}

void PhotoView::slotSelectAll()
{
  QHash<QModelIndex, GPhotoWidgets::PhotoItem*>::const_iterator end = m_itemHash.constEnd();
  for (QHash<QModelIndex, GPhotoWidgets::PhotoItem*>::const_iterator count = m_itemHash.constBegin(); count != end; count++)
    count.value()->setSelected(true);
}

void PhotoView::slotDeselectAll()
{
  QHash<QModelIndex, GPhotoWidgets::PhotoItem*>::const_iterator end = m_itemHash.constEnd();
  for (QHash<QModelIndex, GPhotoWidgets::PhotoItem*>::const_iterator count = m_itemHash.constBegin(); count != end; count++)
    count.value()->setSelected(false);
}

void PhotoView::slotInvertSelection()
{
  QHash<QModelIndex, GPhotoWidgets::PhotoItem*>::const_iterator end = m_itemHash.constEnd();
  for (QHash<QModelIndex, GPhotoWidgets::PhotoItem*>::const_iterator count = m_itemHash.constBegin(); count != end; count++)
    count.value()->setSelected(!(*count)->isSelected());
}

void PhotoView::slotRetakeFocus()
{
  setFocus();
}

void PhotoView::setFilter(const QString &filter)
{
  m_filter = filter;
  rearrangeItems();
}

void PhotoView::initiateOperation(int operation)
{
  if (!m_editMode)
    return;

  switch (operation) {
    case (GJobs::TransformationJob::Crop) : {
      m_needRubberBand = true;

      connect(this, SIGNAL(areaSelected(const QRect&)), this, SLOT(cropSelection(const QRect&)));

      viewport()->setCursor(Qt::CrossCursor);

      break;
    }
    default : {
      break;
    }
  }
}

void PhotoView::cancelOperation(int operation)
{
  if (!m_editMode)
    return;

  switch (operation) {
    case (GJobs::TransformationJob::Crop) : {
      // We disable rubberband
      m_needRubberBand = false;

      disconnect(this, SIGNAL(areaSelected(const QRect&)), this, SLOT(cropSelection(const QRect&)));

      viewport()->setCursor(Qt::OpenHandCursor);
      break;
    }
    case (GJobs::TransformationJob::Blur) : {
      m_currentEdited->cancelTransformations();
      break;
    }
    case (GJobs::TransformationJob::Sharpen) : {
      m_currentEdited->cancelTransformations();
      break;
    }
    case (GJobs::TransformationJob::Resize) : {
      m_currentEdited->cancelTransformations();
      break;
    }
    default : {
      break;
    }
  }

  m_currentEdited->closeTransformations();
}

void PhotoView::saveOperation(int operation, const QMap<int, QVariant> &params)
{
  Q_UNUSED(params)
  if (!m_editMode)
    return;

  switch (operation) {
    case (GJobs::TransformationJob::Crop) : {
      // We enable rubberband
      m_needRubberBand = false;

      disconnect(this, SIGNAL(areaSelected(const QRect&)), this, SLOT(cropSelection(const QRect&)));

      viewport()->setCursor(Qt::OpenHandCursor);

      m_currentEdited->saveCrop();
      break;
    }
    case (GJobs::TransformationJob::Blur) : {
      m_currentEdited->saveBlur();
      break;
    }
    case (GJobs::TransformationJob::Sharpen) : {
      m_currentEdited->saveSharpen();
      break;
    }
    case (GJobs::TransformationJob::Resize) : {
      m_currentEdited->saveResize();
      break;
    }
    default : {
      qDebug("Operation not supported.");
      break;
    }
  }
}

void PhotoView::previewOperation(int operation, const QMap<int, QVariant> &params)
{
  if (!m_editMode)
    return;

  switch (operation) {
    case (GJobs::TransformationJob::Blur) : {
      m_currentEdited->blurPreview(params.value(GJobs::TransformationJob::NumberFilter).toInt());
      break;
    }
    case (GJobs::TransformationJob::Sharpen) : {
      m_currentEdited->sharpenPreview(params.value(GJobs::TransformationJob::NumberFilter).toInt());
      break;
    }
    case (GJobs::TransformationJob::Resize) : {
      m_currentEdited->resizePreview(params.value(GJobs::TransformationJob::Size).toSize());
      break;
    }
    default : {
      qDebug("Operation not supported.");
      break;
    }
  }
}

void PhotoView::rotateSelectedImageCW()
{
  if (!m_editMode)
    return;

  m_currentEdited->rotateCW();
}

void PhotoView::rotateSelectedImageCCW()
{
  if (!m_editMode)
    return;

  m_currentEdited->rotateCCW();
}

void PhotoView::slotNextPhoto()
{
  // !m_editMode || !m_currentEdited
  if (!(m_editMode && m_currentEdited))
    return;

  int newIndex = m_itemVector.indexOf(m_currentEdited) + 1;

  if (m_itemVector.count() <= newIndex)
    return;

  emit photoEditSelectionChanged(newIndex, m_itemVector.count());

  m_currentEdited = m_itemVector.at(newIndex);

  emit imageSwitched(m_currentEdited->getIndex().data(ImageModel::ImagePictureRole).value<QImage>().size());

  updateScene();
}

void PhotoView::slotPreviousPhoto()
{
  // !m_editMode || !m_currentEdited
  if (!(m_editMode && m_currentEdited))
    return;

  int newIndex = m_itemVector.indexOf(m_currentEdited) - 1;

  if (newIndex < 0)
    return;

  emit photoEditSelectionChanged(newIndex, m_itemVector.count());

  m_currentEdited = m_itemVector.at(newIndex);

  emit imageSwitched(m_currentEdited->getIndex().data(ImageModel::ImagePictureRole).value<QImage>().size());

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
  if (newZoom == -1)
    return;

  m_currentEdited->setZoom(newZoom);
  updateScrollBars();
}

void PhotoView::slotEditPhoto()
{
  QString photo = m_currentEdited->getIndex().data(ImageModel::ImageFilepathRole).toString();

  QStringList arguments;
  arguments << QDir::toNativeSeparators(photo);

  if (!QProcess::startDetached(Data::self()->value(Data::EditorPath).toString(), arguments))
    QMessageBox::critical(this, tr("External edit failed"), tr("The external editor, which was specified in configuration, couldn't be run."));
}

void PhotoView::slotExitEdit()
{
  if (m_currentEdited) {
    setEditMode(false, m_currentEdited);
  }
}

GPhotoWidgets::PhotoItem *PhotoView::itemForIndex(const QModelIndex &index)
{
  // !index.isValid() || m_itemVector.count() < index.row()
  if (!index.isValid())
    return 0;

  return m_itemHash.value(index);
}

void PhotoView::readModel()
{
  // Clear selection and focus. If we don't do this, it crashes
  scene()->clearFocus();
  scene()->clearSelection();

  // Remove all current items
  foreach(GPhotoWidgets::PhotoItem *item, m_itemHash) {
    if (!item->deleteItself())
      m_removeList << item;
  }

  m_itemHash.clear();
  m_itemVector.clear();

  int rowCount = m_model->rowCount(m_rootIndex);

  for (int i = 0; i < rowCount; i++) {
    QModelIndex row = m_model->index(i, 0, m_rootIndex);

    // We don't want to show directories(galleries), don't we?
    if (row.data(ImageModel::ImageTypeRole).toInt() == ImageItem::Gallery)
      continue;

    GPhotoWidgets::PhotoItem *item = new GPhotoWidgets::PhotoItem(this, row);

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
    if (row.data(ImageModel::ImageTypeRole) != ImageItem::Image)
      continue;

    GPhotoWidgets::PhotoItem *item = new GPhotoWidgets::PhotoItem(this, row);

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
    GPhotoWidgets::PhotoItem *picture = m_itemVector.value(count);
    QModelIndex index = picture->getIndex();
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
      GPhotoWidgets::PhotoItem *item;
      int scrollbarPosition = verticalScrollBar()->value();
      if ((item = itemForIndex(picture))) {
        item->setText(picture.data(ImageModel::ImageNameRole).toString(), picture.data(ImageModel::ImageDescriptionRole).toString());
        item->setPixmap(picture.data(ImageModel::ImageThumbnailRole).value<QIcon>().pixmap(128, 128));
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

  qreal y = m_spacing;
  qreal x = m_spacing;

  if (m_editMode && m_currentEdited)
    x = - (5000) * m_itemVector.indexOf(m_currentEdited);

  //int itemsPerRow = width() / (GPhotoWidgets::PhotoItem::ItemWidth + (m_spacing * 2));
  int itemsPerRow = viewport()->width() / (GPhotoWidgets::PhotoItem::ItemWidth + (m_spacing));

  QList<GPhotoWidgets::PhotoItem*> items = m_itemVector.toList();
  QList<GPhotoWidgets::PhotoItem*>::iterator end = items.end();
  for (QList<GPhotoWidgets::PhotoItem*>::iterator i = items.begin(); i != end; i++) {

    if (m_removeList.contains(*i))
      continue;

    GPhotoWidgets::PhotoItem *item = *i;

    if (!item->getText().contains(m_filter, Qt::CaseInsensitive)) {
      item->setPos(1000, -500);
      continue;
    }

    if (!item->group()) {
      if (item->pos() != QPointF(x, y))
        change = true;

      item->setPos(x, y);

      if (m_currentEdited == item)
        item->fullSizePixmap();

      if (m_editMode)
        x += 5000;
      else
        x += GPhotoWidgets::PhotoItem::ItemWidth + m_spacing;

      if (++count >= itemsPerRow && !m_editMode) {
        x = m_zero.x() + m_spacing;
        y += GPhotoWidgets::PhotoItem::ItemHeight + m_spacing;
        count = 0;
        rows++;
      }
    }
  }

  if (m_editMode)
    updateScrollBars();

  if (update)
    QGraphicsView::update();

  foreach(GPhotoWidgets::PhotoItem *picture, m_removeList) {
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

  updateScene();
}

void PhotoView::mousePressEvent(QMouseEvent *event)
{
  // We store our currently selected items if the user uses control modifier (to expand our selection, not replace)
  if (event->modifiers() == Qt::ControlModifier)
    m_oldSelectedItems = scene()->selectedItems();

  // We store the position of this event and create the rubberband
  if (dragMode() == QGraphicsView::NoDrag || m_needRubberBand) {
    m_rubberStartPos = event->pos();
    m_rubberScrollValue = verticalScrollBar()->value();
    m_rubberBand = new QRubberBand(QRubberBand::Rectangle, viewport());

    if (m_needRubberBand)
      return;
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
  if ((dragMode() == QGraphicsView::NoDrag || m_needRubberBand) && event->buttons() & Qt::LeftButton) {
    if (m_rubberBand) {
      // Define the selection rectangle
      QRect selectionRect = QRect(m_rubberStartPos, event->pos());

      // Rubberband should expand with scroll
      selectionRect.setTop(selectionRect.top() - (verticalScrollBar()->value() - m_rubberScrollValue));
      selectionRect = selectionRect.normalized();

      if (!m_needRubberBand) {
        // We change selection of items only at nonedit mode

        // Sets the selection area
        QPainterPath selectionArea;
        selectionArea.addPolygon(mapToScene(selectionRect));
        scene()->setSelectionArea(selectionArea);

        // Now we reselect our previous selection
        QList<QGraphicsItem*>::const_iterator end = m_oldSelectedItems.constEnd();
        for (QList<QGraphicsItem*>::const_iterator count = m_oldSelectedItems.constBegin(); count != end; count++)
          (*count)->setSelected(true);

      }

      // Set rubberbands geometry and show it
      m_rubberBand->setGeometry(selectionRect);
      m_rubberBand->show();

      return;
    }
  } else if (m_rubberBand) {
    // Now we hide and delete the rubber band
    m_rubberBand->hide();
    delete m_rubberBand;
    m_rubberBand = 0;

    // Our selection has ended. We can now empty our list
    m_oldSelectedItems.clear();
  }

  // If it's not our call, we pass it to QGraphicsView
  QGraphicsView::mouseMoveEvent(event);

}

void PhotoView::mouseReleaseEvent(QMouseEvent *event)
{
  if (m_rubberBand && m_needRubberBand) {
    emit areaSelected(m_rubberBand->geometry());
    return;
  }

  // If it's not our call, we pass it to QGraphicsView
  QGraphicsView::mouseReleaseEvent(event);

  if (m_editMode)
    viewport()->setCursor(Qt::OpenHandCursor);
}

void PhotoView::contextMenuEvent(QContextMenuEvent *event)
{
  if (!m_editMode)
    static_cast<QMenu*>(Data::self()->value(Data::PhotoContextMenu).value<QWidget*>())->exec(event->globalPos());
  else
    static_cast<QMenu*>(Data::self()->value(Data::PhotoContextMenu).value<QWidget*>())->exec(event->globalPos());
}

void PhotoView::dragEnterEvent(QDragEnterEvent *event)
{
  QList<QUrl> imageUrls = event->mimeData()->urls();
  QList<QUrl>::const_iterator end = imageUrls.constEnd();

  bool haveImages = false;

  for (QList<QUrl>::const_iterator count = imageUrls.constBegin(); count != end; count++) {
    QString path = (*count).toLocalFile();
    if (path.contains(Data::self()->value(Data::SupportedFormats).toRegExp())) {
      haveImages = true;
      break;
    }
  }

  if (haveImages)
    event->acceptProposedAction();
}

void PhotoView::dragMoveEvent(QDragMoveEvent *event)
{
  event->acceptProposedAction();
}

void PhotoView::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

void PhotoView::dropEvent(QDropEvent *event)
{
  const QMimeData *mimeData = event->mimeData();

  QStringList imagePaths;
  QList<QUrl> imageUrls = mimeData->urls();
  QList<QUrl>::const_iterator end = imageUrls.constEnd();

  for (QList<QUrl>::const_iterator count = imageUrls.constBegin(); count != end; count++) {
    QString path = (*count).toLocalFile();
    if (!path.isEmpty() && path.contains(Data::self()->value(Data::SupportedFormats).toRegExp()))
      imagePaths << path;
  }

  QString image = imagePaths.first();
  image.remove(QRegExp("^.+/"));
  QString path = imagePaths.first();
  path.remove(image);

  QStringList pictures = imagePaths;
  pictures.replaceInStrings(path, QString());

  QMenu dropdownMenu(tr("Dropped images"), this);
  QAction *newDestination = dropdownMenu.addAction(QIcon(":/images/new.png"), tr("New gallery"));
  newDestination->setStatusTip(tr("Create a new gallery with the dropped in images."));
  newDestination->setParent(Data::self()->value(Data::MainWindow).value<QWidget*>());
  QAction *existing = dropdownMenu.addAction(QIcon(":/images/add-existing.png"), tr("Existing gallery"));
  existing->setStatusTip(tr("Insert the dropped in images into the selected gallery."));
  existing->setParent(Data::self()->value(Data::MainWindow).value<QWidget*>());

  if (m_rootIndex.data(ImageModel::ImageTypeRole).toInt() != ImageItem::Gallery)
    existing->setEnabled(false);

  QAction *choice = dropdownMenu.exec(mapToGlobal(event->pos()));

  if (choice == newDestination) {
    GDialogs::NewGalleryWizard *wizard = new GDialogs::NewGalleryWizard(path, pictures, this);
    wizard->show();
  } else if (choice == existing) {
    connect(static_cast<ImageModel*>(Data::self()->value(Data::ImageModel).value<QObject*>())->addImages(m_rootIndex, path, pictures), SIGNAL(signalProgress(int, int, const QString&, const QImage&)), Data::self()->value(Data::ImageAddProgress).value<QObject*>(), SLOT(setProgress(int, int, const QString&, const QImage&)));
  }

  event->acceptProposedAction();
}

void PhotoView::keyPressEvent(QKeyEvent *event)
{
  SearchBar *searchBar = static_cast<SearchBar*>(Data::self()->value(Data::SearchBar).value<QWidget*>());
  // If a normal text has been pressed... Then we start a search
  if (!event->text().isEmpty() && event->key() != Qt::Key_Escape && event->key() != Qt::Key_Return && !m_editMode && (event->modifiers() == Qt::NoModifier || event->matches(QKeySequence::Find))) {
    if (event->matches(QKeySequence::Find))
      searchBar->addLetter("");
    else
      searchBar->addLetter(event->text());
    return;
  }

  if (!scene()->focusItem() && !m_itemHash.isEmpty())
    scene()->setFocusItem(static_cast<QGraphicsItem*>(m_itemVector.at(0)));

  GPhotoWidgets::PhotoItem *focusedItem = static_cast<GPhotoWidgets::PhotoItem*>(scene()->focusItem());

  int itemsPerRow = width() / (GPhotoWidgets::PhotoItem::ItemWidth + m_spacing);

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
    case(Qt::Key_Escape): {
      searchBar->hide();
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
    // Properly resize the scene

    int scrollbarWidth = verticalScrollBar()->width();
    qreal sceneWidth = viewport()->width() - scrollbarWidth;

    int rows = ceil(m_itemVector.count() / (sceneWidth / (GPhotoWidgets::PhotoItem::ItemWidth + m_spacing)));

    qreal sceneHeight = rows * (GPhotoWidgets::PhotoItem::ItemHeight + (m_spacing)) + m_spacing;

    if (sceneHeight < viewport()->height())
      sceneHeight = viewport()->height();

    setSceneRect(0, 0, sceneWidth, sceneHeight);
  }

  // Let the animation to do it's job
  if (!m_timerId)
    m_timerId = startTimer(1000 / 25);
}

void PhotoView::updateScrollBars()
{
  setSceneRect(-m_spacing, 0, m_currentEdited->getScaledSize().width() + m_spacing *2, m_currentEdited->getScaledSize().height() + m_spacing*2);

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
    GPhotoWidgets::PhotoItem *picture = static_cast<GPhotoWidgets::PhotoItem*>(*count);

    if (!picture->group()) {
      QModelIndex index = picture->getIndex();
      if (index.isValid()) {
        selectedIndexes << index;

        static_cast<ImageModel*>(m_model)->removeImages(selectedIndexes);
        selectedIndexes.clear();
      }
    }
  }
}

void PhotoView::slotConnectNavButtons()
{
  static_cast<SearchBar*>(Data::self()->value(Data::SearchBar).value<QWidget*>())->setListFilter(this);
}

void PhotoView::cropSelection(const QRect &area)
{
  if (!m_editMode)
    return;

  // Remove the spacing
  QRect temp = area;
  temp.setTop(temp.top() - m_spacing);
  temp.setBottom(temp.bottom() - m_spacing);

  // Crops the image
  m_currentEdited->crop(mapToScene(temp).boundingRect().toRect());
}

QAbstractItemModel *PhotoView::model()
{
  return m_model;
}

void PhotoView::setEditMode(bool editMode, GWidgets::GPhotoWidgets::PhotoItem *selectedItem)
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
    emit imageSwitched(m_currentEdited->getIndex().data(ImageModel::ImagePictureRole).value<QImage>().size());
    emit photoEditSelectionChanged(m_itemVector.indexOf(m_currentEdited), m_itemVector.count());
    setDragMode(QGraphicsView::ScrollHandDrag);
    viewport()->setCursor(Qt::OpenHandCursor);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  } else if (m_currentEdited) {
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

void PhotoView::showLoading(bool show, const QString &text)
{
  // Set loading image
  m_loading->setText(text);

  m_loading->setPos(mapToScene(static_cast<int>((width() / 2) - (m_loading->boundingRect().width() / 2)), static_cast<int>((height() / 2) - (m_loading->boundingRect().height() / 2))));

  m_loading->setVisible(show);

  emit enableControls(!show);
}

GPhotoWidgets::PhotoItem *PhotoView::getSelectedPhoto()
{
  if (scene()->selectedItems().count() == 1) {
    return static_cast<GPhotoWidgets::PhotoItem*>(scene()->selectedItems().at(0));
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
