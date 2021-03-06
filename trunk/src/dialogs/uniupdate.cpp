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

#ifdef WANT_UPDATER

#include "uniupdate.h"

#include <QtCore/QProcess>

#include <QtGui/QApplication>

#include "core/data.h"

#include "core/network/uniupdatemanager.h"

using namespace GCore;

namespace GDialogs
{

UniUpdate::UniUpdate(QWidget *parent)
    : QDialog(parent)
{
  setupUi(this);

  // Making sure it's going to be deleted upon close
  setAttribute(Qt::WA_DeleteOnClose, true);

  changelogFrame->hide();

  layout()->setSizeConstraint(QLayout::SetFixedSize);

  m_updateManager = new GNetwork::UniUpdateManager(this);

  connect(m_updateManager, SIGNAL(updateAvailability(int, const QString&)), this, SLOT(updateProcess(int, const QString&)));
  connect(m_updateManager, SIGNAL(downloadProgress(int, int)), this, SLOT(updateDownloadProgress(int, int)));
  connect(m_updateManager, SIGNAL(patchReady(const QString&)), this, SLOT(preparePatch(const QString&)));
  connect(updateButton, SIGNAL(clicked()), this, SLOT(beginDownload()));

  progressBar->setMaximum(0);

  statusLabel->setText(tr("Retrieving changelog..."));

  // TODO: need to get changed...
  m_updateManager->checkUpdate(Data::self()->value(Data::AppName).toString(), "windows", "unstable", Data::self()->value(Data::AppVersion).toString(), Data::self()->value(Data::AppBranch).toString());
}

UniUpdate::~UniUpdate()
{}

void UniUpdate::checkUpdates(QWidget *parent)
{
  UniUpdate *dialog = new UniUpdate(parent);
  dialog->show();
}

void UniUpdate::checkUpdatesStartup(QWidget *parent)
{
  UniUpdate *dialog = new UniUpdate(parent);
  dialog->hide();
}

void UniUpdate::updateProcess(int availability, const QString &changelog)
{
  progressBar->setMaximum(1);
  switch (availability) {
    case (GNetwork::UniUpdateManager::CannotConnect) : {
      statusLabel->setText(tr("Cannot contact UniUpdate server."));
      break;
    }
    case (GNetwork::UniUpdateManager::ApplicationUnsupported) : {
      statusLabel->setText(tr("This application is not supported by the UniUpdate service."));
      break;
    }
    case (GNetwork::UniUpdateManager::PlatformUnsupported) : {
      statusLabel->setText(tr("This platform is not supported by the UniUpdate service."));
      break;
    }
    case (GNetwork::UniUpdateManager::BranchUnsupported) : {
      statusLabel->setText(tr("Selected branch does not exist on UniUpdate server. You could change the preferred branch in the configuration."));
      break;
    }
    case (GNetwork::UniUpdateManager::Latest) : {
      statusLabel->setText(tr("You already have the latest %1 installed.").arg(Data::self()->value(Data::AppName).toString()));
      break;
    }
    case (GNetwork::UniUpdateManager::Available) : {
      statusLabel->setText(tr("A new version of %1 is available.").arg(Data::self()->value(Data::AppName).toString()));
      changelogOutput->setPlainText(changelog);

      // Lets show ourselfs!
      show();

      // Enable buttons
      updateButton->setEnabled(true);
      changelogButton->setEnabled(true);
      break;
    }
  }
}

void UniUpdate::beginDownload()
{
  updateButton->setEnabled(false);
  progressBar->setTextVisible(true);
  m_updateManager->downloadUpdate();
}

void UniUpdate::preparePatch(const QString &filename)
{
  m_patchFilename = filename;

  updateButton->setText(tr("&Quit and patch"));

  closeButton->setEnabled(false);
  updateButton->setEnabled(true);

  connect(updateButton, SIGNAL(clicked()), this, SLOT(executePatch()));
  disconnect(updateButton, SIGNAL(clicked()), this, SLOT(beginDownload()));
}

void UniUpdate::executePatch()
{
  if (QProcess::startDetached(m_patchFilename))
    qApp->quit();
}

void UniUpdate::updateDownloadProgress(int done, int total)
{
  progressBar->setMaximum(total);
  progressBar->setValue(done);
}

}

#endif
