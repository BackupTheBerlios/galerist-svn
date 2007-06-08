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
#ifndef GJOBSCOPYJOB_H
#define GJOBSCOPYJOB_H

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
class CopyJob : public GCore::GJobs::AbstractJob
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
     * Defines what kind of copy to use.
     */
    enum CopyMode
    {
      /** Copy only one image. */
      SingleMode,
      /** Copy whole bunch of images. */
      MultiMode
  };

    /**
     * A constructor for copiing the whole folder.
     *
     * @param source Source path.
     * @param destination Destination path.
     * @param gallery Index of the gallery to which the images will be added.
     * @param parent Parent of this job.
     */
    CopyJob(const QString &source, const QString &destination, const QModelIndex &gallery, bool deleteSource = false, QObject *parent = 0);
    /**
     * A constructor for copiing a list of images.
     *
     * @param source Source path.
     * @param fileNames List of files to copy.
     * @param destination Destination path.
     * @param gallery Index of the gallery to which the images will be added.
     * @param parent Parent of this job.
     */
    CopyJob(const QString &source, const QStringList &fileNames, const QString &destination, const QModelIndex &gallery, QObject *parent = 0);

    /**
     * Returns the index of the destination gallery.
     *
     * @return Gallery index.
     */
    QModelIndex getGalleryIndex();

    /**
     * A destructor.
     */
    ~CopyJob();

    /**
     * Pauses the copy. Usable only when using multi copy option.
     *
     * @see CopyJob#unpause()
     */
    void pause();

    /**
     * Unpauses the copy process. Usable only when in MultiCopy mode.
     *
     * @see CopyJob#pause()
     */
    void unpause();

  protected:
    /**
     * Reimplemented method to define the work process.
     */
    void job();

  private:
    //Copy methods.
    /**
     * The copy process for copiing the whole folder.
     */
    void multiCopy();
    /**
     * The copy process for a list of files.
     */
    void singleCopy();

    /**
     * Deletes the copied images.
     */
    void deleteSourceImages(const QStringList &images);

    //Variables.
    QDir *m_source;
    QDir *m_destination;
    QStringList m_fileNames;
    QModelIndex m_gallery;
    CopyMode m_mode;
    bool m_deleteSource;

    bool m_paused;

};

}
}

#endif
