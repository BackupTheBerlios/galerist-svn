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
#include "newgallerywizard.h"

#include "widgets/wizard/welcomepage.h"
#include "widgets/wizard/selectionpage.h"
#include "widgets/wizard/summarypage.h"
#include "widgets/wizard/copypage.h"
#include "widgets/wizard/finishpage.h"

#include <QtGui/QMessageBox>

namespace GDialogs
{

NewGalleryWizard::NewGalleryWizard(QWidget *parent)
    : QWizard(parent)
{
  setupUi();
}

NewGalleryWizard::NewGalleryWizard(const QString &path, const QStringList &images, QWidget *parent)
    : QWizard(parent)
{
  setupUi();

  GWidgets::GWizard::SelectionPage *selectionPage = static_cast<GWidgets::GWizard::SelectionPage*>(page(m_selectionPage));

  selectionPage->setPredefinedImages(path, images);
}

NewGalleryWizard::~NewGalleryWizard()
{}

void NewGalleryWizard::reject()
{
  if (currentId() == m_copyPage)
    static_cast<GWidgets::GWizard::CopyPage*>(currentPage())->pauseCopy();

  if (QMessageBox::question(0, tr("Confirm cancel"), tr("Are you sure you want to cancel this wizard?"), tr("Cancel"), tr("Continue"), QString(), 1, 1) == 0) {
    if (currentId() == m_copyPage) {
      static_cast<GWidgets::GWizard::CopyPage*>(currentPage())->stopCopy();
    }
    QWizard::reject();
  } else {
    if (currentId() == m_copyPage)
      static_cast<GWidgets::GWizard::CopyPage*>(currentPage())->resumeCopy();
  }
}

void NewGalleryWizard::setupUi()
{
  setWindowTitle(tr("Create a new gallery"));

  setDefaultProperty("QComboBox", "currentText", "currentIndexChanged()");

  setOption(QWizard::DisabledBackButtonOnLastPage);

  addPage(new GWidgets::GWizard::WelcomePage);
  m_selectionPage = addPage(new GWidgets::GWizard::SelectionPage);
  addPage(new GWidgets::GWizard::SummaryPage);
  m_copyPage = addPage(new GWidgets::GWizard::CopyPage);
  addPage(new GWidgets::GWizard::FinishPage);

  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/galerist.png").scaled(55, 55, Qt::KeepAspectRatio));
  setPixmap(QWizard::BannerPixmap, QPixmap(":/images/newgallerywizard-banner.png"));
  setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/galerist-big.png"));

#ifdef Q_WS_X11
  setWizardStyle(QWizard::ModernStyle);
#endif

  if (wizardStyle() == QWizard::MacStyle)
    setFixedSize(770, 570);
  else
    setFixedSize(580, 570);
}

}
