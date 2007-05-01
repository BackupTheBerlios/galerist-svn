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
#include "photocontrol.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedWidget>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>
#include <QtGui/QIcon>

#include "ui_photocontrol-main.h"

namespace GWidgets
{

PhotoControl::PhotoControl(QWidget *parent)
    : QWidget(parent)
{
  QHBoxLayout *mainLayout = new QHBoxLayout(this);

  m_controlPanel = new QStackedWidget(this);
  mainLayout->addWidget(m_controlPanel);

  // Setup main page
  QWidget *mainPage = new QWidget(m_controlPanel);
  Ui::PhotoControlMain mainPageLayout;
  mainPageLayout.setupUi(mainPage);

  rotateCCWButton = mainPageLayout.rotateCCWButton;
  rotateCWButton = mainPageLayout.rotateCWButton;
  editButton = mainPageLayout.editButton;
  cropButton = mainPageLayout.cropButton;

  zoomOutButton = mainPageLayout.zoomOutButton;
  zoomInButton = mainPageLayout.zoomInButton;
  zoomInputButton = mainPageLayout.zoomInputButton;
  zoomScreenButton = mainPageLayout.zoomScreenButton;
  actualSizeButton = mainPageLayout.actualSizeButton;

  nextButton = mainPageLayout.nextButton;
  backButton = mainPageLayout.backButton;
  closeButton = mainPageLayout.closeButton;
  saveButton = mainPageLayout.saveButton;

  m_controlPanel->addWidget(mainPage);

  setLayout(mainLayout);
}


PhotoControl::~PhotoControl()
{}


}
