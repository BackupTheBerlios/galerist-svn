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
#ifndef GCORE_GJOBSREADJOB_H
#define GCORE_GJOBSREADJOB_H

#include "core/jobs/abstractjob.h"
#include "core/imageitem.h"

#include <QtCore/QModelIndex>
#include <QtCore/QDir>

namespace GCore
{

namespace GJobs
{

/**
 * @short Creates the missing thumbnails.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class ReadJob : public GCore::GJobs::AbstractJob
{
    Q_OBJECT
  signals:
    /**
     * Progress signal.
     */
    void progress(const QString &filename, const QImage &image, int parentId);

  public:
    /**
     * A multi purpose constructor ;).
     *
     * @param source The source directory of images.
     * @param images List of images (filenames) taht needs to be read. If empty, whole directory is going to be read.
     * @param output Destination path to save thumbnails. If empty, no thumbnails will be saved.
     * @param parent Parent object.
     */
    ReadJob(const QDir &source, const QStringList &images, const QDir &output, int parentId, QObject *parent);

    QStringList imagesList() const;

  protected:
    /**
     * Reimplemented method.
     * Defines the loop.
     */
    void job();

  private:
    QDir m_source;
    QStringList m_images;
    QDir m_destination;
    int m_parentId;

};

}

}

#endif
