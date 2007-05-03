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
  m_controlPanel->addWidget(setupBlurPage());
  m_controlPanel->addWidget(setupSharpenPage());

  setLayout(mainLayout);
}


PhotoControl::~PhotoControl()
{}

void PhotoControl::connectView(PhotoView *view)
{
  // Transformation commands
  connect(m_mainPage.rotateCCWButton, SIGNAL(clicked()), view, SLOT(rotateSelectedImageCCW()));
  connect(m_mainPage.rotateCWButton, SIGNAL(clicked()), view, SLOT(rotateSelectedImageCW()));
  connect(m_mainPage.editButton, SIGNAL(clicked()), view, SLOT(slotEditPhoto()));

  // Zoom commands
  connect(m_mainPage.zoomOutButton, SIGNAL(clicked()), view, SLOT(slotZoomOutPhoto()));
  connect(m_mainPage.zoomInButton, SIGNAL(clicked()), view, SLOT(slotZoomInPhoto()));
  connect(m_mainPage.zoomInputButton, SIGNAL(clicked()), view, SLOT(slotZoomInputPhoto()));
  connect(m_mainPage.zoomScreenButton, SIGNAL(clicked()), view, SLOT(slotZoomScreenPhoto()));
  connect(m_mainPage.actualSizeButton, SIGNAL(clicked()), view, SLOT(slotZoomActualPhoto()));

  // Navigation commands & close command
  connect(m_mainPage.backButton, SIGNAL(clicked()), view, SLOT(slotPreviousPhoto()));
  connect(m_mainPage.nextButton, SIGNAL(clicked()), view, SLOT(slotNextPhoto()));
  connect(m_mainPage.closeButton, SIGNAL(clicked()), view, SLOT(slotExitEdit()));

  // Notifications
  connect(this, SIGNAL(operationSelected(int)), view, SLOT(initiateOperation(int)));
  connect(this, SIGNAL(cancelOperation(int)), view, SLOT(cancelOperation(int)));
  connect(this, SIGNAL(saveChange(int, const QMap<int, QVariant>&)), view, SLOT(saveOperation(int, const QMap<int, QVariant>&)));
  connect(this, SIGNAL(valuesChange(int, const QMap<int, QVariant>&)), view, SLOT(previewOperation(int, const QMap<int, QVariant>&)));

  // Feedback information
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
  connect(m_mainPage.blurButton, SIGNAL(clicked()), this, SLOT(selectBlur()));
  connect(m_mainPage.sharpButton, SIGNAL(clicked()), this, SLOT(selectSharpen()));

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

QWidget *PhotoControl::setupBlurPage()
{
  QWidget *page = new QWidget(m_controlPanel);
  m_blurPage.setupUi(page);

    // Connect the buttons
  connect(m_blurPage.cancelButton, SIGNAL(clicked()), this, SLOT(restore()));
  connect(m_blurPage.saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
  connect(m_blurPage.slider, SIGNAL(valueChanged(int)), this, SLOT(valuesChanged()));
  connect(m_blurPage.previewButton, SIGNAL(clicked()), this, SLOT(requestPreview()));

  return page;
}

QWidget *PhotoControl::setupSharpenPage()
{
  QWidget *page = new QWidget(m_controlPanel);
  m_sharpenPage.setupUi(page);

    // Connect the buttons
  connect(m_sharpenPage.cancelButton, SIGNAL(clicked()), this, SLOT(restore()));
  connect(m_sharpenPage.saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
  connect(m_sharpenPage.slider, SIGNAL(valueChanged(int)), this, SLOT(valuesChanged()));
  connect(m_sharpenPage.previewButton, SIGNAL(clicked()), this, SLOT(requestPreview()));

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
    case (Blur) : {
      m_blurPage.saveButton->setEnabled(false);
      break;
    }
    default : {
      break;
    }
  }

  m_params.clear();

  m_currentOperation = NoOperation;
}

void PhotoControl::saveChanges()
{/*
  switch (m_currentOperation) {
    case (Crop) : {
      emit saveChange(Crop, m_params);
      break;
    }
    default : {
      qDebug("This option is unknown!");
      break;
    }
  }*/
  emit saveChange(m_currentOperation, m_params);

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
    case (Blur) : {
      m_blurPage.saveButton->setEnabled(true);
      m_params.insert(RepeatNumber, m_blurPage.slider->value());
      break;
    }
    case (Sharpen) : {
      m_sharpenPage.saveButton->setEnabled(true);
      m_params.insert(RepeatNumber, m_sharpenPage.slider->value());
      break;
    }
    default : {
      qDebug("This option is unknown.");
      break;
    }
  }
}

void PhotoControl::requestPreview()
{
  emit valuesChange(m_currentOperation, m_params);
}

void PhotoControl::selectCrop()
{
  m_controlPanel->setCurrentIndex(1);

  m_currentOperation = Crop;
  emit operationSelected(m_currentOperation);
}

void PhotoControl::selectBlur()
{
  m_controlPanel->setCurrentIndex(2);

  // Add the default value
  m_params.insert(RepeatNumber, 0);

  m_currentOperation = Blur;
  emit operationSelected(m_currentOperation);
}

void PhotoControl::selectSharpen()
{
  m_controlPanel->setCurrentIndex(3);

  // Add the default value
  m_params.insert(RepeatNumber, 0);

  m_currentOperation = Sharpen;
  emit operationSelected(m_currentOperation);
}

}
