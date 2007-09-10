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

#ifndef GWIDGETSGWIZARDSELECTIONPAGE_H
#define GWIDGETSGWIZARDSELECTIONPAGE_H

#include <QtGui/QWizardPage>

#include "ui_selectionpage.h"

#include "core/jobmanager.h"

namespace GWidgets
{

namespace GWizard
{

/**
 * @short Selection page for GDialogs::NewGalleryWizard.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class SelectionPage : public QWizardPage, private Ui::SelectionPage
{
    Q_OBJECT

  public:
    /**
     * A default constructor.
     */
    SelectionPage();

    /**
     * A default destructor.
     */
    ~SelectionPage();

    /**
     * Reimplemented method.
     * Sets all values to default.
     */
    void initializePage();

    /**
     * Reimplemented method.
     * Making sure we are not going to lose data.
     */
    void cleanupPage();

    /**
     * Reimplemented method for determening if this page is fully filled up.
     */
    bool isComplete() const;

    /**
     * Sets predefined images.
     *
     * @param path Location of the predefined images.
     * @param images List of predefined images.
     */
    void setPredefinedImages(const QString &path, const QStringList &images = QStringList());

  public slots:

  private:
    bool m_initialised;
    mutable GCore::Job m_job;

    /**
     * Shows what images will be added.
     *
     * @param path Path in where the potential images are stored.
     */
    void makePreview(const QString &path, const QStringList &images = QStringList()) const;

  private slots:
    /**
     * Defines the behaviour when browse button is clicked.
     */
    void slotBrowseClicked();

    /**
     * Adds images to the preview list.
     */
    void addImage(const QString&, const QImage &image, int);

    void clearPreview(bool isValid);

};

}

}

#endif

