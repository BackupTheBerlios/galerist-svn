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
#include "dialogs/wizard.h"
#include "dialogs/newwizard.h"

#include "newselectionpage.h"

#include <QtCore/QDir>
#include <QtGui/QPixmap>
#include <QtCore/QtDebug>

namespace GWidgets
{

NewCopyPage::NewCopyPage(Step step, QWidget *parent)
    : QWidget(parent), Ui::NewCopyPage()
{
  //Setup the .ui file.
  setupUi(this);

#ifdef _WIN32
  linuxLabel->setVisible(false);
#endif
#ifdef unix
  windowsLabel->setVisible(false);
#endif

  //Hide parts of the page.
  switch (step) {
    case (StepWarning) : {
      hideProgress();
      hideFinish();
      break;
    }
    case (StepCopy) : {
      startCopy();
      hideFinish();

      break;
    }
  }
}

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
  GDialogs::NewWizard *wizard = static_cast<GDialogs::NewWizard*>(parent());
  wizard->enableButtons(false);

  // We copy the images to the right place
  QObject *copyProcess = GCore::Data::self()->getImageModel()->createGallery(wizard->getGalleryName(), wizard->getGalleryPath());

  // We close the wizard if the copy failed
  if (!copyProcess) {
    wizard->reject();
    return;
  }

  // Connect the gallery handler.
  qRegisterMetaType<QImage>("QImage");

  connect(copyProcess, SIGNAL(signalProgress(int, int, const QString&, const QImage&)), this, SLOT(slotProgress(int, int, const QString&, const QImage&)));
}

void NewCopyPage::slotProgress(int finished, int total, const QString &current, const QImage &image)
{
  GDialogs::Wizard *wizard = static_cast<GDialogs::Wizard*>(parent());

  progressBar->setMaximum(total);
  progressBar->setValue(finished);

  if (!image.isNull())
    imageLabel->setPixmap(QPixmap::fromImage(image));

  wizard->enableBack(false);

  if (current.isEmpty()) {
    progressLabel->setText(tr("Copy cancelled."));
    wizard->enableButtons(true, true);
    return;
  }

  if (finished != total) {
    //Add the name to the progress label.
    progressLabel->setText(tr("Copying picture %1 ...").arg(current));
    wizard->enableButtons(false);
  } else if (current.isEmpty()) {
    progressLabel->setText(tr("Copy cancelled."));
    wizard->enableButtons(true, true);
    imageLabel->setPixmap(QPixmap(":/images/cancel.png").scaled(128, 128));
    return;
  } else if (current == "failed") {
    progressLabel->setText(tr("Copy failed."));
    wizard->enableButtons(true, true);
    imageLabel->setPixmap(QPixmap(":/images/failed.png").scaled(128, 128));
    return;
  } else {
    //The copy has finished!
    progressLabel->setText(tr("Copy finished"));
    imageLabel->setPixmap(QPixmap(":/images/complete.png"));
    finishLabel->setVisible(true);
    wizard->enableButtons(true);
  }
}

NewCopyPage::~NewCopyPage()
{}


}
