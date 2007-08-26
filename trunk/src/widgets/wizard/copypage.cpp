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
#include "copypage.h"

#include "core/data.h"
#include "core/imagemodel.h"
#include "core/jobmanager.h"

#include <QtCore/QDir>

#include <QtGui/QPixmap>

using namespace GCore;

namespace GWidgets
{

namespace GWizard
{

CopyPage::CopyPage()
    : QWizardPage(),
    m_finished(false)
{
  setTitle(tr("Copy"));
  setSubTitle(tr("Creation progress"));

  setupUi(this);
}

CopyPage::~CopyPage()
{}

void CopyPage::initializePage()
{
  // We determine which gallery is the parent
  QModelIndex parentGallery = QModelIndex();
  if (field("ParentGallery").toString() != "None")
    parentGallery = Data::self()->imageModel()->findGallery(field("ParentGallery").toString());

  // We copy the images to the right place
  QString hash = JobManager::self()->createGalleryJob(field("GalleryName").toString(), parentGallery, field("GalleryPath").toString(), m_prefedinedImages, field("DeleteSourceImages").toBool());

  connect(JobManager::self()->job(hash), SIGNAL(progress(int, int, const QString&, const QImage&)), this, SLOT(slotProgress(int, int, const QString&, const QImage&)));

  JobManager::self()->startJob(hash);
}

bool CopyPage::isComplete() const
{
  return m_finished;
}

void CopyPage::pauseCopy()
{
  JobManager::self()->pauseJob("CopyImages");
}

void CopyPage::resumeCopy()
{
  JobManager::self()->unpauseJob("CopyImages");
}

void CopyPage::stopCopy()
{
  JobManager::self()->stopJob("CopyImages");
}

void CopyPage::setPredefinedImages(const QString &path, const QStringList &images)
{
  m_predefinedPath = path;
  m_prefedinedImages = images;
}

void CopyPage::slotProgress(int finished, int total, const QString &current, const QImage &image)
{
  progressBar->setMaximum(total);
  progressBar->setValue(finished);

  if (!image.isNull())
    imageLabel->setPixmap(QPixmap::fromImage(image));

  if (finished != total) {
    //Add the name to the progress label.
    progressLabel->setText(tr("Copying picture %1 ...").arg(current));
  } else {
    //The copy has finished!
    progressLabel->setText(tr("Copy finished"));
    m_finished = true;
    emit completeChanged();
    wizard()->next();
  }
}

}

}
