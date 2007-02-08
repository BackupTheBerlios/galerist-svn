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
#include "imagemodel.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QtCore/QSize>
#include <QtCore/QRegExp>

#include "core/imageitem.h"
#include "core/metadatamanager.h"
#include "core/data.h"
#include "core/jobs/readjob.h"
#include "core/jobs/copyjob.h"

#include <QtCore/QtDebug>

namespace GCore
{

ImageModel::ImageModel(const QString &path, QObject *parent)
    : QAbstractItemModel(parent),
    m_path(path),
    m_currentJob(0),
    m_currentCopyJob(0),
    m_currentCopyParent(QModelIndex()),
    m_delete(false)
{
  m_rootItem = new ImageItem(path, 0, ImageItem::Root);
  setupModelData(m_path);

  m_pendingUpdate.clear();
}

ImageModel::~ImageModel()
{
  delete m_rootItem;
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

  if (parentItem->imageType() == ImageItem::Gallery)
    connect(parentItem->metadata(), SIGNAL(signalInsert()), this, SLOT(slotRefresh()));

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
  if (parent.isValid())
    return static_cast<ImageItem*>(parent.internalPointer())->columnCount();
  else
    return m_rootItem->columnCount();
}

QVariant ImageModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  if (!item)
    return QVariant();

  switch (role) {
    case (Qt::DisplayRole) : {
        switch (index.column()) {
          case (0) : {
            if (item->imageType() == ImageItem::Image)
              return item->name();
            else
              return item->data(0);
          }
        }
      }
    case (Qt::ToolTipRole) : {
        if (item->imageType() == ImageItem::Gallery) {
          return item->data(index.column());
        } else {
          return item->name() + ": <i>" + item->description().replace('\n', "<br/>") + "</i>";
        }
      }
    case (Qt::DecorationRole) : {
        if (index.column() == 0)
          return fileIcon(index);
      }
    case (ImageModel::ImageDescriptionRole) : {
        if (item->imageType() != ImageItem::Image)
          return QVariant();

        return item->description();
      }
    case (ImageModel::ImageTypeRole) : {
        return item->imageType();
      }
    case (ImageModel::ImageMetadataRole) : {
        return QVariant::fromValue(static_cast<QObject*>(item->metadata()));
      }
    case (ImageModel::ImageFilenameRole) : {
        return item->getFileName();
      }
    case (ImageModel::ImageFilepathRole) : {
        return item->getFilePath();
      }
    case (ImageModel::ImageDirPathRole) : {
        return item->getFilePath().remove(QDir::fromNativeSeparators(item->getFilePath()).remove(QRegExp("^.+/")));
      }
    case (ImageModel::ImageThumbnailPathRole) : {
        return item->getThumbName();
      }
    case (Qt::EditRole) : {
        return item->data(index.column());
      }
    case (ImageModel::ImagePictureRole) : {
        return QImage(item->getFilePath());
      }
    default:
      return QVariant();

  }
}

bool ImageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid() || index.column() != 0 || (flags(index) & Qt::ItemIsEditable) == 0 || role != Qt::EditRole)
    return false;

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  switch (item->imageType()) {
    case (ImageItem::Image) : {
        item->setName(value.toString());
        return true;
      }
    case (ImageItem::Gallery) : {
        bool status = false;
        QDir gallery(item->getFilePath());
        gallery.cdUp();
        status = gallery.rename(item->getFileName(), value.toString());

        if (status)
          item->setName(value.toString());
        return status;
      }
    default : {
      return false;
    }
  }

  //emit layoutChanged();
}

Qt::ItemFlags ImageModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant ImageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return m_rootItem->data(section);

  return QVariant();
}

