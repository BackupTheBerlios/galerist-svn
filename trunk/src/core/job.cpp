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
#include "job.h"

#include "core/jobmanager.h"

namespace GCore
{

Job::Job()
  : m_id(0)
{}

Job::Job(unsigned int id) : m_id(id)
{}

GJobs::AbstractJob *Job::jobPtr() const
{
  if (!isValid())
    qCritical("GCore::Job: Using invalid job!");

  return JobManager::self()->job(m_id);
}

void Job::stop() const
{
  if (isValid())
    JobManager::self()->job(m_id)->stop();
}

void Job::pause() const
{
  if (isValid())
    JobManager::self()->job(m_id)->pause();
}

void Job::resume() const
{
  if (isValid())
    JobManager::self()->job(m_id)->unpause();
}

bool Job::isValid() const
{
  return JobManager::self()->job(m_id);
}

}
