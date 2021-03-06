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
#include "mainwindow.h"

#include <QtCore/QPoint>
#include <QtCore/QDir>
#include <QtCore/QTimer>

#include <QtGui/QHeaderView>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtGui/QMessageBox>
#include <QtGui/QProgressBar>
#include <QtGui/QFileDialog>
#include <QtGui/QPixmap>
#include <QtGui/QSortFilterProxyModel>

#include "core/data.h"
#include "core/imagemodel.h"
#include "core/metadatamanager.h"
#include "core/jobmanager.h"

#include "dialogs/newgallerywizard.h"
#include "dialogs/configuration.h"
#include "dialogs/uniupdate.h"

#include <QtDebug>

using namespace GCore;

MainWindow::MainWindow()
    : QMainWindow(), Ui::MainWindow()
{
  setupUi(this);

  setWindowIcon(QIcon(":/images/galerist.png"));
  setWindowTitle(Data::self()->value(Data::AppName).toString());

  startTimer(100);

  initActionButtons();
  initToolbar();
  initDocks();

  // Check if the user has the required Sql driver
  if (!MetaDataManager::driverAvailable()) {
    QMessageBox::critical(this, tr("Fatal error"), tr("Qt doesn't have Sql driver for Sqlite3! %1 cannot operate without it! %1 will now close.").arg(Data::self()->value(Data::AppName).toString()));
    QTimer::singleShot(1, qApp, SLOT(quit()));
  }

#ifdef WANT_UPDATER
  // Lets check for updates
  if (Data::self()->value(Data::UpdateStartup).toBool())
    GDialogs::UniUpdate::checkUpdatesStartup(this);
#endif
}

void MainWindow::initActionButtons()
{
  // Connect file menu
  connect(actionNew, SIGNAL(triggered()), this, SLOT(slotNew()));
  connect(actionRemoveGallery, SIGNAL(triggered()), albumView, SLOT(slotDelete()));

  connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

  // Connect edit menu
  connect(actionAdd, SIGNAL(triggered()), this, SLOT(slotAddImages()));
  actionAdd->setShortcut(Qt::Key_Insert);
  //connect(actionPreview, SIGNAL(triggered()), imageList, SLOT(slotEdit()));
  connect(actionRemove, SIGNAL(triggered()), imageList, SLOT(slotRemove()));
  actionRemove->setShortcut(Qt::Key_Delete);

  connect(actionSelectAll, SIGNAL(triggered()), imageList, SLOT(selectAll()));
  actionSelectAll->setShortcut(Qt::ControlModifier | Qt::Key_A);
  connect(actionDeselectAll, SIGNAL(triggered()), imageList, SLOT(clearSelection()));
  actionDeselectAll->setShortcut(Qt::ControlModifier | Qt::Key_U);
  connect(actionInvertSelection, SIGNAL(triggered()), imageList, SLOT(invertSelection()));
  actionInvertSelection->setShortcut(Qt::ControlModifier | Qt::Key_Asterisk);

  connect(actionConfiguration, SIGNAL(triggered()), this, SLOT(slotConfiguration()));

  // Connect view menu
  connect(actionRectangularBackground, SIGNAL(toggled(bool)), this, SLOT(slotRectangularBackground(bool)));
  connect(actionRoundBackground, SIGNAL(toggled(bool)), this, SLOT(slotRoundBackground(bool)));

  viewMenu->addAction(galleryDock->toggleViewAction());

  // Connect Help menu
  connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  actionAbout->setText(tr("&About %1").arg(Data::self()->value(Data::AppName).toString()));
  connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

#ifdef WANT_UPDATER
  actionUpdate->setEnabled(true);
  connect(actionUpdate, SIGNAL(triggered()), this, SLOT(startUpdater()));
#endif

  // Connect other actions
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuit()));

  // Store gallery view context menu
  QMenu *contextMenu = new QMenu(this);
  contextMenu->addAction(actionNew);
  contextMenu->addAction(actionRemoveGallery);
  Data::self()->setValue(Data::GalleryContextMenu, QVariant::fromValue(static_cast<QWidget*>(contextMenu)));

  // Store photo view context menu
  contextMenu = new QMenu(this);
  contextMenu->addAction(actionAdd);
  contextMenu->addAction(actionPreview);
  contextMenu->addAction(actionRemove);
  contextMenu->addSeparator();
  contextMenu->addMenu(menuSelection);
  Data::self()->setListContextMenu(contextMenu);

  // Create drop context menu
  QMenu *dropdownMenu = new QMenu(tr("Dropped images"), this);
  dropdownMenu->addAction(actionDropNew);
  actionDropNew->setData(0);
  dropdownMenu->addAction(actionDropInsert);
  actionDropInsert->setData(1);

  Data::self()->setDropContextMenu(dropdownMenu);
}

void MainWindow::initToolbar()
{
  // Load the configuration

  switch (Data::self()->value(Data::BackgroundType).toInt()) {
    case (Data::Round) : {
      actionRoundBackground->setChecked(true);
      break;
    }
    default: {
      actionRectangularBackground->setChecked(true);
      break;
    }
  }
}

