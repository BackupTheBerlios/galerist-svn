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
#ifndef GCOREJOBMANAGER_H
#define GCOREJOBMANAGER_H

#include <QtCore/QThread>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QModelIndex>

#include "core/job.h"

#include "core/jobs/abstractjob.h"

namespace GCore
{

/**
 * @short Manages the jobs and acts as a job storage class.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class JobManager : public QThread
{
    Q_OBJECT
  public:
    /**
     * A constructor.
     */
    JobManager();

    /**
     * A destructor.
     */
    ~JobManager();

    Job createGalleryJob(const QString &name, const QModelIndex &parentItem, const QString &source, const QStringList &images, bool deleteSource);

    Job addImages(const QModelIndex &galleryIndex, const QStringList &images, bool deleteSource = false);

    Job deleteGallery(const QModelIndex &galleryIndex);
    Job deleteImages(const QModelIndexList &images);

    Job readImages(const QDir &source, const QStringList &images, const QDir &destination = QDir(), int parentId = 0);

    Job moveGalleries(const QString &destination);

    /**
     * Method to stop the JobManager.
     */
    void stop();

    /**
     * Static self method.
     */
    static JobManager *self();

  protected:
    /**
     * Reimplemented method.
     * @overload
     */
    void run();

  private:
    QMap<uint, GJobs::AbstractJob*> m_jobMap;
    bool m_stop;
    static JobManager *m_self;
    uint m_jobNumber;

    uint registerJob(GJobs::AbstractJob *job);
    GJobs::AbstractJob *job(uint id) const;

    friend class Job;
};

}

#endif
