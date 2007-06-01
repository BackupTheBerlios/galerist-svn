/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÅ¡nik                                 *
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

#include "core/jobs/transformationjob.h"

#include "widgets/photoview.h"

namespace GWidgets
{

PhotoControl::PhotoControl(QWidget *parent)
    : QWidget(parent),
    m_currentOperation(GCore::GJobs::TransformationJob::NoOperation),
    m_saved(false)
{
  QHBoxLayout *mainLayout = new QHBoxLayout(this);

  m_controlPanel = new QStackedWidget(this);
  mainLayout->addWidget(m_controlPanel);

  m_controlPanel->addWidget(setupMainPage());
  m_controlPanel->addWidget(setupCropControl());
  m_controlPanel->addWidget(setupBlurPage());
  m_controlPanel->addWidget(setupSharpenPage());
  m_controlPanel->addWidget(setupResizePage());

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
  connect(view, SIGNAL(imageSwitched(const QSize&)), this, SLOT(switchedImage(const QSize&)));
  connect(view, SIGNAL(enableControls(bool)), this, SLOT(enableControls(bool)));
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
  connect(m_mainPage.resizeButton, SIGNAL(clicked()), this, SLOT(selectResize()));

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

  m_blurPage.titleBox->setTitle(tr("Blur"));

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

  m_sharpenPage.titleBox->setTitle(tr("Sharpen"));

  // Connect the buttons
  connect(m_sharpenPage.cancelButton, SIGNAL(clicked()), this, SLOT(restore()));
  connect(m_sharpenPage.saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
  connect(m_sharpenPage.slider, SIGNAL(valueChanged(int)), this, SLOT(valuesChanged()));
  connect(m_sharpenPage.previewButton, SIGNAL(clicked()), this, SLOT(requestPreview()));

  return page;
}

QWidget *PhotoControl::setupResizePage()
{
  QWidget *page = new QWidget(m_controlPanel);
  m_resizePage.setupUi(page);

  // Connect the buttons
  connect(m_resizePage.heightBox, SIGNAL(valueChanged(int)), this, SLOT(keepAspectHeight(int)));
  connect(m_resizePage.widthBox, SIGNAL(valueChanged(int)), this, SLOT(keepAspectWidth(int)));
  connect(m_resizePage.aspectBox, SIGNAL(clicked()), this, SLOT(valuesChanged()));
  connect(m_resizePage.cancelButton, SIGNAL(clicked()), this, SLOT(restore()));
  connect(m_resizePage.saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
  connect(m_resizePage.previewButton, SIGNAL(clicked()), this, SLOT(requestPreview()));

  return page;
}

void PhotoControl::keepAspectRatio(bool heightChanged, int value)
{
  if (m_resizePage.aspectBox->isChecked()) {
    // Temporarily disconnect the eight and width boxes
    disconnect(m_resizePage.heightBox, SIGNAL(valueChanged(int)), this, SLOT(keepAspectHeight(int)));
    disconnect(m_resizePage.widthBox, SIGNAL(valueChanged(int)), this, SLOT(keepAspectWidth(int)));

    qreal ratio = (qreal) m_imageSize.height() / (qreal) m_imageSize.width();
    if (heightChanged)
      m_resizePage.widthBox->setValue(static_cast<int>(static_cast<qreal>(value) / ratio));
    else
      m_resizePage.heightBox->setValue(static_cast<int>(static_cast<qreal>(value) * ratio));

    // Reconnect the eight and width boxes
    connect(m_resizePage.heightBox, SIGNAL(valueChanged(int)), this, SLOT(keepAspectHeight(int)));
    connect(m_resizePage.widthBox, SIGNAL(valueChanged(int)), this, SLOT(keepAspectWidth(int)));
  }
}

void PhotoControl::enableControls(bool enable)
{
  setEnabled(enable);
}

void PhotoControl::restore()
{
  m_controlPanel->setCurrentIndex(0);

  if (!m_saved)
    emit cancelOperation(m_currentOperation);
  else
    m_saved = false;

  // Crop
  m_cropPage.saveButton->setEnabled(false);

  // Blur
  m_blurPage.saveButton->setEnabled(false);
  m_blurPage.slider->setValue(0);

  // Sharpen
  m_sharpenPage.saveButton->setEnabled(false);
  m_sharpenPage.slider->setValue(0);

  // Resize
  m_resizePage.saveButton->setEnabled(false);
  m_resizePage.widthBox->setValue(m_imageSize.width());
  m_resizePage.heightBox->setValue(m_imageSize.height());

  m_params.clear();

  m_currentOperation = GCore::GJobs::TransformationJob::NoOperation;
}

void PhotoControl::switchedImage(const QSize &size)
{
  m_imageSize = size;
  restore();
}

void PhotoControl::saveChanges()
{
  emit saveChange(m_currentOperation, m_params);

  m_saved = true;
  restore();
}

void PhotoControl::valuesChanged()
{
  switch (m_currentOperation) {
    case (GCore::GJobs::TransformationJob::Crop) : {
      m_cropPage.saveButton->setEnabled(true);
      break;
    }
    case (GCore::GJobs::TransformationJob::Blur) : {
      m_params.insert(GCore::GJobs::TransformationJob::NumberFilter, static_cast<quint8>(m_blurPage.slider->value()));
      break;
    }
    case (GCore::GJobs::TransformationJob::Sharpen) : {
      m_params.insert(GCore::GJobs::TransformationJob::NumberFilter, m_sharpenPage.slider->value());
      break;
    }
    case (GCore::GJobs::TransformationJob::Resize) : {
      m_params.insert(GCore::GJobs::TransformationJob::Size, QSize(m_resizePage.widthBox->value(), m_resizePage.heightBox->value()));
      break;
    }
    default : {
      break;
    }
  }
}

void PhotoControl::keepAspectHeight(int height)
{
  keepAspectRatio(true, height);
  valuesChanged();
}

void PhotoControl::keepAspectWidth(int width)
{
  keepAspectRatio(false, width);
  valuesChanged();
}

void PhotoControl::requestPreview()
{
  m_blurPage.saveButton->setEnabled(true);
  m_sharpenPage.saveButton->setEnabled(true);
  m_resizePage.saveButton->setEnabled(true);
  emit valuesChange(m_currentOperation, m_params);
}

void PhotoControl::selectCrop()
{
  m_controlPanel->setCurrentIndex(1);

  m_currentOperation = GCore::GJobs::TransformationJob::Crop;
  emit operationSelected(m_currentOperation);
}

void PhotoControl::selectBlur()
{
  m_controlPanel->setCurrentIndex(2);

  // Add the default value
  m_params.insert(GCore::GJobs::TransformationJob::NumberFilter, 0);

  m_currentOperation = GCore::GJobs::TransformationJob::Blur;
  emit operationSelected(m_currentOperation);
}

void PhotoControl::selectSharpen()
{
  m_controlPanel->setCurrentIndex(3);

  // Add the default value
  m_params.insert(GCore::GJobs::TransformationJob::NumberFilter, 0);

  m_currentOperation = GCore::GJobs::TransformationJob::Sharpen;
  emit operationSelected(m_currentOperation);
}

void PhotoControl::selectResize()
{
  m_controlPanel->setCurrentIndex(4);

  m_currentOperation = GCore::GJobs::TransformationJob::Resize;
  emit operationSelected(m_currentOperation);
}

}
