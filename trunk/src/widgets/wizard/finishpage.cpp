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
#include "finishpage.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>

namespace GWidgets {

namespace GWizard {

FinishPage::FinishPage()
 : QWizardPage()
{
  setTitle(tr("Finish"));
  setSubTitle(tr("Wizard is complete"));
  QLabel *label = new QLabel(this);
  label->setText(tr("<p>The gallery has been created. You can now edit the pictures, remove them or add new pictures to your liking.</p><p>To close this wizard, press <br>Finish</b>.</p>"));
  label->setWordWrap(true);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(label);

  setLayout(layout);
}


FinishPage::~FinishPage()
{
}


}

}
