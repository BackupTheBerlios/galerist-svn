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
#ifndef GCORE_GJOBSREADJOB_H
#define GCORE_GJOBSREADJOB_H

#include "core/jobs/abstractjob.h"
#include "core/imageitem.h"

#include <QtCore/QModelIndex>
#include <QtCore/QDir>

namespace GCore
{

namespace GJobs
{

/**
 * @short Creates the missing thumbnails.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class ReadJob : public GCore::GJobs::AbstractJob
{
    Q_OBJECT
  signals:
    /**
     * Emitted when a thumbnail is finished.
     */
    void signalThumb(const QString &fileName);
    /**
     * Same signal as above but using a QModelIndex.
     *
     * @see GCore#GJobs#ReadJob#signalThumb
     */
    void signalProcessed(const QModelIndex &item);
    /**
     * Same as above signal but has the filename of the thumbnail and the thumbnail itself.
     *
     * @see GCore#GJobs#ReadJob#signalThumb
     */
    void signalProgress(const QString &filename, const QImage &image, const QString &directory = 0);

  public:
    /**
     * Constructor for reading and adding to the model.
     *
     * @param model Model to which new images will be added.
     */
    ReadJob(const QAbstractItemModel *model);

    /**
     * Constructor for reading only.
     *
     * @param parent Parent of this job.
     * @param path Path to the gallery.
     * @param recursive Read recursevly.
     */
    ReadJob(QObject *parent, const QDir &path, bool recursive = false);

    /**
     * Constructor for reading preselected list of images only.
     *
     * @param parent Parent of this job.
     * @param path Path to the gallery.
     * @param images List of images.
     * @param recursive Read recursevly.
     */
    ReadJob(QObject *parent, const QDir &path, const QStringList &images, bool recursive = false);

    /**
     * Returns the currently processed item.
     */
    QModelIndex getItem();

    /**
     * Adds a photo to the queue.
     */
    void queuePhoto(const QModelIndex &item);

    /**
     * A destructor.
     */
    ~ReadJob();

  protected:
    /**
     * Reimplemented method.
     * Defines the loop.
     */
    void job();

  private:
    /**
     * Reads the directory and make image thumbnails.
     *
     * @param path Path to the directory with images.
     */
    void readPath(const QDir &path);

    // Variables
    const QAbstractItemModel *m_model;
    QString m_filePath;
    QList<QModelIndex> m_items;

    QDir m_path;
    bool m_recursive;

    QStringList m_images;

};

}

}

#endif
