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

#include "core/imageitem.h"
#include "core/metadatamanager.h"
#include "core/exifmanager.h"
#include "core/data.h"

#include "core/jobs/readjob.h"
#include "core/jobs/copyjob.h"

namespace GCore
{

ImageModel::ImageModel(QObject *parent)
    : QAbstractItemModel(parent),
    m_currentJob(0),
    m_currentCopyJob(0),
    m_currentCopyParent(QModelIndex()),
    m_delete(false)
{
  m_rootItem = new ImageItem(GCore::Data::self()->getGalleriesPath(), 0, ImageItem::Root);
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
      if (item->imageType() == ImageItem::Gallery) {
        return item->name();
      } else {
        ExifManager exifData(item->getFilePath(), GCore::Data::self()->getImageModel());

        QString date;
        if (!exifData.getCreationDate().isValid())
          date = tr("Unavailable");
        else
          date = exifData.getCreationDate().toString(Qt::SystemLocaleDate);

        QString message = tr("EXIF information:") + "<table>";
        message += "<tr><td>" + tr("Camera manufacturer:") + " </td><td> " + exifData.getCameraMaker() + "</td></tr>";
        message += "<tr><td>" + tr("Camera model:") + " </td><td> " + exifData.getCameraModel() + "</td></tr>";
        message += "<tr><td>" + tr("Aperture:") + " </td><td> " + exifData.getAperture() + "</td></tr>";
        message += "<tr><td>" + tr("Creation Date:") + " </td><td> " + date + "</td></tr>";
        message += "<tr><td>" + tr("Shutter speed:") + " </td><td> " + exifData.getShutterSpeed() + "</td></tr>";
        message += "<tr><td>" + tr("Exposure time:") + " </td><td> " + exifData.getExposureTime() + "</td></tr>";
        message += "<tr><td>" + tr("Focal length:") + " </td><td> " + exifData.getFocalLength() + "</td></tr>";
        message += "<tr><td>" + tr("Flash: ") + " </td><td> " + exifData.getFlash() + "</td></tr></table>";
        return item->name() + ": <i>" + item->description().replace('\n', "<br/>") + "</i>" + "<p>" + message + "</p>";
      }
    }
    case Qt::DecorationRole : {
      if (index.column() == 0)
        return fileIcon(index);
    }
    case ImageModel::ImageDescriptionRole : {
      if (item->imageType() != ImageItem::Image)
        return QVariant();

      return item->description();
    }
    case ImageModel::ImageTypeRole : {
      return item->imageType();
    }
    case ImageModel::ImageMetadataRole : {
      return QVariant::fromValue(static_cast<QObject*>(item->metadata()));
    }
    case ImageModel::ImageFilenameRole : {
      return item->getFileName();
    }
    case ImageModel::ImageFilepathRole : {
      return item->getFilePath();
    }
    case ImageModel::ImageDirPathRole : {
      return item->getFilePath().remove(QDir::fromNativeSeparators(item->getFilePath()).remove(QRegExp("^.+/")));
    }
    case ImageModel::ImageThumbnailPathRole : {
      return item->getThumbName();
    }
    case Qt::EditRole : {
      return item->data(index.column());
    }
    case (ImageModel::ImagePictureRole) : {
      return QImage(item->getFilePath());
    }
    case ImageModel::ObjectRole : {
      return qVariantFromValue(static_cast<QObject*>(item));
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
  if (parent.isValid())
    return static_cast<ImageItem*>(parent.internalPointer())->columnCount();
  else
    return m_rootItem->columnCount();
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

QIcon ImageModel::fileIcon(const QModelIndex &item) const
{
  QIcon icon;

  // We check if this is a gallery or a image
  if (item.data(ImageModel::ImageTypeRole).toInt() == ImageItem::Gallery) {
    // We set the gallery icon
    icon.addFile(":/images/folder.png", QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(":/images/folder-open.png", QSize(), QIcon::Normal, QIcon::On);
  } else {

    QDir thumbPath(item.data(ImageModel::ImageFilepathRole).toString());
    thumbPath.cdUp();

    if (!thumbPath.cd(".thumbnails") || !QFile::exists(thumbPath.absoluteFilePath(item.data(ImageModel::ImageThumbnailPathRole).toString()))) {

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

  return icon;
}

void ImageModel::reconstruct()
{
  ImageItem *temp = m_rootItem;
  m_rootItem = new ImageItem(GCore::Data::self()->getGalleriesPath(), 0, ImageItem::Root);
  setupModelData();

  delete temp;

  reset();
}

QObject *ImageModel::addImages(const QModelIndex &parent, const QString &sourcePath, const QStringList &fileNames)
{
  if (!parent.isValid())
    return 0;

  ImageItem *gallery = static_cast<ImageItem*>(parent.internalPointer());
  QString a = gallery->getFilePath();

  // Copy the whole directory
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

  // Copy only the fileNames
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

QObject *ImageModel::createGallery(const QString &name, const QString &sourcePath, const QModelIndex &parent, const QStringList &fileNames)
{
  QString path;
  if (parent.isValid())
    path = parent.data(ImageModel::ImageFilepathRole).toString();
  else
    path = GCore::Data::self()->getGalleriesPath();

  // We create the destination directory
  QDir destPath(path);
  if (!destPath.mkpath(name)) {
    ErrorHandler::reportMessage(tr("Cannot create gallery directory."), ErrorHandler::Critical);
    return 0;
  }
  destPath.cd(name);

  QModelIndex parentIndex;

  if (!parent.isValid()) {
    // Notify the viewers we are making a new gallery
    beginInsertRows(QModelIndex(), m_rootItem->childCount(), m_rootItem->childCount() + 1);

    m_rootItem->appendChild(new GCore::ImageItem(name, m_rootItem, GCore::ImageItem::Gallery));

    parentIndex = index(m_rootItem->childCount() - 1, 0);
  } else {
    ImageItem *parentItem = static_cast<ImageItem*>(parent.internalPointer());

    beginInsertRows(parent, parentItem->childCount(), parentItem->childCount() + 1);

    parentItem->appendChild(new GCore::ImageItem(name, parentItem, GCore::ImageItem::Gallery));

    parentIndex = parent.child(parentItem->childCount() - 1, 0);
  }

  endInsertRows();
  GCore::Data::self()->getModelProxy()->setSourceModel(this);

  return addImages(parentIndex, sourcePath, fileNames);
}

bool ImageModel::removeGallery(const QModelIndex &index)
{
  if (!index.isValid())
    return false;

  // Remove all child galleries first
  foreach (QModelIndex child, childs(index))
    if (child.data(GCore::ImageModel::ImageTypeRole) == GCore::ImageItem::Gallery)
      removeGallery(child);

  ImageItem *item = static_cast<ImageItem*>(index.internalPointer());

  if (item->imageType() != GCore::ImageItem::Gallery)
    return false;

  QDir gallery(item->getFilePath());

  beginRemoveRows(index.parent(), item->row(), item->row());

  // Remove the images
  removeImages(childs(index));

  // Delete what is left in the gallery
  QDir thumbnails(gallery);
  if (thumbnails.cd(".thumbnails")) {
    QDirIterator thumbnailsImages(thumbnails.absolutePath(), QDir::Files | QDir::Hidden | QDir::System);
    while (thumbnailsImages.hasNext())
      thumbnails.remove(thumbnailsImages.next());

    thumbnails.cdUp();

    // Delete the folders.
    if (!gallery.rmdir(".thumbnails")) {
      ErrorHandler::reportMessage(tr("Cannot delete gallery. Thumbnails directory cannot be deleted."), ErrorHandler::Critical);
      return false;
    }
  }

  QString fileName = item->getFileName();

  ImageItem *rootItem = item->parent();

  rootItem->removeChild(item);

  QDirIterator images(gallery.absolutePath(), QDir::Files | QDir::Hidden | QDir::System);
  while (images.hasNext())
    if (!gallery.remove(images.next())){
      ErrorHandler::reportMessage(tr("Cannot delete gallery. Images and ohter files in gallery cannot be deleted."), ErrorHandler::Critical);
      return false;
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

  foreach (QModelIndex index, indexList) {
    ImageItem *picture = static_cast<ImageItem*>(index.internalPointer());

    if (picture->imageType() != GCore::ImageItem::Image)
      continue;

    beginRemoveRows(parent, picture->row(), picture->row());

    QDir gallery(picture->getFilePath());
    gallery.cdUp();
    gallery.cd(".thumbnails");

    // Remove the picture's thumbnail
    if (!QFile::remove(gallery.absoluteFilePath(picture->getThumbName()))) {
      qDebug() << "Cannot remove thumbnail: " << gallery.absoluteFilePath(picture->getThumbName()) << endl;
      status = false;
      break;
    }

    gallery.cdUp();

    // Remove the picture
    if (!QFile::remove(picture->getFilePath())) {
      qDebug() << "Cannot remove image: " << picture->getFilePath() << endl;
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

bool ImageModel::checkName(const QString &filename, const QModelIndex &root) const
{
  ImageItem *rootItem = static_cast<ImageItem*>(root.internalPointer());
  if (!root.isValid())
    rootItem = m_rootItem;

  int numberRows = rootItem->childCount();
  for (int count = 0; count < numberRows; count++)
    if (rootItem->child(count)->getFileName().toLower() == filename.toLower())
      return true;

  return false;
}

QStringList ImageModel::getGalleriesList()
{
  return processGalleriesList();
}

QModelIndex ImageModel::findGallery(const QString &name)
{
  return processGallerySearch(name);
}

void ImageModel::stopCopy()
{
  if (m_currentCopyJob) {
    m_currentCopyJob->stop();
    m_delete = true;
  }
}

void ImageModel::setupModelData() const
{
  processPath(GCore::Data::self()->getGalleriesDir());
}

void ImageModel::processPath(const QDir &path, ImageItem *root) const
{
  if (!root)
    root = m_rootItem;

  QStringList items = path.entryList(QDir::Dirs);
  items.removeAll(".");
  items.removeAll("..");
  items.removeAll(".thumbnails");

  foreach (QString item, items) {
    QDir temp = path;
    temp.cd(item);
    ImageItem *gallery = new ImageItem(item, root, ImageItem::Gallery);
    root->appendChild(gallery);
    processPath(temp, gallery);
  }

  QStringList images = path.entryList(QDir::Files);
  images.removeAll(".metadata");

  foreach (QString image, images)
    root->appendChild(new ImageItem(image, root, ImageItem::Image));
}

QStringList ImageModel::processGalleriesList(ImageItem *root)
{
  QStringList output;
  if (!root)
    root = m_rootItem;

  int childCount = root->childCount();
  for (int count = 0; count < childCount; count++) {
    // Check if the item is a gallery
    if (root->child(count)->imageType() == ImageItem::Gallery) {
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
  foreach (QModelIndex child, childs(parent)) {
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

void ImageModel::slotProcess(const QString &fileName)
{
  if (!m_currentCopyJob && !m_currentCopyParent.isValid())
    return;

  ImageItem *gallery = static_cast<ImageItem*>(m_currentCopyParent.internalPointer());

  if (!fileName.isEmpty() && fileName != "~ERROR~") {
    beginInsertRows(m_currentCopyParent, gallery->childCount(), gallery->childCount() + 1);

    ImageItem *image = new ImageItem(fileName, gallery, ImageItem::Image);
    image->metadata()->addImage(fileName);

    gallery->appendChild(image);

    endInsertRows();

    // Is there any better option?
    Data::self()->getModelProxy()->setSourceModel(this);

  } else if (fileName != "~ERROR~") {
    removeGallery(m_currentCopyParent);
  }
}

void ImageModel::slotRemoveJob()
{
  // Delete the ReadJob!
  disconnect(this, SLOT(slotRemoveJob()));
  m_currentJob->deleteLater();
  m_currentJob = 0;
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

  // Delete the CopyJob
  disconnect(this, SLOT(slotRemoveCopyJob()));
  m_currentCopyJob->deleteLater();
  m_currentCopyJob = 0;
  m_currentCopyParent = QModelIndex();
  m_delete = false;
}

}
