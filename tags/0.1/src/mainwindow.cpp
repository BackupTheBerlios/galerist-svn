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
#include "mainwindow.h"

#include <QtGui/QHeaderView>
#include <QtCore/QSize>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtGui/QMessageBox>
#include <QtCore/QTimer>
#include <QtGui/QProgressBar>
#include <QtGui/QFileDialog>
#include <QtCore/QPoint>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPixmap>
#include <QtCore/QDir>

#include <QtCore/QtDebug>

#include "core/data.h"
#include "core/imagemodel.h"
#include "core/metadatamanager.h"

#include "core/network/updater.h"

#include "dialogs/newwizard.h"
#include "dialogs/configuration.h"

#include "widgets/imageaddprogress.h"
#include "widgets/photocontrol.h"


MainWindow::MainWindow()
    : QMainWindow(), Ui::MainWindow()
{
  setupUi(this);

  GCore::Data::self()->setMainWindow(this);

  setWindowIcon(QIcon(":/images/galerist.png"));
  setWindowTitle(GCore::Data::self()->getAppName());

  statusBar()->showMessage(tr("Ready"));
  m_progressBar = new QProgressBar();
  statusBar()->addPermanentWidget(m_progressBar);
  m_progressBar->hide();

  m_imageProgress = new GWidgets::ImageAddProgress(this);
  QPoint position;
  QRect desktop = QApplication::desktop()->screenGeometry();
  position.setY(desktop.bottom() - 200);
  position.setX(desktop.right() - 300);
  m_imageProgress->move(position);

  GCore::Data::self()->setPhotoControl(photoControl);
  setEditMode(false);

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
    GCore::Data::self()->getUpdater()->checkUpdate();
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
  connect(actionDefaultBackground, SIGNAL(toggled(bool)), this, SLOT(slotDefaultBackground(bool)));
  connect(actionRoundBackground, SIGNAL(toggled(bool)), this, SLOT(slotRoundBackground(bool)));

  connect(actionShowProgress, SIGNAL(toggled(bool)), this, SLOT(slotProgressShow(bool)));
  connect(actionHideProgress, SIGNAL(toggled(bool)), this, SLOT(slotProgressHide(bool)));

  viewMenu->addAction(galleryDock->toggleViewAction());

  // Connect Help menu
  connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
  actionAbout->setText(tr("&About %1").arg(GCore::Data::self()->getAppName()));
  connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

#ifdef WANT_UPDATER
  actionUpdate->setEnabled(true);
  connect(actionUpdate, SIGNAL(triggered()), GCore::Data::self()->getUpdater(), SLOT(slotCheckUpdate()));
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
      actionDefaultBackground->setChecked(true);
      break;
    }
  }

  switch (GCore::Data::self()->getProgressDialog()) {
    case (GCore::Data::Hide) : {
        actionHideProgress->setChecked(true);
        break;
      }
    default: {
      actionShowProgress->setChecked(true);
      break;
    }
  }
}

void MainWindow::initDocks()
{
  GCore::ImageModel *imageModel = GCore::Data::self()->getImageModel();

  albumView->header()->setVisible(false);
  albumView->setModel(imageModel);
  imageList->setModel(imageModel);

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
  QString about = tr("With %1 you can create your own galleries of pictures, comment all pictures and upload them to a gallery site.\n\nCurrent version: %2\nAuthor name: %3\nAuthor email: %4").arg(GCore::Data::self()->getAppName()).arg(GCore::Data::self()->getAppVersion()).arg("Gregor Kališnik").arg("gregor@podnapisi.net");
  QMessageBox::about(this, tr("About %1").arg(GCore::Data::self()->getAppName()), about);
}

void MainWindow::slotNew()
{
  //We create a wizard for creation a new gallery.
  GDialogs::NewWizard *newWizard = new GDialogs::NewWizard(tr("Create new gallery wizard"), this);
  newWizard->resize(600, 500);
  newWizard->show();
}

void MainWindow::slotDefaultBackground(bool checked)
{
  if (checked) {
    GCore::Data::self()->setBackgroundType(GCore::Data::Default);
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
    actionDefaultBackground->setChecked(false);
    imageList->viewport()->update();
  } else {
    initToolbar();
  }
}

void MainWindow::slotProgressShow(bool checked)
{
  if (checked) {
    GCore::Data::self()->setProgressDialog(GCore::Data::Show);
    actionHideProgress->setChecked(false);
  } else {
    initToolbar();
  }
}

void MainWindow::slotProgressHide(bool checked)
{
  if (checked) {
    GCore::Data::self()->setProgressDialog(GCore::Data::Hide);
    actionShowProgress->setChecked(false);
  } else {
    initToolbar();
  }
}

void MainWindow::slotQuit()
{
  //GCore::Data::self()->saveChanges();
  GCore::Data::self()->clear();
}

void MainWindow::slotStatusProgress(int finished, int total, const QString &current, const QImage &currentImage)
{
  if (GCore::Data::self()->getProgressDialog() == GCore::Data::Show) {
    m_imageProgress->setProgress(finished, total, current, QPixmap::fromImage(currentImage));
    m_imageProgress->show();
  } else {
    statusBar()->showMessage(tr("Copying picture %1...").arg(current));
    m_progressBar->setMaximum(total);
    m_progressBar->setValue(finished);
    m_progressBar->show();
  }

  if (finished == total) {
    if (GCore::Data::self()->getProgressDialog() == GCore::Data::Show)
      m_imageProgress->setFinish(tr("Copiing of pictures is successful."));
    else
      statusBar()->showMessage(tr("Copiing of pictures is successful."));

    QTimer::singleShot(1000, this, SLOT(slotHideProgress()));
  }
}

void MainWindow::slotHideProgress()
{
  m_imageProgress->hide();
  m_progressBar->hide();
  statusBar()->showMessage(tr("Ready"));
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

  addImages(pictures);
}

void MainWindow::addImages(const QStringList &images)
{
  if (images.isEmpty())
    return;

  QString image = images.first();
  image.remove(QRegExp("^.+/"));
  QString path = images.first();
  path.remove(image);

  QStringList pictures = images;
  pictures.replaceInStrings(path, QString());
  QStringList::const_iterator end = pictures.end();

  qRegisterMetaType<QImage>("QImage");
  connect(GCore::Data::self()->getImageModel()->addImages(imageList->rootIndex(), path, pictures), SIGNAL(signalProgress(int, int, const QString&, const QImage&)), this, SLOT(slotStatusProgress(int, int, const QString&, const QImage&)));
}

void MainWindow::slotConfiguration()
{
  GDialogs::Configuration *config = new GDialogs::Configuration(this);
  config->show();
}

MainWindow::~MainWindow()
{
  delete m_imageProgress;
}

void MainWindow::setEditMode(bool edit)
{
  photoControl->setVisible(edit);
}
