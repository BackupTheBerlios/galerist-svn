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
#ifndef GCOREIMAGEITEM_H
#define GCOREIMAGEITEM_H

#include <QtCore/QList>
#include <QtCore/QVariant>

class QString;

namespace GCore
{

class MetaDataManager;

/**
 * @short Class that represends a tree of images.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class ImageItem
{
  public:
    enum Type {
      Root,
      Gallery,
      Image
  };

    /**
     * Class constructor.
     *
     * @param path The location of this image item relative to it's parent
     * @param parent Optional pointer to the parent item
     * @param type Optional image item type
     */
    ImageItem(const QString &path, ImageItem *parent = 0, Type type = Image);

    /**
     * Class destructor.
     */
    ~ImageItem();

    /**
     * Appends a child.
     *
     * @param child The new child.
     */
    void appendChild(ImageItem *child);
    /**
     * Removes a child.
     *
     * @param child Child reference.
     */
    void removeChild(ImageItem *child);

    /**
     * Gets the child at the row.
     *
     * @param row Row where the child resides.
     *
     * @return A child.
     */
    ImageItem *child(int row);
    /**
     * Returns the number of childs that resides in this item.
     *
     * @return Number of childs.
     */
    int childCount() const;
    /**
     * Number of columns (the different data).
     *
     * @return Number of columns.
     */
    int columnCount() const;
    /**
     * Gets the data at the column.
     *
     * @param column Column of which the data we need.
     *
     * @return The data.
     */
    QVariant data(int column) const;
    /**
     * The row number of this item at parent's child list.
     *
     * @return Row.
     */
    int row() const;
    /**
     * Returns the reference of item's parent.
     *
     * @return The reference.
     */
    ImageItem *parent();

    /**
     * Additional methods, for gathering additional information.
     */
    Type imageType();

    /**
     * Change the file path.
     *
     * @param path New file path
     */
    void setFilePath(const QString &path);

    /**
     * Get the full file path.
     *
     * @return A full path comprised of this item's path and the parent's path
     */
    QString getFilePath();

    /**
     * Get only the filename associated with this item.
     *
     * @return Item's filename
     */
    QString getFileName();

    /**
     * Get the thumbnail name (diference betwen fileName and thumbName is in the extension!).
     *
     * @return Name of the thumbnail image
     */
    QString getThumbName();

    /**
     * Returns the MetaData manager.
     *
     * @return Reference to the MetaData manager.
     */
    MetaDataManager *metadata();

    /**
     * Returns item's name.
     */
    QString name();
    /**
     * Returns item's description.
     */
    QString description();

    /**
     * Sets the item's name.
     *
     * @return @c true Change was successful.
     * @return @c false Change was unsuccesful.
     */
    bool setName(const QString &name);
    /**
     * Sets the item's description.
     *
     * @return @c true Change was successful.
     * @return @c false Change was unsuccesful.
     */
    void setDescription(const QString &description);

    /**
     * Removes the item from the metadata.
     *
     * @return @c true Removal was successful.
     * @return @c false Removal was unsuccesful.
     */
    bool remove();

    /**
     * Rotates for 90 degrees into clock-wise direction.
     */
    void rotateCW();

    /**
     * Rotates for 90 degrees into counter clock-wise direction.
     */
    void rotateCCW();

  private:
    QList<ImageItem*> m_childItems;
    QList<QVariant> m_itemData;

    // Additional information
    ImageItem *m_parentItem;
    Type m_type;
    QString m_path;
    QString m_fileName;
    MetaDataManager *m_metadata;
    int m_id;

};

}

#endif
