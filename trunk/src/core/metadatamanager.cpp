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

#include "metadatamanager.h"

#include <QtSql>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "core/data.h"
#include "core/imageitem.h"
#include "core/errorhandler.h"

namespace GCore
{

MetaDataManager::MetaDataManager(const QString &galleryPath, QObject *parent)
    : QObject(parent)
{
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
}

int MetaDataManager::addImage(const QString &filename) const
{
  QVariant id = m_metadataFile.exec("INSERT INTO image (filename) VALUES ('" + filename + "');").lastInsertId();

  if (id.isValid())
    return id.toInt();
  else
    return -1;
}

int MetaDataManager::imageId(const QString &filename) const
{
  int id;

  if (filename.isEmpty())
    return -1;

  QVariant result = query("SELECT image_id FROM image WHERE filename = '" + filename + "';");
  if (result.isValid()) {
    id = result.toInt();
  } else {
    id = addImage(filename);
  }

  return id;
}

QString MetaDataManager::name(int id) const
{
  return metadataInfo(id)["name"];
}

bool MetaDataManager::setName(const QString &name, int id) const
{
  bool status = true;

  if (!query("UPDATE image SET image_name = '" + name + "' WHERE image_id = '" + QString::number(id) + "';").toBool())
    status = false;

  return status;
}

bool MetaDataManager::checkName(const QString &name) const
{
  return query("SELECT image_id FROM image WHERE LOWER(image_name) = LOWER('" + name + "') OR LOWER(filename) = LOWER('" + name + "');").isValid();
}

QString MetaDataManager::description(int id) const
{
  return metadataInfo(id)["description"];
}

void MetaDataManager::setDescription(const QString &description, int id) const
{
  query("UPDATE image SET description = '" + description + "' WHERE image_id = '" + QString::number(id) + "';");
}

QMap<QString, QString> MetaDataManager::metadataInfo(int id) const
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

bool MetaDataManager::removePicture(int id) const
{
  bool status = true;

  QVariant result = query("DELETE FROM image WHERE image_id = '" + QString::number(id) + "';");
  if (result.isValid() && !result.toBool())
    status = false;

  return status;
}

QVariant MetaDataManager::query(const QString &rawQuery) const
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

}
