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
#include "copyjob.h"

#include <QtCore/QDir>

#include "core/imagemodel.h"
#include "core/metadatamanager.h"
#include "core/data.h"

#include <QtDebug>

namespace GCore
{

namespace GJobs
{

CopyJob::CopyJob(const QModelIndex &galleryIndex, const QStringList &images, bool deleteSources, QObject *parent)
    : GCore::GJobs::AbstractJob(parent),
    m_galery(galleryIndex),
    m_images(images),
             m_deleteSources(deleteSources)
{}

void CopyJob::job()
{
  if (!m_galery.isValid()) {
    qWarning("GCore::GJobs::CopyJob::job: Wrong gallery index used!");
    return;
  }

  QDir destinationDir = m_galery.data(ImageModel::ImagePathRole).toString();
  destinationDir.cd(m_galery.data(ImageModel::ImageNameRole).toString());

  QDir thumbnailDir = destinationDir;
  thumbnailDir.cd("thumbnails");

  QDir sourceDir = m_images.at(0);
  sourceDir.cdUp();

  int galleryId = m_galery.data(ImageModel::IdRole).toInt();

  int processed = 1;
  int total = m_images.count();

  QList<ImageItem*> processedImages;

  foreach (QString imagePath, m_images) {
    QString imageName = imagePath;
    imageName.remove(sourceDir.absolutePath()).remove("/");
    qDebug() << imageName;

    // If the file is 0 in size, it's a fake (need to get a better verification process), or it is not a supported image
    if (QFileInfo(sourceDir, imageName).size() == 0 || !(imageName).contains(Data::self()->supportedFormats())) {
      processed++;
      continue;
    }

    QImage image = QImage(imagePath).scaled(128, 128, Qt::KeepAspectRatio);

    // Checks if the filename already exists and get a new filename
    QString fileName = imageName;
    int appendNumber = 0;
    while (QFile::exists(destinationDir.absoluteFilePath(fileName))) {
      fileName = imageName;
      if (fileName.indexOf('.', -4) != -1)
        fileName.insert(fileName.indexOf('.', -4), QString::number(appendNumber));
      else
        fileName.append(QString::number(appendNumber));
      appendNumber++;
    }

    QString thumbName = fileName + ".jpg";

    if (!QFile::copy(imagePath, destinationDir.absoluteFilePath(fileName)))
      emit failed(tr("Copy failed for %1 to %2.").arg(imagePath).arg(destinationDir.absoluteFilePath(fileName)), ErrorHandler::Warning);
    else {
      image.save(thumbnailDir.absoluteFilePath(thumbName), "JPG");
      emit process(MetaDataManager::self()->registerImage(fileName, galleryId));
    }
    emit progress(processed, total, imageName, image);

    processed++;
  }
}

}

}
