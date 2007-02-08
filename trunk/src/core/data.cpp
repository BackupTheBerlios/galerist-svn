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
#include "data.h"

#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtGui/QDirModel>
#include <QtGui/QCompleter>
#include <QtCore/QSettings>
#include <QtGui/QMenu>
#include <QtGui/QSortFilterProxyModel>

#include "core/errorhandler.h"
#include "core/imagemodel.h"
#include "core/imageitem.h"

#include "core/network/updater.h"

namespace GCore
{

Data *Data::m_self = 0;

Data::Data(QObject *parent)
    : QObject(parent),
    m_errorHandler(0),
    m_viewType(Undefined),
    m_backgroundType(NotDefined),
    m_progressDialog(Unknown),
    m_imageModel(0),
    m_photoControl(0),
    m_photoContextMenu(0),
    m_photoEditContextMenu(0),
    m_galleryContextMenu(0),
#ifdef WANT_UPDATER
    m_updater(0),
#endif
    m_mainWindow(0),
    m_modelProxy(0)

{
  m_self = this;
  m_settings = new QSettings(this);
  m_dirCompleter = new QCompleter(this);
  m_dirCompleter->setModel(new QDirModel(QStringList(), QDir::AllDirs, QDir::Name, m_dirCompleter));

  // Setting up supported image formats
  m_imageFormats << "*.jpg"; //JPEG
  m_imageFormats << "*.jpeg"; //JPEG
  m_imageFormats << "*.png"; //PNG
  m_imageFormats << "*.gif"; //GIF

  // Search for all available translations
  QStringList translationsFilter;
  translationsFilter << "*.qm";

  // We insert the default language
  m_availableTranslations.insert("English", QString());

  // First we look into the local "repository" of translations
  QDir localTranslations(getSettingsPath());
  if (!localTranslations.cd("translations")) {
    localTranslations.mkdir("translations");
    localTranslations.cd("translations");
  }
  QStringList localTranslationsList = localTranslations.entryList(translationsFilter, QDir::Files);
  QStringList::const_iterator localEnd = localTranslationsList.constEnd();

  // We add the local translations
  for (QStringList::const_iterator localCount = localTranslationsList.constBegin(); localCount != localEnd; localCount++) {
    QString languageName = QString(*localCount).remove(".qm");
    languageName[0] = QChar(languageName[0]).toUpper();
    m_availableTranslations.insert(languageName, localTranslations.absoluteFilePath(*localCount));
  }

  // Now comes the global one -> Disabled for now
  /*QDir globalTranslations(QDir::currentPath());
  qDebug(globalTranslations.absolutePath().toAscii());
  if (globalTranslations.cd("translations")) {
    qDebug(globalTranslations.absolutePath().toAscii());
    QStringList globalTranslationsList = globalTranslations.entryList(translationsFilter, QDir::Files);
    QStringList::const_iterator globalEnd = globalTranslationsList.constEnd();

    // We combine translations from both locations. Local translations overwrite global translations
    for (QStringList::const_iterator globalCount = globalTranslationsList.constBegin(); globalCount != globalEnd; globalCount++) {
      // If there is no local translation with the same name, we add the global one
      if (!m_availableTranslations.contains(*globalCount)) {
        QString languageName = QString(*globalCount).remove(".qm");
        languageName[0] = QChar(languageName[0]).toUpper();
        m_availableTranslations.insert(languageName, globalTranslations.absoluteFilePath(*globalCount));
      }
    }
  }*/
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

QStringList Data::getImageFormats()
{
  return m_imageFormats;
}

void Data::setImageView(ViewType type)
{
  m_viewType = type;
  m_settings->setValue("ViewType", type);
}

GCore::Data::ViewType Data::getImageView()
{
  // If the value hasn't been fetched before, we get it now
  if (m_viewType == Undefined)
    m_viewType = static_cast<ViewType>(m_settings->value("ViewType", Icon).toInt());

  // Return the value
  return m_viewType;
}

void Data::saveChanges()
{
  m_settings->sync();
}

QString Data::getGalleriesPath()
{
  QDir galleriesPath(QDir::homePath() + "/.goya/galleries");
  if (!galleriesPath.exists()) {
    // .galerist directory doen't exists... Creating one
    galleriesPath.mkpath(galleriesPath.absolutePath());
  }

  return QDir::toNativeSeparators(galleriesPath.absolutePath());
}

QString Data::getSettingsPath()
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

QCompleter *Data::getDirCompleter()
{
  return m_dirCompleter;
}

void Data::setBackgroundType(BackgroundType type)
{
  m_settings->setValue("BackgroundType", type);
  m_backgroundType = type;
}

GCore::Data::BackgroundType Data::getBackgroundType()
{
  // If the value hasn't been fetched before, we get it now
  if (m_backgroundType == NotDefined)
    m_backgroundType = static_cast<BackgroundType>(m_settings->value("BackgroundType", Default).toInt());

  // Return the value
  return m_backgroundType;
}

void Data::setProgressDialog(ProgressDialog showOption)
{
  m_settings->setValue("ProgressDialog", showOption);
  m_progressDialog = showOption;
}

GCore::Data::ProgressDialog Data::getProgressDialog()
{
  // If the value hasn't been fetched before, we get it now
  if (m_progressDialog == Unknown)
    m_progressDialog = static_cast<ProgressDialog>(m_settings->value("ProgressDialog", Show).toInt());

  // Return the value
  return m_progressDialog;
}

void Data::setOpengl(bool enable)
{
  m_settings->setValue("OpenGlRendering", enable);
  m_opengl = enable;
}

bool Data::getOpengl()
{
  QString temp;

  // If the value hasn't been fetched before, we get it now
  m_opengl = m_settings->value("OpenGlRendering", false).toBool();

  // Return the value
  return m_opengl;
}

void Data::setPhotoControl(GWidgets::PhotoControl *photoControl)
{
  m_photoControl = photoControl;
}

GWidgets::PhotoControl *Data::getPhotoControl()
{
  return m_photoControl;
}

void Data::setUpdateStartup(bool enable)
{
  m_settings->setValue("UpdateAtStartup", enable);
}

bool Data::getUpdateStartup()
{
  return m_settings->value("UpdateAtStartup", true).toBool();
}

void Data::setPhotoEditor(const QString &filePath)
{
  m_settings->setValue("EditorPath", filePath);
}

QString Data::getPhotoEditor()
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

void Data::setEnableToolTips(bool enable)
{
  m_settings->setValue("ToolTips", enable);
}

bool Data::getEnableToolTips()
{
  return m_settings->value("ToolTips", true).toBool();
}

QStringList Data::getAvailableTranslations()
{
  return QStringList(m_availableTranslations.keys());
}

void Data::setTranslation(const QString &name)
{
  m_settings->setValue("TranslationName", name);
  m_settings->setValue("TranslationFilepath", m_availableTranslations.value(name));
}

QString Data::getTranslationName()
{
  return m_settings->value("TranslationName", "English").toString();
}

QString Data::getTranslationFilePath()
{
  return QDir::toNativeSeparators(m_settings->value("TranslationFilepath", QString()).toString());
}

QString Data::getTranslationFileName()
{
  return getTranslationName().toLower().append(".qm");
}

QString Data::getTranslationPath()
{
  return QDir::toNativeSeparators(getTranslationFilePath().remove(getTranslationName().toLower().append(".qm")));
}

void Data::setMainWindow(QWidget *mainWindow)
{
  m_mainWindow = mainWindow;
  //setParent(m_mainWindow);
}

QWidget *Data::getMainWindow()
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

#ifdef WANT_UPDATER
GNetwork::Updater *Data::getUpdater()
{
  if (!m_updater)
    m_updater = new GNetwork::Updater(this);

  return m_updater;
}
#endif

QString Data::getAppName()
{
  return qApp->applicationName();
}

void Data::setAppVersion(const QString &version)
{
  m_appVersion = version;
}

QString Data::getAppVersion()
{
  return m_appVersion;
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

}
