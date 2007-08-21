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

#include <QtGui/QSortFilterProxyModel>

#include "core/imagemodel.h"

class QCompleter;
class QSettings;
class QMenu;

namespace GCore
{
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
      DirCompleter,
      FileCompleter,
      GalleryContextMenu,
      MainWindow,
      AppName,
      AppVersion,
      AppBranch,
      SearchBar,
      DeleteSource,
      BackgroundType,
      OpenGL,
      UpdateStartup,
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
     * Returns the dir where galleries resides.
     */
    QDir galleriesDir() const;

    /**
     * Returns path where galleries resides.
     */
    QString galleriesPath() const;

    /**
     * Sets the gallery path.
     */
    void setGalleriesPath(const QString &path);

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

    /**
     * Gets proxy model for ListView.
     */
    ImageModel *imageProxy();

    /**
     * Gets proxy model for GalleryView.
     */
    QSortFilterProxyModel *galleryProxy();

    /**
     * Gets image model.
     */
    ImageModel *imageModel();

    /**
     * Gets the context menu used by ListView.
     */
    QMenu *listContextMenu() const;

    /**
     * Sets the context menu used by ListView.
     */
    void setListContextMenu(QMenu *menu);

    /**
     * Gets the drop context menu.
     */
    QMenu *dropContextMenu() const;

    /**
     * Sets the drop context menu.
     */
    void setDropContextMenu(QMenu *dropContextMenu);

    /**
     * Gets supported format.
     */
    QRegExp supportedFormats() const;

    QStringList supportedFormatsList() const;

    /**
     * Gets the progress widget.
     */
    QWidget *imageAddProgress() const;

    /**
     * Sets the progress widget.
     */
    void setImageAddProgress(QWidget *imageAddProgress);

    /**
     * Gets the editor.
     */
    QString imageEditor() const;

    /**
     * Sets the editor.
     */
    void setImageEditor(const QString &editor) const;

  private:
    // Variables.
    static Data *m_self;
    QSettings *m_settings;
    QObject *m_errorHandler;
    QStringList m_imageFormats;
    ImageModel *m_imageModel;
    QCompleter *m_dirCompleter;
    QCompleter *m_fileCompleter;
    QMenu *m_listContextMenu;
    QMenu *m_galleryContextMenu;
    QMenu *m_dropContextMenu;
    QMap<QString, QString> m_availableTranslations;
    QString m_appVersion;
    QString m_branch;
    QWidget *m_mainWindow;
    QSortFilterProxyModel *m_galleryProxy;
    ImageModel *m_imageProxy;
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
