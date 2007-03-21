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
#ifndef GCOREIMAGEMODEL_H
#define GCOREIMAGEMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtGui/QIcon>

class QDir;

namespace GCore
{

class ImageItem;

namespace GJobs
{
class ReadJob;
class CopyJob;
}

/**
 * @short The model with images and all of it's data.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class ImageModel : public QAbstractItemModel
{
    Q_OBJECT
  signals:
    /**
     * Signal that is emited when reading (creating) thumbnails.
     *
     * @param fileName The name of the current thumbnail.
     */
    void signalThumb(const QString &fileName);

  public:
    /**
     * Roles that tells data() method what information to return.
     */
    enum Roles
    {
      /** Image's name. */
      ImageNameRole          = Qt::DisplayRole,
      /** Image's thumbnail. */
      ImageThumbnailRole     = Qt::DecorationRole,
      /** Image's tooltip. */
      ImageTooltipRole       = Qt::ToolTipRole,
      /** Image's description. */
      ImageDescriptionRole   = Qt::UserRole + 1,
      /** Image type (Gallery or image). */
      ImageTypeRole          = Qt::UserRole + 2,
      /** Image's metadata handler. */
      ImageMetadataRole      = Qt::UserRole + 3,
      /** Image's filename (example.jpg). */
      ImageFilenameRole      = Qt::UserRole + 4,
      /** Image's path with filename (/path/to/image.jpg). */
      ImageFilepathRole      = Qt::UserRole + 5,
      /** Image's directory path (/path/to). */
      ImageDirPathRole       = Qt::UserRole + 6,
      /** Path to image's thumbnail. */
      ImageThumbnailPathRole = Qt::UserRole + 7,
      /** Returns the actual image. */
      ImagePictureRole       = Qt::UserRole + 8
    };

    /**
     * A constructor.
     *
     * @param path Path of the galleries.
     * @param parent Parent of this model.
     */
    ImageModel(const QString &path, QObject *parent = 0);
    /**
     * A destructor.
     */
    ~ImageModel();

    /**
     * Method for retrieving data.
     *
     * @param index Index of the item.
     * @param role Role of the data.
     */
    QVariant data(const QModelIndex &index, int role) const;
    /**
     * Sets the data.
     *
     * @param index Index of the item.
     * @param value The new value.
     * @param role Role of the data.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    /**
     * Returns the flag that defines what can be done with the item.
     *
     * @param index Index of the item.
     */
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /**
     * Returns the name of the column.
     *
     * @param section Which column.
     * @param role Role of the name.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    /**
     * Gets the index of a item at a specific row and column.
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    /**
     * Get the item's parent index.
     *
     * @param index Item's index.
     */
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
    /**
     * Counts the item's rows.
     *
     * @param parent Parent's index.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    /**
     * Counts the number of columns of an item.
     *
     * @param parent Parent's index.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Sets the item's name.
     *
     * @param name The new name.
     * @param index Item's index.
     */
    void setName(const QString &name, const QModelIndex &index);
    /**
     * Sets item's description.
     *
     * @param description The new description.
     * @param index Item's index.
     */
    void setDescription(const QString &description, const QModelIndex &index);

    /**
     * Checks if the item already exists.
     *
     * @param name Name of the searched item.
     * @param index Parent's index.
     */
    bool exists(const QString &name, const QModelIndex &index);

    /**
     * Returns the icon (or thumbnail) of the item.
     *
     * @param item Item's index.
     */
    QIcon fileIcon(const QModelIndex &item) const;
    /**
     * Adds photo t the queue for thumbing.
     */
    void queueIconMake(const QModelIndex &item);

    /**
     * Refreshes the model
     *
     * @param root Root item.
     */
    bool refresh(ImageItem *root = 0);
    /**
     * Remakes the model from 0.
     */
    void reconstruct();

    /**
     * Adds images to the model
     *
     * @param parent Parent's index.
     * @param sourcePath Self explanatory.
     * @param fileNames List of images to copy and add.
     *
     * @return Reference to the job.
     */
    QObject *addImages(const QModelIndex &parent, const QString &sourcePath, const QStringList &fileNames = QStringList());
    /**
     * Creates a gallery and adds it into the model.
     *
     * @param name Name of the gallery.
     * @param sourcePath Self explanatory.
     * @param parent Parent's index.
     * @param fileNames List of image filenames that will be added to the new gallery.
     *
     * @return Reference to the job.
     */
    QObject *createGallery(const QString &name, const QString &sourcePath, const QModelIndex &parent = QModelIndex(), const QStringList &fileNames = QStringList());

    /**
     * Removes a gallery.
     *
     * @param index Gallery's index.
     *
     * @return @c true Removal was successful.
     * @return @c false Removal was unsuccessful.
     */
    bool removeGallery(const QModelIndex &index);
    /**
     * Removes the images.
     *
     * @param indexList List of items to remove.
     *
     * @return Parent's index.
     */
    QModelIndex removeImages(const QModelIndexList &indexList);

    /**
     * Method for getting list of childs.
     * @param parent Parent of childs.
     *
     * @return List of childs.
     */
    QModelIndexList childs(const QModelIndex &parent = QModelIndex());

    /**
     * A method to find if a file with the given filename allready exists.
     * @param filename The searched filename.
     * @param root The root in where you want to search for the filename. Defaults to the main root.
     *
     * @return @c true Filename is found, and it exists.
     * @return @c false Filename isn't found.
     */
    bool checkName(const QString &filename, const QModelIndex &root = QModelIndex()) const;

    /**
     * Gets the list of all galleries in the model.
     *
     * @return List of galleries.
     */
    QStringList getGalleriesList();

    /**
     * Get the model index by name.
     *
     * @param name Name of the gallery.
     *
     * @return Index of the gallery.
     */
    QModelIndex findGallery(const QString &name);

    /**
     * Stops the copy process.
     */
    void stopCopy();

  protected:

  private:
    /**
     * Setups the model tree.
     *
     * @param path Path of the data to be parsed.
     */
    void setupModelData(const QString &path) const;
    /**
     * Parses a path and adds the items to a root.
     *
     * @param path Path of the potential items.
     * @param root Root to which to add items. If null they will be added to the main root.
     */
    void processPath(const QDir &path, ImageItem *root = 0) const;

    /**
     * A recursive method for actualy getting the gallery list.
     *
     * @param root Root item.
     *
     * @return The gallery list.
     */
    QStringList processGalleriesList(ImageItem *root = 0);

    /**
     * A recursive method for searching a gallery by name.
     *
     * @param name Name of the gallery.
     * @param parent Parent index.
     *
     * @return Index of the found gallery.
     */
    QModelIndex processGallerySearch(const QString &name, const QModelIndex &parent = QModelIndex());

    QString m_path;
    mutable GCore::GJobs::ReadJob *m_currentJob;
    //QModelIndex m_currentThumbnail;
    GCore::GJobs::CopyJob *m_currentCopyJob;
    QModelIndex m_currentCopyParent;
    bool m_delete;
    QList<QModelIndex> m_pendingUpdate;

    ImageItem *m_rootItem;

  private slots:
    /**
     * The process of the add of an image.
     *
     * @param fileName Name of the item.
     */
    void slotProcess(const QString &fileName);
    /**
     * Removes a finished read job.
     */
    void slotRemoveJob();
    /**
     * Changes the data (or just reports it?).
     *
     * @param item Item's index.
     */
    void slotChange(const QModelIndex &item);
    /**
     * Removes a finished copy job.
     */
    void slotRemoveCopyJob();
    /**
     * Refreshes the model.
     */
    void slotRefresh();

};

}

#endif
