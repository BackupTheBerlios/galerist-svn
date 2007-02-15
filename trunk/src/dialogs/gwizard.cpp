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

#include "gwizard.h"

#include "widgets/wizardpage.h"

#include <QtGui/QStackedWidget>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtCore/QVariant>

namespace GDialogs
{

GWizard::GWizard(QWidget *parent)
  : QDialog(parent),
    m_helpDialog(0),
    m_bigImage(0)
{
  setAttribute(Qt::WA_DeleteOnClose, true);

  // Create main layout
  QVBoxLayout *main = new QVBoxLayout(this);

  // Create the page area
  m_pages = new QStackedWidget(this);
  main->addWidget(m_pages);

  // Buttons' layout
  QHBoxLayout *buttons = new QHBoxLayout(this);

  // Help button
  m_help = new QPushButton(tr("&Help"), this);
  m_help->setEnabled(false);
  buttons->addWidget(m_help);

  // Spacing
  buttons->addStretch();

  // Back button
  m_back = new QPushButton(tr("&Back"), this);
  m_back->setEnabled(false);
  buttons->addWidget(m_back);

  // Next button
  m_next = new QPushButton(tr("&Next"), this);
  buttons->addWidget(m_next);

  // Finish button (hidden)
  m_finish = new QPushButton(tr("&Finish"), this);
  m_finish->setVisible(false);
  buttons->addWidget(m_finish);

  // Cancel button
  m_cancel = new QPushButton(tr("&Cancel"), this);
  buttons->addWidget(m_cancel);

  // Adding button layout adds to main layout
  main->addLayout(buttons);

  setLayout(main);

  // Connecting
  connect(m_cancel, SIGNAL(clicked()), this, SLOT(reject()));
  connect(m_finish, SIGNAL(clicked()), this, SLOT(accept()));
  connect(m_next, SIGNAL(clicked()), this, SLOT(next()));
  connect(m_back, SIGNAL(clicked()), this, SLOT(back()));
  connect(m_help, SIGNAL(clicked()), this, SLOT(help()));
  connect(this, SIGNAL(signalPageAdded()), this, SLOT(slotUpdateNavigation()));
  connect(this, SIGNAL(signalChangedPage(int)), this, SLOT(slotUpdateNavigation(int)));
}

void GWizard::setValidInput(bool valid, const QWidget *requester)
{
  if (m_pages->currentWidget() == requester) {
    m_next->setEnabled(valid);
    m_finish->setEnabled(valid);
  } else {
    qWarning("Wizard: Widget requested an action before it's time!");
  }
}

void GWizard::setValue(const QString &atribute, const QVariant &value)
{
  QString a = value.toString();
  m_wizardData.insert(atribute, value);
}

QVariant GWizard::getValue(const QString &atribute)
{
  return m_wizardData.value(atribute);
}

GWizard::~GWizard()
{}

void GWizard::addPage(GWidgets::WizardPage *page)
{
  if (!page) {
    qWarning("Wizard: Tried to add an invalid page!");
    return;
  }

  // Initialise the first page
  bool initialise = false;
  if (!m_pages->count())
    initialise = true;

  if (initialise)
    page->startInitialisation();

  page->setParent(this);
  m_pages->addWidget(page);

  emit signalPageAdded();
}

void GWizard::setHelp(QWidget *help)
{
  if (!help) {
    m_helpDialog->close();
    delete m_helpDialog;
    m_helpDialog = 0;
    m_help->setEnabled(false);
  } else {
    m_helpDialog = help;
    m_helpDialog->setParent(this);
    m_help->setEnabled(true);
  }
}

void GWizard::stop()
{
  static_cast<GWidgets::WizardPage*> (m_pages->currentWidget())->stopEvent();
}

void GWizard::accept()
{
  QDialog::accept();
}

void GWizard::slotUpdateNavigation(int index)
{
  if (index == 0)
    m_back->setEnabled(false);
  else
    m_back->setEnabled(true);

  if ((m_pages->count() - 1) == index) {
    m_next->setVisible(false);
    m_finish->setVisible(true);
  } else {
    m_next->setVisible(true);
    m_finish->setVisible(false);
  }
}

void GWizard::next()
{
  if ((m_pages->count() - 1) == m_pages->currentIndex()) {
    qWarning("Wizard: Page out of bounds!");
    return;
  }

  GWidgets::WizardPage *oldPage = static_cast<GWidgets::WizardPage*>(m_pages->widget(m_pages->currentIndex()));
  disconnect(oldPage, SIGNAL(verificationChanged(bool)), m_next, SLOT(setEnabled(bool)));
  disconnect(oldPage, SIGNAL(verificationChanged(bool)), m_finish, SLOT(setEnabled(bool)));

  oldPage->nextEvent();

  if (oldPage->steps() == 1 || (oldPage->steps() - 1) == oldPage->currentStep()) {
    GWidgets::WizardPage *page = static_cast<GWidgets::WizardPage*>(m_pages->widget(m_pages->currentIndex() + 1));
    connect(page, SIGNAL(verificationChanged(bool)), m_next, SLOT(setEnabled(bool)));
    connect(page, SIGNAL(verificationChanged(bool)), m_finish, SLOT(setEnabled(bool)));
    page->startInitialisation();
    page->viewEvent();

    m_pages->setCurrentIndex(m_pages->currentIndex() + 1);
  }

  emit signalChangedPage(m_pages->currentIndex());
}

void GWizard::back()
{
  if ((m_pages->count() - 1) == 0) {
    qWarning("Wizard: Page out of bounds!");
    return;
  }

  GWidgets::WizardPage *oldPage = static_cast<GWidgets::WizardPage*>(m_pages->widget(m_pages->currentIndex()));
  disconnect(oldPage, SIGNAL(verificationChanged(bool)), m_next, SLOT(setEnabled(bool)));
  disconnect(oldPage, SIGNAL(verificationChanged(bool)), m_finish, SLOT(setEnabled(bool)));
  oldPage->backEvent();

  GWidgets::WizardPage *page = static_cast<GWidgets::WizardPage*>(m_pages->widget(m_pages->currentIndex() - 1));
  connect(page, SIGNAL(verificationChanged(bool)), m_next, SLOT(setEnabled(bool)));
  connect(page, SIGNAL(verificationChanged(bool)), m_finish, SLOT(setEnabled(bool)));
  page->viewEvent();

  m_pages->setCurrentIndex(m_pages->currentIndex() - 1);

  emit signalChangedPage(m_pages->currentIndex());
}

void GWizard::help()
{
  if (!m_helpDialog) {
    qWarning("Wizard: Invalid Help call!");
    return;
  }

  m_helpDialog->show();
}

}
