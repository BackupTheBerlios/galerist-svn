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
#ifndef GWIDGETS_GWIZARDCOPYPAGE_H
#define GWIDGETS_GWIZARDCOPYPAGE_H

#include <QtGui/QWizardPage>
#include "ui_copypage.h"

namespace GWidgets {

namespace GWizard {

/**
 * @short Copy page for GDialogs::NewGalleryWizard.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
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

  private:
    bool m_finished;

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
