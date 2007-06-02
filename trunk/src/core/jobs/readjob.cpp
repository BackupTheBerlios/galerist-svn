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
#include "readjob.h"

#include "core/data.h"
#include "core/errorhandler.h"
#include "core/imageitem.h"
#include "core/imagemodel.h"

#include <QtCore/QDir>
#include <QtGui/QImage>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QAbstractItemModel>

namespace GCore
{

namespace GJobs
{

ReadJob::ReadJob(const QAbstractItemModel *model)
    : GCore::GJobs::AbstractJob(const_cast<QAbstractItemModel*>(model)),
    m_model(model)
{}

ReadJob::ReadJob(QObject *parent, const QDir &path, bool recursive)
    : GCore::GJobs::AbstractJob(parent),
    m_model(0),
    m_path(path),
    m_recursive(recursive)
{}

ReadJob::ReadJob(QObject *parent, const QDir &path, const QStringList &images, bool recursive)
    : GCore::GJobs::AbstractJob(parent),
    m_model(0),
    m_path(path),
    m_recursive(recursive),
    m_images(images)
{}

QModelIndex ReadJob::getItem()
{
  return QModelIndex();
}

void ReadJob::queuePhoto(const QModelIndex &item)
{
  m_items << item;
}

void ReadJob::job()
{
  if (m_model) {
    QModelIndex item;
    while (!m_items.isEmpty()) {
      if (getStop())
        break;

      // Let's use our first item
      item = m_items.takeFirst();

      // We set gallery path.
      QDir galleryPath(m_model->data(item, GCore::ImageModel::ImageDirPathRole).toString());
      //galleryPath.cdUp();

      // We set the name of the file.
      QString fileName(m_model->data(item, GCore::ImageModel::ImageFilenameRole).toString());
      //fileName.remove(QDir::toNativeSeparators(galleryPath.absolutePath() + "/"));

      // We make sure, the others know what we are doing!
      emit signalThumb(fileName);

      // Thumbnails path.
      QDir thumbPath(galleryPath);
      if (!thumbPath.cd(".thumbnails")) {
        thumbPath.mkdir(".thumbnails");
        thumbPath.cd(".thumbnails");
      }

      // Create the thumbnail.
      QImage thumbnail = QImage(QDir::toNativeSeparators(galleryPath.absoluteFilePath(fileName))).scaled(128, 128, Qt::KeepAspectRatio);
      thumbnail.save(QDir::toNativeSeparators(thumbPath.absoluteFilePath(fileName.remove(QRegExp("\\..+$")).append(".jpg"))), "JPG");

      // We report about our achievement.
      emit signalThumb(QString());
      emit signalProcessed(item);

      if (getStop())
        break;
    }
  } else {
    readPath(m_path);
    emit signalProgress("", QImage());
  }
}

void ReadJob::readPath(const QDir &path)
{
  if (!path.exists() || getStop())
    return;

  QImage imageThumbnail;

  if (m_recursive) {
    QStringList items = path.entryList(QDir::Dirs);
    items.removeAll(".");
    items.removeAll("..");

    foreach(QString item, items) {
      QDir temp = path;
      temp.cd(item);
      readPath(temp);
      if (getStop())
        return;
    }
  }

  QStringList images = path.entryList(QDir::Files);
  if (!m_images.isEmpty())
    images = m_images;
  foreach(QString image, images) {
    if (getStop())
      return;

    if (!image.contains(GCore::Data::self()->getSupportedFormats()) || QFileInfo(path, image).size() == 0)
      continue;

    imageThumbnail = QImage(path.absoluteFilePath(image)).scaled(128, 128, Qt::KeepAspectRatio);

    emit signalProgress(image, imageThumbnail, path.absolutePath());
  }
}

ReadJob::~ReadJob()
{}

}

}
