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

#ifndef GWIDGETSGWIZARDSELECTIONPAGE_H
#define GWIDGETSGWIZARDSELECTIONPAGE_H

#include <QtGui/QWizardPage>
#include "ui_selectionpage.h"

namespace GWidgets {

namespace GWizard {

/**
 * @short Selection page for GDialogs::NewGalleryWizard.
 * @author Gregor Kališnik <gregor@podnapisi.net>
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
     * Reimplemented method. Sets all values to default.
     */
    void initializePage();
    /**
     * Reimplemented method. Cleans the values.
     */
    void cleanupPage();

    /**
     * Reimplemented method for determening if this page is fully filled up.
     */
    bool isComplete() const;

  public slots:

  private:
    /**
     * Checks if the image path is valid.
     *
     * @param path Path to verify.
     *
     * @return @c true Path is valid.
     * @return @c false Path is invalid.
     */
    bool checkImagesPath(const QString &path) const;
    /**
     * Checks if the name is valid.
     *
     * @param name Name to verify.
     *
     * @return @c true Name is valid.
     * @return @c false Name is invalid.
     */
    bool checkName(const QString &name) const;

    bool m_initialised;

  private slots:
    /**
     * Defines the behaviour when browse button is clicked.
     */
    void slotBrowseClicked();

    /**
     * Checks if the image path is valid.
     *
     * @param path Path to verify.
     */
    void slotCheckImagesPath(const QString &path);
    /**
     * Checks if the name is valid.
     *
     * @param name Name to verify.
     */
    void slotCheckName(const QString &name);

};

}

}

#endif

