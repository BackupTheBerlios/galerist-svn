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
#include "transformationjob.h"

#include <QtCore/QFile>

#include <QtGui/QImage>

#include <Magick++.h>

#include "core/imageitem.h"

#include <QtCore/QDebug>

namespace GCore
{

namespace GJobs
{

TransformationJob::TransformationJob(ImageItem *parent)
    : AbstractJob(parent),
    m_image(0),
    m_operation(NoOperation),
    m_operationDone(false),
    m_item(parent),
    m_imageLoaded(false)
{}

TransformationJob::~TransformationJob()
{
  if (m_image)  {
    delete m_image;
    m_image = 0;
  }
}

void TransformationJob::loadImage()
{
  m_locker.lock();

  m_operation = LoadImage;

  m_locker.unlock();
  //start();
}

void TransformationJob::closeImage(bool)
{
  //stop();

  /*if (save) {
    saveImage();
  }*/

}

void TransformationJob::saveImage()
{
  m_operation = SaveImage;
  /*
  try {
    m_image->write(m_item->getFilePath().toStdString());

    QDir dir(m_item->getFilePath());
    dir.cdUp();

    Magick::Blob blob;
    m_image->write(&blob);

    dir.cd(".thumbnails");

    QImage newThumb(m_image->rows(), m_image->columns(), QImage::Format_RGB32);
    newThumb.loadFromData((const uchar*) blob.data(), blob.length());
    newThumb.scaled(128, 128, Qt::KeepAspectRatio).save(dir.absoluteFilePath(m_item->getThumbName()));
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    return;
  }
  */
}

void TransformationJob::cropImage(const QRect &area)
{
  m_locker.lock();

  m_operation = Crop;
  m_params.insert(Area, area);

  m_locker.unlock();
}

void TransformationJob::job()
{
  while (!m_stop) {
    msleep(10);

    switch (m_operation) {
      case (NoOperation) : {
        break;
      }
      case (LoadImage) : {
        m_image = new Magick::Image();
        try {
          m_image->read(m_item->getFilePath().toStdString());
        } catch (Magick::Exception &error) {
          qDebug(QString::fromStdString(error.what()).toAscii().data());
          m_locker.unlock();
          return;
        }
        m_locker.lock();
        m_operation = NoOperation;
        m_locker.unlock();
        break;
      }
      case (SaveImage) : {
        try {
          m_image->write(m_item->getFilePath().toStdString());

          QDir dir(m_item->getFilePath());
          dir.cdUp();

          Magick::Blob blob;
          m_image->write(&blob);

          dir.cd(".thumbnails");

          QImage newThumb(m_image->rows(), m_image->columns(), QImage::Format_RGB32);
          newThumb.loadFromData((const uchar*) blob.data(), blob.length());

          emit completed(newThumb);

          newThumb.scaled(128, 128, Qt::KeepAspectRatio).save(dir.absoluteFilePath(m_item->getThumbName()));
        } catch (Magick::Exception &error) {
          qDebug(QString::fromStdString(error.what()).toAscii().data());
        }
        m_locker.lock();
        m_operation = NoOperation;
        m_locker.unlock();
        break;
      }
      case (Crop) : {
        doCrop();
        break;
      }
      default : {
        qDebug("TransformationJob: Operation is unknown!");
        break;
      }
    }
  }

  delete m_image;
  m_image = 0;
}

void TransformationJob::doCrop()
{
  // m_params.value(GWidgets::PhotoControl::Area).isNull() || m_params.isEmpty()
  if (!(!m_params.value(Area).isNull()) && !m_params.isEmpty()) {
    qDebug("TransformationJob: No params defined!");
    return;
  }

  QRect area = m_params.value(Area).toRect();

  m_locker.lock();

  try {
    m_image->crop(Magick::Geometry(area.width(), area.height(), area.left(), area.top()));
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
    m_locker.unlock();
    return;
  }

  m_operation = SaveImage;

  m_locker.unlock();

}

}

}
