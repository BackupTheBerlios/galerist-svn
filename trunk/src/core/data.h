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
#ifndef GCOREDATA_H
#define GCOREDATA_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QDir>

class QCompleter;
class QSettings;
class QSortFilterProxyModel;

namespace GCore
{
class ImageModel;

/**
 * @short Contains all the data for Goya.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class Data : public QObject
{
    Q_OBJECT
  public:
    /**
     * Set of options for background type (PhotoView).
     */
    enum BackgroundType {
      /** Rectangls with round cornes. */
      Round,
      /** Rectangle with rectangular corners. */
      Rectangular,
      /** Undefined state. */
      NotDefined
  };

    /**
     * List of available data types.
     */
    enum Type {
      ErrorHandler,
      ImageFormats,
      GalleriesPath,
      SettingsPath,
      ImageModel,
      DirCompleter,
      FileCompleter,
      PhotoContextMenu,
      PhotoEditContextMenu,
      GalleryContextMenu,
      MainWindow,
      ModelProxy,
      AppName,
      AppVersion,
      AppBranch,
      SupportedFormats,
      SearchBar,
      ImageAddProgress,
      DeleteSource,
      BackgroundType,
      OpenGL,
      UpdateStartup,
      EditorPath,
      Translations,
      TranslationName,
      TranslationPath,
      DisableVisualEffects
  };

    /**
     * A constructor.
     *
     * @param parent Parent of this data class.
     */
    Data(QObject *parent = 0);

    /**
     * Returns a reference to this class and creates it if it doesn't exist.
     *
     * @return This object.
     */
    static Data *self();

    QObject *setValue(int type, const QVariant &value);
    QVariant value(int type);

    /**
     * Returns path where galleries resides.
     *
     * @return Path of galleries as a QDir.
     */
    QDir galleriesDir() const;

    /**
     * Returns the path to the settings.
     *
     * @return Path of settings as a string.
     */
    QString settingsPath() const;

    /**
     * Saves all the changes.
     */
    void saveChanges() const;

    /**
     * Frees all dynamically allocated member variables.
     */
    void clear();

    /**
     * A destructor.
     */
    ~Data();



  private:
    // Variables.
    static Data *m_self;
    QSettings *m_settings;
    QObject *m_errorHandler;
    QStringList m_imageFormats;
    GCore::ImageModel *m_imageModel;
    QCompleter *m_dirCompleter;
    QCompleter *m_fileCompleter;
    QWidget *m_photoContextMenu;
    QWidget *m_photoEditContextMenu;
    QWidget *m_galleryContextMenu;
    QMap<QString, QString> m_availableTranslations;
    QString m_appVersion;
    QString m_branch;
    QWidget *m_mainWindow;
    QSortFilterProxyModel *m_modelProxy;
    QRegExp m_supportedFormats;

    QWidget *m_searchBar;
    QWidget *m_imageAddProgress;

    // Backup data
    QMap<QString, QVariant> m_backup;

    // Methods

  private slots:
    /**
     * Processes the gallery move.
     */
    void processGalleryMove(bool successful);

};

}

#endif
