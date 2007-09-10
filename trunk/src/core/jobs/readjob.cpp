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

#include <QtDebug>

namespace GCore
{

namespace GJobs
{

ReadJob::ReadJob(const QDir &source, const QStringList &images, const QDir &output, int parentId, QObject *parent)
    : GCore::GJobs::AbstractJob(parent),
    m_source(source),
    m_images(images),
    m_destination(output),
    m_parentId(parentId)
{}

    QStringList ReadJob::imagesList() const
    {
      return m_images;
    }

void ReadJob::job()
{
  QStringList images = m_images.isEmpty() ? m_source.entryList(Data::self()->supportedFormatsList()) : m_images;
  int total = images.count();
  int read = 1;

  if (m_destination == QDir()) {
    m_destination.mkdir("thumbnails");
    m_destination.cd("thumbnails");
  }

  foreach(QString imageName, images) {
    if (freeze())
      break;

    QImage image(m_source.absoluteFilePath(imageName));
    image = image.scaled(128, 128, Qt::KeepAspectRatio);

    if (m_destination != QDir())
      image.save(m_destination.absoluteFilePath(imageName + ".jpg"), "JPG");

    emit progress(imageName, image, m_parentId);
    emit AbstractJob::progress(read, total, imageName, image);
    read++;
  }
}

}

}
