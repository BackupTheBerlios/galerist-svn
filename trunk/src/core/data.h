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
#ifndef GCOREDATA_H
#define GCOREDATA_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QMap>

class QCompleter;
class QSettings;
class QMenu;
class QSortFilterProxyModel;

typedef QMap<QString, QString> Translations;

namespace GWidgets
{
class PhotoControl;
}

namespace GCore
{

namespace GNetwork
{

class Updater;

}

class ErrorHandler;
class ImageModel;

/**
 @author Gregor Kalisnik <gregor@podnapisi.net>
*/
class Data : public QObject
{
    Q_OBJECT
  public:
    enum ViewType {
      Icon,
      List,
      Slide,
      Undefined
  };

    enum BackgroundType {
      Round,
      Default,
      NotDefined
  };

    enum ProgressDialog {
      Show,
      Hide,
      Unknown
  };

    Data(QObject *parent = 0);

    static Data *self();

    ErrorHandler *getErrorHandler();

    QStringList getImageFormats();

    void setImageView(ViewType type);
    GCore::Data::ViewType getImageView();

    QString getGalleriesPath();
    QString getSettingsPath();

    ImageModel *getImageModel();

    QCompleter *getDirCompleter();

    void setBackgroundType(BackgroundType type);
    GCore::Data::BackgroundType getBackgroundType();

    void setProgressDialog(ProgressDialog showOption);
    GCore::Data::ProgressDialog getProgressDialog();

    void setOpengl(bool enable);
    bool getOpengl();

    void setPhotoControl(GWidgets::PhotoControl *photoControl);
    GWidgets::PhotoControl *getPhotoControl();

    /**
     * Sets the update at startup value.
     *
     * @param enable Enable the update feature at startup.
     */
    void setUpdateStartup(bool enable);

    /**
     * Returns the status of update at strtup.
     *
     * @return State of update at startup.
     */
    bool getUpdateStartup();

    /**
     * Method for defining image editor.
     * @param filePath Path to the editor.
     */
    void setPhotoEditor(const QString &filePath);
    /**
     * Method for retrieving file path of the editor.
     *
     * @return Path to the editor.
     */
    QString getPhotoEditor();

    /**
     * Method for storing photo context menu.
     *
     * @param contexMenu Context menu of actions for photo editing.
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
     * Sets the configuration for Tool tips.
     *
     * @param enable Enables the use of Goya's tool tips.
     */
    void setEnableToolTips(bool enable);

    /**
     * Returns whetever Goya's tool tips are enabled.
     *
     * @return State of bubble tooltips.
     */
    bool getEnableToolTips();

    /**
     * Returns list of available translations.
     *
     * @return List of translations with full path.
     */
    QStringList getAvailableTranslations();

    /**
     * Sets the translation to use.
     *
     * @param name Name of the selected translation.
     */
    void setTranslation(const QString &name);

    /**
     * Returns the name of the translation in use.
     *
     * @return Name of the translation in use.
     */
    QString getTranslationName();

    /**
     * Returns the full file path to the used translation.
     *
     * @return Full file path to the translation in use. If empty, english is used.
     */
    QString getTranslationFilePath();

    /**
     * Returns the file name of the translation in use.
     *
     * @return File name.
     */
    QString getTranslationFileName();

    /**
     * Returns absolute path to the translation in use.
     *
     * @return Absolute path.
     */
    QString getTranslationPath();

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
    QWidget *getMainWindow();

    /**
     * Gets the model proxy.
     */
    QSortFilterProxyModel *getModelProxy();

#ifdef WANT_UPDATER
    /**
     * Returns an Updater object.
     */
    GNetwork::Updater *getUpdater();
#endif

    /**
     * Returns the name of the program.
     *
     * @return The name.
     */
    QString getAppName();

    /**
     * Sets the version (hard-coded just used in main.cpp).
     *
     * @param version Version string
     */
    void setAppVersion(const QString &version);

    /**
     * Returns the version of the application.
     *
     * @return Version string.
     */
    QString getAppVersion();

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
    QRegExp getSupportedFormats();

    /**
     * Saves all the changes.
     */
    void saveChanges();

    /**
     * Frees all dynamically allocated member variables.
     */
    void clear();

    ~Data();

  private:
    // Variables.
    static Data *m_self;
    QSettings *m_settings;
    ErrorHandler *m_errorHandler;
    QStringList m_imageFormats;
    ViewType m_viewType;
    BackgroundType m_backgroundType;
    ProgressDialog m_progressDialog;
    ImageModel *m_imageModel;
    QCompleter *m_dirCompleter;
    bool m_opengl;
    GWidgets::PhotoControl *m_photoControl;
    QMenu *m_photoContextMenu;
    QMenu *m_photoEditContextMenu;
    QMenu *m_galleryContextMenu;
    Translations m_availableTranslations;
    QString m_appVersion;
#ifdef WANT_UPDATER
    GNetwork::Updater *m_updater;
#endif
    QWidget *m_mainWindow;
    QSortFilterProxyModel *m_modelProxy;
    QRegExp m_supportedFormats;

    // Methods

};

}

#endif
