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
#ifndef JOBSABSTRACTJOB_H
#define JOBSABSTRACTJOB_H

#include <QtCore/QThread>
#include <QtCore/QMutex>

#include "core/errorhandler.h"

class QImage;

namespace GCore
{
class ImageItem;
namespace GJobs
{

/**
 * @short Abstract class for various types of jobs.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
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
    void progress(int finished, int total, const QString &current, const QImage &image);
    /**
     * Signal for reporting an error.
     *
     * @param errorMessage Error string to show the user.
     */
    void failed(const QString &errorMessage, int);
    /**
     * Reports that the job has been finished.
     *
     * @param successful Tells if the job has been successful.
     */
    void finished(bool successful);
    void finished(ImageItem *item);

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

    /**
     * Pauses the thread.
     */
    void pause();

    /**
     * Unpauses the thread.
     */
    void unpause();

    /**
     * A destructor.
     */
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

    /**
     * Pauses if needed.
     */
    bool freeze();

    bool m_stop;
    bool m_paused;
    QMutex m_locker;

};

}
}

#endif
