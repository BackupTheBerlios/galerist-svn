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
#ifndef GWIDGETSWIZARDPAGE_H
#define GWIDGETSWIZARDPAGE_H

#include <QtGui/QWidget>

namespace GDialogs
{
class GWizard;
}

namespace GWidgets
{

/**
 * Template class for wizard pages.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class WizardPage : public QWidget
{
    Q_OBJECT
  signals:
    void verificationChanged(bool verification);

  public:
    WizardPage(QWidget *parent = 0);

    ~WizardPage();

    /**
     * Tells to the wizard if this page has been verified.
     */
    bool verified();

    /**
     * Security wrapper method.
     */
    void startInitialisation();

    /**
     * Tells the wizard if this page has already been initialised.
     */
    bool initialised();

    /**
     * Returns the number of steps this page has.
     */
    unsigned short steps();

    /**
     * Returns the number at which step the page is.
     * Note: The first step is 0.
     */
    unsigned short currentStep();

    /**
     * Overloaded method.
     */
    void setParent(QObject *parent);

  protected:
    /**
     * Gets the wizard.
     */
    GDialogs::GWizard *getWizard();

    /**
     * Initialisation method. Used for creating UI. Called when this page is selected.
     */
    virtual void initialise() = 0;

    /**
     * Sets if the page has been verified
     *
     * @param verify Verified?
     */
    void setVerification(bool verify);

    /**
     * Sets number of steps this page has.
     *
     * @param steps Number of steps.
     */
    void setSteps(unsigned short steps);

    /**
     * Go to next step.
     */
    void nextStep();

    /**
     * Go to previous step.
     */
    void previousStep();

    /**
     * An event that is called right before going to next page.
     */
    virtual void nextEvent();

    /**
     * An event that is called right before going to previous page.
     */
    virtual void backEvent();

    /**
     * An event that is called when the page has been selected.
     */
    virtual void viewEvent();

    /**
     * An event that is called when the wizard requests for all activities to stop.
     */
    virtual void stopEvent();

  private:
    bool m_initialised;
    GDialogs::GWizard *m_wizard;
    bool m_verified;
    unsigned short m_steps;
    unsigned short m_currentStep;

    friend class GDialogs::GWizard;

};

}

#endif
