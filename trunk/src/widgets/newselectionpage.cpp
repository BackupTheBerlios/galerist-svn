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
#include "newselectionpage.h"

#include "dialogs/newgallery.h"
#include "core/data.h"
#include "core/imagemodel.h"

#include <QtCore/QDir>
#include <QtGui/QFileDialog>
#include <QtGui/QDirModel>

namespace GWidgets
{

NewSelectionPage::NewSelectionPage(QWidget *parent)
    : WizardPage(parent)
{
}

QString NewSelectionPage::getName()
{
  return nameEdit->text();
}

QString NewSelectionPage::getImagePath()
{
  return QDir::fromNativeSeparators(imagesEdit->text());
}

void NewSelectionPage::initialise()
{
  setVerification(false);
  
  // Setup the .ui file
  setupUi(this);

  // Connect the buttons and line edits
  connect(nameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheckName(const QString&)));
  connect(imagesEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheckImagesPath(const QString&)));
  connect(browseButton, SIGNAL(clicked()), this, SLOT(slotBrowseClicked()));

  nameEdit->setType(GWidgets::LineEdit::WithVerify);
  imagesEdit->setType(GWidgets::LineEdit::FileSelector);

  QString homeDir = QDir::homePath();

  // Insert the path to the home directory
  imagesEdit->setText(homeDir);

  // Set focus to the name edit
  imagesEdit->setFocus();
}

void NewSelectionPage::slotCheckImagesPath(const QString &path)
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

  slotTextChanged();
}

void NewSelectionPage::slotBrowseClicked()
{
  QString directory = QFileDialog::getExistingDirectory(0, tr("Select the directory with images for you gallary."), imagesEdit->text());
  if (!directory.isEmpty())
    imagesEdit->setText(directory);
}

void NewSelectionPage::slotCheckName(const QString &name)
{
  if (nameEdit->text().isEmpty()) {
    nameEdit->setValidity(false, tr("Please enter a name."));
    //messageLabel->setText(tr("Please enter a name."));
  } else if (!nameEdit->text().isEmpty() && !GCore::Data::self()->getImageModel()->checkName(name)) {
    messageLabel->setText("");
    nameEdit->setValidity(true);
  } else {
    //messageLabel->setText(tr("Gallery allready exists. Please select a different name."));
    nameEdit->setValidity(false, tr("Gallery allready exists. Please select a different name."));
  }

  slotTextChanged();
}

bool NewSelectionPage::checkImagesPath(const QString &path)
{
  if (imagesEdit->text().isEmpty())
    return false;
  else if (!QDir(path).exists())
    return false;
  else
    return true;
}

bool NewSelectionPage::checkName(const QString &name)
{
  if (nameEdit->text().isEmpty())
    return false;
  else if (!nameEdit->text().isEmpty() && !GCore::Data::self()->getImageModel()->checkName(name))
    return true;
  else
    return false;
}

void NewSelectionPage::slotTextChanged()
{
  bool nameCheck = checkName(nameEdit->text());
  bool imagesCheck = checkImagesPath(imagesEdit->text());

//   getWizard()->setValidInput(nameCheck && imagesCheck, this);
  setVerification(nameCheck && imagesCheck);
}

NewSelectionPage::~NewSelectionPage()
{
}


}
