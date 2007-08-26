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
#ifndef GCORE_GJOBSCOPYJOB_H
#define GCORE_GJOBSCOPYJOB_H

#include "core/jobs/abstractjob.h"

#include <QtCore/QModelIndex>
#include <QtCore/QStringList>

namespace GCore
{

  class ImageItem;
  
namespace GJobs
{

/**
 * @short Job for copiing new images.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class CopyJob : public GCore::GJobs::AbstractJob
{
    Q_OBJECT
  signals:
    void process(ImageItem *image);

  public:
    CopyJob(const QModelIndex &galleryIndex, const QStringList &images, bool deleteSources = false, QObject *parent = 0);

  protected:
    void job();
    
  private:
    QModelIndex m_galery;
    QStringList m_images;
    bool m_deleteSources;

};

}

}

#endif
