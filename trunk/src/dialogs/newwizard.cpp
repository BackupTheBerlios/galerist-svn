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
#include "newwizard.h"

#include "widgets/newwelcomepage.h"
#include "widgets/newselectionpage.h"
#include "widgets/newcopypage.h"
#include "widgets/newfinishpage.h"

#include "core/data.h"
#include "core/imagemodel.h"

#include <QtCore/QDir>

namespace GDialogs
{

NewWizard::NewWizard(const QString &caption, QWidget *parent)
    : GDialogs::Wizard(caption, parent)
{
  // Sets the number of pages
  setNumPages(5);
}

QWidget *NewWizard::createPage(int index)
{
  // Decide wich page is to create now
  switch (index) {
    case (0) : {
      m_welcomePage = new GWidgets::NewWelcomePage(this);
      m_welcomePage->resize(600, 500);
      return m_welcomePage;
    }
    case (1) : {
      m_selectionPage = new GWidgets::NewSelectionPage(this);
      m_selectionPage->resize(600, 500);
      return m_selectionPage;
    }
    case (2) : {
      m_copyWarningPage = new GWidgets::NewCopyPage(GWidgets::NewCopyPage::StepWarning, this);
      m_copyWarningPage->resize(600, 500);
      return m_copyWarningPage;
    }
    case (3) : {
      m_copyProgressPage = new GWidgets::NewCopyPage(GWidgets::NewCopyPage::StepCopy, this);
      m_copyProgressPage->resize(600, 500);
      return m_copyProgressPage;
    }
    case(4): {
      m_finishPage = new GWidgets::NewFinishPage(this);
      m_finishPage->resize(600, 500);
      return m_finishPage;
    }
  }

  //If it is out of range... We return null.
  return 0;
}

void NewWizard::accept()
{
  //@TODO need to be done!
  //GCore::Data::self()->getImageModel()->refresh();

  //We close the wizard.
  QDialog::accept();
}

void NewWizard::reject()
{
  GCore::Data::self()->getImageModel()->stopCopy();

  failed();

  //QDialog::reject();
}

QString NewWizard::getGalleryName()
{
  return m_selectionPage->nameEdit->text();
}

QString NewWizard::getGalleryPath()
{
  return m_selectionPage->imagesEdit->text();
}

void NewWizard::failed()
{
  m_nextButton->setEnabled(false);
  m_finishButton->setEnabled(true);
  m_cancelButton->setEnabled(false);
  m_numPages = 1;

  QWidget *oldPage = m_history.takeLast();
  qDeleteAll(m_history);
  m_history.clear();
  m_history << new GWidgets::NewFinishPage(this, false);
  swapPage(oldPage);
  delete oldPage;
}

NewWizard::~NewWizard()
{}

}
