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

#include "widgets/photoview.h"

namespace GWidgets
{

PhotoControl::PhotoControl(QWidget *parent)
    : QWidget(parent),
      m_currentOperation(NoOperation),
      m_saved(false)
{
  QHBoxLayout *mainLayout = new QHBoxLayout(this);

  m_controlPanel = new QStackedWidget(this);
  mainLayout->addWidget(m_controlPanel);

  m_controlPanel->addWidget(setupMainPage());
  m_controlPanel->addWidget(setupCropControl());

  setLayout(mainLayout);
}


PhotoControl::~PhotoControl()
{}

void PhotoControl::connectView(PhotoView *view)
{
  // Generic commands
  connect(m_mainPage.rotateCCWButton, SIGNAL(clicked()), view, SLOT(rotateSelectedImageCCW()));
  connect(m_mainPage.rotateCWButton, SIGNAL(clicked()), view, SLOT(rotateSelectedImageCW()));
  connect(m_mainPage.editButton, SIGNAL(clicked()), view, SLOT(slotEditPhoto()));
  
  connect(m_mainPage.zoomOutButton, SIGNAL(clicked()), view, SLOT(slotZoomOutPhoto()));
  connect(m_mainPage.zoomInButton, SIGNAL(clicked()), view, SLOT(slotZoomInPhoto()));
  connect(m_mainPage.zoomInputButton, SIGNAL(clicked()), view, SLOT(slotZoomInputPhoto()));
  connect(m_mainPage.zoomScreenButton, SIGNAL(clicked()), view, SLOT(slotZoomScreenPhoto()));
  connect(m_mainPage.actualSizeButton, SIGNAL(clicked()), view, SLOT(slotZoomActualPhoto()));
  connect(m_mainPage.backButton, SIGNAL(clicked()), view, SLOT(slotPreviousPhoto()));
  connect(m_mainPage.nextButton, SIGNAL(clicked()), view, SLOT(slotNextPhoto()));
  connect(m_mainPage.closeButton, SIGNAL(clicked()), view, SLOT(slotExitEdit()));

  connect(this, SIGNAL(operationSelected(int)), view, SLOT(initiateOperation(int)));
  connect(this, SIGNAL(cancelOperation(int)), view, SLOT(cancelOperation(int)));
  connect(this, SIGNAL(saveChange(int, const QMap<int, QVariant>&)), view, SLOT(saveOperation(int, const QMap<int, QVariant>&)));

  connect(view, SIGNAL(photoEditSelectionChanged(int, int)), this, SLOT(checkNavigation(int, int)));

  connect(view, SIGNAL(areaSelected(const QRect&)), this, SLOT(valuesChanged()));
}

void PhotoControl::checkNavigation(int newLocation, int totalImages)
{
  if (totalImages == 1) {
    m_mainPage.backButton->setEnabled(false);
    m_mainPage.nextButton->setEnabled(false);
  } else if (totalImages <= newLocation + 1) {
    m_mainPage.backButton->setEnabled(true);
    m_mainPage.nextButton->setEnabled(false);
  } else if (newLocation == 0) {
    m_mainPage.backButton->setEnabled(false);
    m_mainPage.nextButton->setEnabled(true);
  } else {
    m_mainPage.backButton->setEnabled(true);
    m_mainPage.nextButton->setEnabled(true);
  }
}

QWidget *PhotoControl::setupMainPage()
{
  QWidget *mainPage = new QWidget(m_controlPanel);
  m_mainPage.setupUi(mainPage);

  // Connect the buttons
  connect(m_mainPage.cropButton, SIGNAL(clicked()), this, SLOT(selectCrop()));

  return mainPage;
}

QWidget *PhotoControl::setupCropControl()
{
  QWidget *page = new QWidget(m_controlPanel);
  m_cropPage.setupUi(page);

  // Connect the buttons
  connect(m_cropPage.cancelButton, SIGNAL(clicked()), this, SLOT(restore()));
  connect(m_cropPage.saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));

  return page;
}

void PhotoControl::restore()
{
  m_controlPanel->setCurrentIndex(0);

  if (!m_saved)
    emit cancelOperation(m_currentOperation);
  else
    m_saved = false;

  switch (m_currentOperation) {
    case (Crop) : {
      m_cropPage.saveButton->setEnabled(false);
      break;
    }
    default : {
      break;
    }
  }

  m_currentOperation = NoOperation;
}

void PhotoControl::saveChanges()
{
  QMap<int, QVariant> params;
  switch (m_currentOperation) {
    case (Crop) : {
      emit saveChange(Crop, params);
      break;
    }
    default : {
      qDebug("This option is unknown!");
      break;
    }
  }

  m_saved = true;
  restore();
}

void PhotoControl::valuesChanged()
{
  switch (m_currentOperation) {
    case (Crop) : {
      m_cropPage.saveButton->setEnabled(true);
      break;
    }
    default : {
      qDebug("This option is unknown.");
      break;
    }
  }
}

void PhotoControl::selectCrop()
{
  m_controlPanel->setCurrentIndex(1);

  m_currentOperation = Crop;
  emit operationSelected(m_currentOperation);
}

}
