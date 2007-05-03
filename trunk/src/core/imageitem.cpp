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
#include "imageitem.h"

#include <QtCore/QTimer>
#include <QtCore/QRect>

#include <QtGui/QImage>

#include "core/data.h"
#include "core/imagemodel.h"
#include "core/metadatamanager.h"

#include <Magick++.h>

namespace GCore
{

ImageItem::ImageItem(const QString &path, ImageItem *parent, Type type)
    : QObject(0),
    m_parentItem(parent),
    m_type(type),
    m_path(path),
    m_metadata(0),
    m_id(-1),
    m_image(0),
    m_changes(false)
{
  if (m_type == Image)
    m_id = metadata()->imageId(getFileName());
  else
    m_metadata = new MetaDataManager(getFilePath());
}

ImageItem::~ImageItem()
{
  if (!m_childItems.isEmpty() && m_type != Image)
    qDeleteAll(m_childItems.begin(), m_childItems.end());

  if (m_metadata && m_type != Image) {
    delete m_metadata;
    m_metadata = 0;
  }
}

void ImageItem::appendChild(ImageItem *item)
{
  m_childItems.append(item);
}

void ImageItem::removeChild(ImageItem *child)
{
  m_childItems.removeAt(m_childItems.indexOf(child));
  delete child;
}

ImageItem *ImageItem::child(int row) const
{
  return m_childItems.value(row);
}

int ImageItem::childCount() const
{
  return m_childItems.count();
}

int ImageItem::row() const
{
  if (m_parentItem)
    return m_parentItem->m_childItems.indexOf(const_cast<ImageItem*>(this));

  return 0;
}

int ImageItem::columnCount() const
{
  // We have a fixed number of data
  return 1;
}

QVariant ImageItem::data(int column) const
{
  // We (I) don't like columns :)
  if (!column)
    return name();
  else
    return description();
}

ImageItem *ImageItem::parent() const
{
  return m_parentItem;
}

ImageItem::Type ImageItem::imageType() const
{
  return m_type;
}

void ImageItem::setFilePath(const QString &path)
{
  m_path = path;
  emit valuesChanged();
}

QString ImageItem::getFilePath() const
{
  if (!m_parentItem) {
    return m_path;
  } else {
    QDir parentPath(m_parentItem->getFilePath());
    return parentPath.absoluteFilePath(m_path);
  }
}

QString ImageItem::getFileName() const
{
  if (!m_parentItem) {
    // There is no parent item, so path contains the full path
    QFileInfo currentPath(m_path);

    return currentPath.fileName();
  } else {
    return m_path;
  }
}

MetaDataManager *ImageItem::metadata() const
{
  MetaDataManager *output = 0;
  if (!m_metadata && m_type == Gallery)
    qFatal("Error! MetadataManager not initialised! But should be. BUG!");
  //output = m_metadata = new MetaDataManager(getFilePath());

  if (m_type == Gallery)
    output = m_metadata;

  if (!m_metadata && m_type == Image) {
    output = parent()->metadata();
    //m_id = output->imageId(getFileName());
  }

  return output;
}

QString ImageItem::name() const
{
  if (m_type == Image)
    return metadata()->name(m_id);
  else
    return getFileName();
}

QString ImageItem::description() const
{
  if (m_type == Image)
    return metadata()->description(m_id);
  else
    return QString();
}

bool ImageItem::setName(const QString &name)
{
  bool output = true;
  if (imageType() == Image)
    output = metadata()->setName(name, m_id);
  else if (imageType() == Gallery)
    setFilePath(name);


  emit valuesChanged();
  return output;
}

void ImageItem::setDescription(const QString &description)
{
  if (m_type != Image)
    return;

  metadata()->setDescription(description, m_id);

  emit valuesChanged();
}

bool ImageItem::remove()
{
  return metadata()->removePicture(m_id);
}

void ImageItem::rotateCW()
{
  if (!m_image)
    return;

  m_image->rotate(90);
  QTimer::singleShot(1000, this, SLOT(saveRotation()));
}

void ImageItem::rotateCCW()
{
  if (!m_image)
    return;

  m_image->rotate(270);
  QTimer::singleShot(1000, this, SLOT(saveRotation()));
}

void ImageItem::crop(const QRect &area)
{
  if (!m_image)
    return;

  try {
    m_image->crop(Magick::Geometry(area.width(), area.height(), area.left(), area.top()));
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    return;
  }

  saveImage();

}

QImage ImageItem::createBlurPreview(int blurFilters)
{
  Magick::Image preview;

  try {
    preview.read(getFilePath().toStdString());

    for (quint8 count = 0; count < blurFilters; count++)
      preview.blur();

    Magick::Blob blob;
    preview.write(&blob);

    QImage previewImage(preview.rows(), preview.columns(), QImage::Format_RGB32);
    previewImage.loadFromData((const uchar*) blob.data(), blob.length());

    return previewImage;
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    return QImage();
  }
}

void ImageItem::saveBlur(int blurFilters)
{
  try {
    for (quint8 count = 0; count < blurFilters; count++)
      m_image->blur();
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    return;
  }

  saveImage();
}

QImage ImageItem::createSharpenPreview(int sharpenFilters)
{
  Magick::Image preview;

  try {
    preview.read(getFilePath().toStdString());

    for (quint8 count = 0; count < sharpenFilters; count++)
      preview.sharpen();

    Magick::Blob blob;
    preview.write(&blob);

    QImage previewImage(preview.rows(), preview.columns(), QImage::Format_RGB32);
    previewImage.loadFromData((const uchar*) blob.data(), blob.length());

    return previewImage;
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    return QImage();
  }
}

void ImageItem::saveSharpen(int sharpenFilters)
{
  try {
    for (quint8 count = 0; count < sharpenFilters; count++)
      m_image->sharpen();
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    return;
  }

  saveImage();
}

QString ImageItem::getThumbName() const
{
  return getFileName().remove(QRegExp("\\..+$")).append(".jpg");
}

void ImageItem::loadImage()
{
  m_image = new Magick::Image();

  try {
    m_image->read(getFilePath().toStdString());
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    return;
  }
}

void ImageItem::saveImage()
{
  if (!m_image)
    return;

  QDir dir(getFilePath());
  dir.cdUp();

  try {
    m_image->write(getFilePath().toStdString());

    Magick::Blob blob;
    m_image->write(&blob);

    dir.cd(".thumbnails");

    QImage newThumb(m_image->rows(), m_image->columns(), QImage::Format_RGB32);
    newThumb.loadFromData((const uchar*) blob.data(), blob.length());
    newThumb.scaled(128, 128, Qt::KeepAspectRatio).save(dir.absoluteFilePath(getThumbName()));
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    return;
  }

  delete m_image;
  m_image = 0;

  loadImage();
}

void ImageItem::closeImage()
{
  if (!m_image)
    return;

  delete m_image;
  m_image = 0;
}

void ImageItem::prepareForEdit(bool open)
{
  if (open) {
    loadImage();
  } else {
    if (m_changes)
      saveImage();
    closeImage();
  }
}

void ImageItem::saveRotation()
{
  if (!m_image)
    return;

  QDir dir(getFilePath());
  dir.cdUp();

  try {
    m_image->write(getFilePath().toStdString());

    Magick::Blob blob;
    m_image->write(&blob);

    dir.cd(".thumbnails");

    QImage newThumb(m_image->rows(), m_image->columns(), QImage::Format_RGB32);
    newThumb.loadFromData((const uchar*) blob.data(), blob.length());
    newThumb.scaled(128, 128, Qt::KeepAspectRatio).save(dir.absoluteFilePath(getThumbName()));
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    return;
  }
}

}