void MainWindow::initDocks()
{
  albumView->header()->setVisible(false);
  albumView->setModel(Data::self()->galleryProxy());
  imageList->setModel(Data::self()->imageModel());

  connect(imageList, SIGNAL(signalOneSelected(bool)), actionPreview, SLOT(setEnabled(bool)));
  connect(imageList, SIGNAL(signalSelected(bool)), actionRemove, SLOT(setEnabled(bool)));
  connect(imageList, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(openProperties(const QModelIndex&)));

  connect(propertiesView, SIGNAL(closed(const QModelIndex&)), this, SLOT(closeProperties(const QModelIndex&)));

  connect(albumView, SIGNAL(signalSelected(bool)), actionRemoveGallery, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionAdd, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionSelectAll, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionDeselectAll, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionInvertSelection, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionDropInsert, SLOT(setEnabled(bool)));
}

void MainWindow::about()
{
  QString about = tr("With %1 you can create your own galleries of pictures, comment all pictures and upload them to a gallery site.\n\nCurrent version: %2 %3\nAuthor name: Gregor Kališnik\nAuthor email: gregor@unimatrix-one.org").arg(Data::self()->value(Data::AppName).toString()).arg(Data::self()->value(Data::AppVersion).toString()).arg(Data::self()->value(Data::AppBranch).toString());
  QMessageBox::about(this, tr("About %1").arg(Data::self()->value(Data::AppName).toString()), about);
}

void MainWindow::slotNew()
{
  //We create a wizard for creation a new gallery.
  GDialogs::NewGalleryWizard *newWizard = new GDialogs::NewGalleryWizard(this);
  newWizard->show();
}

void MainWindow::slotRectangularBackground(bool checked)
{
  if (checked) {
    Data::self()->setValue(Data::BackgroundType, Data::Rectangular);
    actionRoundBackground->setChecked(false);
    imageList->viewport()->update();
  } else {
    initToolbar();
  }
}

void MainWindow::slotRoundBackground(bool checked)
{
  if (checked) {
    Data::self()->setValue(Data::BackgroundType, Data::Round);
    actionRectangularBackground->setChecked(false);
    imageList->viewport()->update();
  } else {
    initToolbar();
  }
}

void MainWindow::slotQuit()
{
  Data::self()->clear();
}

void MainWindow::slotUpdateEditButtons(bool selected)
{
  if (selected)
    actionRemove->setEnabled(true);
  else
    actionRemove->setEnabled(false);
}

void MainWindow::slotAddImages()
{
  QStringList pictures = QFileDialog::getOpenFileNames(this, tr("Add images"), QDir::homePath(), tr("Images (*.png *.gif *.jpg *.jpeg)"));

  if (pictures.isEmpty())
    return;

  // Add images
  QString image = pictures.first();
  image.remove(QRegExp("^.+/"));
  QString path = pictures.first();
  path.remove(image);

  QStringList images = pictures;
  images.replaceInStrings(path, QString());

  Job job = JobManager::self()->addImages(imageList->rootIndex(), pictures);
  connect(job.jobPtr(), SIGNAL(progress(int, int, const QString&, const QImage&)), Data::self()->imageAddProgress(), SLOT(setProgress(int, int, const QString&, const QImage&)));
}

void MainWindow::startUpdater()
{
#ifdef WANT_UPDATER
  GDialogs::UniUpdate::checkUpdates(this);
#endif
}

void MainWindow::slotConfiguration()
{
  GDialogs::Configuration *config = new GDialogs::Configuration(this);
  config->show();
}

MainWindow::~MainWindow()
{}

void MainWindow::timerEvent(QTimerEvent*)
{
  if (statusBar()->currentMessage().isEmpty())
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::openProperties(const QModelIndex &index)
{
  if (index.data(ImageModel::ImageTypeRole).toInt() == ImageItem::Gallery) {
    albumView->checkSelection(Data::self()->galleryProxy()->mapFromSource(index));
    imageList->setRootIndex(index);
    return;
  }

  centerWidget->setCurrentIndex(1);

  galleryDock->hide();
  galleryDock->toggleViewAction()->setEnabled(false);

  // Disable all actions
  actionNew->setDisabled(true);
  actionAdd->setDisabled(true);
  actionRemove->setDisabled(true);
  actionRemoveGallery->setDisabled(true);
  actionRectangularBackground->setDisabled(true);
  actionRoundBackground->setDisabled(true);
  menuSelection->setDisabled(true);

  propertiesView->setCurrentIndex(index);
}

void MainWindow::closeProperties(const QModelIndex &index)
{
  centerWidget->setCurrentIndex(0);

  galleryDock->show();
  galleryDock->toggleViewAction()->setEnabled(true);

  // Enable all actions
  actionNew->setDisabled(false);
  actionAdd->setDisabled(false);
  actionRemove->setDisabled(false);
  actionRemoveGallery->setDisabled(false);
  actionRectangularBackground->setDisabled(false);
  actionRoundBackground->setDisabled(false);
  menuSelection->setDisabled(false);

  imageList->scrollTo(index, QListView::PositionAtCenter);
  imageList->clearSelection();
}

