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
#include "newcopypage.h"

#include "core/data.h"
#include "core/imagemodel.h"
#include "dialogs/gwizard.h"

#include "widgets/newselectionpage.h"

#include <QtCore/QDir>
#include <QtGui/QPixmap>

namespace GWidgets
{

NewCopyPage::NewCopyPage(QWidget *parent)
    : WizardPage(parent)
{}

void NewCopyPage::hideProgress()
{
  progressFrame->setVisible(false);
  progressThumbnailFrame->setVisible(false);
}

void NewCopyPage::hideFinish()
{
  finishLabel->setVisible(false);
}

void NewCopyPage::startCopy()
{
  // Disable the next button.
  //GDialogs::NewWizard *wizard = static_cast<GDialogs::NewWizard*>(parent());
  //wizard->enableButtons(false);

  progressFrame->setVisible(true);
  progressThumbnailFrame->setVisible(true);

  setVerification(false);

  // We copy the images to the right place
  QObject *copyProcess = GCore::Data::self()->getImageModel()->createGallery(getWizard()->getValue("GalleryName").toString(), getWizard()->getValue("GalleryPath").toString());

  // We close the wizard if the copy failed
  /*if (!copyProcess) {
    getWizard->reject();
    return;
  }*/

  // Connect the gallery handler.
  qRegisterMetaType<QImage>("QImage");

  connect(copyProcess, SIGNAL(signalProgress(int, int, const QString&, const QImage&)), this, SLOT(slotProgress(int, int, const QString&, const QImage&)));
}

void NewCopyPage::slotProgress(int finished, int total, const QString &current, const QImage &image)
{
  //GDialogs::Wizard *wizard = static_cast<GDialogs::Wizard*>(parent());

  progressBar->setMaximum(total);
  progressBar->setValue(finished);

  if (!image.isNull())
    imageLabel->setPixmap(QPixmap::fromImage(image));

//  getWizard()->enableBack(false);

//   if (current.isEmpty()) {
//     progressLabel->setText(tr("Copy cancelled."));
//     setVerification(true);
//     //wizard->enableButtons(true, true);
//     return;
//   }

  if (finished != total) {
    //Add the name to the progress label.
    progressLabel->setText(tr("Copying picture %1 ...").arg(current));
    //wizard->enableButtons(false);
    setVerification(false);
  } else /*if (current.isEmpty()) {
        progressLabel->setText(tr("Copy cancelled."));
        //wizard->enableButtons(true, true);
        setVerification(true);
        imageLabel->setPixmap(QPixmap(":/images/cancel.png").scaled(128, 128));
        return;
      } else if (current == "failed") {
        progressLabel->setText(tr("Copy failed."));
        //wizard->enableButtons(true, true);
        setVerification(true);
        imageLabel->setPixmap(QPixmap(":/images/failed.png").scaled(128, 128));
        return;
      } else */ {
    //The copy has finished!
    progressLabel->setText(tr("Copy finished"));
    imageLabel->setPixmap(QPixmap(":/images/complete.png"));
    finishLabel->setVisible(true);
    nextEvent();
    //wizard->enableButtons(true);
    setVerification(true);
  }
}

NewCopyPage::~NewCopyPage()
{}

void NewCopyPage::initialise()
{
  setupUi(this);

  hideProgress();
  hideFinish();

  setSteps(3);

#ifdef _WIN32
  linuxLabel->setVisible(false);
#endif
#ifdef unix
  windowsLabel->setVisible(false);
#endif
}

void NewCopyPage::nextEvent()
{
  nextStep();
  switch (currentStep()) {
      // First step. The warning
    case (0) : {
      break;
    }
    // Second step. The copy
    case (1) : {
      startCopy();
      break;
    }
    // Third step. The finish
    case (2) : {
      finishLabel->setVisible(true);
      break;
    }
  }
}


}
