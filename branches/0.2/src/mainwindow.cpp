/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÅ¡nik                                 *
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

#include "dialogs/newgallerywizard.h"
#include "dialogs/configuration.h"
#include "dialogs/uniupdate.h"

#include "widgets/photocontrol.h"


#include "widgets/textedit.h"

MainWindow::MainWindow()
    : QMainWindow(), Ui::MainWindow()
{
  setupUi(this);

  setWindowIcon(QIcon(":/images/galerist.png"));
  setWindowTitle(GCore::Data::self()->getAppName());

  // FIXME This ugly, quick-fixing HACK needs to get OUT! pronto!
  GWidgets::TextEdit(this);

  startTimer(100);

  GCore::Data::self()->setSearchBar(searchBar);

  connect(imageList, SIGNAL(signalEditMode(bool)), this, SLOT(setEditMode(bool)));

  initActionButtons();
  initToolbar();
  initDocks();

  // Check if the user has the required Sql driver
  if (!GCore::MetaDataManager::driverAvailable()) {
    QMessageBox::critical(this, tr("Fatal error"), tr("Qt doesn't have Sql driver for Sqlite3! %1 cannot operate without it! %1 will now close.").arg(GCore::Data::self()->getAppName()));
    QTimer::singleShot(1, qApp, SLOT(quit()));
  }

#ifdef WANT_UPDATER
  // Lets check for updates
  if (GCore::Data::self()->getUpdateStartup())
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
  connect(actionPreview, SIGNAL(triggered()), imageList, SLOT(slotEdit()));
  connect(actionRename, SIGNAL(triggered()), imageList, SLOT(slotRename()));
  actionRename->setShortcut(Qt::Key_F2);
  connect(actionDescribe, SIGNAL(triggered()), imageList, SLOT(slotDescribe()));
  actionDescribe->setShortcut(Qt::SHIFT | Qt::Key_F2);
  connect(actionRemove, SIGNAL(triggered()), imageList, SLOT(slotRemove()));
  actionRemove->setShortcut(Qt::Key_Delete);

  connect(actionSelectAll, SIGNAL(triggered()), imageList, SLOT(slotSelectAll()));
  actionSelectAll->setShortcut(Qt::ControlModifier | Qt::Key_A);
  connect(actionDeselectAll, SIGNAL(triggered()), imageList, SLOT(slotDeselectAll()));
  actionDeselectAll->setShortcut(Qt::ControlModifier | Qt::Key_U);
  connect(actionInvertSelection, SIGNAL(triggered()), imageList, SLOT(slotInvertSelection()));
  actionInvertSelection->setShortcut(Qt::ControlModifier | Qt::Key_Asterisk);

  connect(actionConfiguration, SIGNAL(triggered()), this, SLOT(slotConfiguration()));

  // Connect view menu
  connect(actionRectangularBackground, SIGNAL(toggled(bool)), this, SLOT(slotRectangularBackground(bool)));
  connect(actionRoundBackground, SIGNAL(toggled(bool)), this, SLOT(slotRoundBackground(bool)));

  viewMenu->addAction(galleryDock->toggleViewAction());

  // Connect Help menu
  connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  actionAbout->setText(tr("&About %1").arg(GCore::Data::self()->getAppName()));
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
  GCore::Data::self()->setGalleryContextMenu(contextMenu);

  // Store photo view context menu
  contextMenu = new QMenu(this);
  contextMenu->addAction(actionAdd);
  contextMenu->addAction(actionPreview);
  contextMenu->addAction(actionRename);
  contextMenu->addAction(actionDescribe);
  contextMenu->addAction(actionRemove);
  contextMenu->addSeparator();
  contextMenu->addMenu(menuSelection);
  GCore::Data::self()->setPhotoContextMenu(contextMenu);


  photoControl->connectView(imageList);
}

void MainWindow::initToolbar()
{
  // Load the configuration

  switch (GCore::Data::self()->getBackgroundType()) {
    case (GCore::Data::Round) : {
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
  albumView->setModel(GCore::Data::self()->getModelProxy());
  imageList->setModel(GCore::Data::self()->getImageModel());
  imageControlWidget->setVisible(false);

  connect(imageList, SIGNAL(signalOneSelected(bool)), actionPreview, SLOT(setEnabled(bool)));
  connect(imageList, SIGNAL(signalOneSelected(bool)), actionRename, SLOT(setEnabled(bool)));
  connect(imageList, SIGNAL(signalOneSelected(bool)), actionDescribe, SLOT(setEnabled(bool)));
  connect(imageList, SIGNAL(signalSelected(bool)), actionRemove, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionRemoveGallery, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionAdd, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionSelectAll, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionDeselectAll, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(signalSelected(bool)), actionInvertSelection, SLOT(setEnabled(bool)));
  connect(albumView, SIGNAL(pressed(const QModelIndex&)), imageList, SLOT(setRootIndex(const QModelIndex&)));
}

void MainWindow::about()
{
  QString about = tr("With %1 you can create your own galleries of pictures, comment all pictures and upload them to a gallery site.\n\nCurrent version: %2\nAuthor name: Gregor KaliÅ¡nik\nAuthor email: gregor@unimatrix-one.org").arg(GCore::Data::self()->getAppName()).arg(GCore::Data::self()->getAppVersion());
  QMessageBox::about(this, tr("About %1").arg(GCore::Data::self()->getAppName()), about);
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
    GCore::Data::self()->setBackgroundType(GCore::Data::Rectangular);
    actionRoundBackground->setChecked(false);
    imageList->viewport()->update();
  } else {
    initToolbar();
  }
}

void MainWindow::slotRoundBackground(bool checked)
{
  if (checked) {
    GCore::Data::self()->setBackgroundType(GCore::Data::Round);
    actionRectangularBackground->setChecked(false);
    imageList->viewport()->update();
  } else {
    initToolbar();
  }
}

void MainWindow::slotQuit()
{
  GCore::Data::self()->clear();
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

  // Add images
  QString image = pictures.first();
  image.remove(QRegExp("^.+/"));
  QString path = pictures.first();
  path.remove(image);

  QStringList images = pictures;
  images.replaceInStrings(path, QString());

  connect(GCore::Data::self()->getImageModel()->addImages(imageList->rootIndex(), path, images), SIGNAL(signalProgress(int, int, const QString&, const QImage&)), GCore::Data::self()->getImageAddProgress(), SLOT(setProgress(int, int, const QString&, const QImage&)));
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

void MainWindow::setEditMode(bool edit)
{
  imageControlWidget->setVisible(edit);
  galleryDock->setVisible(!edit);
  galleryDock->toggleViewAction()->setEnabled(!edit);

  // Hide main toolbar
  mainBar->setVisible(!edit);

  // Disable all actions
  actionNew->setDisabled(edit);
  actionAdd->setDisabled(edit);
  actionRemove->setDisabled(edit);
  actionRemoveGallery->setDisabled(edit);
  actionRectangularBackground->setDisabled(edit);
  actionRoundBackground->setDisabled(edit);
  menuSelection->setDisabled(edit);
}

void MainWindow::timerEvent(QTimerEvent*)
{
  if (statusBar()->currentMessage().isEmpty())
    statusBar()->showMessage(tr("Ready"));
}
