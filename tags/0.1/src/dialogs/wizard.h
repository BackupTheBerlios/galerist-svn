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
#ifndef GCOREGDIALOGSWIZARD_H
#define GCOREGDIALOGSWIZARD_H

#include <QtGui/QDialog>

class QHBoxLayout;
class QVBoxLayout;

namespace GDialogs
{

/**
 * A template class for wizards.
 * @short A template class for wizards.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class Wizard : public QDialog
{
    Q_OBJECT
  public:
    /**
     * A default constructor.
     *
     * @param caption Caption of the wizard.
     * @param parent Pointer to the parent widget.
     */
    Wizard(const QString &caption, QWidget *parent = 0);

    /**
     * Method that enables or disables next or finish button. Depends on which page you are or the wizard has been cancelled.
     *
     * @param enable Self explanatory.
     * @param cancel Is the wizard cancelled.
     */
    void enableButtons(bool enable, bool cancel = false);
    /**
     * Enables or disables back button.
     *
     * @param enable Self explanatory.
     */
    void enableBack(bool enable);

    /**
     * Virtual method for defining how to fail a process.
     */
    virtual void failed() = 0;

    /**
     * A default destructor.
     */
    ~Wizard();

  protected:
    /**
     * A virtual method for defining how to create a page.
     *
     * @param index Index of the new page.
     *
     * @return Pointer to the newly created page.
     */
    virtual QWidget *createPage(int index) = 0;
    /**
     * Method for setting number of pages that the wizard will have.
     *
     * @param numPages Number of pages.
     */
    void setNumPages(int numPages);

    // @TODO: Need to find a better way for this !!!
  protected:
    /**
     * Method for swapping pages. (When you press next or back you need to swap the page)
     *
     * @param oldPage Pointer to the old page.
     */
    void swapPage(QWidget *oldPage);

    //GUI items.
    QPushButton *m_cancelButton;
    QPushButton *m_backButton;
    QPushButton *m_nextButton;
    QPushButton *m_finishButton;

    //Layouts.
    QHBoxLayout *m_buttonLayout;
    QVBoxLayout *m_mainLayout;

    //Variables.
    QList<QWidget*> m_history;
    int m_numPages;

  private slots:
    /**
     * Slot that is called when the next button is pressed.
     */
    void slotNextPressed();
    /**
     * Slot that is called when the back button is pressed.
     */
    void slotBackPressed();

};

}

#endif
