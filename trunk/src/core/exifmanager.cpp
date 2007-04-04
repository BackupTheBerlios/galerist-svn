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
#include "exifmanager.h"

#include <libexif/exif-data.h>

#include <QtCore/QStringList>

namespace GCore
{

ExifManager::ExifManager(const QString &fileName, QObject *parent)
    : QObject(parent)
{
  m_data = exif_data_new_from_file(fileName.toUtf8().data());
}

ExifManager::~ExifManager()
{
  exif_data_free(m_data);
}

QString ExifManager::getAperture() const
{
  return parse(exif_data_get_entry(m_data, EXIF_TAG_APERTURE_VALUE));
}

QString ExifManager::getCameraMaker() const
{
  return parse(exif_data_get_entry(m_data, EXIF_TAG_MAKE));
}

QString ExifManager::getCameraModel() const
{
  return parse(exif_data_get_entry(m_data, EXIF_TAG_MODEL));
}

QDateTime ExifManager::getCreationDate() const
{
  QDateTime creationDate;

  QString date = parse(exif_data_get_entry(m_data, EXIF_TAG_DATE_TIME_ORIGINAL));
  if (date != tr("Unavailable")) {
    QStringList datePart = date.split(" ").at(0).split(":");
    QStringList timePart = date.split(" ").at(1).split(":");

    creationDate.setTime(QTime(timePart.at(0).toInt(), timePart.at(1).toInt(), timePart.at(2).toInt()));
    creationDate.setDate(QDate(datePart.at(0).toInt(), datePart.at(1).toInt(), datePart.at(2).toInt()));
  }

  return creationDate;
}

QString ExifManager::getShutterSpeed() const
{
  return parse(exif_data_get_entry(m_data, EXIF_TAG_SHUTTER_SPEED_VALUE));
}

QString ExifManager::getExposureTime() const
{
  return parse(exif_data_get_entry(m_data, EXIF_TAG_EXPOSURE_TIME));
}

QString ExifManager::getFocalLength() const
{
  return parse(exif_data_get_entry(m_data, EXIF_TAG_FOCAL_LENGTH));
}

QString ExifManager::getFlash() const
{
  return parse(exif_data_get_entry(m_data, EXIF_TAG_FLASH));
}

QString ExifManager::parse(ExifEntry *entry) const
{
  char buffer[1024];

  if (entry)
    return QString::fromAscii(exif_entry_get_value(entry, buffer, sizeof(buffer)));

  return tr("Unavailable");
}

}
