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

#include <QtCore/QQueue>

#include <QtGui/QSortFilterProxyModel>

#include "core/metadatamanager.h"
#include "core/exifmanager.h"
#include "core/data.h"
#include "core/jobmanager.h"

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
    case ThumbnailsPathRole : {
      return item->thumbDir().absolutePath();
    }
    case Qt::EditRole : {
      return item->name();
    }
    case (ImageModel::ImagePictureRole) : {
      return QImage(item->absoluteFilePath());
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
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant ImageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

QModelIndex ImageModel::index(int row, int column, const QModelIndex &parent) const
{
  ImageItem *parentItem;

  if (column != 0)
    return QModelIndex();

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
  if (!childItem)
    return QModelIndex();

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
      if (!JobManager::self()->job(m_progressRead.value(item.data(ImageFilenameRole).toString()))) {
      QString hash = JobManager::self()->readImages(item.data(ImagePathRole).toString(), item.data(ImageFilenameRole).toStringList(), item.data(ThumbnailsPathRole).toString());
      connect(JobManager::self()->job(hash), SIGNAL(progress(const QString&, const QImage&, int)), this, SLOT(processThumbnail(const QString&, const QImage&, int)));
      m_progressRead.insert(item.data(ImageFilenameRole).toString(), hash);
      }

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

void ImageModel::removeItem(const QModelIndex &index)
{
  if (!index.isValid())
    return;

  ImageItem *parent = index.parent() == QModelIndex() ? m_rootItem : static_cast<ImageItem*> (index.parent().internalPointer());
  ImageItem *child = static_cast<ImageItem*> (index.internalPointer());

  beginRemoveRows(index.parent(), child->row(), child->row());
  parent->removeChild(child);
  endRemoveRows();
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

QModelIndex ImageModel::findGallery(const QString &name)
{
  QQueue<ImageItem*> queue;
  queue.enqueue(m_rootItem);

  while(!queue.isEmpty()) {
    ImageItem *item = queue.dequeue();

    if (item->type() == ImageItem::Gallery && item->name() == name)
      return createIndex(item->row(), 0, item);

    int childs = item->childCount();
    for (int count = 0; count < childs; count++) {
      ImageItem *child = item->child(count);
      if (child->type() == ImageItem::Gallery)
        queue.enqueue(child);
    }
  }

  return QModelIndex();
}

void ImageModel::setupModelData()
{
  m_rootItem = MetaDataManager::self()->readManifest();
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
  ImageItem *parent = itemForId(item->parentId(), ImageItem::Gallery);
  if (item->type() == ImageItem::Gallery) {
    beginInsertRows(index(parent), 0, 0);
    parent->appendChild(item);
    endInsertRows();
  } else if (item->type() == ImageItem::Image) {
    beginInsertRows(index(parent), parent->childCount(), parent->childCount());
    parent->appendChild(item);
    endInsertRows();
  }
}

void ImageModel::processThumbnail(const QString &fileName, const QImage&, int parentId)
{
  int imageId = MetaDataManager::self()->imageId(fileName, parentId);
  QModelIndex currentItem = index(itemForId(imageId, ImageItem::Image));
  m_progressRead.remove(fileName);
  emit dataChanged(currentItem, currentItem);
}

}
