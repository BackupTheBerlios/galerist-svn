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
#include "createjob.h"

#include <QtGui/QImage>
#include <QtCore/QFileInfo>

#include "core/data.h"
#include "core/errorhandler.h"
#include "core/imagemodel.h"
#include "core/metadatamanager.h"

namespace GCore
{
namespace GJobs
{

CreateJob::CreateJob(const QString &name, const QModelIndex &parentIndex, const QString &source, const QStringList &images, bool deleteSource, QObject *parent)
    : GCore::GJobs::AbstractJob(parent),
    m_parent(parentIndex),
    m_name(name),
    m_sourcePath(source),
    m_images(images),
    m_deleteSource(deleteSource)
{}

void CreateJob::job()
{
  bool failure = false;
  QDir sourceDir(m_sourcePath);
  QStringList images = m_images.isEmpty() ? sourceDir.entryList(Data::self()->supportedFormatsList()) : m_images;
  int numberImages = images.count();
  int processed = 1;

  // We set the destination dir
  QDir destinationDir = m_parent.isValid() ? m_parent.data(ImageModel::ImagePathRole).toString() : Data::self()->galleriesDir();
  destinationDir.cd(m_parent.data(ImageModel::ImageNameRole).toString());
  destinationDir.mkdir(m_name);
  destinationDir.cd(m_name);

  // We set the thumbnails dir
  QDir thumbnailDir = destinationDir;
  thumbnailDir.mkdir("thumbnails");
  thumbnailDir.cd("thumbnails");

  ImageItem *gallery = MetaDataManager::self()->registerGallery(m_name, m_parent.isValid() ? m_parent.data(ImageModel::IdRole).toInt() : -1);

  foreach (QString imageName, images) {
    if (freeze()) {
      emit progress(processed, numberImages, QString(), QImage());
      failure = true;
      break;
    }

    // If the file is 0 in size, it's a fake (need to get a better verification process), or it is not a supported image
    if (QFileInfo(sourceDir, imageName).size() == 0 || !(imageName).contains(Data::self()->supportedFormats())) {
      processed++;
      continue;
    }

    // Create the thumbnail
    QImage image = QImage(sourceDir.absoluteFilePath(imageName)).scaled(128, 128, Qt::KeepAspectRatio);

    // Send the progress Signal
    emit progress(processed, numberImages, imageName, image);

    // Save the thumbnail
    QString thumbName = imageName;
    image.save(thumbnailDir.absoluteFilePath(thumbName.append(".jpg")), "JPG");

    QFile::remove(destinationDir.absoluteFilePath(imageName));

    // Copy the image.
    if (!QFile::copy(sourceDir.absoluteFilePath(imageName), destinationDir.absoluteFilePath(imageName))) {
      emit failed(tr("Copy failed for %1 .").arg(sourceDir.absoluteFilePath(imageName)), ErrorHandler::Critical);
      failure = true;
      break;
    }

    // We add the image to the gallery
    gallery->appendChild(MetaDataManager::self()->registerImage(imageName, gallery->id()));

    processed++;
  }

  // Upon failure, we delete the semi-copied images
  if (failure) {
    foreach (QString file, thumbnailDir.entryList(QDir::AllEntries | QDir::Hidden))
      QFile::remove(thumbnailDir.absoluteFilePath(file));

    thumbnailDir.cdUp();
    thumbnailDir.rmdir("thumbnails");

    foreach (QString file, destinationDir.entryList(QDir::AllEntries | QDir::Hidden))
      QFile::remove(destinationDir.absoluteFilePath(file));

    destinationDir.cdUp();
    destinationDir.rmdir(m_name);

    MetaDataManager::self()->unregisterGallery(gallery->id(), gallery->parentId());

    return;
  }

  // We publish the gallery
  emit finished(gallery);
}

void CreateJob::deleteSourceImages(const QStringList &images)
{
  foreach(QString image, images)
    QFile::remove(image);
}

}
}
