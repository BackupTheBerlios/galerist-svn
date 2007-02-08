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
#include "wizardpage.h"

#include "dialogs/gwizard.h"

namespace GWidgets
{

WizardPage::WizardPage(QWidget *parent)
    : QWidget(parent),
    m_initialised(false),
    m_verified(true),
    m_steps(1),
    m_currentStep(0)
{}

WizardPage::~WizardPage()
{
  m_wizard = 0;
}

bool WizardPage::verified()
{
  return m_verified;
}

void WizardPage::startInitialisation()
{
  if (!m_initialised) {
    initialise();
    m_initialised = true;
  }
}

bool WizardPage::initialised()
{
  return m_initialised;
}

unsigned short WizardPage::steps()
{
  return m_steps;
}

unsigned short WizardPage::currentStep()
{
  return m_currentStep;
}

void WizardPage::setParent(QObject *parent)
{
  m_wizard = static_cast<GDialogs::GWizard*>(parent);
  QObject::setParent(parent);
}

GDialogs::GWizard *WizardPage::getWizard()
{
  return m_wizard;
}

void WizardPage::setVerification(bool verified)
{
  m_verified = verified;
  emit verificationChanged(verified);
}

void WizardPage::setSteps(unsigned short steps)
{
  m_steps = steps;
}

void WizardPage::nextStep()
{
  if ((m_steps - 1) == m_currentStep)
    qWarning("WizardPage: Out of bounds!");
  else
    m_currentStep++;
}

void WizardPage::previousStep()
{
  if (!m_currentStep)
    qWarning("WizardPage: Out of bounds!");
  else
    m_currentStep--;
}

void WizardPage::nextEvent()
{}

}
