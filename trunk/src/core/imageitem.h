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
 @author Gregor Kalisnik <gregor@podnapisi.net>
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

    void appendChild(ImageItem *child);
    void removeChild(ImageItem *child);

    ImageItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
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

    MetaDataManager *metadata();

    QString name();
    QString description();

    bool setName(const QString &name);
    void setDescription(const QString &description);

    bool remove();

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
