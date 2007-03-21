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
#ifndef GWIDGETS_GWIZARDFINISHPAGE_H
#define GWIDGETS_GWIZARDFINISHPAGE_H

#include <QtGui/QWizardPage>

namespace GWidgets {

namespace GWizard {

/**
 * @short Finish page for GDialogs::NewGalleryWizard.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class FinishPage : public QWizardPage
{
  Q_OBJECT
  public:
    /**
     * A default constructor.
     */
    FinishPage();

    /**
     * A default destructor.
     */
    ~FinishPage();

};

}

}

#endif
