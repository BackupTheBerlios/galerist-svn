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
#include "configuration.h"

#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QThread>

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <QtOpenGL/QGLFormat>

#include "core/data.h"
#include "core/errorhandler.h"
#include "core/jobmanager.h"

using namespace GCore;

namespace GDialogs
{

Configuration::Configuration(QWidget *parent)
    : QDialog(parent)
{
  // Initialises the GUI
  setupUi(this);

  // Making sure it's going to be closed upon deletion
  setAttribute(Qt::WA_DeleteOnClose, true);

  // Additional initialisations
  imageEditorEdit->setType(GWidgets::LineEdit::FileSelector);
  imageEditorEdit->setValidAttribute(GWidgets::LineEdit::Executable);
  imageEditorEdit->setEmptyValid(true);
  imageEditorEdit->setEmptyMessage(tr("Goya will open images with the default program."));
  dirEdit->setType(GWidgets::LineEdit::DirSelector);
  dirEdit->setValidAttribute(GWidgets::LineEdit::Writable);
  dirEdit->setNeedExisting(false);

  moveGroup->hide();

  QString pathString;
  QStringList environment = QProcess::systemEnvironment();
  foreach(QString line, environment)
  if (line.contains(QRegExp("^PATH="))) {
    pathString = line;
    continue;
  }

#ifdef Q_WS_WIN
  QStringList path = pathString.remove("PATH=").split(';');
#else
  QStringList path = pathString.remove("PATH=").split(':');
#endif
  foreach(QString directory, path) {
    foreach(QString executable, QDir(directory).entryList(QDir::Files)) {
      executable.remove(".exe");
      imageEditorEdit->addValidValue(executable);
    }
  }

  glRadio->setEnabled(QGLFormat::hasOpenGL());

  translationBox->addItems(Data::self()->value(Data::Translations).toStringList());
  translationBox->setCurrentIndex(translationBox->findText(Data::self()->value(Data::TranslationName).toString()));

  // Set the configuration
  if (Data::self()->value(Data::OpenGL).toBool())
    glRadio->setChecked(true);
  else
    nonGlRadio->setChecked(true);

  imageEditorEdit->setText(Data::self()->imageEditor());
  imageEditorEdit->setNeedTest(true);

  dirEdit->setText(Data::self()->galleriesPath());

#ifdef WANT_UPDATER
  updateBox->setChecked(GCore::Data::self()->value(Data::UpdateStartup).toBool());
  updateBox->setEnabled(true);
#else
  updateBox->setChecked(false);
  updateBox->setEnabled(false);
#endif

  effectsBox->setChecked(GCore::Data::self()->value(Data::DisableVisualEffects).toBool());

  imageEditorEdit->setValidMessage(tr("Click Test button to verify the program."));

  moveBox->setChecked(GCore::Data::self()->value(Data::DeleteSource).toBool());

  // Connect all the slots
  connect(this, SIGNAL(signalFailed(const QString&, int)), GCore::Data::self()->value(Data::ErrorHandler).value<QObject*>(), SLOT(slotReporter(const QString&, int)));
  connect(testButton, SIGNAL(clicked()), this, SLOT(slotTest()));
  connect(imageEditorEdit, SIGNAL(validityChanged(bool)), testButton, SLOT(setEnabled(bool)));
  connect(browseButton, SIGNAL(clicked()), this, SLOT(slotBrowse()));
  //connect(browseDirButton);
}

Configuration::~Configuration()
{}

void Configuration::accept()
{
  bool wait = false;
  if (GCore::Data::self()->galleriesPath() != dirEdit->text() && dirEdit->isValid())
    if (QMessageBox::question(this, tr("Confirm move"), tr("Are you sure you want to move all the galleries?"), tr("&Move"), tr("&No"), "", 1, 1) == 0) {
      QString hash = Data::self()->setGalleriesPath(dirEdit->text());
      QObject *job = JobManager::self()->job(hash);
      //job->setParent(this);
      moveGroup->show();
      buttonBox->setDisabled(true);

      wait = true;

      connect(job, SIGNAL(directoryProgress(int, int, const QString&)), this, SLOT(updateGalleryProgress(int, int, const QString&)));
      connect(job, SIGNAL(progress(int, int, const QString&, const QImage&)), this, SLOT(updateImagesProgress(int, int, const QString&, const QImage&)));
      connect(job, SIGNAL(finished(bool)), this, SLOT(finish(bool)));
    } else {
      return;
    }

  QString restartMessage = tr("You need to restart %1 for changes to take effect.").arg(GCore::Data::self()->value(Data::AppName).toString());

  // We save the changes
  if (glRadio->isChecked() != GCore::Data::self()->value(Data::OpenGL).toBool()) {
    GCore::Data::self()->setValue(Data::OpenGL, glRadio->isChecked());
    emit signalFailed(restartMessage, GCore::ErrorHandler::Information);
  }

  if (imageEditorEdit->isValid())
    GCore::Data::self()->setImageEditor(imageEditorEdit->text());

  if (translationBox->currentText() != GCore::Data::self()->value(Data::TranslationName).toString()) {
    GCore::Data::self()->setValue(Data::TranslationName, translationBox->currentText());
    emit signalFailed(restartMessage, GCore::ErrorHandler::Information);
  }

#ifdef WANT_UPDATER
  GCore::Data::self()->setValue(Data::UpdateStartup, updateBox->isChecked());
#endif

  GCore::Data::self()->setValue(Data::DisableVisualEffects, effectsBox->isChecked());

  GCore::Data::self()->setValue(Data::DeleteSource, moveBox->isChecked());

  GCore::Data::self()->saveChanges();

  if (!wait)
    QDialog::accept();
}

void Configuration::slotTest()
{
  if (!imageEditorEdit->text().isEmpty())
    QProcess::startDetached("\"" + imageEditorEdit->text() + "\"") ? imageEditorEdit->setValidity(true, tr("File successfully executed."), true) : imageEditorEdit->setValidity(false, tr("File cannot be executed."));
}

void Configuration::slotBrowse()
{
#ifdef Q_WS_WIN
  QString editorPath = QFileDialog::getOpenFileName(this, tr("Select image editor"), "/", tr("Executables (*.exe)"));
#else
  QString editorPath = QFileDialog::getOpenFileName(this, tr("Select image editor"), "/");
#endif
  if (!editorPath.isEmpty())
    imageEditorEdit->setText(editorPath);
}

void Configuration::updateGalleryProgress(int done, int total, const QString &name)
{
  galleriesBar->setMaximum(total);
  galleriesBar->setValue(done);
  galleryLabel->setText(name);
}

void Configuration::updateImagesProgress(int done, int total, const QString &name, const QImage&)
{
  imagesBar->setMaximum(total);
  imagesBar->setValue(done);
  imageLabel->setText(name);
}

void Configuration::finish(bool successful)
{
  if (successful)
    QDialog::accept();

  moveGroup->hide();
  buttonBox->setDisabled(false);
}

}
