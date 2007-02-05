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
#ifndef GCOREGDIALOGSNEWWIZARD_H
#define GCOREGDIALOGSNEWWIZARD_H

#include "dialogs/wizard.h"

namespace GWidgets
{
class NewWelcomePage;
class NewSelectionPage;
class NewCopyPage;
class NewFinishPage;
}

namespace GDialogs
{

/**
 * Class that creates a simple wizard for creating a gallery.
 * @short A wizard class.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class NewWizard : public GDialogs::Wizard
{
    Q_OBJECT
  public:
    /**
     * A default constructor.
     *
     * @param caption Caption of the window.
     * @param parent Pointer to the parent widget.
     */
    NewWizard(const QString &caption, QWidget *parent = 0);

    /**
     * Method for retrieving gallery name.
     *
     * @return Returns the name of the gallery.
     */
    QString getGalleryName();
    /**
     * Method for retrieving path of the photos for the new gallery. (Path to import)
     *
     * @return Path to the photos.
     */
    QString getGalleryPath();

    //void reject(bool stop);

    /**
     * A default destructor.
     */
    ~NewWizard();

  protected:
    /**
     * Overloaded method to create a page.
     *
     * @param index Index of this page.
     *
     * @return Pointer to the new page.
     */
    QWidget *createPage(int index);
    /**
     * Overloaded function for accepting data.
     */
    void accept();
    /**
     * Overloaded function for rejecting data.
     */
    void reject();

  private:
    GWidgets::NewWelcomePage *m_welcomePage;
    GWidgets::NewSelectionPage *m_selectionPage;
    GWidgets::NewCopyPage *m_copyWarningPage;
    GWidgets::NewCopyPage *m_copyProgressPage;
    GWidgets::NewFinishPage *m_finishPage;

    /**
     * Overloaded method for changing wizard status to failed.
     */
    void failed();

    friend class GWidgets::NewWelcomePage;
    friend class GWidgets::NewSelectionPage;
    friend class GWidgets::NewCopyPage;
    friend class GWidgets::NewFinishPage;

};

}

#endif
