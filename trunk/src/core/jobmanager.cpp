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

using namespace GCore::GJobs;

namespace GCore
{

JobManager *JobManager::m_self = 0;

JobManager::JobManager()
    : QThread(qApp),
      m_stop(false)
{}

JobManager::~JobManager()
{
  stop();

  foreach (AbstractJob *job, m_jobHash) {
    job->stop();
    job->wait();
    delete job;
  }

  wait();
}

void JobManager::registerJob(const QString &jobName, AbstractJob *job)
{
  m_mutex.lock();
  m_jobHash.insert(jobName, job);
  m_mutex.unlock();
}

void JobManager::registerJob(const QString &jobName, QObject *job)
{
  registerJob(jobName, static_cast<AbstractJob*> (job));
}

QString JobManager::createGalleryJob(const QString &name, const QModelIndex &parent, const QString &source, const QStringList &images, bool deleteSource)
{
  qsrand(time(0));
  QString hash = QCryptographicHash::hash(QString(QString::number(qrand()) + QString::number(time(0))).toAscii(), QCryptographicHash::Md5);

  CreateJob *job = new CreateJob(name, parent, source, images, deleteSource, this);
  registerJob(hash, job);

  connect(job, SIGNAL(finished(ImageItem*)), Data::self()->imageModel(), SLOT(addItem(ImageItem*)));

  return hash;
}

void JobManager::startJob(const QString &jobName)
{
  m_jobHash.value(jobName)->start();
}

void JobManager::stopJob(const QString &jobName)
{
  m_jobHash.value(jobName)->stop();
}

void JobManager::stop()
{
  m_mutex.lock();
  m_stop = true;
  m_mutex.unlock();
}

bool JobManager::isRunning(const QString &jobName)
{
  AbstractJob *job = m_jobHash.value(jobName);
  if (job)
    return job->isRunning();
  else
    return false;
}

AbstractJob *JobManager::job(const QString &jobName)
{
  return m_jobHash.value(jobName);
}

void JobManager::pauseJob(const QString &jobName)
{
  m_jobHash.value(jobName)->pause();
}

void JobManager::unpauseJob(const QString &jobName)
{
  m_jobHash.value(jobName)->unpause();
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
    if (m_jobHash.isEmpty())
      usleep(50);

    QStringList deletedJobs;
    QHashIterator<QString, AbstractJob*> count(m_jobHash);
    while (count.hasNext()) {
      count.next();
      if (count.value()->wait(50)) {
        delete count.value();
        deletedJobs << count.key();
      }
    }

    foreach (QString jobName, deletedJobs)
      m_jobHash.remove(jobName);
    deletedJobs.clear();
  }
}

}
