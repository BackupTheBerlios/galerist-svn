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
class QMenu;
class QSortFilterProxyModel;

typedef QMap<QString, QString> Translations;

namespace GWidgets
{
class SearchBar;
}

namespace GCore
{

class ErrorHandler;
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

    /**
     * Returns a refernce to the ErrorHandler.
     *
     * @return ErrorHandler object.
     */
    ErrorHandler *getErrorHandler();

    /**
     * Returns the list of supported image formats.
     *
     * @return List of supported image formats.
     */
    QStringList getImageFormats() const;

    /**
     * Returns path where galleries resides.
     *
     * @return Path of galleries as a QDir.
     */
    QDir getGalleriesDir() const;
    /**
     * Returns path where galleries resides.
     *
     * @return Path of galleries as a string.
     */
    QString getGalleriesPath() const;
    /**
     * Sets the new gallery path.
     *
     * @param path New gallery path.
     *
     * @return Job doing the move.
     */
    QObject *setGalleriesPath(const QString &path);

    /**
     * Gets the default state of the "Delete source images" checkbox.
     */
    bool getDeleteSourceImagesDefault() const;

    /**
     * Sets the default state of the "Delete source images" checkbox.
     */
    void setDeleteSourceImagesDefault(bool state) const;

    /**
     * Returns the path to the settings.
     *
     * @return Path of settings as a string.
     */
    QString getSettingsPath() const;

    /**
     * Returns the image model.
     *
     * @return Image model.
     */
    ImageModel *getImageModel();

    /**
     * Get the dir completer. Used most by LineEdit.
     */
    QCompleter *getDirCompleter() const;

    /**
     * Get the file completer. Used most by LineEdit.
     */
    QCompleter *getFileCompleter() const;

    /**
     * Sets the look of image's background in PhotoView.
     *
     * @param type The type of the background.
     */
    void setBackgroundType(BackgroundType type) const;
    /**
     * Returns the background type.
     *
     * @return The type of background.
     */
    GCore::Data::BackgroundType getBackgroundType() const;

    /**
     * Sets if the PhotoView should use OpenGL instructions.
     *
     * @param enable Enable it or not.
     */
    void setOpengl(bool enable) const;
    /**
     * Returns the status of OpenGL usement.
     *
     * @return Status as a boolean.
     */
    bool getOpengl() const;

    /**
     * Sets the update at startup value.
     *
     * @param enable Enable the update feature at startup.
     */
    void setUpdateStartup(bool enable) const;

    /**
     * Returns the status of update at strtup.
     *
     * @return State of update at startup.
     */
    bool getUpdateStartup() const;

    /**
     * Method for defining image editor.
     * @param filePath Path to the editor.
     */
    void setPhotoEditor(const QString &filePath) const;
    /**
     * Method for retrieving file path of the editor.
     *
     * @return Path to the editor.
     */
    QString getPhotoEditor() const;

    /**
     * Method for storing photo context menu.
     *
     * @param contextMenu Context menu of actions for photo editing.
     */
    void setPhotoContextMenu(QMenu *contextMenu);
    /**
     * Method for retrieving photo context menu.
     *
     * @return Context menu of actions for photo editing.
     */
    QMenu *getPhotoContextMenu();

    /**
     * Method for setting photo editing context menu.
     *
     * @param contextMenu The context menu itself.
     */
    void setPhotoEditingContextMenu(QMenu *contextMenu);
    /**
     * Returns the context menu for photo editing.
     */
    QMenu *getPhotoEditingContextMenu();

    /**
     * Sets the context menu used in Gallery tree view.
     *
     * @param contextMenu Context menu itself.
     */
    void setGalleryContextMenu(QMenu *contextMenu);
    /**
     * Returns the context menu for it to be used in gallery tree view.
     *
     * @return The context menu.
     */
    QMenu *getGalleryContextMenu();

    /**
     * Returns list of available translations.
     *
     * @return List of translations with full path.
     */
    QStringList getAvailableTranslations() const;

    /**
     * Sets the translation to use.
     *
     * @param name Name of the selected translation.
     */
    void setTranslation(const QString &name) const;

    /**
     * Returns the name of the translation in use.
     *
     * @return Name of the translation in use.
     */
    QString getTranslationName() const;

    /**
     * Returns the full file path to the used translation.
     *
     * @return Full file path to the translation in use. If empty, english is used.
     */
    QString getTranslationFilePath() const;

    /**
     * Returns the file name of the translation in use.
     *
     * @return File name.
     */
    QString getTranslationFileName() const;

    /**
     * Returns absolute path to the translation in use.
     *
     * @return Absolute path.
     */
    QString getTranslationPath() const;

    /**
     * Set the main window reference. Used for accessing main windows function throughout Goya.
     *
     * @param mainWindow Reference to the main window.
     */
    void setMainWindow(QWidget *mainWindow);

    /**
     * Returns the main window reference. Used for accessing main windows function throughout Goya.
     *
     * @return Reference to the main window.
     */
    QWidget *getMainWindow() const;

    /**
     * Gets the model proxy.
     */
    QSortFilterProxyModel *getModelProxy();

    /**
     * Returns the name of the program.
     *
     * @return The name.
     */
    QString getAppName() const;

    /**
     * Sets the version (hard-coded just used in main.cpp).
     *
     * @param version Version string
     * @param branch Branch name
     */
    void setAppVersion(const QString &version, const QString& branch);

    /**
     * Returns the version of the application.
     *
     * @return Version string.
     */
    QString getAppVersion() const;

    /**
     * Returns the branch.
     *
     * @return Branch name.
     */
    QString getBranch() const;

    /**
     * Set the list of supported images.
     *
     * @param supportedFormats List of the supported formats (QRegExp).
     */
    void setSupportedFormats(const QRegExp &supportedFormats);

    /**
     * Get the list of supported images.
     *
     * @return Returns the list of supported image formats in a QRegExp.
     */
    QRegExp getSupportedFormats()const ;

    /**
     * Saves all the changes.
     */
    void saveChanges() const;

    /**
     * Frees all dynamically allocated member variables.
     */
    void clear();

    ~Data();

    /**
     * Sets the search bar.
     */
    void setSearchBar(GWidgets::SearchBar* searchBar);

    /**
     * Gets the search bar.
     */
    GWidgets::SearchBar* getSearchBar() const;

    /**
     * Sets the ImageAddProgress.
     */
    void setImageAddProgress(QWidget* imageAddProgress);

    /**
     * Gets and creates if neccesary ImageAddProgress.
     */
    QWidget *getImageAddProgress() const;

    /**
     * Get the Visual effects disabling flag.
     */
    bool isVisualEffectsDisabled() const;

    /**
     * Sets the Visual effects disabling flag.
     */
    void setVisualEffects(bool disable) const;



  private:
    // Variables.
    static Data *m_self;
    QSettings *m_settings;
    ErrorHandler *m_errorHandler;
    QStringList m_imageFormats;
    ImageModel *m_imageModel;
    QCompleter *m_dirCompleter;
    QCompleter *m_fileCompleter;
    QMenu *m_photoContextMenu;
    QMenu *m_photoEditContextMenu;
    QMenu *m_galleryContextMenu;
    Translations m_availableTranslations;
    QString m_appVersion;
    QString m_branch;
    QWidget *m_mainWindow;
    QSortFilterProxyModel *m_modelProxy;
    QRegExp m_supportedFormats;

    GWidgets::SearchBar *m_searchBar;
    QWidget *m_imageAddProgress;

    // Backup data
    QMap<QString, QVariant> m_backup;

    // Methods

  private slots:
    void processGalleryMove(bool successful);

};

}

#endif
