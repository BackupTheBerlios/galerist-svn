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
#include "jobmanager.h"

#include <QtCore/QDateTime>
#include <QtCore/QCryptographicHash>

#include <QtGui/QApplication>

#include "core/data.h"

#include "core/jobs/createjob.h"
#include "core/jobs/copyjob.h"
#include "core/jobs/deletejob.h"
#include "core/jobs/readjob.h"
#include "core/jobs/movejob.h"


#include <QtDebug>

using namespace GCore::GJobs;

namespace GCore
{

JobManager *JobManager::m_self = 0;

JobManager::JobManager()
    : QThread(qApp),
    m_stop(false),
           m_jobNumber(0)
{}

JobManager::~JobManager()
{
  stop();

  foreach(AbstractJob *job, m_jobMap) {
    job->stop();
    job->wait();
    delete job;
  }

  wait();
}

Job JobManager::createGalleryJob(const QString &name, const QModelIndex &parent, const QString &source, const QStringList &images, bool deleteSource)
{
  CreateJob *job = new CreateJob(name, parent, source, images, deleteSource, this);
  uint id = registerJob(job);

  connect(job, SIGNAL(finished(ImageItem*)), Data::self()->imageModel(), SLOT(addItem(ImageItem*)));

  return Job(id);
}

Job JobManager::addImages(const QModelIndex &galleryIndex, const QStringList &images, bool deleteSource)
{
  CopyJob *job = new CopyJob(galleryIndex, images, deleteSource, this);
  uint id = registerJob(job);

  connect(job, SIGNAL(process(ImageItem*)), Data::self()->imageModel(), SLOT(addItem(ImageItem*)));

  return Job(id);
}

Job JobManager::deleteGallery(const QModelIndex &galleryIndex)
{
  DeleteJob *job = new DeleteJob(galleryIndex, this);
  uint id = registerJob(job);

  connect(job, SIGNAL(remove(const QModelIndex&)), Data::self()->imageModel(), SLOT(removeItem(const QModelIndex&)));

  return Job(id);
}

Job JobManager::deleteImages(const QModelIndexList &images)
{
  DeleteJob *job = new DeleteJob(images, this);
  uint id = registerJob(job);

  connect(job, SIGNAL(remove(const QModelIndex&)), Data::self()->imageModel(), SLOT(removeItem(const QModelIndex&)));

  return Job(id);
}

Job JobManager::readImages(const QDir &source, const QStringList &images, const QDir &destination, int parentId)
{
  ReadJob *job = new ReadJob(source, images, destination, parentId, this);
  uint id = registerJob(job);

  return Job(id);
}

Job JobManager::moveGalleries(const QString &destination)
{
  MoveJob *job = new MoveJob(destination, this);
  uint id = registerJob(job);

  return Job(id);
}

void JobManager::stop()
{
  m_stop = true;
}

JobManager *JobManager::self()
{
  if (!m_self) {
    m_self = new JobManager();
    m_self->start();
  }

  return m_self;
}

void JobManager::run()
{
  while (!m_stop) {
    if (m_jobMap.isEmpty()) {
      usleep(50);
      continue;
    }

    QList<AbstractJob*> runningJobs;
    int idealThreads = idealThreadCount();

    QList<uint> deletedJobs;
    QMapIterator<uint, AbstractJob*> count(m_jobMap);
    while (count.hasNext()) {
      count.next();
      AbstractJob *job = count.value();
      job->start();
      runningJobs << job;

      if (runningJobs.count() >= idealThreads) {
        foreach (AbstractJob *job, runningJobs)
          job->wait();

        runningJobs.clear();
      }

      deletedJobs << count.key();
    }

    foreach(uint jobId, deletedJobs) {
      m_jobMap.value(jobId)->wait();
      delete m_jobMap.value(jobId);
      m_jobMap.remove(jobId);
    }
    deletedJobs.clear();

    if (m_jobMap.isEmpty())
      m_jobNumber = 0;
  }
}

uint JobManager::registerJob(AbstractJob *job)
{
  m_jobNumber++;
  uint id = m_jobNumber;

  m_jobMap.insert(id, job);

  return id;
}

AbstractJob *JobManager::job(uint id) const
{
  return m_jobMap.value(id);
}

}
