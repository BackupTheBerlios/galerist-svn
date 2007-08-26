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
#include <QtCore/QModelIndex>

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

    /**
     * Registers the job with the manager.
     *
     * @param jobName Name to use to access this job.
     * @param job The actual job.
     *
     * @return Pointer to the job.
     */
    void registerJob(const QString &jobName, GJobs::AbstractJob *job);
    void registerJob(const QString &jobName, QObject *job);

    QString createGalleryJob(const QString &name, const QModelIndex &parentItem, const QString &source, const QStringList &images, bool deleteSource);

    QString addImages(const QModelIndex &galleryIndex, const QStringList &images, bool deleteSource = false);

    QString deleteGallery(const QModelIndex &galleryIndex);
    QString deleteImages(const QModelIndexList &images);

    /**
     * Starts the job.
     */
    void startJob(const QString &jobName);

    /**
     * Stops the job.
     */
    void stopJob(const QString &jobName);

    /**
     * Method to stop the JobManager.
     */
    void stop();

    /**
     * Checks if the job is running.
     */
    bool isRunning(const QString &jobName);

    /**
     * Gets the job.
     */
    GJobs::AbstractJob *job(const QString &jobName);

    /**
     * Pauses the job.
     */
    void pauseJob(const QString &jobName);

    /**
     * Unpauses the job.
     */
    void unpauseJob(const QString &jobName);

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
    QHash<QString, GJobs::AbstractJob*> m_jobHash;
    QMutex m_mutex;
    bool m_stop;
    static JobManager *m_self;

    inline QString createHash() const;
};

}

#endif
