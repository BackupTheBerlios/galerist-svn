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
#include "deletejob.h"

#include <QtCore/QQueue>

#include "core/imagemodel.h"
#include "core/metadatamanager.h"

namespace GCore
{

namespace GJobs
{

DeleteJob::DeleteJob(const QModelIndex &galleryIndex, QObject *parent)
    : GCore::GJobs::AbstractJob(parent),
    m_gallery(galleryIndex)
{}

DeleteJob::DeleteJob(const QModelIndexList &images, QObject *parent)
    : GCore::GJobs::AbstractJob(parent),
    m_images(images)
{}

void DeleteJob::job()
{
  if (m_images.isEmpty() && m_gallery.isValid())
    deleteGallery();
  else
    deleteImages();
}

void DeleteJob::deleteGallery()
{
  QDir galleryDir = m_gallery.data(ImageModel::ImagePathRole).toString();
  galleryDir.cd(m_gallery.data(ImageModel::ImageNameRole).toString());

  MetaDataManager::self()->unregisterGallery(m_gallery.data(ImageModel::IdRole).toInt(), m_gallery.parent().data(ImageModel::IdRole).toInt());

  QQueue<QDir> queue;
  queue.enqueue(galleryDir);

  QList<QFileInfo> images;

  emit remove(m_gallery);

  while (!queue.isEmpty()) {
    QDir temp = queue.dequeue();
    images << temp.entryInfoList(QDir::Files);

    foreach(QFileInfo gallery, temp.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      if (gallery.fileName() == "thumbnails")
        continue;
      QDir newGallery = gallery.dir();
      newGallery.cd(gallery.fileName());
      queue.enqueue(newGallery);
    }
  }

  int total = images.count();
  int deleted = 1;
  foreach(QFileInfo image, images) {
    QDir dir = image.dir();
    dir.cd("thumbnails");
    QString thumbnailName = image.fileName() + ".jpg";
    QImage thumbnail(dir.absoluteFilePath(thumbnailName));

    emit progress(deleted, total, image.fileName(), thumbnail);

    QFile::remove(image.absoluteFilePath());
    QFile::remove(dir.absoluteFilePath(thumbnailName));

    deleted++;
  }

  queue.enqueue(galleryDir);

  while (!queue.isEmpty()) {
    QDir temp = queue.dequeue();
    foreach(QFileInfo dir, temp.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
      QDir newGallery = dir.dir();
      if (!newGallery.rmdir(dir.fileName()) && QFile::exists(dir.absoluteFilePath())) {
        newGallery.cd(dir.fileName());
        queue.enqueue(newGallery);
      }
    }
    QString name = temp.dirName();
    temp.cdUp();
    if (!temp.rmdir(name)  && QFile::exists(temp.absoluteFilePath(name))) {
      temp.cd(name);
      queue.enqueue(temp);
    }
  }

  emit progress(total, total, QString(), QImage());
}

void DeleteJob::deleteImages()
{
  int total = m_images.count();
  int deleted = 1;
  foreach(QModelIndex image, m_images) {
    QString fileName = image.data(ImageModel::ImageFilepathRole).toString();
    QString thumbName = image.data(ImageModel::ImageThumbnailPathRole).toString();
    QString name = image.data(ImageModel::ImageNameRole).toString();
    QImage thumbnail(thumbName);

    emit progress(deleted, total, name, thumbnail);

    if (!QFile::remove(thumbName) || !QFile::remove(fileName)) {
          emit failed(tr("Couldn't deleted the image %1.").arg(name), ErrorHandler::Warning);
        }
    else {
      MetaDataManager::self()->unregisterImage(image.data(ImageModel::IdRole).toInt(), image.parent().data(ImageModel::IdRole).toInt());
      emit remove(image);
    }

    deleted++;
  }
}

}

}
