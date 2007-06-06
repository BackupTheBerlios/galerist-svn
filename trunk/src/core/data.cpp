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

ErrorHandler *Data::getErrorHandler()
{
  // We create the Error handler if necessary
  if (!m_errorHandler)
    m_errorHandler = new ErrorHandler(this);

  // Return it's pointer
  return m_errorHandler;
}

QStringList Data::getImageFormats() const
{
  return m_imageFormats;
}

QDir Data::getGalleriesDir() const
{
  QString galleriesPath = m_settings->value("GalleriesPath", getSettingsPath() + "/galleries").toString();

  QDir galleriesDir(galleriesPath);
  if (!galleriesDir.exists()) {
    // .galerist directory doen't exists... Creating one
    galleriesDir.mkpath(galleriesDir.absolutePath());
  }

  return galleriesDir.absolutePath();
}

QString Data::getGalleriesPath() const
{
  return QDir::toNativeSeparators(getGalleriesDir().absolutePath());
}

QObject *Data::setGalleriesPath(const QString &path) const
{
  GCore::GJobs::MoveJob *job = new GCore::GJobs::MoveJob(getGalleriesDir(), QDir(path), m_mainWindow);
  job->start();

  m_settings->setValue("GalleriesPath", QDir::toNativeSeparators(path));

  return job;
}

QString Data::getSettingsPath() const
{
  QDir settingsPath(QDir::homePath() + "/.goya");
  if (!settingsPath.exists()) {
    // .galerist directory doen't exists... Creating one
    settingsPath.mkpath(settingsPath.absolutePath());
  }

  return QDir::toNativeSeparators(settingsPath.absolutePath());
}

ImageModel *Data::getImageModel()
{
  if (!m_imageModel) {
    m_imageModel = new ImageModel(getGalleriesPath(), this);
  }

  return m_imageModel;
}

QCompleter *Data::getDirCompleter() const
{
  return m_dirCompleter;
}

QCompleter *Data::getFileCompleter() const
{
  return m_fileCompleter;
}

void Data::setBackgroundType(BackgroundType type) const
{
  m_settings->setValue("BackgroundType", type);
}

GCore::Data::BackgroundType Data::getBackgroundType() const
{
  return static_cast<BackgroundType>(m_settings->value("BackgroundType", Round).toInt());
}

void Data::setOpengl(bool enable) const
{
  m_settings->setValue("OpenGlRendering", enable);
}

bool Data::getOpengl() const
{
  return m_settings->value("OpenGlRendering", false).toBool();
}

void Data::setUpdateStartup(bool enable) const
{
  m_settings->setValue("UpdateAtStartup", enable);
}

bool Data::getUpdateStartup() const
{
  return m_settings->value("UpdateAtStartup", true).toBool();
}

void Data::setPhotoEditor(const QString &filePath) const
{
  m_settings->setValue("EditorPath", filePath);
}

QString Data::getPhotoEditor() const
{
  return QDir::toNativeSeparators(m_settings->value("EditorPath").toString());
}

void Data::setPhotoContextMenu(QMenu *contextMenu)
{
  m_photoContextMenu = contextMenu;
}

QMenu *Data::getPhotoContextMenu()
{
  if (m_photoContextMenu) {
    return m_photoContextMenu;
  } else {
    QMenu *temp = new QMenu(m_mainWindow);
    temp->addAction(tr("Nothing available"))->setDisabled(true);
    return temp;
  }
}

void Data::setPhotoEditingContextMenu(QMenu *contextMenu)
{
  m_photoEditContextMenu = contextMenu;
}

QMenu *Data::getPhotoEditingContextMenu()
{
  if (m_photoEditContextMenu) {
    return m_photoEditContextMenu;
  } else {
    QMenu *temp = new QMenu(m_mainWindow);
    temp->addAction(tr("Nothing available"))->setDisabled(true);
    return temp;
  }
}

void Data::setGalleryContextMenu(QMenu *contextMenu)
{
  m_galleryContextMenu = contextMenu;
}

QMenu *Data::getGalleryContextMenu()
{
  if (m_galleryContextMenu) {
    return m_galleryContextMenu;
  } else {
    QMenu *temp = new QMenu(m_mainWindow);
    temp->addAction(tr("Nothing available"))->setDisabled(true);
    return temp;
  }
}

QStringList Data::getAvailableTranslations() const
{
  return QStringList(m_availableTranslations.keys());
}

void Data::setTranslation(const QString &name) const
{
  m_settings->setValue("TranslationName", name);
  m_settings->setValue("TranslationFilepath", m_availableTranslations.value(name));
}

QString Data::getTranslationName() const
{
  return m_settings->value("TranslationName", "English").toString();
}

QString Data::getTranslationFilePath() const
{
  return QDir::toNativeSeparators(m_settings->value("TranslationFilepath", QString()).toString());
}

QString Data::getTranslationFileName() const
{
  return getTranslationName().toLower().append(".qm");
}

QString Data::getTranslationPath() const
{
  return QDir::toNativeSeparators(getTranslationFilePath().remove(getTranslationName().toLower().append(".qm")));
}

void Data::setMainWindow(QWidget *mainWindow)
{
  m_mainWindow = mainWindow;
  //setParent(m_mainWindow);
}

QWidget *Data::getMainWindow() const
{
  return m_mainWindow;
}

QSortFilterProxyModel *Data::getModelProxy()
{
  if (!m_modelProxy) {
    m_modelProxy = new QSortFilterProxyModel(this);
    m_modelProxy->setFilterRole(GCore::ImageModel::ImageTypeRole);
    m_modelProxy->setFilterWildcard(QString::number(GCore::ImageItem::Gallery));
    m_modelProxy->setSourceModel(getImageModel());
  }

  return m_modelProxy;
}

QString Data::getAppName() const
{
  return qApp->applicationName();
}

void Data::setAppVersion(const QString &version, const QString &branch)
{
  m_appVersion = version;
  m_branch = branch;
}

QString Data::getAppVersion() const
{
  return m_appVersion;
}

QString Data::getBranch() const
{
  return m_branch;
}

void Data::setSupportedFormats(const QRegExp &supportedFormats)
{
  m_supportedFormats = supportedFormats;
  m_supportedFormats.setCaseSensitivity(Qt::CaseInsensitive);
}

QRegExp Data::getSupportedFormats() const
{
  return m_supportedFormats;
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

GWidgets::SearchBar* Data::getSearchBar() const
{
  return m_searchBar;
}

void Data::setSearchBar(GWidgets::SearchBar* searchBar)
{
  m_searchBar = searchBar;
}

QWidget* Data::getImageAddProgress() const
{
  return m_imageAddProgress;
}

void Data::setImageAddProgress(QWidget* imageAddProgress)
{
  m_imageAddProgress = imageAddProgress;
}

}

