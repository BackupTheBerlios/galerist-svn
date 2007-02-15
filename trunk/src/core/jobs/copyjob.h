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
#ifndef GJOBSCOPYJOB_H
#define GJOBSCOPYJOB_H

#include "core/jobs/abstractjob.h"

#include <QtCore/QModelIndex>
#include <QtCore/QStringList>

class QDir;

namespace GCore
{
class ImageItem;

namespace GJobs
{

/**
 @author Gregor Kalisnik <gregor@podnapisi.net>
*/
class CopyJob : public GCore::GJobs::AbstractJob
{
    Q_OBJECT
  signals:
    void signalProcess(const QString &name);

  public:
    //Types of copy.
    enum CopyMode
    {
      SingleMode,
      MultiMode
  };

    CopyJob(const QString &source, const QString &destination, const QModelIndex &gallery, QObject *parent = 0);
    CopyJob(const QString &source, const QStringList &fileNames, const QString &destination, const QModelIndex &gallery, QObject *parent = 0);

    ~CopyJob();

  protected:
    //Inherited method.
    void job();

  private:
    //Copy methods.
    void multiCopy();
    void singleCopy();
    void deleteCopied();

    //Variables.
    QDir *m_source;
    QDir *m_destination;
    QStringList m_fileNames;
    QModelIndex m_gallery;
    CopyMode m_mode;
    //QRegExp m_supportedFormats;

};

}
}

#endif
