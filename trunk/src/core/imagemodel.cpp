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
#include "imagemodel.h"

#include <QtGui/QSortFilterProxyModel>

#include "core/metadatamanager.h"
#include "core/exifmanager.h"
#include "core/data.h"
#include "core/jobmanager.h"

#include "core/jobs/readjob.h"

#include <QtDebug>

using namespace GCore::GJobs;

namespace GCore
{

ImageModel::ImageModel(QObject *parent)
    : QAbstractItemModel(parent),
    m_delete(false)
{
  setupModelData();
}

ImageModel::~ImageModel()
{
  delete m_rootItem;
}

QVariant ImageModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  if (!item)
    return QVariant();

  switch (role) {
    case Qt::DisplayRole : {
      return item->name();
    }
    case Qt::ToolTipRole : {
      if (item->type() == ImageItem::Gallery) {
        return item->name();
      } else {
        ExifManager exifData(item->filePath(), Data::self()->imageModel());

        QString date;
        if (!exifData.getCreationDate().isValid())
          date = tr("Unavailable");
        else
          date = exifData.getCreationDate().toString(Qt::SystemLocaleDate);

        QString message = "<u>" + tr("EXIF information:") + "</u><table>";
        message += "<tr><td><b>" + tr("Camera manufacturer:") + "</b> </td><td> " + exifData.getCameraMaker() + "</td></tr>";
        message += "<tr><td><b>" + tr("Camera model:") + "</b> </td><td> " + exifData.getCameraModel() + "</td></tr>";
        message += "<tr><td><b>" + tr("Aperture:") + "</b> </td><td> " + exifData.getAperture() + "</td></tr>";
        message += "<tr><td><b>" + tr("Creation Date:") + "</b> </td><td> " + date + "</td></tr>";
        message += "<tr><td><b>" + tr("Shutter speed:") + "</b> </td><td> " + exifData.getShutterSpeed() + "</td></tr>";
        message += "<tr><td><b>" + tr("Exposure time:") + "</b> </td><td> " + exifData.getExposureTime() + "</td></tr>";
        message += "<tr><td><b>" + tr("Focal length:") + "</b> </td><td> " + exifData.getFocalLength() + "</td></tr>";
        message += "<tr><td><b>" + tr("Flash: ") + "</b> </td><td> " + exifData.getFlash() + "</td></tr></table>";
        return "<b>" + item->name() + ":</b> <i>" + item->description().replace('\n', "<br/>") + "</i>" + "<p>" + message + "</p>";
      }
    }
    case Qt::DecorationRole : {
      if (index.column() == 0)
        return fileIcon(index);
    }
    case ImageModel::ImageDescriptionRole : {
      if (item->type() != ImageItem::Image)
        return QVariant();

      return item->description();
    }
    case ImageModel::ImageTypeRole : {
      return item->type();
    }
    case ImageModel::ImageFilenameRole : {
      return item->fileName();
    }
    case ImageModel::ImageFilepathRole : {
      return item->absoluteFilePath();
    }
    case ImageModel::ImagePathRole : {
      return item->absolutePath();
    }
    case ImageModel::ImageThumbnailPathRole : {
      return item->absoluteThumbPath();
    }
    case Qt::EditRole : {
      return item->name();
    }
    case (ImageModel::ImagePictureRole) : {
      return QImage(item->filePath());
    }
    case (ImageModel::ImageRotateCW) : {
      return item->previewRotate(90);
    }
    case (ImageModel::ImageRotateCCW) : {
      return item->previewRotate(270);
    }
    case (ImageModel::IdRole) : {
      return item->id();
    }
    default:
      return QVariant();
  }
}

bool ImageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid() || index.column() != 0 || (flags(index) & Qt::ItemIsEditable) == 0)
    return false;

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  switch (role) {
    case (Qt::EditRole) : {
      bool output = item->setName(value.toString());
      if (output)
        emit dataChanged(index, index);
      return output;
    }
    case (ImageDescriptionRole) : {
      if (item->type() != ImageItem::Image)
        return false;

      item->setDescription(value.toString());
      emit dataChanged(index, index);
      return true;
    }
    case (ImageRotateCW) : {
      if (item->type() != ImageItem::Image)
        return false;

      item->rotate(value.toInt());
      emit dataChanged(index, index);
      return true;
    }
    default : {
      return false;
    }
  }
}

