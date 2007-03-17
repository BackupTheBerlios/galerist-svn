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
#include "newgallerywizard.h"

#include "widgets/wizard/welcomepage.h"
#include "widgets/wizard/selectionpage.h"
#include "widgets/wizard/summarypage.h"
#include "widgets/wizard/copypage.h"
#include "widgets/wizard/finishpage.h"

namespace GDialogs {

NewGalleryWizard::NewGalleryWizard(QWidget *parent)
 : QWizard(parent)
{
  setWindowTitle(tr("Create a new gallery"));

  setDefaultProperty("QComboBox", "currentText", "currentIndexChanged()");

  addPage(new GWidgets::GWizard::WelcomePage);
  addPage(new GWidgets::GWizard::SelectionPage);
  addPage(new GWidgets::GWizard::SummaryPage);
  addPage(new GWidgets::GWizard::CopyPage);
  addPage(new GWidgets::GWizard::FinishPage);
}


NewGalleryWizard::~NewGalleryWizard()
{
}


}
