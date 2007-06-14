/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÃÂÃÂ¡nik                                 *
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
#include <QtCore/QVariant>

#include <QtGui/QApplication>
#include <QtGui/QImage>

namespace GCore
{

namespace GJobs
{

AbstractJob::AbstractJob(QObject *parent)
    : QThread(parent),
    m_stop(false),
    m_paused(false)
{
  // Connect with the error handler.
  connect(this, SIGNAL(signalFailed(const QString&, int)), Data::self()->value(Data::ErrorHandler).value<QObject*>(), SLOT(slotReporter(const QString&, int)));
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(terminate()));
}

void AbstractJob::stop()
{
  m_locker.lock();
  m_stop = true;
  m_locker.unlock();
}

void AbstractJob::pause()
{
  m_locker.lock();
  m_paused = true;
  m_locker.unlock();
}

void AbstractJob::unpause()
{
  m_locker.lock();
  m_paused = false;
  m_locker.unlock();
}

AbstractJob::~AbstractJob()
{
  qDebug("bu");
}

void AbstractJob::run()
{
  job();
  if (m_stop)
    emit signalProgress(0, -1, QString(), QImage());
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