void ImageModel::setupModelData(const QString &path)
{
  processPath(QDir(QDir::fromNativeSeparators(path)));
  
//   // Initialise the path to the gallery
//   QDir galleriesPath(path);
// 
//   // We get the list of galleries
//   QStringList galleries(galleriesPath.entryList(QDir::Dirs));
// 
//   // Calculate the number of galleries
//   QStringList::const_iterator limit = galleries.constEnd();
// 
//   // We go through every gallery and create a tree of galleries
//   for (QStringList::const_iterator count = galleries.constBegin(); count != limit; count++) {
//     // Btw. We skip the . and .. directories, because they are not galleries!
//     if (!(*count != "." && *count != ".." && *count != ".thumbnails" && *count != ".metadata"))
//       continue;
// 
//     // and images
//     QDir imagesPath(galleriesPath);
//     imagesPath.cd(*count);
// 
//     // Get the list of images
//     QStringList images(imagesPath.entryList(Data::self()->getImageFormats(), QDir::Files));
// 
//     // Calculate the number of images
//     QStringList::const_iterator limitImage = images.constEnd();
// 
//     // Let's add the galleries and images into a healthy tree
//     ImageItem *gallery = new ImageItem(QDir::fromNativeSeparators(*count), m_rootItem, ImageItem::Gallery);
//     m_rootItem->appendChild(gallery);
// 
//     for (QStringList::const_iterator countImage = images.constBegin(); countImage != limitImage; countImage++) {
//       gallery->appendChild(new ImageItem(*countImage, gallery, ImageItem::Image));
//     }
// 
//     // Get the list of subgalleries
// 
//   }
}

void ImageModel::processPath(const QDir &path, ImageItem *root)
{
  if (!root)
    root = m_rootItem;
  
  QStringList items = path.entryList(QDir::Dirs);
  items.removeAll(".");
  items.removeAll("..");
  items.removeAll(".thumbnails");

  for (QStringList::const_iterator count = items.begin(); count != items.end(); count++) {
    QDir temp = path;
    temp.cd(*count);
    ImageItem *gallery = new ImageItem(*count, root, ImageItem::Gallery);
    root->appendChild(gallery);
    processPath(temp, gallery);
  }

  if (!items.isEmpty())
  qDebug() << items;

  QStringList images = path.entryList(QDir::Files);
  images.removeAll(".metadata");

  for (QStringList::const_iterator count = images.begin(); count != images.end(); count++) {
    root->appendChild(new ImageItem(*count, root, ImageItem::Image));
  }
}

