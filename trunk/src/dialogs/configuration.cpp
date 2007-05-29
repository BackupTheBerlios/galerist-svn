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
#include "configuration.h"

#include "core/data.h"
#include "core/errorhandler.h"

#include <QtOpenGL/QGLFormat>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtGui/QFileDialog>

namespace GDialogs
{

Configuration::Configuration(QWidget *parent)
    : QDialog(parent)
{
  // Initialises the GUI
  setupUi(this);

  // Additional initialisations
  imageEditorEdit->setType(GWidgets::LineEdit::FileSelector);

  glRadio->setEnabled(QGLFormat::hasOpenGL());

  translationBox->addItems(GCore::Data::self()->getAvailableTranslations());
  translationBox->setCurrentIndex(translationBox->findText(GCore::Data::self()->getTranslationName()));

  // Set the configuration
  if (GCore::Data::self()->getOpengl())
    glRadio->setChecked(true);
  else
    nonGlRadio->setChecked(true);

  imageEditorEdit->setText(QString(GCore::Data::self()->getPhotoEditor()).remove("\""));
  slotProcessEdit(imageEditorEdit->text(), true);

#ifdef WANT_UPDATER
  updateBox->setChecked(GCore::Data::self()->getUpdateStartup());
  updateBox->setEnabled(true);
#else
  updateBox->setChecked(false);
  updateBox->setEnabled(false);
#endif


  // Connect all the slots
  connect(this, SIGNAL(signalFailed(const QString&, int)), GCore::Data::self()->getErrorHandler(), SLOT(slotReporter(const QString&, int)));
  connect(testButton, SIGNAL(clicked()), this, SLOT(slotTest()));
  connect(imageEditorEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotProcessEdit(const QString&)));
  connect(browseButton, SIGNAL(clicked()), this, SLOT(slotBrowse()));
}

void Configuration::accept()
{
  // We save the changes
  if (glRadio->isChecked() != GCore::Data::self()->getOpengl()) {
    GCore::Data::self()->setOpengl(glRadio->isChecked());
    emit signalFailed(tr("You need to restart %1 for changes to take effect.").arg(GCore::Data::self()->getAppName()), GCore::ErrorHandler::Information);
  }

  GCore::Data::self()->setPhotoEditor(imageEditorEdit->text());

  if (translationBox->currentText() != GCore::Data::self()->getTranslationName()) {
    GCore::Data::self()->setTranslation(translationBox->currentText());
    emit signalFailed(tr("You need to restart %1 for changes to take effect.").arg(GCore::Data::self()->getAppName()), GCore::ErrorHandler::Information);
  }

  GCore::Data::self()->setUpdateStartup(updateBox->isChecked());

  GCore::Data::self()->saveChanges();

  QDialog::accept();
}

void Configuration::slotTest()
{
  if (!imageEditorEdit->text().isEmpty())
    QProcess::startDetached("\"" + imageEditorEdit->text() + "\"") ? testLabel->setText(tr("Editor command is working.")) : testLabel->setText(tr("Editor command isn't working."));
  else
    testLabel->setText(tr("Editor is not defined."));
}

void Configuration::slotProcessEdit(const QString &filePath, bool firstRun)
{
  QFileInfo fileInfo(filePath);
  // Checks if the file exists if not, he tries if it wsists in the PATH variable
  if (fileInfo.exists()) {
    // If it exists, checks if it is a file. If it isn't reports File path is invalid
    if (fileInfo.isFile())
      // Last thing to check is if it is executable. If it isn't reports Editor is not executable
      if (fileInfo.isExecutable()) {
        imageEditorEdit->setValidity(true);
        testLabel->setText(tr("Click Test button to test the editor."));
      } else {
        imageEditorEdit->setValidity(false, tr("Editor is not executable."), firstRun);
        testLabel->setText(tr("Editor is not executable."));
      }
    else {
      imageEditorEdit->setValidity(false, tr("File path is invalid."), firstRun);
      testLabel->setText(tr("File path is invalid."));
    }
  } else {
    // We set the wrong option first, as it is the default behaviour
    imageEditorEdit->setValidity(false, tr("File path specified is invalid."), firstRun);
    testLabel->setText(tr("File path specified is invalid."));

    // If the listed filePath contains /, we just skip the PATH checking
    if (!filePath.contains('/')) {
      // We look at the system environment
      QStringList environment = QProcess::systemEnvironment();
      QStringList::const_iterator end = environment.constEnd();
      // We go through all the variables
      for (QStringList::const_iterator count = environment.constBegin(); count != end; count++) {
        // We stop at the PATH variable
        if (!(*count).contains(QRegExp("^PATH=")))
          continue;

        // Convert to a non const, so we can remove the PATH=
        QString pathString = *count;

        // Remove PATH= and split it
#ifdef Q_WS_WIN
        QStringList path = pathString.remove("PATH=").split(';');
#endif
#ifdef Q_WS_X11
        QStringList path = pathString.remove("PATH=").split(':');
#endif

        QStringList::const_iterator endPath = path.constEnd();
        // We go through the PATH variable
        for (QStringList::const_iterator countPath = path.constBegin(); countPath != endPath; countPath++) {
          fileInfo.setFile((*countPath) + "/" + filePath);
          // Then we check if it is a executable file
          if (fileInfo.isFile() && fileInfo.isExecutable()) {
            imageEditorEdit->setValidity(true);
            testLabel->setText(tr("Click Test button to test the editor."));
            // We found the asumed (test button is there for futher testing) editor and we leave this function
            return;
          }
#ifdef Q_WS_WIN
          // Windows ignores extensions in DOS (idiotic?)
          QFileInfo fileInfoExtension((*countPath) + "/" + filePath + ".exe");
          if (fileInfoExtension.isFile() && fileInfoExtension.isExecutable()) {
            imageEditorEdit->setValidity(true);
            testLabel->setText(tr("Click Test button to test the editor."));
            // We found the asumed (test button is there for futher testing) editor and we leave this function
            return;
          }
#endif
        }
        // We haven't find any editor, so we finish it with a sad taste (??) ;)
        return;
      }
    }
  }
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

Configuration::~Configuration()
{}

}
