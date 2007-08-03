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
#include "data.h"

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QMetaType>

#include <QtGui/QApplication>
#include <QtGui/QDirModel>
#include <QtGui/QCompleter>
#include <QtGui/QMenu>
#include <QtGui/QSortFilterProxyModel>

#include "core/errorhandler.h"
#include "core/imagemodel.h"
#include "core/imageitem.h"

#include "core/jobs/movejob.h"

namespace GCore
{

Data *Data::m_self = 0;

Data::Data(QObject *parent)
    : QObject(parent),
    m_errorHandler(0),
    m_imageModel(0),
    m_photoContextMenu(0),
    m_photoEditContextMenu(0),
    m_galleryContextMenu(0),
    m_mainWindow(0),
    m_modelProxy(0),
    m_imageAddProgress(0)

{
  m_self = this;
  m_settings = new QSettings(this);
  m_dirCompleter = new QCompleter(this);
  m_dirCompleter->setModel(new QDirModel(QStringList(), QDir::AllDirs | QDir::Drives, QDir::Name, m_dirCompleter));

  m_fileCompleter = new QCompleter(this);
  m_fileCompleter->setModel(new QDirModel(QStringList(), QDir::AllEntries, QDir::Name, m_fileCompleter));

  m_errorHandler = new GCore::ErrorHandler(this);

  m_imageModel = new GCore::ImageModel(this);

  m_modelProxy = new QSortFilterProxyModel(this);
  m_modelProxy->setFilterRole(ImageModel::ImageTypeRole);
  m_modelProxy->setFilterWildcard(QString::number(ImageItem::Gallery));
  m_modelProxy->setSourceModel(m_imageModel);

  QMenu *temp = new QMenu(m_mainWindow);
  temp->addAction(tr("Nothing available"))->setDisabled(true);

  m_galleryContextMenu = temp;
  m_photoEditContextMenu = temp;
  m_photoContextMenu = temp;

  // Setting up supported image formats
  m_imageFormats << "*.jpg"; //JPEG
  m_imageFormats << "*.jpeg"; //JPEG
  m_imageFormats << "*.png"; //PNG
  m_imageFormats << "*.gif"; //GIF

  // We check our built-in translations
  QDir translations(":/translations");
  QStringList translationsList = translations.entryList();
  QStringList::const_iterator end = translationsList.constEnd();

  // And add them to The list ;)
  for (QStringList::const_iterator count = translationsList.constBegin(); count != end; count++) {
    QString languageName = QString(*count).remove(".qm");
    languageName[0] = QChar(languageName[0]).toUpper();
    m_availableTranslations.insert(languageName, translations.absoluteFilePath(*count));
  }
}

Data *Data::self()
{
  // If exists, return
  if (m_self)
    return m_self;

  // If doesn't, create and return
  m_self = new Data(qApp);
  return m_self;
}

QObject *Data::setValue(int type, const QVariant &value)
{
  switch (type) {
    case (GalleriesPath) : {
      GCore::GJobs::MoveJob *job = new GCore::GJobs::MoveJob(galleriesDir(), value.toString(), m_mainWindow);
      job->start();

      connect(job, SIGNAL(finished(bool)), this, SLOT(processGalleryMove(bool)));

      m_backup.insert("GalleriesPath", galleriesDir().absolutePath());

      m_settings->setValue("GalleriesPath", QDir::toNativeSeparators(value.toString()));

      return job;
    }
    case (PhotoEditContextMenu) : {
      m_photoEditContextMenu = value.value<QWidget*>();
      break;
    }
    case (PhotoContextMenu) : {
      m_photoContextMenu = value.value<QWidget*>();
      break;
    }
    case (GalleryContextMenu) : {
      m_galleryContextMenu = value.value<QWidget*>();
      break;
    }
    case (MainWindow) : {
      m_mainWindow = value.value<QWidget*>();
      break;
    }
    case (AppVersion) : {
      m_appVersion = value.toString();
      break;
    }
    case (AppBranch) : {
      m_branch = value.toString();
      break;
    }
    case (SupportedFormats) : {
      m_supportedFormats = value.toRegExp();
      break;
    }
    case (SearchBar) : {
      m_searchBar = value.value<QWidget*>();
      break;
    }
    case (DeleteSource) : {
      m_settings->setValue("DeleteSource", value);
      break;
    }
    case (BackgroundType) : {
      m_settings->setValue("BackgroundType", value);
      break;
    }
    case (OpenGL) : {
      m_settings->setValue("OpenGlRendering", value);
      break;
    }
    case (UpdateStartup) : {
      m_settings->setValue("UpdateAtStartup", value);
      break;
    }
    case (EditorPath) : {
      m_settings->setValue("EditorPath", value);
      break;
    }
    case (TranslationName) : {
      m_settings->setValue("TranslationName", value);
      break;
    }
    case (TranslationPath) : {
      m_settings->setValue("TranslationPath", value);
      break;
    }
    case (DisableVisualEffects) : {
      m_settings->setValue("DisableVisualEffects", value);
      break;
    }
    case (ImageAddProgress) : {
      m_imageAddProgress = value.value<QWidget*>();
      break;
    }
  }

  return 0;
}

QVariant Data::value(int type)
{
  switch (type) {
    case (ErrorHandler) :
            return qVariantFromValue(m_errorHandler);
    case (ImageFormats) :
            return qVariantFromValue(m_imageFormats);
    case (GalleriesPath) :
            return QDir::toNativeSeparators(galleriesDir().absolutePath());
    case (SettingsPath) :
            return settingsPath();
    case (ImageModel) :
            return QVariant::fromValue<QObject*>(m_imageModel);
    case (DirCompleter) :
            return QVariant::fromValue<QObject*>(m_dirCompleter);
    case (FileCompleter) :
            return QVariant::fromValue<QObject*>(m_fileCompleter);
    case (PhotoContextMenu) :
            return QVariant::fromValue<QWidget*>(m_photoContextMenu);
    case (PhotoEditContextMenu) :
            return QVariant::fromValue<QWidget*>(m_photoEditContextMenu);
    case (GalleryContextMenu) :
            return QVariant::fromValue<QWidget*>(m_galleryContextMenu);
    case (MainWindow) :
            return QVariant::fromValue<QWidget*>(m_mainWindow);
    case (ModelProxy) :
            return QVariant::fromValue<QObject*>(m_modelProxy);
    case (AppName) :
            return qApp->applicationName();
    case (AppVersion) :
            return m_appVersion;
    case (AppBranch) :
            return m_branch;
    case (SupportedFormats) :
            return m_supportedFormats;
    case (SearchBar) :
            return QVariant::fromValue<QWidget*>(m_searchBar);
    case (ImageAddProgress) :
            return QVariant::fromValue<QWidget*>(m_imageAddProgress);
    case (DeleteSource) :
            return m_settings->value("DeleteSource", false);
    case (BackgroundType) :
            return m_settings->value("BackgroundType", Round);
    case (OpenGL) :
            return m_settings->value("OpenGlRendering", false);
    case (UpdateStartup) :
            return m_settings->value("UpdateAtStartup", true);
    case (EditorPath) :
            return QDir::toNativeSeparators(m_settings->value("EditorPath").toString());
    case (Translations) :
            return QStringList(m_availableTranslations.keys());
    case (TranslationName) :
            return m_settings->value("TranslationName");
    case (TranslationPath) :
            return QDir::toNativeSeparators(m_settings->value("TranslationPath").toString());
    case (DisableVisualEffects) :
            return m_settings->value("DisableVisualEffects", false);
  }

  return QVariant();
}

QDir Data::galleriesDir() const
{
  QString galleriesPath = m_settings->value("GalleriesPath", settingsPath() + "/galleries").toString();

  QDir galleriesDir(galleriesPath);
  if (!galleriesDir.exists()) {
    // Temporary directory for galleries doesn't exist, creating it
    galleriesDir.mkpath(galleriesDir.absolutePath());
  }

  return galleriesDir.absolutePath();
}

QString Data::settingsPath() const
{
  QDir settingsPath(QDir::homePath() + "/.goya");
  if (!settingsPath.exists()) {
    // .goya directory doen't exists... Creating one
    settingsPath.mkpath(settingsPath.absolutePath());
  }

  return QDir::toNativeSeparators(settingsPath.absolutePath());
}

void Data::saveChanges() const
{
  m_settings->sync();
}

void Data::clear()
{
  saveChanges();
  delete m_imageModel;
  m_imageModel = 0;
}

Data::~Data()
{
  saveChanges();
}

void Data::processGalleryMove(bool successful)
{
  if (!successful)
    m_settings->setValue("GalleriesPath", m_backup.take("GalleriesPath"));
  else
    m_imageModel->reconstruct();
}

}