QIcon ImageModel::fileIcon(const QModelIndex &item) const
{
  QIcon icon;

  // We check if this is a gallery or a image
  if (item.data(ImageModel::ImageTypeRole).toInt() == ImageItem::Gallery) {
    // We set the gllery icon
    icon.addFile(":/images/folder.png", QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(":/images/folder-open.png", QSize(), QIcon::Normal, QIcon::On);
  } else {
    // We check if we need to show thumbnails or a standard icon
    if (Data::self()->getImageView() == Data::List) {
      icon.addFile(":/images/image.png");
    } else { // Thumbnail
      QDir thumbPath(item.data(ImageModel::ImageFilepathRole).toString());
      thumbPath.cdUp();

      if (!thumbPath.cd(".thumbnails") || !QFile::exists(thumbPath.absoluteFilePath(item.data(ImageModel::ImageThumbnailPathRole).toString()))) {
        /* if (!m_currentJob) {
           m_currentJob = new GJobs::ReadJob(item.data(ImageModel::ImageFilepathRole).toString(), item);
           connect(m_currentJob, SIGNAL(finished()), this, SLOT(slotRemoveJob()));
           connect(m_currentJob, SIGNAL(signalThumb(const QString&)), this, SIGNAL(signalThumb(const QString&)));
           m_currentJob->start();
         } else {*/
        //if (!m_pendingUpdate.contains(item))
        // m_pendingUpdate << item;
        //}
        //queueIconMake(item);

        if (!m_currentJob) {
          m_currentJob = new GJobs::ReadJob(this);
          connect(m_currentJob, SIGNAL(finished()), this, SLOT(slotRemoveJob()));
          connect(m_currentJob, SIGNAL(signalThumb(const QString&)), this, SIGNAL(signalThumb(const QString&)));
          qRegisterMetaType<QModelIndex>("QModelIndex");
          connect(m_currentJob, SIGNAL(signalProcessed(const QModelIndex&)), this, SLOT(slotChange(const QModelIndex&)));
        }

        m_currentJob->queuePhoto(item);

        m_currentJob->start();

        icon.addFile(":/images/image-big.png");
      } else {
        icon.addFile(thumbPath.absoluteFilePath(item.data(ImageModel::ImageThumbnailPathRole).toString()));
      }
    }
  }

  return icon;
}

void ImageModel::queueIconMake(const QModelIndex &item)
{
  if (!m_pendingUpdate.contains(item))
    m_pendingUpdate << item;
}

bool ImageModel::refresh(ImageItem *root)
{
  // Declare variables
  QDir path(m_path);
  ImageItem::Type type;
  QDir::Filter filter = QDir::NoFilter;
  bool changed = false;

  // If root is not defined, we go from m_rootItem down
  if (!root)
    root = m_rootItem;

  if (root->imageType() == ImageItem::Image)
    // We are too deep! Bail out NOW!
    return false;

  if (root->imageType() == ImageItem::Gallery) {
    // Initialise the variables
    path.cd(root->getFileName());
    type = ImageItem::Image;
  } else {
    // Only type must be initialised here. path is allready initialised
    type = ImageItem::Gallery;
    filter = QDir::Dirs;
  }

  // We read the Gallery path
  QStringList items(path.entryList(filter));
  QStringList::const_iterator limit = items.constEnd();

  // Calculate how many children the root has
  int numberIndexedItems = root->childCount();

  // We add the missing items
  for (QStringList::const_iterator count = items.constBegin(); count != limit; count++) {
    // Btw. We skip the . and .. directories. Because, they are not galleries!
    if (!(*count != "." && *count != ".." && *count != ".thumbnails" && *count != ".metadata"))
      continue;

    bool exists = false;
    for (int countIndexed = 0; countIndexed < numberIndexedItems; countIndexed++) {
      if (*count == root->child(countIndexed)->getFileName()) {
        // If exists, skip
        exists = true;

        // We check if there is any new image... Nothing new
        changed = refresh(root->child(countIndexed));

        break;
      }
    }

    // We add the intem to the index, if necessary
    if (!exists) {
      ImageItem *newItem = new ImageItem(*count, root, type);
      root->appendChild(newItem);

      changed = true;

      // We go where no method has gone before... Simply put. We index all the images of the new gallery
      refresh(newItem);
    }
  }

  // We emit the changeLayout signal
// if (root->imageType() == ImageItem::Root)
  emit layoutChanged();

  // We bail out
  return changed;
}

void ImageModel::slotRemoveJob()
{
  // Report that the photo has been thumbnailed
  //emit dataChanged(m_currentJob->getItem(), m_currentJob->getItem());

  // Delete the ReadJob!
  disconnect(this, SLOT(slotRemoveJob()));
  m_currentJob->deleteLater();
  m_currentJob = 0;
  //m_currentThumbnail = QModelIndex();
}

void ImageModel::slotChange(const QModelIndex &item)
{
  QModelIndex currentItem = item;
  emit dataChanged(currentItem, currentItem);
}

void ImageModel::slotRemoveCopyJob()
{
  if (m_delete) {
    beginRemoveRows(QModelIndex(), m_rootItem->childCount() - 1, 1);
    ImageItem *gallery = static_cast<ImageItem*>(m_currentCopyParent.internalPointer());
    gallery->parent()->removeChild(gallery);
    endRemoveRows();
  }

  // Delete the CopyJob!
  disconnect(this, SLOT(slotRemoveCopyJob()));
  m_currentCopyJob->deleteLater();
  m_currentCopyJob = 0;
  m_currentCopyParent = QModelIndex();
  m_delete = false;

  // Force ImageListView to refresh
  //emit layoutChanged();
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

  return item->metadata()->checkName(name);
}

QObject *ImageModel::addImages(const QModelIndex &parent, const QString &sourcePath, const QStringList &fileNames)
{
  if (!parent.isValid())
    return 0;

  ImageItem *gallery = static_cast<ImageItem*>(parent.internalPointer());

  qDebug() << gallery->getFilePath() << endl;

  if (fileNames.isEmpty()) {
    if (!m_currentCopyJob) {
      m_currentCopyParent = parent;
      m_currentCopyJob = new GCore::GJobs::CopyJob(sourcePath, gallery->getFilePath(), parent);
      connect(m_currentCopyJob, SIGNAL(signalProcess(const QString&)), this, SLOT(slotProcess(const QString&)));
      connect(m_currentCopyJob, SIGNAL(finished()), this, SLOT(slotRemoveCopyJob()));
      m_currentCopyJob->start();
      return m_currentCopyJob;
    }
  }

  if (!m_currentCopyJob) {
    m_currentCopyParent = parent;
    m_currentCopyJob = new GCore::GJobs::CopyJob(sourcePath, fileNames, gallery->getFilePath(), parent);
    connect(m_currentCopyJob, SIGNAL(signalProcess(const QString&)), this, SLOT(slotProcess(const QString&)));
    connect(m_currentCopyJob, SIGNAL(finished()), this, SLOT(slotRemoveCopyJob()));
    m_currentCopyJob->start();
    return m_currentCopyJob;
  }

  return 0;
}

QObject *ImageModel::createGallery(const QString &name, const QString &sourcePath)
{
  // We create the destination directory
  QDir destPath(GCore::Data::self()->getGalleriesPath());

  if (!destPath.mkdir(name)) {
    ErrorHandler::reportMessage(tr("Cannot create gallery directory."), ErrorHandler::Critical);
    return 0;
  }
  destPath.cd(name);

  // Notify the viewers we are making a new gallery
  beginInsertRows(QModelIndex(), m_rootItem->childCount(), m_rootItem->childCount() + 1);

  m_rootItem->appendChild(new GCore::ImageItem(name, m_rootItem, GCore::ImageItem::Gallery));

  endInsertRows();

  return addImages(index(m_rootItem->childCount() - 1, 0), sourcePath);
}

void ImageModel::slotProcess(const QString &fileName)
{
  if (!m_currentCopyJob && !m_currentCopyParent.isValid())
    return;

  ImageItem *gallery = static_cast<ImageItem*>(m_currentCopyParent.internalPointer());

  if (!fileName.isEmpty()) {
    beginInsertRows(m_currentCopyParent, rowCount(m_currentCopyParent), rowCount(m_currentCopyParent) + 1);

    ImageItem *image = new ImageItem(fileName, gallery, ImageItem::Image);
    image->metadata()->addImage(fileName);

    gallery->appendChild(image);

    endInsertRows();
  } else
    removeGallery(m_currentCopyParent);
}

bool ImageModel::removeGallery(const QModelIndex &index)
{
  if (!index.isValid())
    return false;

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  beginRemoveRows(QModelIndex(), 0, item->childCount());

  // Remove the images
  removeImages(childs(index));

  QDir gallery(item->getFilePath());

  // Delete what is left in the gallery
  QDir thumbnails(gallery);
  if (thumbnails.cd(".thumbnails")) {

    QStringList thumbs = thumbnails.entryList(QDir::Files | QDir::Hidden | QDir::System);
    QStringList::const_iterator end = thumbs.constEnd();
    for (QStringList::const_iterator count = thumbs.constBegin(); count != end; count++) {
      thumbnails.remove(*count);
    }

    thumbnails.cdUp();

    // Delete the folders.
    if (!gallery.rmdir(".thumbnails")) {
      ErrorHandler::reportMessage(tr("Cannot delete gallery. Thumbnails directory cannot be deleted."), ErrorHandler::Critical);
      return false;
    }
  }

  QString fileName = item->getFileName();

  //delete item->metadata();
  m_rootItem->removeChild(item);

  QStringList images = gallery.entryList(QDir::Files | QDir::Hidden | QDir::System);
  QStringList::const_iterator end = images.constEnd();
  for (QStringList::const_iterator count = images.constBegin(); count != end; count++) {
    if (!gallery.remove(*count)) {
      ErrorHandler::reportMessage(tr("Cannot delete gallery. Images and ohter files in gallery cannot be deleted."), ErrorHandler::Critical);
      return false;
    }
  }

  gallery.cdUp();

  bool status;
  if (!(status = gallery.rmdir(fileName)))
    ErrorHandler::reportMessage(tr("Cannot delete gallery. It's directory cannot be removed."), ErrorHandler::Critical);

  endRemoveRows();

  return status;
}

QModelIndex ImageModel::removeImages(const QModelIndexList &indexList)
{
  if (indexList.isEmpty())
    return QModelIndex();

  bool status = true;
  QModelIndex parent;
  int parentNum = 0;

  parent = this->parent(indexList.first());
  parentNum = parent.row();

  ImageItem *parentItem = static_cast<ImageItem*>(parent.internalPointer());


  QModelIndexList::const_iterator end = indexList.constEnd();
  for (QModelIndexList::const_iterator count = indexList.constBegin(); count != end; count++) {

    // beginRemoveRows(parent, (*count).row(), (*count).row());

    ImageItem *picture = static_cast<ImageItem*>((*count).internalPointer());

    beginRemoveRows(parent, picture->row(), picture->row());

    // Remove the metadata
    if (!picture->remove()) {
      status = false;
      break;
    }

    QDir gallery(picture->getFilePath());
    gallery.cdUp();
    gallery.cd(".thumbnails");

    // Remove the picture's thumbnail
    if (!QFile::remove(gallery.absoluteFilePath(picture->getThumbName()))) {
        qDebug() << gallery.absoluteFilePath(picture->getThumbName()) << endl;
        status = false;
        break;
      }

    gallery.cdUp();

    // Remove the picture
    if (!QFile::remove(picture->getFilePath())) {
        qDebug() << picture->getFilePath() << endl;
        status = false;
        break;
      }

    parentItem->removeChild(picture);

    endRemoveRows();
  }

  //reconstruct();

  return index(parentNum, 0);
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

void ImageModel::reconstruct()
{
  ImageItem *temp = m_rootItem;
  m_rootItem = new ImageItem(m_path, 0, ImageItem::Root);
  setupModelData(m_path);

  delete temp;
}

bool ImageModel::checkName(const QString &filename, const QModelIndex &root) const
{
  ImageItem *rootItem = static_cast<ImageItem*>(root.internalPointer());
  if (!root.isValid())
    rootItem = m_rootItem;

  int numberRows = rootItem->childCount();
  for (int count = 0; count < numberRows; count++) {
    if (rootItem->child(count)->getFileName().toLower() == filename.toLower())
      return true;
  }

  return false;
}

void ImageModel::stopCopy()
{
  if (m_currentCopyJob) {
    m_currentCopyJob->stop();
    m_delete = true;
  }
}

void ImageModel::timerEvent(QTimerEvent*)
{
  /*if (m_pendingUpdate.isEmpty() || m_currentJob)
    return;

  if (!m_currentJob) {
    QModelIndex processedItem = m_pendingUpdate.takeFirst();
    m_currentJob = new GJobs::ReadJob(processedItem.data(ImageModel::ImageFilepathRole).toString(), processedItem);
    connect(m_currentJob, SIGNAL(finished()), this, SLOT(slotRemoveJob()));
    connect(m_currentJob, SIGNAL(signalThumb(const QString&)), this, SIGNAL(signalThumb(const QString&)));
    m_currentJob->start();
  }*/
}

void ImageModel::slotRefresh()
{
  //emit layoutChanged();
}

}
