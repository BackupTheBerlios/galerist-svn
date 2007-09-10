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
#ifndef GCOREIMAGEMODEL_H
#define GCOREIMAGEMODEL_H

#include <QtCore/QAbstractItemModel>

#include <QtGui/QIcon>

#include "core/imageitem.h"

class QDir;

namespace GCore
{

namespace GJobs
{
class ReadJob;
class CopyJob;
}

/**
 * @short The model with images and all of its data.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class ImageModel : public QAbstractItemModel
{
    Q_OBJECT
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
      /** Image's filename (example.jpg). */
      ImageFilenameRole      = Qt::UserRole + 3,
      /** Image's path with filename (/path/to/image.jpg). */
      ImageFilepathRole      = Qt::UserRole + 4,
      RelativeFilePathRole   = Qt::UserRole + 5,
      /** Image's directory path (/path/to). */
      ImagePathRole          = Qt::UserRole + 6,
      RelativePathRole           = Qt::UserRole + 7,
      /** Path to image's thumbnail. */
      ImageThumbnailPathRole = Qt::UserRole + 8,
      RelativeThumbnailPathRole = Qt::UserRole + 9,
      /** Thumbnails path. */
      ThumbnailsPathRole     = Qt::UserRole + 10,
      RelativeThumbnailsPathRole     = Qt::UserRole + 11,
      /** Returns the actual image. */
      ImagePictureRole       = Qt::UserRole + 12,
      ImageRotateCW          = Qt::UserRole + 13,
      ImageRotateCCW         = Qt::UserRole + 14,
      IdRole                 = Qt::UserRole + 15
  };

    /**
     * A constructor.
     *
     * @param path Path of the galleries.
     * @param parent Parent of this model.
     */
    ImageModel(QObject *parent = 0);
    /**
     * A destructor.
     */
    ~ImageModel();

    void showGalleries(bool set);

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
     * Gets the list of indexes
     */
    QModelIndexList indexList(const QList<int> &ids, ImageItem::Type type) const;

    QModelIndexList indexList(ImageItem::Type type, const QModelIndex &parent = QModelIndex()) const;

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
     * Remakes the model from 0.
     */
    void reconstruct();

    /**
     * Method for getting list of childs.
     * @param parent Parent of childs.
     *
     * @return List of childs.
     */
    QModelIndexList childs(const QModelIndex &parent = QModelIndex());

    /**
     * Get the model index by name.
     *
     * @param name Name of the gallery.
     *
     * @return Index of the gallery.
     */
    QModelIndex findGallery(const QString &name);

  public slots:
  /**
   * The process of the add of an image.
   *
   * @param fileName Name of the item.
   */
    void addItem(ImageItem *item);

    void removeItem(const QModelIndex &index);

  protected:

  private:
    bool m_delete;
    bool m_showGalleries;
    mutable QMap<QString,QString> m_progressRead;

    ImageItem *m_rootItem;

    /**
     * Setups the model tree.
     *
     * @param path Path of the data to be parsed.
     */
    void setupModelData();

    QModelIndex index(int itemId, ImageItem::Type type) const;
    QModelIndex index(ImageItem *item) const;
    ImageItem *itemForId(int itemId, ImageItem::Type type) const;

  private slots:
    /**
     * Changes the data (or just reports it?).
     *
     * @param item Item's index.
     */
    void processThumbnail(const QString &fileName, const QImage&, int parentId);

};

}

#endif
