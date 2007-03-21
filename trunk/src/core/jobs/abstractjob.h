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
 * @short Abstract class for various types of jobs.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class AbstractJob : public QThread
{
    Q_OBJECT
  signals:
    /**
     * Signal for reporting progress of the job.
     *
     * @param finished How many units are finished.
     * @param total How many units are to be processed in total.
     * @param current Current's unit name.
     * @param image Current's unit image.
     */
    void signalProgress(int finished, int total, const QString &current, const QImage &image);
    /**
     * Signal for reporting an error.
     *
     * @param errorMessage Error string to show the user.
     */
    void signalFailed(const QString &errorMessage, int);

  public:
    /**
     * A constructor.
     *
     * @param parent Parent of this object.
     */
    AbstractJob(QObject *parent = 0);

    /**
     * Stops the thread.
     */
    void stop();

    ~AbstractJob();

  protected:
    /**
     * Pure virtual method for defining the job.
     */
    virtual void job() = 0;
    /**
     * Reimplemented method for running job() method.
     */
    void run();

    /**
     * Returns if the thread needs to stop.
     *
     * @return state of the stopping sequence.
     */
    bool getStop();

    bool m_stop;
    QMutex m_locker;

  private slots:
    /**
     * Deletes the thread (used for initiating the delete sequence).
     */
    void slotDeleteLater();

};

}
}

#endif
