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
#ifndef GWIDGETSNEWWELCOMEPAGE_H
#define GWIDGETSNEWWELCOMEPAGE_H

#include <QtGui/QWidget>
#include "ui_newwelcomepage.h"

namespace GWidgets
{

/**
 * Class that represents a welcome page of the wizard.
 * @short Welcome page.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class NewWelcomePage : public QWidget, private Ui::NewWelcomePage
{
    Q_OBJECT
  public:
    /**
     * Default constructor.
     *
     * @param parent Parent widget.
     */
    NewWelcomePage(QWidget *parent = 0);

    /**
     * Default destructor.
     */
    ~NewWelcomePage();

};

}

#endif
