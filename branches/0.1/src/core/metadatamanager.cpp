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

#include "metadatamanager.h"

#include <QtSql>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include <QtCore/QtDebug>

#include "core/data.h"
#include "core/imageitem.h"
#include "core/errorhandler.h"

namespace GCore
{

MetaDataManager::MetaDataManager(const QString &galleryPath, QObject *parent)
    : QObject(parent),
    m_timerId(0)
{
  // We initialise the insertion timer
  m_timerId = startTimer(1);

  // We initialise the database.
  QDir dir(galleryPath);
  m_galleryName = galleryPath;
  m_galleryName.remove(QDir::homePath() + "/");

  m_metadataFile = QSqlDatabase::addDatabase("QSQLITE", m_galleryName);
  m_metadataFile.setDatabaseName(dir.absoluteFilePath(".metadata"));

  // If it doesn't exists yet. We create the internal structure.
  if (!QFile::exists(dir.absoluteFilePath(".metadata"))) {
    m_metadataFile.open();
    m_metadataFile.transaction();
    m_metadataFile.exec("CREATE TABLE image (image_id INTEGER PRIMARY KEY AUTOINCREMENT, filename CHAR(30) UNIQUE, image_name CHAR(30) UNIQUE, description TEXT);");
    m_metadataFile.exec("CREATE TABLE tag (tag_id INTEGER PRIMARY KEY AUTOINCREMENT, tag_name CHAR(20) UNIQUE NOT NULL);");
    m_metadataFile.exec("CREATE TABLE phototag (tag_id INTEGER NOT NULL, image_id INTEGER NOT NULL, PRIMARY KEY(tag_id, image_id));");
    m_metadataFile.commit();
  } else
    m_metadataFile.open();

  // Prepared SQL statments.
  /*m_getMetadata = QSqlQuery(m_metadataFile);
  m_getMetadata.prepare("SELECT image_name, description, filename FROM image WHERE image_id = :imageId;");

  m_getImageId = QSqlQuery(m_metadataFile);
  m_getImageId.prepare("SELECT image_id FROM image WHERE filename = :filename;");

  m_getTagId = QSqlQuery(m_metadataFile);
  m_getTagId.prepare("SELECT tag_id FROM tag WHERE tag_name = :tagName;");

  m_addImage = QSqlQuery(m_metadataFile);
  m_addImage.prepare("INSERT INTO image (filename) VALUES (:filename);");

  m_addTag = QSqlQuery(m_metadataFile);
  m_addTag.prepare("BEGIN; INSERT INTO tag (tag_name) VALUES (:tagName); INSERT INTO phototag (tag_id, image_id) VALUES (:tagId, :nameId); COMMIT;");

  m_changeName = QSqlQuery(m_metadataFile);
  m_changeName.prepare("UPDATE image SET image_name = :imageName WHERE image_id = :imageId;");

  m_changeDescription = QSqlQuery(m_metadataFile);
  m_changeDescription.prepare("UPDATE image SET description = :imageDescription WHERE image_id = :imageId;");

  m_insertTag = QSqlQuery(m_metadataFile);
  m_insertTag.prepare("INSERT INTO phototag (tag_id, image_id) VALUES(:tagId, :imageId);");

  m_tagExists = QSqlQuery(m_metadataFile);
  m_tagExists.prepare("SELECT tag_id FROM tag WHERE tag_id = :tagId;");

  m_removeTag = QSqlQuery(m_metadataFile);
  m_removeTag.prepare("DELETE FROM phototag WHERE tag_id = :tagId AND image_id = :imageId;");

  m_deleteImage = QSqlQuery(m_metadataFile);
  m_deleteImage.prepare("DELETE FROM image WHERE image_id = :imageId;");

  m_deleteTag = QSqlQuery(m_metadataFile);
  m_deleteTag.prepare("DELETE FROM tag WHERE tag_id = :tagId;");*/

}

void MetaDataManager::addImage(const QString &filename)
{
  if (!m_pendingInsert.contains(filename))
    m_pendingInsert << filename;

  if (!m_timerId)
    m_timerId = startTimer(10);
}

int MetaDataManager::imageId(const QString &filename)
{
  int id;

  if (filename.isEmpty())
    return -1;

  QVariant result = query("SELECT image_id FROM image WHERE filename = '" + filename + "';");
  if (result.isValid()) {
    id = result.toInt();
  } else {
    id = -1;
    addImage(filename);
  }

  return id;
}

QString MetaDataManager::name(int id)
{
  return metadataInfo(id)["name"];
}

bool MetaDataManager::setName(const QString &name, int id)
{
  bool status = true;

  if (!query("UPDATE image SET image_name = '" + name + "' WHERE image_id = '" + QString::number(id) + "';").toBool())
    status = false;

  return status;
}

QString MetaDataManager::description(int id)
{
  return metadataInfo(id)["description"];
}

void MetaDataManager::setDescription(const QString &description, int id)
{
  query("UPDATE image SET description = '" + description + "' WHERE image_id = '" + QString::number(id) + "';");
}

QMap<QString, QString> MetaDataManager::metadataInfo(int id)
{
  QMap<QString, QString> metadata;

  QSqlQuery query("SELECT image_name, description, filename FROM image WHERE image_id = '" + QString::number(id) + "';", m_metadataFile);

  if (!query.next()) {
    query.clear();
    metadata["name"] = tr("Unavailable");
    metadata["description"] = tr("No description");
    return metadata;
  }

  metadata["name"] = query.value(0).toString();
  if (metadata["name"].isEmpty())
    metadata["name"] = query.value(2).toString();

  metadata["description"] = query.value(1).toString();
  if (metadata["description"].isEmpty())
    metadata["description"] = tr("No description");

  query.clear();

  return metadata;
}

bool MetaDataManager::checkName(const QString &name)
{
  return query("SELECT image_id FROM image WHERE LOWER(image_name) = LOWER('" + name + "') OR LOWER(filename) = LOWER('" + name + "');").isValid();
}

bool MetaDataManager::removePicture(int id)
{
  bool status = true;

  QVariant result = query("DELETE FROM image WHERE image_id = '" + QString::number(id) + "';");
  if (result.isValid() && !result.toBool())
    status = false;

  return status;
}

void MetaDataManager::slotInsert()
{
  if (m_pendingInsert.isEmpty())
    return;

  QStringList::const_iterator limit;

  m_metadataFile.transaction();

  limit = m_pendingInsert.constEnd();

  for (QStringList::const_iterator count = m_pendingInsert.constBegin(); count != limit; count++) {
    if ((*count).isEmpty())
      continue;
    query("INSERT INTO image (filename) VALUES ('" + *count + "');");
  }

  m_pendingInsert.clear();

  m_metadataFile.commit();

  emit signalInsert();
}

QVariant MetaDataManager::query(const QString &rawQuery)
{
  QVariant output;

  QSqlQuery query(m_metadataFile);
  if (!query.exec(rawQuery)) {
    qDebug() << query.lastError().text() << endl;
    output = false;
  } else {
    if (query.next())
      output = query.value(0);
  }

  return output;
}

bool MetaDataManager::driverAvailable()
{
  return QSqlDatabase::isDriverAvailable("QSQLITE");
}

MetaDataManager::~MetaDataManager()
{
  m_metadataFile.close();
}

void MetaDataManager::timerEvent(QTimerEvent*)
{
  if (m_timerId) {
    killTimer(m_timerId);
    m_timerId = 0;
  }

  slotInsert();
}

}