Qt::ItemFlags ImageModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant ImageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

QModelIndex ImageModel::index(int row, int column, const QModelIndex &parent) const
{
  ImageItem *parentItem;

  if (!parent.isValid())
    parentItem = m_rootItem;
  else
    parentItem = static_cast<ImageItem*>(parent.internalPointer());

  ImageItem *childItem = parentItem->child(row);
  
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex ImageModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  ImageItem *childItem = static_cast<ImageItem*>(index.internalPointer());
  ImageItem *parentItem = childItem->parent();

  if (parentItem == m_rootItem)
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

int ImageModel::rowCount(const QModelIndex &parent) const
{
  ImageItem *parentItem;

  if (!parent.isValid())
    parentItem = m_rootItem;
  else
    parentItem = static_cast<ImageItem*>(parent.internalPointer());

  return parentItem->childCount();
}

int ImageModel::columnCount(const QModelIndex &parent) const
{
  return 1;
}

void ImageModel::setName(const QString &name, const QModelIndex &index)
{
  if (!index.isValid())
    return;

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  item->setName(name);
}

void ImageModel::setDescription(const QString &description, const QModelIndex &index)
{
  if (!index.isValid())
    return;

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  item->setDescription(description);
}

bool ImageModel::exists(const QString &name, const QModelIndex &index)
{
  if (!index.isValid())
    return false;

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  return MetaDataManager::self()->imageExists(name, item->parent()->id());
}

QIcon ImageModel::fileIcon(const QModelIndex &item) const
{
  QIcon icon;

  // We check if this is a gallery or a image
  if (item.data(ImageModel::ImageTypeRole).toInt() == ImageItem::Gallery) {
    // We set the gallery icon
    icon.addFile(":/images/folder.png", QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(":/images/folder-open.png", QSize(), QIcon::Normal, QIcon::On);
  } else {

    QDir thumbPath(item.data(ImageModel::ImagePathRole).toString());

    if (!thumbPath.cd("thumbnails") || !QFile::exists(item.data(ImageModel::ImageThumbnailPathRole).toString())) {
      GJobs::ReadJob *job = new GJobs::ReadJob(this);
      job->queuePhoto(item);
      JobManager::self()->registerJob("ThumbnailJob-" + item.data(ImageFilenameRole).toString(), job);
      connect(job, SIGNAL(signalThumb(const QString&)), this, SIGNAL(signalThumb(const QString&)));
      qRegisterMetaType<QModelIndex>("QModelIndex");
      connect(job, SIGNAL(signalProcessed(const QModelIndex&)), this, SLOT(slotChange(const QModelIndex&)));
      JobManager::self()->startJob("ThumbnailJob-" + item.data(ImageFilenameRole).toString());

      icon.addFile(":/images/image-big.png");
    } else {
      icon.addFile(item.data(ImageModel::ImageThumbnailPathRole).toString());
    }
  }

  return icon;
}

void ImageModel::reconstruct()
{
  ImageItem *temp = m_rootItem;
  setupModelData();

  delete temp;

  reset();
}

QObject *ImageModel::addImages(const QModelIndex &parent, const QString &sourcePath, const QStringList &fileNames, bool deleteSource)
{/*
  if (!parent.isValid())
    return 0;

  ImageItem *gallery = static_cast<ImageItem*>(parent.internalPointer());
  QString a = gallery->filePath();

  // Copy the whole directory
  if (fileNames.isEmpty()) {
    if (!JobManager::self()->job("CopyImages")) {
      m_currentCopyParent = parent;
      QObject *job = JobManager::self()->registerJob("CopyImages", new GJobs::CopyJob(sourcePath, gallery->filePath(), parent, deleteSource, this));
      connect(job, SIGNAL(signalProcess(const QString&)), this, SLOT(slotProcess(const QString&)));
      JobManager::self()->startJob("CopyImages");
      return job;
    }
  } else {
  // Copy only the fileNames
    if (!JobManager::self()->job("CopyImages")) {
    m_currentCopyParent = parent;
    QObject *job = JobManager::self()->registerJob("CopyImages", new GJobs::CopyJob(sourcePath, fileNames, gallery->filePath(), parent));
    connect(job, SIGNAL(signalProcess(const QString&)), this, SLOT(slotProcess(const QString&)));
    JobManager::self()->startJob("CopyImages");
    return job;
  }
  }

  return 0;*/
}

/*QString ImageModel::createGallery(const QString &name, const QString &sourcePath, const QModelIndex &parent, bool deleteSources, const QStringList &fileNames)
{
  ImageItem *parentItem;
  if (parent.isValid())
    parentItem = static_cast<ImageItem*>(parent.internalPointer());
  else
    parentItem = m_rootItem;

  QString hash = JobManager::self()->createGalleryJob(name, parentItem, sourcePath, fileNames, deleteSources);
  connect(JobManager::self()->job(hash), SIGNAL(finished(ImageItem*)), this, SLOT(addItem(ImageItem*)));
  JobManager::self()->startJob(hash);

  return hash;
}*/

bool ImageModel::removeGallery(const QModelIndex &index)
{/*
  if (!index.isValid())
    return false;

  // Remove all child galleries first
  foreach(QModelIndex child, childs(index))
  if (child.data(GCore::ImageModel::ImageTypeRole) == GCore::ImageItem::Gallery)
    removeGallery(child);

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  if (item->imageType() != GCore::ImageItem::Gallery)
    return false;

  QDir gallery(item->filePath());

  beginRemoveRows(index.parent(), item->row(), item->row());

  // Remove the images
  removeImages(childs(index));

  // Delete what is left in the gallery
  QDir thumbnails(gallery);
  if (thumbnails.cd("thumbnails")) {
    QDirIterator thumbnailsImages(thumbnails.absolutePath(), QDir::Files | QDir::Hidden | QDir::System);
    while (thumbnailsImages.hasNext())
      thumbnails.remove(thumbnailsImages.next());

    thumbnails.cdUp();

    // Delete the folders.
    if (!gallery.rmdir("thumbnails")) {
      ErrorHandler::reportMessage(tr("Cannot delete gallery. Thumbnails directory cannot be deleted."), ErrorHandler::Critical);
      return false;
    }
  }

  QString fileName = item->fileName();

  ImageItem *rootItem = item->parent();

  rootItem->removeChild(item);

  QDirIterator images(gallery.absolutePath(), QDir::Files | QDir::Hidden | QDir::System);
  while (images.hasNext())
    if (!gallery.remove(images.next())) {
      ErrorHandler::reportMessage(tr("Cannot delete gallery. Images and ohter files in gallery cannot be deleted."), ErrorHandler::Critical);
      return false;
    }

  gallery.cdUp();

  bool status;
  if (!(status = gallery.rmdir(fileName)))
    ErrorHandler::reportMessage(tr("Cannot delete gallery. It's directory cannot be removed."), ErrorHandler::Critical);

  endRemoveRows();

  return status;*/
}

QModelIndex ImageModel::removeImages(const QModelIndexList &indexList)
{/*
  if (indexList.isEmpty())
    return QModelIndex();

  bool status = true;
  QModelIndex parent;
  int parentNum = 0;

  parent = this->parent(indexList.first());
  parentNum = parent.row();

  ImageItem *parentItem = static_cast<ImageItem*>(parent.internalPointer());

  foreach(QModelIndex index, indexList) {
    ImageItem *picture = static_cast<ImageItem*>(index.internalPointer());

    if (picture->imageType() != GCore::ImageItem::Image)
      continue;

    beginRemoveRows(parent, picture->row(), picture->row());

    QDir gallery(picture->filePath());
    gallery.cdUp();
    gallery.cd("thumbnails");

    // Remove the picture's thumbnail
    if (!QFile::remove(gallery.absoluteFilePath(picture->thumbName()))) {
        qWarning() << "Cannot remove thumbnail: " << gallery.absoluteFilePath(picture->thumbName()) << endl;
        status = false;
        break;
      }

    gallery.cdUp();

    // Remove the picture
    if (!QFile::remove(picture->filePath())) {
        qWarning() << "Cannot remove image: " << picture->filePath() << endl;
        status = false;
        break;
      }

    // Remove the metadata
    if (!picture->remove()) {
      status = false;
      break;
    }

    parentItem->removeChild(picture);

    endRemoveRows();
  }

  return index(parentNum, 0);*/
}

QModelIndexList ImageModel::childs(const QModelIndex &parent)
{
  ImageItem *parentItem;
  if (!parent.isValid())
    parentItem = m_rootItem;
  else
    parentItem = static_cast<ImageItem*>(parent.internalPointer());

  QModelIndexList output;

  for (int count = 0; count < parentItem->childCount(); count++) {
    QModelIndex child = index(count, 0, parent);
    if (child.isValid())
      output << child;
  }

  return output;
}

bool ImageModel::checkName(const QString &filename, const QModelIndex &root) const
{
  ImageItem *rootItem = static_cast<ImageItem*>(root.internalPointer());
  if (!root.isValid())
    rootItem = m_rootItem;

  int numberRows = rootItem->childCount();
  for (int count = 0; count < numberRows; count++)
    if (rootItem->child(count)->fileName().toLower() == filename.toLower())
      return true;

  return false;
}

QStringList ImageModel::getGalleriesList()
{
  return processGalleriesList();
}

QStringList ImageModel::imagesNames(const QModelIndex &parent)
{
  QStringList output;

  if (!parent.isValid())
    return output;

  int rows = rowCount(parent);

  for (int count = 0; count < rowCount(parent); count++)
    output << parent.child(count, 0).data(ImageNameRole).toString();

  return output;
}

QModelIndex ImageModel::findGallery(const QString &name)
{
  return processGallerySearch(name);
}

void ImageModel::stopCopy()
{
  JobManager::self()->stopJob("CopyImages");
}

void ImageModel::setupModelData()
{
  m_rootItem = MetaDataManager::self()->readManifest();
}

QStringList ImageModel::processGalleriesList(ImageItem *root)
{
  QStringList output;
  if (!root)
    root = m_rootItem;

  int childCount = root->childCount();
  for (int count = 0; count < childCount; count++) {
    // Check if the item is a gallery
    if (root->child(count)->type() == ImageItem::Gallery) {
      output << root->child(count)->name();

      // Check if this gallery has childs
      if (root->child(count)->childCount())
        output << processGalleriesList(root->child(count));
    }
  }

  return output;
}

QModelIndex ImageModel::processGallerySearch(const QString &name, const QModelIndex &parent)
{
  foreach(QModelIndex child, childs(parent)) {
    // Check if the item is a gallery and has the name that we seek
    if (child.data(ImageModel::ImageTypeRole) == ImageItem::Gallery && child.data(ImageModel::ImageNameRole).toString() == name) {
      return child;
    }

    // Check if this gallery has childs
    if (!childs(child).isEmpty()) {
      QModelIndex result = processGallerySearch(name, child);
      if (result.isValid())
        return result;
    }
  }

  return QModelIndex();
}

QModelIndex ImageModel::index(int itemId, ImageItem::Type type) const
{
  return index(itemForId(itemId, type));
}

QModelIndex ImageModel::index(ImageItem *item) const
{
  if (item && item != m_rootItem)
    return createIndex(item->row(), 0, item);

  return QModelIndex();
}

ImageItem *ImageModel::itemForId(int itemId, ImageItem::Type type) const
{
  QQueue<ImageItem*> queue;

  if (itemId == -1)
    return m_rootItem;

  queue.enqueue(m_rootItem);

  while (!queue.isEmpty()) {
    ImageItem *item = queue.dequeue();
    int childCount = item->childCount();

    for (int count = 0; count < childCount; count++) {
      ImageItem *child = item->child(count);
      if (child->id() == itemId && child->type() == type)
        return child;

      if (child->childCount() > 0)
        queue.enqueue(child);
    }
  }

  return 0;
}

void ImageModel::addItem(ImageItem *item)
{
  if (item->type() == ImageItem::Gallery) {
    ImageItem *parent = itemForId(item->parentId(), ImageItem::Gallery);

    beginInsertRows(index(parent), 0, 0);
    parent->appendChild(item);
    endInsertRows();

  } else if (item->type() == ImageItem::Image) {
  }
}

void ImageModel::slotChange(const QModelIndex &item)
{
  QModelIndex currentItem = item;
  emit dataChanged(currentItem, currentItem);
}

}
