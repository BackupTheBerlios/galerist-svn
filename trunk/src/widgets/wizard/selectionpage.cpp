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

#include "selectionpage.h"

#include "core/data.h"
#include "core/imagemodel.h"

#include "widgets/lineedit.h"

#include <QtCore/QDir>
#include <QtCore/QFile>

#include <QtGui/QFileDialog>

namespace GWidgets {

namespace GWizard {

SelectionPage::SelectionPage()
    : QWizardPage()
{
  setTitle(tr("Gallery settings"));
  
  setupUi(this);
  
  registerField("GalleryName*", nameEdit);
  registerField("GalleryPath*", imagesEdit);
  registerField("RecursiveSearch", recursiveBox);
  registerField("RecursiveSubGallery", subRadio);
  registerField("ParentGallery", parentBox);
  
  connect(browseButton, SIGNAL(clicked()), this, SLOT(slotBrowseClicked()));
}

SelectionPage::~SelectionPage()
{}

void SelectionPage::initializePage()
{
  // Set focus to the name edit
  nameEdit->setFocus();
  setField("GalleryName", "");

  // Add all available galleries
  parentBox->addItems(GCore::Data::self()->getImageModel()->getGalleriesList());

  nameEdit->setType(GWidgets::LineEdit::WithVerify);
  imagesEdit->setType(GWidgets::LineEdit::FileSelector);

  connect(nameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheckName(const QString&)));
  connect(imagesEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheckImagesPath(const QString&)));

  // Insert the path to the home directory
  setField("GalleryPath", QDir::homePath());
}

void SelectionPage::cleanupPage()
{
  disconnect(nameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheckName(const QString&)));
  disconnect(imagesEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheckImagesPath(const QString&)));
  
  // Set focus to the name edit
  nameEdit->setFocus();

  setField("GalleryName", "");

  // Insert the path to the home directory
  setField("GalleryPath", QDir::homePath());

  // Add all available galleries
  parentBox->clear();
}

bool SelectionPage::isComplete() const
{
  if (checkName(nameEdit->text()) && checkImagesPath(imagesEdit->text()))
    return QWizardPage::isComplete();
  
  return false;
}

void SelectionPage::slotCheckImagesPath(const QString &path)
{
  if (imagesEdit->text().isEmpty()) {
    imagesEdit->setValidity(false, tr("Please enter a path to the directory with images."));
    //messageLabel->setText(tr("Please enter a path to the directory with images."));
  } else if (!QDir(path).exists()) {
    imagesEdit->setValidity(false, tr("The entered path doesn't exists. Please select another path."));
    //messageLabel->setText(tr("The entered path doesn't exists. Please select another path."));
  } else {
    messageLabel->setText("");
    imagesEdit->setValidity(true);
  }

  //slotTextChanged();
}

void SelectionPage::slotBrowseClicked()
{
  QString directory = QFileDialog::getExistingDirectory(0, tr("Select the directory with images for you gallary."), imagesEdit->text());
  if (!directory.isEmpty())
    imagesEdit->setText(directory);
}

void SelectionPage::slotCheckName(const QString &name)
{
  if (nameEdit->text().isEmpty()) {
    nameEdit->setValidity(false, tr("Please enter a name."));
    //messageLabel->setText(tr("Please enter a name."));
  } else if (!nameEdit->text().isEmpty() && !GCore::Data::self()->getImageModel()->checkName(name)) {
    //messageLabel->setText("");
    nameEdit->setValidity(true);
  } else {
    //messageLabel->setText(tr("Gallery allready exists. Please select a different name."));
    nameEdit->setValidity(false, tr("Gallery allready exists. Please select a different name."));
  }

  //slotTextChanged();
}

bool SelectionPage::checkImagesPath(const QString &path) const
{
  if (imagesEdit->text().isEmpty())
    return false;
  else if (!QDir(path).exists())
    return false;
  else
    return true;
}

bool SelectionPage::checkName(const QString &name) const
{
  if (nameEdit->text().isEmpty())
    return false;
  else if (!nameEdit->text().isEmpty() && !GCore::Data::self()->getImageModel()->checkName(name))
    return true;
  else
    return false;
}

}

}

