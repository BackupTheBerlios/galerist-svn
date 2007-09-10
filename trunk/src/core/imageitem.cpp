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

ImageItem::ImageItem(int id, Type type)
    : m_parentItem(0),
    m_type(type),
    m_id(id),
    m_tempImage(0)
{}

ImageItem::~ImageItem()
{
  if (!m_childItems.isEmpty())
    qDeleteAll(m_childItems);
}

int ImageItem::id() const
{
  return m_id;
}

void ImageItem::setParent(ImageItem *parent)
{
  m_parentItem = parent;
}

void ImageItem::setParentId(int id)
{
  m_parentId = id;
}

void ImageItem::appendChild(ImageItem *item)
{
  item->setParent(this);
  if (item->type() == Image)
    m_childItems.append(item);
  else
    m_childItems.prepend(item);
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

ImageItem *ImageItem::parent() const
{
  return m_parentItem;
}

int ImageItem::parentId() const
{
  return m_parentId;
}

ImageItem::Type ImageItem::type() const
{
  return m_type;
}

QDir ImageItem::dir() const
{
  QDir dir = Data::self()->galleriesDir();
  QString relativePath = path();
  if (relativePath != "/")
    dir.cd(relativePath);
  return dir;
}

QString ImageItem::path() const
{
  QStringList names;

  ImageItem *parent = m_parentItem;
  while (parent && parent->type() != Root) {
    names.prepend(parent->name());
    parent = parent->parent();
  }

  return names.join("/") + "/";
}

QString ImageItem::absolutePath() const
{
  return dir().path();
}

QString ImageItem::filePath() const
{
  return path() + fileName();
}

QString ImageItem::absoluteFilePath() const
{
  return dir().absoluteFilePath(fileName());
}

QString ImageItem::fileName() const
{
  if (m_type != Image) {
    qWarning("GCore::ImageItem::fileName: Gallery does not have a file name!");
    return QString();
  }

  return MetaDataManager::self()->fileName(m_id);
}

QDir ImageItem::thumbDir() const
{
  QDir thumbnails = QDir(Data::self()->galleriesDir());
  thumbnails.cd(path());
  thumbnails.cd("thumbnails");
  return thumbnails;
}

QString ImageItem::thumbPath() const
{
  return Data::self()->galleriesDir().relativeFilePath(path() + "thumbnails/" + thumbName());
}

QString ImageItem::absoluteThumbPath() const
{
  return thumbDir().absoluteFilePath(thumbName());
}

QString ImageItem::thumbName() const
{
  return fileName().append(".jpg");
}

QString ImageItem::name() const
{
  if (m_type == Image)
    return MetaDataManager::self()->imageName(m_id);

  if (m_type == Gallery)
    return MetaDataManager::self()->galleryName(m_id);

  return "Root";
}

QString ImageItem::description() const
{
  if (m_type == Image)
    return MetaDataManager::self()->imageDescription(m_id);
  else
    return QString();
}

bool ImageItem::setName(const QString &name)
{
  if (m_type == Image) {
    MetaDataManager::self()->setImageName(m_id, name);
    return true;
  }

  if (m_type == Gallery)
    return MetaDataManager::self()->setGalleryName(m_id, name, absolutePath());

  return false;
}

void ImageItem::setDescription(const QString &description)
{
  if (m_type != Image)
    return;

  MetaDataManager::self()->setDescription(m_id, description);
}

void ImageItem::rotate(short degrees)
{
  if (!degrees)
    return;

  QString file = absoluteFilePath();
  Magick::Image image;
  image.read(file.toStdString());
  image.rotate(degrees);
  image.write(file.toStdString());

  Magick::Blob buffer;
  image.write(&buffer);

  QImage thumbnail;
  thumbnail.loadFromData((const uchar*) buffer.data(), buffer.length());
  thumbnail.save(absoluteThumbPath(), "JPEG");
}

QImage ImageItem::previewRotate(short degrees)
{
  Magick::Image image;
  if (m_tempImage) {
    m_tempImage->open();
    image.read(m_tempImage->fileName().toStdString());
    image.magick("jpg");
  } else {
    image.read(absoluteFilePath().toStdString());
    m_tempImage = new QTemporaryFile(qApp);
    m_tempImage->open();
  }

  image.rotate(degrees);
  Magick::Blob buffer;
  image.write(&buffer);

  QImage output;
  output.loadFromData((const uchar*) buffer.data(), buffer.length());
  output.save(m_tempImage, "JPEG");
  m_tempImage->close();
  return output;
}

}
