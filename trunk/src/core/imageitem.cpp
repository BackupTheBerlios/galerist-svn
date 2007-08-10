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
#include "imageitem.h"

#include <QtCore/QTimer>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QTemporaryFile>

#include <QtGui/QImage>
#include <QtGui/QApplication>

#include <Magick++.h>

#include "core/data.h"
#include "core/imagemodel.h"
#include "core/metadatamanager.h"

#include <QtDebug>

namespace GCore
{

ImageItem::ImageItem(const QString &path, ImageItem *parent, Type type)
    : m_parentItem(parent),
    m_type(type),
    m_path(path),
    m_metadata(0),
    m_id(-1),
          m_tempImage(0)
{
  if (m_type == Image)
    m_id = metadata()->imageId(fileName());
  else
    m_metadata = new MetaDataManager(filePath(), qApp);
}

ImageItem::~ImageItem()
{
  if (!m_childItems.isEmpty() && m_type != Image)
    qDeleteAll(m_childItems.begin(), m_childItems.end());
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
}

QString ImageItem::path() const
{
  QFileInfo currentPath(filePath());
  return currentPath.absolutePath();
}

QString ImageItem::filePath() const
{
  if (!m_parentItem) {
    return m_path;
  } else {
    QDir parentPath(m_parentItem->filePath());
    return parentPath.absoluteFilePath(m_path);
  }
}

QString ImageItem::fileName() const
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

  if (m_type == Gallery)
    output = m_metadata;

  if (!m_metadata && m_type == Image)
    output = parent()->metadata();

  return output;
}

QString ImageItem::name() const
{
  if (m_type == Image)
    return metadata()->name(m_id);
  else
    return fileName();
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

  return output;
}

void ImageItem::setDescription(const QString &description)
{
  if (m_type != Image)
    return;

  metadata()->setDescription(description, m_id);

}

bool ImageItem::remove()
{
  return metadata()->removePicture(m_id);
}

void ImageItem::rotate(short degrees)
{
  QString file = filePath();
  Magick::Image image;
  image.read(file.toStdString());
  image.rotate(90);
  image.write(file.toStdString());

  Magick::Blob buffer;
  image.write(&buffer);

  QImage thumbnail;
  thumbnail.loadFromData((const uchar*) buffer.data(), buffer.length());
  thumbnail.save(thumbPath(), "JPEG");
}

QImage ImageItem::previewCW()
{
  Magick::Image image;
  if (m_tempImage) {
    m_tempImage->open();
    image.read(m_tempImage->fileName().toStdString());
    image.magick("jpg");
  } else {
    image.read(filePath().toStdString());
    m_tempImage = new QTemporaryFile(qApp);
    m_tempImage->open();
  }

  image.rotate(90);
  Magick::Blob buffer;
  image.write(&buffer);

  QImage output;
  output.loadFromData((const uchar*) buffer.data(), buffer.length());
  output.save(m_tempImage, "JPEG");
  m_tempImage->close();
  return output;
}

QImage ImageItem::previewCCW()
{
  Magick::Image image;
  if (m_tempImage) {
    m_tempImage->open();
    image.read(m_tempImage->fileName().toStdString());
    image.magick("jpg");
  } else {
    image.read(filePath().toStdString());
    m_tempImage = new QTemporaryFile(qApp);
    m_tempImage->open();
  }

  image.rotate(270);
  Magick::Blob buffer;
  image.write(&buffer);

  QImage output;
  output.loadFromData((const uchar*) buffer.data(), buffer.length());
  output.save(m_tempImage, "JPEG");
  m_tempImage->close();
  return output;
}

QString ImageItem::thumbName() const
{
  return fileName().append(".jpg");
}

QString ImageItem::thumbPath() const
{
  return path().append("/.thumbnails/").append(fileName()).append(".jpg");
}

}
