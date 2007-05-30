/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÅ¡nik                                 *
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

#include <QtGui/QImage>

#include "core/data.h"
#include "core/imagemodel.h"
#include "core/metadatamanager.h"

#include "core/jobs/transformationjob.h"

namespace GCore
{

ImageItem::ImageItem(const QString &path, ImageItem *parent, Type type)
    : QObject(0),
    m_parentItem(parent),
    m_type(type),
    m_path(path),
    m_metadata(0),
    m_id(-1),
    m_transformator(0)
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

  if (m_transformator) {
    m_transformator->stop();
    m_transformator->wait();
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
  if (!m_transformator)
    return;

  m_transformator->rotateImage(90);
}

void ImageItem::rotateCCW()
{
  if (!m_transformator)
    return;

  m_transformator->rotateImage(270);
}

void ImageItem::crop(const QRect &area)
{
  if (!m_transformator)
    return;

  m_transformator->cropImage(area);
}

void ImageItem::blur(int blurFilters)
{
  if (!m_transformator)
    return;

  m_transformator->blurImage(blurFilters);
}

void ImageItem::sharpen(int sharpenFilters)
{
  if (!m_transformator)
    return;

  m_transformator->sharpenImage(sharpenFilters);
}

void ImageItem::resize(const QSize &size)
{
  if (!m_transformator)
    return;

  m_transformator->resizeImage(size);
}

void ImageItem::cancelTransformations()
{
  if (!m_transformator)
    return;

  m_transformator->loadImage();
}

QString ImageItem::getThumbName() const
{
  return getFileName().remove(QRegExp("\\..+$")).append(".jpg");
}

void ImageItem::loadImage()
{
  if (!m_transformator) {
    m_transformator = new GJobs::TransformationJob(this);
    m_transformator->loadImage();
    m_transformator->start();

    connect(m_transformator, SIGNAL(completed(const QImage&)), this, SIGNAL(imageChanged(const QImage&)));
    connect(m_transformator, SIGNAL(preview(const QImage&)), this, SIGNAL(imageChanged(const QImage&)));
  }
}

void ImageItem::saveImage()
{
  if (!m_transformator)
    return;

  m_transformator->saveImage();
}

void ImageItem::closeImage()
{
  if (m_transformator) {
    disconnect(m_transformator, SIGNAL(completed(const QImage&)), this, SIGNAL(imageChanged(const QImage&)));

    m_transformator->stop();
    m_transformator = 0;
  }
}

void ImageItem::prepareForEdit(bool open)
{
  if (open) {
    loadImage();
  } else {
    closeImage();
  }
}

}
