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
#ifndef GCORE_GJOBSMOVEJOB_H
#define GCORE_GJOBSMOVEJOB_H

#include <QtCore/QDir>
#include <QtCore/QModelIndex>

#include "core/jobs/abstractjob.h"

namespace GCore
{

namespace GJobs
{

/**
 * @short Job for moving objects.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class MoveJob : public GCore::GJobs::AbstractJob
{
    Q_OBJECT
  signals:
    void directoryProgress(const QString &name);

  public:
    /**
     * A constructor.
     */
    MoveJob(const QDir &destination, QObject *parent = 0);

    /**
     * A destructor.
     */
    ~MoveJob();

  protected:
    /**
     * Reimplemented method.
     */
    void job();

  private:
    QDir m_destination;

    /**
     * Deletes the old galleries.
     *
     * @param indexes Indexes to delete.
     *
     * @return @c true "The roots go deep, Saruman." <= Report a warning :D.
     * @return @c false It went like a charm.
     */
    inline bool deleteOldGalleries(const QModelIndexList &indexes) const;

};

}

}

#endif
