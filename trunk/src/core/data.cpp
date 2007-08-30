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
#include "core/jobmanager.h"

#include "core/jobs/movejob.h"

namespace GCore
{

Data *Data::m_self = 0;

Data::Data(QObject *parent)
    : QObject(parent),
    m_errorHandler(0),
    m_imageModel(0),
    m_galleryContextMenu(0),
    m_mainWindow(0),
    m_imageProxy(0),
    m_galleryProxy(0)
{
  m_self = this;
  m_settings = new QSettings(this);
  m_dirCompleter = new QCompleter(this);
  m_dirCompleter->setModel(new QDirModel(QStringList(), QDir::AllDirs | QDir::Drives, QDir::Name, m_dirCompleter));

  m_fileCompleter = new QCompleter(this);
  m_fileCompleter->setModel(new QDirModel(QStringList(), QDir::AllEntries, QDir::Name, m_fileCompleter));

  m_errorHandler = new GCore::ErrorHandler(this);

  // Setting up supported image formats
  m_imageFormats << "*.jpg"; //JPEG
  m_imageFormats << "*.jpeg"; //JPEG
  m_imageFormats << "*.png"; //PNG
  m_imageFormats << "*.gif"; //GIF

  m_supportedFormats = QRegExp("gif|jpg|jpeg|png", Qt::CaseInsensitive);

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
    case (GalleryContextMenu) : {
      m_galleryContextMenu = static_cast<QMenu*>(value.value<QWidget*>());
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
  }

  return 0;
}

QVariant Data::value(int type)
{
  switch (type) {
    case (ErrorHandler) :
            return qVariantFromValue(m_errorHandler);
    case (DirCompleter) :
            return QVariant::fromValue<QObject*>(m_dirCompleter);
    case (FileCompleter) :
            return QVariant::fromValue<QObject*>(m_fileCompleter);
    case (GalleryContextMenu) :
            return QVariant::fromValue<QWidget*>(m_galleryContextMenu);
    case (MainWindow) :
            return QVariant::fromValue<QWidget*>(m_mainWindow);
    case (AppName) :
            return qApp->applicationName();
    case (AppVersion) :
            return m_appVersion;
    case (AppBranch) :
            return m_branch;
    case (SearchBar) :
            return QVariant::fromValue<QWidget*>(m_searchBar);
    case (DeleteSource) :
            return m_settings->value("DeleteSource", false);
    case (BackgroundType) :
            return m_settings->value("BackgroundType", Round);
    case (OpenGL) :
            return m_settings->value("OpenGlRendering", false);
    case (UpdateStartup) :
            return m_settings->value("UpdateAtStartup", true);
    case (Translations) :
            return QStringList(m_availableTranslations.keys());
    case (TranslationName) :
            return m_settings->value("TranslationName", "English");
    case (TranslationPath) :
            return QDir::toNativeSeparators(m_settings->value("TranslationPath", ":translations/english.qm").toString());
    case (DisableVisualEffects) :
            return m_settings->value("DisableVisualEffects", false);
  }

  return QVariant();
}

QDir Data::galleriesDir() const
{
  QDir galleriesDir(galleriesPath());
  if (!galleriesDir.exists()) {
    // Temporary directory for galleries doesn't exist, creating it
    galleriesDir.mkpath(galleriesDir.absolutePath());
  }

  return galleriesDir;
}

QString Data::galleriesPath() const
{
  return m_settings->value("GalleriesPath", settingsPath() + "/galleries").toString();;
}

void Data::setGalleriesPath(const QString &path)
{
  GCore::GJobs::MoveJob *job = new GCore::GJobs::MoveJob(galleriesDir(), path, m_mainWindow);

  JobManager::self()->registerJob("GalleriesMove", job);

  connect(job, SIGNAL(finished(bool)), this, SLOT(processGalleryMove(bool)));

  m_backup.insert("GalleriesPath", galleriesDir().absolutePath());

  m_settings->setValue("GalleriesPath", QDir::toNativeSeparators(path));
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

ImageModel *Data::imageProxy()
{
  if (!m_imageProxy)
    m_imageProxy = new ImageModel(this);

  return m_imageProxy;
}

QSortFilterProxyModel *Data::galleryProxy()
{
  if (!m_galleryProxy) {
    m_galleryProxy = new QSortFilterProxyModel(this);
    m_galleryProxy->setSourceModel(imageModel());

    m_galleryProxy->setFilterRole(ImageModel::ImageTypeRole);
    m_galleryProxy->setFilterWildcard(QString::number(ImageItem::Gallery));
  }

  return m_galleryProxy;
}

ImageModel *Data::imageModel()
{
  if (!m_imageModel)
    m_imageModel = new ImageModel(this);

  return m_imageModel;
}

QMenu *Data::listContextMenu() const
{
  return m_listContextMenu;
}

void Data::setListContextMenu(QMenu *menu)
{
  m_listContextMenu = menu;
}

QMenu *Data::dropContextMenu() const
{
  return m_dropContextMenu;
}

void Data::setDropContextMenu(QMenu *dropContextMenu)
{
  m_dropContextMenu = dropContextMenu;
}

QRegExp Data::supportedFormats() const
{
  return m_supportedFormats;
}

QStringList Data::supportedFormatsList() const
{
  return m_imageFormats;
}

QWidget *Data::imageAddProgress() const
{
  return m_imageAddProgress;
}

void Data::setImageAddProgress(QWidget *imageAddProgress)
{
  m_imageAddProgress = imageAddProgress;
}

QString Data::imageEditor() const
{
  return QDir::toNativeSeparators(m_settings->value("EditorPath").toString());
}

void Data::setImageEditor(const QString &editor) const
{
  m_settings->setValue("EditorPath", QDir::fromNativeSeparators(editor));
}

void Data::processGalleryMove(bool successful)
{
  if (!successful)
    m_settings->setValue("GalleriesPath", m_backup.take("GalleriesPath"));
  else
    m_imageModel->reconstruct();
}

}

