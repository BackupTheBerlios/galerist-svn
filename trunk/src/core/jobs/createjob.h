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
#ifndef GJOBSCREATEJOB_H
#define GJOBSCREATEJOB_H

#include <QtCore/QModelIndex>
#include <QtCore/QStringList>

#include "core/jobs/abstractjob.h"

class QDir;

namespace GCore
{
class ImageItem;

namespace GJobs
{

/**
 * @short Class for copiing images.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class CreateJob : public GCore::GJobs::AbstractJob
{
    Q_OBJECT
  signals:
    /**
     * Signal for reporting process.
     *
     * @param name Name of the processed image.
     */
    void signalProcess(const QString &name);

  public:
    /**
     * A constructor for copiing the whole folder.
     *
     * @param name Gallery's name.
     * @param parentItem Parent item.
     * @param source Source path.
     * @param images Images to copy.
     * @param deleteSource Deletes or keeps the source images.
     * @param parent Parent of this job.
     */
    CreateJob(const QString &name, const QModelIndex &parentIndex, const QString &source, const QStringList &images = QStringList(), bool deleteSource = false, QObject *parent = 0);

  protected:
    /**
     * Reimplemented method to define the work process.
     */
    void job();

  private:
    /**
     * Deletes the copied images.
     */
    void deleteSourceImages(const QStringList &images);

    //Variables.
    QString m_name;
    QModelIndex m_parent;
    QString m_sourcePath;
    QStringList m_images;
    bool m_deleteSource;

};

}
}

#endif
