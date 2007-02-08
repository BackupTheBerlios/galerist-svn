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

#ifndef GWIZARD__H
#define GWIZARD__H

#include <QtGui/QDialog>
#include <QtCore/QMap>


class QStackedWidget;
class QPushButton;
class QLabel;
class QVariant;

namespace GWidgets
{
class WizardPage;
}

namespace GDialogs
{

/**
 * A template class for wizards.
 * @short A template class for wizards.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class GWizard : public QDialog
{
    Q_OBJECT
  signals:
    void signalChangedPage(int index);
    void signalPageAdded();

  public:
    /**
     * A default constructor.
     *
     * @param parent Parent widget of this wizard.
     */
    GWizard(QWidget *parent = 0);

    /**
     * Is input valid.
     *
     * @param valid Valid input or not.
     * @param requester Who is requesting this action.
     */
    void setValidInput(bool valid, const QWidget *requester);

    /**
     * Sets a value for the atribute.
     *
     * @param atribute Name of the atribute.
     * @param value Value of the atribute.
     */
    void setValue(const QString &atribute, const QVariant &value);
    /**
     * Gets atributes value.
     *
     * @param atribute Name of the atribute.
     *
     * @return Value of the atribute.
     */
    QVariant getValue(const QString &atribute);

    /**
     * A default destructor.
     */
    ~GWizard();

  protected:
    /**
     * A method for adding pages.
     *
     * @param page A Page to add.
     */
    void addPage(GWidgets::WizardPage *page);

    /**
     * Sets Help dialog.
     *
     * @param help Dialog with help.
     */
    void setHelp(QWidget *help);

  protected slots:
    /**
     * Overloaded accept method.
     */
    virtual void accept() = 0;

    /**
     * Premature cancelation of wizard.
     */
    virtual void reject() = 0;

  private slots:
    /**
     * Update the navigation buttons.
     */
    void slotUpdateNavigation(int index = 0);

    /**
     * Switches to next page.
     */
    void next();
    /**
     * Switches to previous page.
     */
    void back();

    /**
     * Shows the help dialog.
     */
    void help();

  private:
    QStackedWidget *m_pages;
    QPushButton *m_help;
    QPushButton *m_back;
    QPushButton *m_next;
    QPushButton *m_finish;
    QPushButton *m_cancel;

    QWidget *m_helpDialog;
    QLabel *m_bigImage;
    QWidget *m_topSlide;

    QMap<QString, QVariant> m_wizardData;
};

}

#endif
