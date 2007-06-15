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

#include <QtGui/QImage>

#include <QtDebug>

namespace GCore
{

namespace GJobs
{

MoveJob::MoveJob(const QDir &source, const QDir &destination, QObject *parent)
    : GCore::GJobs::AbstractJob(parent),
    m_source(source),
    m_destination(destination)
{}

MoveJob::~MoveJob()
{}

void MoveJob::job()
{
  bool isSuccessful = true;

  if (!m_destination.mkpath(m_destination.absolutePath()) && !m_destination.exists()) {
    qDebug("Error creating directory.");
    emit signalFailed(tr("Cannot create destination directory %1.").arg(m_destination.absolutePath()), GCore::ErrorHandler::Critical);
    return;
  }
  long totalGalleryImages = -1;
  long totalImages = calculateImages();
  QString currentParent = 0;

  long gallerieImagesDone, imagesDone;
  gallerieImagesDone = imagesDone = 0;

  QDir destinationPath(m_destination);

  QDirIterator sourceIterator(m_source.absolutePath(), QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
  while (sourceIterator.hasNext()) {
    sourceIterator.next();
    QString item = sourceIterator.filePath();
    QDir path(item);
    path.cdUp();

    QString itemName = item;
    itemName.remove(path.absolutePath());

    QString parents = item;
    parents.remove(sourceIterator.path()).remove(itemName);

    itemName.remove('/');

    QFileInfo itemInfo(item);
    if (item.contains(".thumbnails") || item.contains(".metadata"))
      continue;

    destinationPath.cd(m_destination.absolutePath());

    foreach(QString parent, parents.split('/'))
    destinationPath.cd(parent);

    if (currentParent != parents) {
      currentParent = parents;
      gallerieImagesDone = 0;
      totalGalleryImages = calculateGallerieImages(parents);
    }

    if (itemInfo.isDir() && !item.contains(".thumbnails")) {
      destinationPath.mkdir(itemName);
      destinationPath.cd(itemName);
      path.cd(itemName);
      if (!QFile::copy(path.absoluteFilePath(".metadata"), destinationPath.absoluteFilePath(".metadata"))) {
        emit signalFailed(tr("Cannot move gallery %1.").arg(item), GCore::ErrorHandler::Critical);
        isSuccessful = false;
        break;
      }
      continue;
    }

    destinationPath.mkdir(".thumbnails");
    destinationPath.cd(".thumbnails");
    path.cd(".thumbnails");

    QString thumbnailName = itemName;
    thumbnailName.append(".jpg");
    QString thumbnailPath = path.absoluteFilePath(thumbnailName);

    // Notifying
    QImage thumbnail(thumbnailPath);
    emit signalProgress(imagesDone, totalImages, itemName, thumbnail);

    if (!QFile::copy(thumbnailPath, destinationPath.absoluteFilePath(thumbnailName))) {
      emit signalFailed(tr("Cannot move thumbnail %1.").arg(thumbnailPath), GCore::ErrorHandler::Critical);
      isSuccessful = false;
      break;
    }

    path.cdUp();
    destinationPath.cdUp();

    if (!QFile::copy(item, destinationPath.absoluteFilePath(itemName))) {
      emit signalFailed(tr("Cannot move image %1.").arg(item), GCore::ErrorHandler::Critical);
      isSuccessful = false;
      break;
    }

    imagesDone++;
    gallerieImagesDone++;
    emit directoryProgress(gallerieImagesDone, totalGalleryImages, parents);
  }

  if (isSuccessful) {
    if (deleteDirectory(m_source))
      emit signalFailed(tr("Old Gallery directory couldn't be deleted."), GCore::ErrorHandler::Warning);
  } else {
    deleteDirectory(m_destination);
  }

  emit finished(isSuccessful);
}

long MoveJob::calculateGallerieImages(const QString &gallery) const
{
  long counter = 0;

  QDir galleryDir(m_source);
  foreach(QString part, gallery.split('/'))
  galleryDir.cd(part);

  foreach(QFileInfo image, galleryDir.entryInfoList())
  if (image.isFile() && image.fileName() != ".metadata")
    counter++;

  return counter;
}

long MoveJob::calculateImages() const
{
  long counter = 0;

  QDirIterator imagesIterator(m_source.absolutePath(), QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);
  while (imagesIterator.hasNext()) {
    QFileInfo info(imagesIterator.filePath());
    if (info.isFile() && !imagesIterator.filePath().contains(".thumbnails") && imagesIterator.fileName() != ".metadata")
      counter++;
    imagesIterator.next();
  }

  return counter;
}

bool MoveJob::deleteDirectory(const QDir &path) const
{
  bool reportWarning = false;
  QList<QDir> lowestGalleries;
  QDirIterator sourceIterator(path.absolutePath(), QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
  while (sourceIterator.hasNext()) {
    QDir gallery(sourceIterator.next());

    // If it is the lowest gallery in the tree, we add it to the list of later deletion (rmpath)
    if (gallery.entryList(QDir::AllDirs | QDir::NoDotAndDotDot).isEmpty())
      lowestGalleries << gallery;

    // Deleting images
    foreach(QString image, gallery.entryList(QDir::Files | QDir::Hidden))
      if (!gallery.remove(image))
        reportWarning = true;

    gallery.cd(".thumbnails");

    // Deleting thumbnails
    foreach(QString thumbnail, gallery.entryList(QDir::Files | QDir::Hidden))
      if (!gallery.remove(thumbnail))
        reportWarning = true;

    // We delete the .thumbnails directory
    gallery.cdUp();
    if (!gallery.rmdir(".thumbnails"))
      reportWarning = true;
  }

  foreach(QDir gallery, lowestGalleries)
    !gallery.rmpath(".");

  path.mkpath(path.absolutePath());

  return reportWarning;
}

}

}
