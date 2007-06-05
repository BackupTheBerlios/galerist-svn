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

#include "selectionpage.h"

#include "core/data.h"
#include "core/imagemodel.h"

#include "core/jobs/readjob.h"

#include "widgets/lineedit.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

#include <QtGui/QFileDialog>
#include <QtGui/QListWidgetItem>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>

namespace GWidgets
{

namespace GWizard
{

SelectionPage::SelectionPage()
    : QWizardPage(),
    m_initialised(false),
    m_readJob(0)
{
  setTitle(tr("Gallery settings"));
  setSubTitle(tr("Main settings of the new gallery"));

  setupUi(this);

  registerField("GalleryName*", nameEdit);
  registerField("GalleryPath*", imagesEdit);
  registerField("ParentGallery", parentBox);

  connect(browseButton, SIGNAL(clicked()), this, SLOT(slotBrowseClicked()));
}

SelectionPage::~SelectionPage()
{
  imagesEdit->setType(GWidgets::LineEdit::Default);
}

void SelectionPage::cleanupPage()
{}

void SelectionPage::initializePage()
{
  // Set focus to the name edit
  nameEdit->setFocus();

  if (!m_initialised) {
    setField("GalleryName", "");

    // Add all available galleries
    parentBox->addItems(GCore::Data::self()->getImageModel()->getGalleriesList());

    nameEdit->setType(GWidgets::LineEdit::WithVerify);
    imagesEdit->setType(GWidgets::LineEdit::DirSelector);

    connect(nameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheckName(const QString&)));
    connect(nameEdit, SIGNAL(validityChanged(bool)), this, SIGNAL(completeChanged()));
    connect(imagesEdit, SIGNAL(validityChanged(bool)), this, SIGNAL(completeChanged()));

    // Insert the path to the home directory
    // Only if there is no predefined images!
    if (!previewList->count())
      setField("GalleryPath", QDir::homePath());

    // Lets make sure we won't be initialising twice
    m_initialised = true;
  }
}

bool SelectionPage::isComplete() const
{
  m_readJob = 0;
  previewList->clear();

  if (imagesEdit->isValid())
    makePreview(imagesEdit->text());
  else
    return false;

  if (nameEdit->isValid())
    return QWizardPage::isComplete();

  return false;
}

void SelectionPage::setPredefinedImages(const QString &path, const QStringList &images)
{
  makePreview(path, images);
  imagesEdit->setText(path);
  imagesEdit->setEnabled(false);
  setField("GalleryPath", path);

  browseButton->setEnabled(false);
}

void SelectionPage::makePreview(const QString &path, const QStringList &images) const
{
  if (!m_readJob) {
    previewList->clear();
    GCore::GJobs::ReadJob *read = new GCore::GJobs::ReadJob((QObject*) this, QDir(path), images);

    connect(read, SIGNAL(signalProgress(const QString&, const QImage&, const QString&)), this, SLOT(addImage(const QString&, const QImage&, const QString&)));

    read->start();

    m_readJob = read;
  }
}

void SelectionPage::slotBrowseClicked()
{
  QString directory = QFileDialog::getExistingDirectory(0, tr("Select the directory with images for you gallary."), imagesEdit->text());
  if (!directory.isEmpty()) {
    m_readJob = 0;
    imagesEdit->setText(directory);
  }
}

void SelectionPage::slotCheckName(const QString &name)
{
  if (nameEdit->text().isEmpty())
    nameEdit->setValidity(false);
  else if (!nameEdit->text().isEmpty() && !GCore::Data::self()->getImageModel()->findGallery(name).isValid())
    nameEdit->setValidity(true);
  else
    nameEdit->setValidity(false, tr("Gallery allready exists. Please select a different name."));
}

void SelectionPage::addImage(const QString&, const QImage &image, const QString&)
{
  if (!image.isNull())
    new QListWidgetItem(QIcon(QPixmap::fromImage(image)), "", previewList);
}

}

}

