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
#ifndef JOBSABSTRACTJOB_H
#define JOBSABSTRACTJOB_H

#include <QtCore/QThread>
#include <QtCore/QMutex>

#include "core/errorhandler.h"

class QImage;

namespace GCore
{
namespace GJobs
{

/**
 @author Gregor Kalisnik <gregor@podnapisi.net>
*/
class AbstractJob : public QThread
{
    Q_OBJECT
  signals:
    void signalProgress(int finished, int total, const QString &current, const QImage &image);
    void signalFailed(const QString &errorMessage, int);

  public:
    AbstractJob(QObject *parent = 0);

    void stop();

    ~AbstractJob();

  protected:
    virtual void job() = 0;
    void run();

    bool getStop();

    bool m_stop;
    QMutex m_locker;

  private slots:
    void slotDeleteLater();

};

}
}

#endif
