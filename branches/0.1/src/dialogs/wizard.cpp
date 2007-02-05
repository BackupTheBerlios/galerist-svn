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
#include "wizard.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

namespace GDialogs
{

Wizard::Wizard(const QString &caption, QWidget *parent)
    : QDialog(parent)
{
  // Create buttons
  m_cancelButton = new QPushButton(tr("&Cancel"));
  m_backButton = new QPushButton(tr("<- &Back"));
  m_nextButton = new QPushButton(tr("&Next ->"));
  m_finishButton = new QPushButton(tr("&Finish"));

  // Connect the buttons
  connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  connect(m_backButton, SIGNAL(clicked()), this, SLOT(slotBackPressed()));
  connect(m_nextButton, SIGNAL(clicked()), this, SLOT(slotNextPressed()));
  connect(m_finishButton, SIGNAL(clicked()), this, SLOT(accept()));

  // Button layout
  m_buttonLayout = new QHBoxLayout;
  m_buttonLayout->addStretch(1);
  m_buttonLayout->addWidget(m_cancelButton);
  m_buttonLayout->addWidget(m_backButton);
  m_buttonLayout->addWidget(m_nextButton);
  m_buttonLayout->addWidget(m_finishButton);

  // Main layout
  m_mainLayout = new QVBoxLayout;
  m_mainLayout->addLayout(m_buttonLayout);

  // Set the QDialog's layout
  setLayout(m_mainLayout);

  // Set the caption of the wizard
  setWindowTitle(caption);
}

void Wizard::enableButtons(bool enable, bool cancel)
{
  // If we are viewing the last page, control the finish button. Else use the next button
  if (m_history.size() == m_numPages || cancel)
    m_finishButton->setEnabled(enable);
  else
    m_nextButton->setEnabled(enable);
}

void Wizard::setNumPages(int numPages)
{
  // Set the number of pages
  m_numPages = numPages;

  // Create the first page
  m_history.append(createPage(0));
  swapPage(0);
}

void Wizard::swapPage(QWidget *oldPage)
{
  // Remove the old page
  if (oldPage) {
    oldPage->hide();
    m_mainLayout->removeWidget(oldPage);
  }

  // Replace it with the new page
  QWidget *newPage = m_history.last();
  m_mainLayout->insertWidget(0, newPage);
  newPage->show();
  newPage->setFocus();

  // Enable the back button when necessary
  m_backButton->setEnabled(m_history.size() != 1);

  // Switch between next and finish buttons
  if (m_history.size() == m_numPages) {
    m_nextButton->setDisabled(true);
    m_finishButton->setDefault(true);
    m_backButton->setEnabled(false);
  } else {
    m_finishButton->setDisabled(true);
    m_nextButton->setDefault(true);
  }
}

void Wizard::slotNextPressed()
{
  // Enables both buttons
  m_nextButton->setEnabled(true);
  m_finishButton->setEnabled(m_history.size() == m_numPages - 1);

  // Takes the current page, and creates the new one
  QWidget *oldPage = m_history.last();
  m_history.append(createPage(m_history.size()));

  // The new page is swaped with the current
  swapPage(oldPage);
}

void Wizard::slotBackPressed()
{
  // Enables both buttons
  m_nextButton->setEnabled(true);
  m_finishButton->setEnabled(true);

  // Destroys the current page, and swaps it with previous page
  QWidget *oldPage = m_history.takeLast();
  swapPage(oldPage);
  delete oldPage;
}

void Wizard::enableBack(bool enable)
{
  m_backButton->setEnabled(enable);
}

Wizard::~Wizard()
{}


}

