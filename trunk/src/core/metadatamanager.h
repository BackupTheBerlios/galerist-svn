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
#ifndef GCOREMETADATAMANAGER_H
#define GCOREMETADATAMANAGER_H

#include <QtCore/QObject>

#include <QtSql>

namespace GCore
{

class ImageItem;

/**
 * @short A manager for the gallerys metadata.
 * @author Gregor Kališnik.
 */
class MetaDataManager : public QObject
{
    Q_OBJECT

  public:
    /**
     * A constructor.
     *
     * @param galleryPath Self explanatory.
     * @param parent Parent of this manager.
     */
    MetaDataManager(const QString &galleryPath, QObject *parent = 0);

    /**
     * Adds image to the metadata.
     *
     * @return ID of the new image.
     * @return -1 Image couldn't be added.
     *
     * @param filename Filename of the image.
     */
    int addImage(const QString &filename) const;

    /**
     * Returns the metadata id of the image.
     *
     * @param filename Filename of the image.
     */
    int imageId(const QString &filename) const;

    /**
     * Returns the logical name of the image.
     *
     * @param id ID of the image.
     */
    QString name(int id) const;
    /**
     * Sets the image's logical name.
     *
     * @param name The new name.
     * @param id Image's id.
     *
     * @return @c true The change was successful.
     * @return @c false The change was unsuccessful.
     */
    bool setName(const QString &name, int id) const;
    /**
     * Checks if the name exists.
     *
     * @param name Name to check.
     */
    bool checkName(const QString &name) const;

    /**
     * Returns the image's description.
     *
     * @param id Image's ID.
     */
    QString description(int id) const;
    /**
     * Sets image's description.
     *
     * @param description The new description.
     * @param id Image's ID.
     */
    void setDescription(const QString &description, int id) const;

    /**
     * Returns the full metadata info of the image.
     *
     * @param id Image's ID.
     */
    QMap<QString, QString> metadataInfo(int id) const;

    /**
     * Removes the image from the metadata.
     *
     * @param id Image's ID.
     *
     * @return @c true Removal was successful.
     * @return @c false Removal was unsuccessful.
     */
    bool removePicture(int id) const;

    /**
     * Executes a query and returns the result.
     *
     * @param rawQuery The SQL query.
     */
    QVariant query(const QString &rawQuery) const;

    /**
     * Checks if the needed SQL driver is available.
     */
    static bool driverAvailable();

    /**
     * A destructor.
     */
    ~MetaDataManager();

  private:
    QSqlDatabase m_metadataFile;
    QString m_galleryName;

};

}

#endif
