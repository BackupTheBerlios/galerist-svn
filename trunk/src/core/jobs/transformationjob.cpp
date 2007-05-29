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
    m_item(parent)
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
}

void TransformationJob::saveImage()
{
  m_locker.lock();

  m_operation = SaveImage;

  m_locker.unlock();
}

void TransformationJob::cropImage(const QRect &area)
{
  m_locker.lock();

  m_operation = Crop;
  m_params.insert(Area, area);

  m_locker.unlock();
}

void TransformationJob::rotateImage(quint16 angle)
{
  m_locker.lock();

  m_params.insert(Angle, angle);
  m_operation = Rotate;

  m_locker.unlock();
}

void TransformationJob::blurImage(int numberFilter)
{
  m_locker.lock();

  m_operation = Blur;
  m_params.insert(NumberFilter, numberFilter);

  m_locker.unlock();
}

void TransformationJob::sharpenImage(int numberFilter)
{
  m_locker.lock();

  m_operation = Sharpen;
  m_params.insert(NumberFilter, numberFilter);

  m_locker.unlock();
}

void TransformationJob::resizeImage(const QSize &size)
{
  m_locker.lock();

  m_operation = Resize;
  m_params.insert(Size, size);

  m_locker.unlock();
}

void TransformationJob::job()
{
  while (!m_stop) {
    switch (m_operation) {
      case (NoOperation) : {
        msleep(10);
        break;
      }
      case (LoadImage) : {
        if (m_image)
          delete m_image;

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
      case (Rotate) : {
        doRotate();
        break;
      }
      case (Crop) : {
        doCrop();
        break;
      }
      case (Blur) : {
        doBlur();
        break;
      }
      case (Sharpen) : {
        doSharpen();
        break;
      }
      case (Resize) : {
        doResize();
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

void TransformationJob::doRotate()
{
  quint16 angle = m_params.value(Angle).value<quint16>();

  m_image->rotate(angle);

  m_locker.lock();
  m_params.clear();
  m_operation = SaveImage;
  m_locker.unlock();
}

void TransformationJob::doBlur()
{
  m_locker.lock();

  int blurFilters = m_params.value(NumberFilter).toInt();
  try {
    for (int count = 0; count < blurFilters; count++)
      m_image->blur();

    Magick::Blob blob;
    m_image->write(&blob);

    QImage previewImage(m_image->rows(), m_image->columns(), QImage::Format_RGB32);
    previewImage.loadFromData((const uchar*) blob.data(), blob.length());

    emit preview(previewImage);
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
  }

  m_operation = NoOperation;
  m_params.clear();

  m_locker.unlock();
}

void TransformationJob::doSharpen()
{
  m_locker.lock();

  int sharpenFilters = m_params.value(NumberFilter).toInt();
  try {
    for (int count = 0; count < sharpenFilters; count++)
      m_image->sharpen();

    Magick::Blob blob;
    m_image->write(&blob);

    QImage previewImage(m_image->rows(), m_image->columns(), QImage::Format_RGB32);
    previewImage.loadFromData((const uchar*) blob.data(), blob.length());

    emit preview(previewImage);
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
  }

  m_operation = NoOperation;
  m_params.clear();

  m_locker.unlock();
}

void TransformationJob::doResize()
{
  m_locker.lock();

  QSize size = m_params.value(Size).toSize();
  try {
    m_image->sample(Magick::Geometry(size.width(), size.height()));

    Magick::Blob blob;
    m_image->write(&blob);

    QImage previewImage(m_image->rows(), m_image->columns(), QImage::Format_RGB32);
    previewImage.loadFromData((const uchar*) blob.data(), blob.length());

    emit preview(previewImage);
  } catch (Magick::Exception &error) {
    qDebug(QString::fromStdString(error.what()).toAscii().data());
  }

  m_operation = NoOperation;
  m_params.clear();

  m_locker.unlock();
}

}

}
