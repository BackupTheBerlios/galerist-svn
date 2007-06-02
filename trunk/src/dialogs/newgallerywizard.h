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
#ifndef GDIALOGSNEWGALLERYWIZARD_H
#define GDIALOGSNEWGALLERYWIZARD_H

#include <QtGui/QWizard>

namespace GDialogs
{

/**
 * @short Wizard for easier creation of a gallery.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class NewGalleryWizard : public QWizard
{
    Q_OBJECT
  public:
    /**
     * A constructor.
     *
     * @param parent Parent of this wizard.
     */
    NewGalleryWizard(QWidget *parent = 0);

    /**
     * A constructor used for allready defined images.
     *
     * @param path Location of the images.
     * @param images List of predefined images.
     */
    NewGalleryWizard(const QString &path, const QStringList &images, QWidget *parent = 0);

    /**
     * A destructor.
     */
    ~NewGalleryWizard();

    /**
     * Reimplemented method.
     * Asks the user if he really wants to stop the wizard. And also reverts the copy.
     */
    void reject();

    /**
     * Setups the UI.
     */
    void setupUi();

  private:
    int m_copyPage;
    int m_selectionPage;

};

}

#endif
