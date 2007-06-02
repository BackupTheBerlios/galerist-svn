/***************************************************************************
 *   Copyright (C) 2006 by Gregor Kališnik                                 *
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
#ifndef GWIDGETS_GWIZARDWELCOMEPAGE_H
#define GWIDGETS_GWIZARDWELCOMEPAGE_H

#include <QWizardPage>

namespace GWidgets
{

namespace GWizard
{

/**
 * @short Welcome page for the GDialogs::NewGalleryWizard.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class WelcomePage : public QWizardPage
{
    Q_OBJECT
  public:
    /**
     * A default constructor.
     */
    WelcomePage();

    /**
     * A destructor.
     */
    ~WelcomePage();

};

}

}

#endif
