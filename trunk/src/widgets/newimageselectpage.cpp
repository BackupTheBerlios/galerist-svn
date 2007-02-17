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

#include "newimageselectpage.h"

#include "dialogs/gwizard.h"

#include "core/data.h"
#include "core/jobs/readjob.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <QtGui/QListWidgetItem>

#include <QtDebug>

namespace GWidgets
{

NewImageSelectPage::NewImageSelectPage(QWidget* parent)
  : WizardPage(parent),
    m_job(0)
{}

NewImageSelectPage::~NewImageSelectPage()
{}

void NewImageSelectPage::initialise()
{
  setupUi(this);
  progressFrame->setVisible(false);
}

void NewImageSelectPage::nextEvent()
{
  QStringList selectedFileNames;

  QList<QListWidgetItem*> selectedImages = imageList->selectedItems();
  foreach (QListWidgetItem *image, selectedImages) {
    selectedFileNames << image->text();
  }

  getWizard()->setValue("SelectedImages", selectedFileNames);
  m_directories.sort();
  getWizard()->setValue("Directories", m_directories);
}

void NewImageSelectPage::backEvent()
{
  if (m_job) {
    m_job->stop();
    m_job = 0;
  }

  imageList->clear();
}

void NewImageSelectPage::viewEvent()
{
  if (!imageList->count()) {
    setVerification(false);
    m_job = new GCore::GJobs::ReadJob(this, getWizard()->getValue("GalleryPath").toString(), getWizard()->getValue("RecursiveSearch").toBool());
    connect(m_job, SIGNAL(signalProgress(const QString&, const QImage&, const QString&)), this, SLOT(slotProcess(const QString&, const QImage&, const QString&)));
    m_job->start();
  }
}

void NewImageSelectPage::stopEvent()
{
  if (m_job)
    m_job->terminate();
}

void NewImageSelectPage::slotProcess(const QString &filename, const QImage &image, const QString &directory)
{
  if (!filename.isEmpty()) {
      if (m_job) {
      new QListWidgetItem(QIcon(QPixmap::fromImage(image)), filename, imageList);
      if (!m_directories.contains(directory))
        m_directories << directory;
      outputLabel->setText(tr("Searching for images... Current image %1").arg(filename));
      progressFrame->setVisible(true);
      return;
    }
  }
  
  progressFrame->setVisible(false);
  m_job = 0;
  setVerification(true);
}

}
