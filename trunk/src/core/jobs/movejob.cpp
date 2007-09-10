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
#include "movejob.h"

#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QQueue>

#include <QtGui/QImage>

#include "core/data.h"

#include <QtDebug>

namespace GCore
{

namespace GJobs
{

MoveJob::MoveJob(const QDir &destination, QObject *parent)
    : GCore::GJobs::AbstractJob(parent),
    m_destination(destination)
{}

MoveJob::~MoveJob()
{}

void MoveJob::job()
{
  QModelIndexList images;
  QQueue<QModelIndex> queue;
  queue << Data::self()->imageModel()->indexList(ImageItem::Gallery);

  QString sourcePath = queue.at(0).data(ImageModel::ImagePathRole).toString();

  while (!queue.isEmpty()) {
    QModelIndexList currentList = Data::self()->imageModel()->indexList(ImageItem::Image, queue.dequeue());
    foreach(QModelIndex current, currentList) {
      if (current.data(ImageModel::ImageTypeRole).toInt() == ImageItem::Image)
        images << current;
    }
  }

  int total = 1;

  foreach (QModelIndex image, images)
    total++;

  int moved = 1;
  foreach(QModelIndex imageIndex, images) {
    if (freeze())
      break;

    m_destination.mkpath(imageIndex.data(ImageModel::RelativePathRole).toString());
    m_destination.mkpath(imageIndex.data(ImageModel::RelativeThumbnailsPathRole).toString());

    if (!QFile::copy(imageIndex.data(ImageModel::ImageFilepathRole).toString(), m_destination.absoluteFilePath(imageIndex.data(ImageModel::RelativeFilePathRole).toString()))) {
      emit failed(tr("Galleries folder move failed!"), ErrorHandler::Critical);
      break;
    }

    QFile::copy(imageIndex.data(ImageModel::ImageThumbnailPathRole).toString(), m_destination.absoluteFilePath(imageIndex.data(ImageModel::RelativeThumbnailPathRole).toString()));

    QString galleryName = imageIndex.parent().data(ImageModel::ImageNameRole).toString();

    emit directoryProgress(galleryName);
    emit progress(moved, total, imageIndex.data(ImageModel::ImageNameRole).toString(), QImage(imageIndex.data(ImageModel::ImageThumbnailPathRole).toString()));
    moved++;
  }

  bool successful = total == moved;

  if (successful) {
    QFile::copy(Data::self()->galleriesDir().absoluteFilePath("manifest.db"), m_destination.absoluteFilePath("manifest.db"));
    QFile::remove(Data::self()->galleriesDir().absoluteFilePath("manifest.db"));
    if (deleteOldGalleries(images))
      emit failed(tr("Old Gallery directory couldn't be deleted."), GCore::ErrorHandler::Warning);
  }

  emit finished(successful);
}

bool MoveJob::deleteOldGalleries(const QModelIndexList &indexes) const
{
  foreach (QModelIndex index, indexes) {
    QString filePath = index.data(ImageModel::ImageFilepathRole).toString();
    QString thumbPath = index.data(ImageModel::ImageThumbnailPathRole).toString();
    QString path = index.data(ImageModel::RelativePathRole).toString();

    if (!QFile::remove(filePath))
      return true;

    QFile::remove(thumbPath);

    Data::self()->galleriesDir().rmpath(path + "/thumbnails");
    Data::self()->galleriesDir().rmpath(path);
  }

  return false;
}

}

}
