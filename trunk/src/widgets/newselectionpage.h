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
#ifndef GWIDGETSNEWSELECTIONPAGE_H
#define GWIDGETSNEWSELECTIONPAGE_H

#include "widgets/wizardpage.h"
#include "widgets/lineedit.h"
#include "ui_newselectionpage.h"

namespace GDialogs
{
class NewWizard;
class GWizard;
}

namespace GWidgets
{

/**
 * Class that represents a selection page in the wizard.
 * @short Selection page.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class NewSelectionPage : public WizardPage, private Ui::NewSelectionPage
{
    Q_OBJECT
  public:
    /**
     * Default constructor.
     *
     * @param parent Parent widget.
     */
    NewSelectionPage(QWidget *parent = 0);

    /**
     * Default destructor.
     */
    ~NewSelectionPage();

    /**
     * Gets the gallery's name. Used by the wizard (usually).
     *
     * @return Name of the gallery.
     */
    QString getName();

    /**
     * Gets path with images. Used by the wizard (usually).
     *
     * @return Image directory path.
     */
    QString getImagePath();

    /**
     * Checks if the image path is valid.
     *
     * @param path Path to verify.
     *
     * @return @c true Path is valid.
     * @return @c false Path is invalid.
     */
    bool checkImagesPath(const QString &path);
    /**
     * Checks if the name is valid.
     *
     * @param name Name to verify.
     *
     * @return @c true Name is valid.
     * @return @c false Name is invalid.
     */
    bool checkName(const QString &name);

  protected:
    /**
     * Overloaded method.
     */
    void initialise();

  private:
    GDialogs::GWizard *m_wizard;

  private slots:
    /**
   * Defines the behaviour when text changes.
     */
    void slotTextChanged();
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

#endif
