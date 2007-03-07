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

#include <QtCore/QString>
#include <QtCore/QtDebug>

#include "core/metadatamanager.h"

namespace GCore
{

ImageItem::ImageItem(const QString &path, ImageItem *parent, Type type)
    : m_parentItem(parent),
    m_type(type),
    m_path(path),
    m_metadata(0),
    m_id(-1)
{
  m_itemData << getFileName();

  if (type == Image)
    m_itemData << "Description";
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

ImageItem *ImageItem::child(int row)
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
    return m_parentItem->m_childItems.indexOf(const_cast<ImageItem*> (this));

  return 0;
}

int ImageItem::columnCount() const
{
  return m_itemData.count();
}

QVariant ImageItem::data(int column) const
{
  return m_itemData.value(column);
}

ImageItem *ImageItem::parent()
{
  return m_parentItem;
}

ImageItem::Type ImageItem::imageType()
{
  return m_type;
}

void ImageItem::setFilePath(const QString &path)
{
  m_path = path;
}

QString ImageItem::getFilePath()
{
  if (!m_parentItem) {
    return m_path;
  } else {
    QDir parentPath(m_parentItem->getFilePath());
    return parentPath.absoluteFilePath(m_path);
  }
}

QString ImageItem::getFileName()
{
  if (!m_parentItem) {
    // There is no parent item, so path contains the full path
    QFileInfo currentPath(m_path);

    return currentPath.fileName();
  } else {
    return m_path;
  }
}

MetaDataManager *ImageItem::metadata()
{
  MetaDataManager *output = 0;
  if (!m_metadata && m_type == Gallery)
    output = m_metadata = new MetaDataManager(getFilePath());

  if (m_type == Gallery)
    output = m_metadata;

  if (!m_metadata && m_type == Image) {
    output = parent()->metadata();
    m_id = output->imageId(m_fileName);
  }

  return output;
}

QString ImageItem::name()
{
  if (m_type == Image && (m_id == -1 || m_itemData.at(0).toString() == QObject::tr("Unavailable"))) {
    m_id = metadata()->imageId(getFileName());

    // Update the metadata
    m_itemData.clear();
    m_itemData << metadata()->name(m_id);
    m_itemData << metadata()->description(m_id);
  }

  // We return the name
  return m_itemData.at(0).toString();
}

QString ImageItem::description()
{
  if (m_type == Image && (m_id == -1 || m_itemData.at(1).toString() == QObject::tr("No description"))) {
    m_id = metadata()->imageId(getFileName());

    // Update the metadata
    m_itemData.clear();
    m_itemData << metadata()->name(m_id);
    m_itemData << metadata()->description(m_id);
  }

  // We return the description
  return m_itemData.at(1).toString();
}

bool ImageItem::setName(const QString &name)
{
  if (imageType() == Image) {
    if (metadata()->setName(name, m_id)) {
      m_itemData.removeFirst();
      m_itemData.prepend(name);
      return true;
    }
  } else if (imageType() == Gallery) {
    m_itemData.removeFirst();
    m_itemData.prepend(name);
    setFilePath(name);
    return true;
  }

  return false;
}

void ImageItem::setDescription(const QString &description)
{
  metadata()->setDescription(description, m_id);

  m_itemData.removeLast();
  m_itemData.append(description);
}

bool ImageItem::remove()
{
  return metadata()->removePicture(m_id);
}

QString ImageItem::getThumbName()
{
  return getFileName().remove(QRegExp("\\..+$")).append(".jpg");
}

}
