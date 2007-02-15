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
#include "copyjob.h"

#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtGui/QImage>
#include <QtCore/QModelIndex>
#include <QtCore/QFileInfo>

#include "core/data.h"
#include "core/errorhandler.h"
#include "core/imagemodel.h"

namespace GCore
{
namespace GJobs
{

CopyJob::CopyJob(const QString &source, const QString &destination, const QModelIndex &gallery, QObject *parent)
    : GCore::GJobs::AbstractJob(parent),
    m_source(new QDir(source)),
    m_destination(new QDir(destination)),
    m_gallery(gallery),
    m_mode(MultiMode)
{
  //m_supportedFormats = QRegExp("gif|jpg|jpeg|png");
  //m_supportedFormats.setCaseSensitivity(Qt::CaseInsensitive);
}

CopyJob::CopyJob(const QString &source, const QStringList &fileNames, const QString &destination, const QModelIndex &gallery, QObject *parent)
    : GCore::GJobs::AbstractJob(parent),
    m_source(new QDir(source)),
    m_destination(new QDir(destination)),
    m_fileNames(fileNames),
    m_gallery(gallery),
    m_mode(SingleMode)
{
  //m_supportedFormats = QRegExp("gif|jpg|jpeg|png");
  //m_supportedFormats.setCaseSensitivity(Qt::CaseInsensitive);
}

void CopyJob::job()
{
  switch (m_mode) {
    case(SingleMode): {
      singleCopy();
      return;
    }
    case(MultiMode): {
      multiCopy();
      return;
    }
    default: {
      //emit signalFailed(tr("Copy mode undefined."), ErrorHandler::Critical);
      ErrorHandler::reportMessage(tr("Copy mode undefined."), ErrorHandler::Critical);
      return;
    }
  }
}

void CopyJob::singleCopy()
{
  QStringList::const_iterator end = m_fileNames.constEnd();
  int numberImages = m_fileNames.count();
  int processed = 1;

  QDir thumbnailPath(*m_destination);
  if (!thumbnailPath.cd(".thumbnails")) {
    thumbnailPath.mkdir(".thumbnails");
    thumbnailPath.cd(".thumbnails");
  }

  for (QStringList::const_iterator count = m_fileNames.constBegin(); count != end; count++) {
    if (getStop()) {
      emit signalProgress(processed, numberImages, QString(), QImage());
      break;
    }

    // If the file is 0 in size, it's a fake (need to get a better verification process)
    if (QFileInfo(*m_source, *count).size() == 0 || !(*count).contains(Data::self()->getSupportedFormats())) {
      emit signalProgress(processed, numberImages, *count, QImage());
      processed++;
      continue;
    }

    // Create the thumbnail
    QImage image = QImage(m_source->absoluteFilePath(*count)).scaled(128, 128, Qt::KeepAspectRatio);

    // Checks if the filename allready exists. And get a new filename
    QString fileName = *count;
    int appendNumber = 0;
    while (static_cast<const GCore::ImageModel*>(m_gallery.model())->checkName(fileName, m_gallery)) {
      fileName = *count;
      if (fileName.indexOf('.', -4) != -1)
        fileName.insert(fileName.indexOf('.', -4), QString::number(appendNumber));
      else
        fileName.append(QString::number(appendNumber));
      appendNumber++;
    }

    // Send the progress Signal
    emit signalProgress(processed, numberImages, *count, image);

    // Save the thumbnail
    QString thumbName = fileName;
    image.save(thumbnailPath.absoluteFilePath(thumbName.remove(QRegExp("\\..+$")).append(".jpg")), "JPG");

    // Copy the image.
    if (!QFile::copy(m_source->absoluteFilePath(*count), m_destination->absoluteFilePath(fileName))) {
      //ErrorHandler::reportMessage(tr("Copy failed for %1 .").arg(m_source->absoluteFilePath(*count)), ErrorHandler::Critical);
      emit signalFailed(tr("Copy failed for %1 .").arg(m_source->absoluteFilePath(*count)), ErrorHandler::Critical);
      break;
    }

    // We finished with this picture. Notifying the model class
    emit signalProcess(fileName);
    processed++;
  }
}

void CopyJob::multiCopy()
{
  // Get the list of images we need to copy.
  QStringList imageFiles;
  imageFiles = m_source->entryList(Data::self()->getImageFormats());

  // Count how many images we need to copy.
  int numberImages = imageFiles.count() - 1;

  // If there is no files to copy.. Why bothering going into the for loop at all?
  if (imageFiles.isEmpty()) {
    emit signalProgress(1, 1, "None", QImage());
    return;
  }

  // Create a directory for thumbnails.
  QDir thumbnailPath(*m_destination);
  if (!thumbnailPath.cd(".thumbnails")) {
    thumbnailPath.mkdir(".thumbnails");
    thumbnailPath.cd(".thumbnails");
  }

  // Copy files.
  for (int count = 0; count <= numberImages; count++) {
    if (getStop()) {
      deleteCopied();
      emit signalProgress(count, numberImages, QString(), QImage());
      break;
    }

    // If the file is 0 in size, it's a fake (need to get a better verification process)
    if (QFileInfo(*m_source, imageFiles.at(count)).size() == 0 || !imageFiles.at(count).contains(Data::self()->getSupportedFormats())) {
      emit signalProgress(count, numberImages, imageFiles.at(count), QImage());
      continue;
    }

    // Create the thumbnail
    QImage image = QImage(m_source->absoluteFilePath(imageFiles.at(count))).scaled(128, 128, Qt::KeepAspectRatio);

    // Send the progress Signal
    emit signalProgress(count, numberImages, imageFiles.at(count), image);

    // Add the picture to the metadata database and get the new filename
    QString fileName = imageFiles.at(count);

    // Save the thumbnail
    QString thumbName = fileName;
    image.save(thumbnailPath.absoluteFilePath(thumbName.remove(QRegExp("\\..+$")).append(".jpg")), "JPG");

    // Copy the image.
    if (!QFile::copy(m_source->absoluteFilePath(imageFiles.at(count)), m_destination->absoluteFilePath(fileName))) {
      emit signalProcess(QString());
      emit signalProgress(count, numberImages, QString("failed"), QImage());
      stop();
      break;
    }

    // We finished with this picture. Notifying the model class
    emit signalProcess(fileName);
  }
}

void CopyJob::deleteCopied()
{
  QDir galleryPath(*m_destination);
  QDir thumbPath(galleryPath);
  thumbPath.cd(".thumbnails");

  QStringList images = galleryPath.entryList(QDir::Files | QDir::Hidden);
  QStringList::const_iterator endImages = images.constEnd();

  QStringList thumbnails = thumbPath.entryList(QDir::Files | QDir::Hidden);
  QStringList::const_iterator endThumbs = thumbnails.constEnd();

  for (QStringList::const_iterator count = thumbnails.constBegin(); count != endThumbs; count++) {
    thumbPath.remove(*count);
  }

  for (QStringList::const_iterator count = images.constBegin(); count != endImages; count++) {
    galleryPath.remove(*count);
  }

  galleryPath.rmdir(".thumbnails");
  galleryPath.cdUp();
  galleryPath.rmpath(m_destination->absolutePath());
}

CopyJob::~CopyJob()
{
  delete m_destination;
  delete m_source;
}

}
}
