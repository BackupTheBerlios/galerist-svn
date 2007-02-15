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
#include "newgallery.h"

#include "widgets/newwelcomepage.h"
#include "widgets/newselectionpage.h"
#include "widgets/newimageselectpage.h"
#include "widgets/newcopypage.h"
#include "widgets/newfinishpage.h"

#include <QtCore/QTimer>

namespace GDialogs
{

NewGallery::NewGallery(QWidget *parent)
    : GWizard(parent)
{
  addPage(new GWidgets::NewWelcomePage);
  addPage(new GWidgets::NewSelectionPage);
  addPage(new GWidgets::NewImageSelectPage);
  addPage(new GWidgets::NewCopyPage);
  addPage(new GWidgets::NewFinishPage);
}


NewGallery::~NewGallery()
{}

void NewGallery::accept()
{
  QDialog::accept();
}

void NewGallery::reject()
{
  stop();
  hide();
  QTimer::singleShot(100, this, SLOT(close()));
}


}
