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

#include "metadatamanager.h"

#include <QtCore/QQueue>

#include <QtGui/QApplication>

#include <QtSql/QSqlDatabase>

#include "core/data.h"
#include "core/imageitem.h"
#include "core/errorhandler.h"

namespace GCore
{

MetaDataManager *MetaDataManager::m_self = 0;

MetaDataManager::MetaDataManager()
    : QObject(qApp)
{
  QSqlDatabase manifest = QSqlDatabase::addDatabase("QSQLITE");
  manifest.setDatabaseName(Data::self()->galleriesDir().absoluteFilePath("manifest.db"));
  manifest.open();

  manifest.exec("CREATE TABLE IF NOT EXISTS gallery (id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(100), parent_id INTEGER);");
  manifest.exec("CREATE TABLE IF NOT EXISTS image (id INTEGER PRIMARY KEY AUTOINCREMENT, gallery_id INTEGER, name VARCHAR(150), file_name VARCHAR(150), description TEXT);");
}

bool MetaDataManager::driverAvailable()
{
  return QSqlDatabase::isDriverAvailable("QSQLITE");
}

MetaDataManager *MetaDataManager::self()
{
  if (!m_self)
    m_self = new MetaDataManager;

  return m_self;
}

ImageItem *MetaDataManager::registerGallery(const QString &name, int parentId)
{
  if (galleryExists(name, parentId))
    qFatal("GCore::MetaDataManager::registerGallery: Tried to register a gallery with an existing name!");

  QSqlQuery query;
  query.prepare("INSERT INTO gallery (name, parent_id) VALUES (:name, :parentId)");
  query.bindValue(":name", name);
  query.bindValue(":parentId", parentId);
  query.exec();

  ImageItem *item = new ImageItem(query.lastInsertId().toInt(), ImageItem::Gallery);
  item->setParentId(parentId);
  return item;
}

ImageItem *MetaDataManager::registerImage(const QString &fileName, int galleryId)
{
  if (!galleryExists(galleryId))
    qFatal("GCore::MetaDataManager::registerImage: Tried to register an image with invalid galleryId. Terminating!");

  if (imageExists(fileName, galleryId)){
    qFatal("GCore::MetaDataManager::registerImage: Tried to register an image with an existing name!");
    return 0;
  }

  QSqlQuery query;
  query.prepare("INSERT INTO image (gallery_id, name, file_name) VALUES (:galleryId, :name, :fileName)");
  query.bindValue(":galleryId", galleryId);
  query.bindValue(":name", fileName);
  query.bindValue(":fileName", fileName);
  query.exec();

  ImageItem *item = new ImageItem(query.lastInsertId().toInt(), ImageItem::Image);
  item->setParentId(galleryId);
  return item;
}

int MetaDataManager::galleryId(const QString &name, int parentId)
{
  QSqlQuery query;
  query.prepare("SELECT id FROM gallery WHERE name = ? AND parent_id = ? LIMIT 1;");
  query.addBindValue(name);
  query.addBindValue(parentId);
  query.exec();

  if (query.next())
    return query.value(0).toInt();

  return -1;
}

int MetaDataManager::imageId(const QString &name, int galleryId)
{
  QSqlQuery query;
  query.prepare("SELECT id FROM image WHERE name = ? AND gallery_id = ?;");
  query.addBindValue(name);
  query.addBindValue(galleryId);
  query.exec();

  if (query.next())
    return query.value(0).toInt();

  return -1;
}

QString MetaDataManager::fileName(int id) const
{
  QSqlQuery query;
  query.prepare("SELECT file_name FROM image WHERE id = ?;");
  query.addBindValue(id);
  query.exec();

  if (query.next())
    return query.value(0).toString();

  return QString();
}

QString MetaDataManager::galleryName(int id) const
{
  QSqlQuery query;
  query.prepare("SELECT name FROM gallery WHERE id = ?;");
  query.addBindValue(id);
  query.exec();

  if (query.next())
    return query.value(0).toString();

  return QString();
}

QString MetaDataManager::imageName(int id) const
{
  QSqlQuery query;
  query.prepare("SELECT name FROM image WHERE id = ?;");
  query.addBindValue(id);
  query.exec();

  if (query.next())
    return query.value(0).toString();

  return QString();
}

bool MetaDataManager::setGalleryName(int id, const QString &name, const QString &path) const
{
  QDir gallery(path);
  if (!gallery.rename(galleryName(id), name))
    return false;

  QSqlQuery query;
  query.prepare("UPDATE gallery SET name = ? WHERE id = ?;");
  query.addBindValue(name);
  query.addBindValue(id);
  query.exec();

  return true;
}

void MetaDataManager::setImageName(int id, const QString &name) const
{
  QSqlQuery query;
  query.prepare("UPDATE image SET name = ? WHERE id = ?;");
  query.addBindValue(name);
  query.addBindValue(id);
  query.exec();
}

QString MetaDataManager::imageDescription(int id) const
{
  QSqlQuery query;
  query.prepare("SELECT description FROM image WHERE id = ?;");
  query.addBindValue(id);
  query.exec();

  if (query.next())
    return query.value(0).toString();

  return QString();
}

void MetaDataManager::setDescription(int id, const QString &description) const
{
  QSqlQuery query;
  query.prepare("UPDATE image SET description = ? WHERE id = ?;");
  query.addBindValue(description);
  query.addBindValue(id);
  query.exec();
}

bool MetaDataManager::galleryExists(const QString &fileName, int parentId)
{
  return galleryId(fileName, parentId) != -1;
}

bool MetaDataManager::galleryExists(int galleryId)
{
  QSqlQuery query;
  query.prepare("SELECT id FROM gallery WHERE id = ?;");
  query.addBindValue(galleryId);
  query.exec();

  return query.next();
}

bool MetaDataManager::imageExists(const QString &name, int galleryId)
{
  QSqlQuery query;
  query.prepare("SELECT id FROM image WHERE name = ? AND galleryId = ?;");
  query.addBindValue(name);
  query.addBindValue(galleryId);
  query.exec();

  return query.next();
}

ImageItem *MetaDataManager::readManifest()
{
  ImageItem *root = new ImageItem(-1, ImageItem::Root);

  QQueue<ImageItem*> queue;
  QList<ImageItem*> galleryList;
  galleryList << root;

  QSqlQuery galleries("SELECT id, parent_id FROM gallery;");

  while (galleries.next()) {
    int id = galleries.value(0).toInt();
    int parentId = galleries.value(1).toInt();

    ImageItem *gallery = new ImageItem(id, ImageItem::Gallery);

    foreach (ImageItem *item, galleryList) {
      if (item->id() == parentId) {
        item->appendChild(gallery);
        break;
      }
    }

    galleryList << gallery;

    if (!gallery->parent())
      queue.enqueue(gallery);

    imageList(gallery);
  }

  while (!queue.isEmpty()) {
    ImageItem *gallery = queue.dequeue();
    foreach (ImageItem *item, galleryList) {
      if (item->id() == gallery->id()) {
        item->appendChild(gallery);
        break;
      }
    }
  }

  return root;
}

void MetaDataManager::imageList(ImageItem *gallery) const
{
  QSqlQuery query("SELECT id FROM image WHERE gallery_id = " + QString::number(gallery->id()) + ";");

  while (query.next()) {
    ImageItem *image = new ImageItem(query.value(0).toInt(), ImageItem::Image);
    gallery->appendChild(image);
  }
}

}
