/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÅ¡nik                                 *
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
#ifndef GWIDGETS_GWIZARDCOPYPAGE_H
#define GWIDGETS_GWIZARDCOPYPAGE_H

#include <QtGui/QWizardPage>
#include "ui_copypage.h"

namespace GWidgets
{

namespace GWizard
{

/**
 * @short Copy page for GDialogs::NewGalleryWizard.
 * @author Gregor KaliÅ¡nik <gregor@unimatrix-one.org>
 */
class CopyPage : public QWizardPage, private Ui::CopyPage
{
    Q_OBJECT
  public:
    /**
     * A default constructor.
     */
    CopyPage();

    /**
     * A default destructor.
     */
    ~CopyPage();

    /**
     * Reimplemented method for starting the copy process.
     */
    void initializePage();

    /**
     * Reimplemented method for defining when the copy has finished.
     */
    bool isComplete() const;

    /**
     * Pauses the copy.
     *
     * @see GCore::GJobs::CopyJob#pause()
     */
    void pauseCopy();

    /**
     * Resumes the copy.
     *
     * @see GCore::GJobs::CopyJob#unpause()
     */
    void resumeCopy();

    /**
     * Stops the copy.
     *
     * @see GCore::GJobs::AbstractJob#stop()
     */
    void stopCopy();

    /**
     * Predefines list of images.
     *
     * @param path Location of the images.
     * @param images List of added images.
     */
    void setPredefinedImages(const QString &path, const QStringList &images);

  private:
    bool m_finished;
    QObject *m_copyProcess;
    QString m_predefinedPath;
    QStringList m_prefedinedImages;

  private slots:
    /**
     * Updates the progress bar.
     *
     * @param finished Finished number of units.
     * @param total Total number of units.
     * @param current Name of the current item.
     * @param image Thumbnail of the current photo.
     */
    void slotProgress(int finished, int total, const QString &current, const QImage &image);

};

}

}

#endif
