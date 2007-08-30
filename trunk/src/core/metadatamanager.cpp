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
#include <QtCore/QMutexLocker>

#include <QtGui/QApplication>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "core/data.h"
#include "core/imageitem.h"
#include "core/errorhandler.h"

#include <QtDebug>
#include <QtSql/QSqlError>

namespace GCore
{

MetaDataManager *MetaDataManager::m_self = 0;

MetaDataManager::MetaDataManager()
    : QObject(qApp)
{
  QSqlDatabase manifest = QSqlDatabase::addDatabase("QSQLITE");
  manifest.setDatabaseName(Data::self()->galleriesDir().absoluteFilePath("manifest.db"));
  manifest.open();

  manifest.exec("CREATE TABLE IF NOT EXISTS gallery (id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(100) UNIQUE, parent_id INTEGER);");
  manifest.exec("CREATE TABLE IF NOT EXISTS image (id INTEGER PRIMARY KEY AUTOINCREMENT, gallery_id INTEGER, name VARCHAR(150) UNIQUE, file_name VARCHAR(150), description TEXT);");
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

  QMutexLocker locker(&m_locker);

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

  QMutexLocker locker(&m_locker);

  QSqlQuery query;
  query.prepare("INSERT INTO image (gallery_id, name, file_name) VALUES (:galleryId, :name, :fileName)");
  query.bindValue(":galleryId", galleryId);
  query.bindValue(":name", fileName);
  query.bindValue(":fileName", fileName);
  qDebug() << query.exec() << query.lastError().text();
  

  ImageItem *item = new ImageItem(query.lastInsertId().toInt(), ImageItem::Image);
  item->setParentId(galleryId);
  return item;
}

void MetaDataManager::unregisterGallery(int id, int parentId)
{
  QMutexLocker locker(&m_locker);

  QList<int> galleriesList;
  galleriesList << id;

  QQueue<int> queue;
  queue.enqueue(id);

  while (!queue.isEmpty()) {
  QSqlQuery galleries;
  galleries.prepare("SELECT id FROM gallery WHERE parent_id = ?");
  galleries.addBindValue(queue.dequeue());
  galleries.exec();

  while (galleries.next()) {
    queue.enqueue(galleries.value(0).toInt());
    galleriesList << galleries.value(0).toInt();
  }
  }

  foreach (int galleryId, galleriesList) {
    QSqlQuery query;
  query.prepare("DELETE FROM gallery WHERE id = ?");
  query.addBindValue(galleryId);
  query.exec();

  query.prepare("DELETE FROM image WHERE gallery_id = ?");
  query.addBindValue(galleryId);
  query.exec();
  }
}

void MetaDataManager::unregisterImage(int id, int galleryId)
{
  QMutexLocker locker(&m_locker);

  QSqlQuery query;
  query.prepare("DELETE FROM image WHERE id = ? AND gallery_id = ?");
  query.addBindValue(id);
  query.addBindValue(galleryId);
  query.exec();
}

int MetaDataManager::galleryId(const QString &name, int parentId)
{
  QMutexLocker locker(&m_locker);
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
  QMutexLocker locker(&m_locker);
  QSqlQuery query;
  query.prepare("SELECT id FROM image WHERE (name = ? OR file_name = ?) AND gallery_id = ?;");
  query.addBindValue(name);
  query.addBindValue(name);
  query.addBindValue(galleryId);
  query.exec();

  if (query.next())
    return query.value(0).toInt();

  return -1;
}

QString MetaDataManager::fileName(int id) const
{
  QMutexLocker locker(&m_locker);
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
  QMutexLocker locker(&m_locker);
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
  QMutexLocker locker(&m_locker);
  QSqlQuery query;
  query.prepare("SELECT name FROM image WHERE id = ?;");
  query.addBindValue(id);
  query.exec();

  if (query.next())
    return query.value(0).toString();

  return QString();
}

QStringList MetaDataManager::imageList(int galleryId) const
{
  QMutexLocker locker(&m_locker);
  QSqlQuery query;
  query.prepare("SELECT name FROM image WHERE gallery_id = ?");
  query.addBindValue(galleryId);
  query.exec();

  QStringList output;

  while (query.next())
    output << query.value(0).toString();

  return output;
}

QStringList MetaDataManager::galleryList() const
{
  QMutexLocker locker(&m_locker);
  QSqlQuery query;
  query.prepare("SELECT name FROM gallery");
  query.exec();

  QStringList output;

  while (query.next())
    output << query.value(0).toString();

  return output;
}

bool MetaDataManager::setGalleryName(int id, const QString &name, const QString &path) const
{
  QDir gallery(path);
  if (!gallery.rename(galleryName(id), name))
    return false;

  QMutexLocker locker(&m_locker);

  QSqlQuery query;
  query.prepare("UPDATE gallery SET name = ? WHERE id = ?;");
  query.addBindValue(name);
  query.addBindValue(id);
  query.exec();

  return true;
}

void MetaDataManager::setImageName(int id, const QString &name) const
{
  QMutexLocker locker(&m_locker);
  QSqlQuery query;
  query.prepare("UPDATE image SET name = ? WHERE id = ?;");
  query.addBindValue(name);
  query.addBindValue(id);
  query.exec();
}

QString MetaDataManager::imageDescription(int id) const
{
  QMutexLocker locker(&m_locker);
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
  QMutexLocker locker(&m_locker);
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
  QMutexLocker locker(&m_locker);
  QSqlQuery query;
  query.prepare("SELECT id FROM gallery WHERE id = ?;");
  query.addBindValue(galleryId);
  query.exec();

  return query.next();
}

bool MetaDataManager::imageExists(const QString &name, int galleryId)
{
  QMutexLocker locker(&m_locker);
  QSqlQuery query;
  query.prepare("SELECT id FROM image WHERE name = ? AND gallery_id = ?;");
  query.addBindValue(name);
  query.addBindValue(galleryId);
  query.exec();

  return query.next();
}

ImageItem *MetaDataManager::readManifest()
{
  QMutexLocker locker(&m_locker);
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
