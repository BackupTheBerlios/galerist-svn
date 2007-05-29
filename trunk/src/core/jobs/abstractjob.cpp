/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÃÂ¡nik                                 *
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
#include "abstractjob.h"

#include "core/data.h"
#include "core/errorhandler.h"

#include <QtCore/QMutex>

#include <QtGui/QApplication>

namespace GCore
{

namespace GJobs
{

AbstractJob::AbstractJob(QObject *parent)
    : QThread(parent),
    m_stop(false)
{
  // Connect with the error handler.
  connect(this, SIGNAL(signalFailed(const QString&, int)), Data::self()->getErrorHandler(), SLOT(slotReporter(const QString&, int)));
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(terminate()));
}

AbstractJob::~AbstractJob()
{}

void AbstractJob::run()
{
  job();
}

void AbstractJob::stop()
{
  m_locker.lock();
  m_stop = true;
  m_locker.unlock();
}

bool AbstractJob::getStop()
{
  return m_stop;
}

void AbstractJob::slotDeleteLater()
{
  usleep(100);
  QThread::deleteLater();
}

}

}
