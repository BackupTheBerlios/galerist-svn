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
#ifndef GCOREMETADATAMANAGER_H
#define GCOREMETADATAMANAGER_H


#include <QtCore/QObject>
#include <QtSql>
#include <QtCore/QMap>
#include <QtCore/QTimer>

namespace GCore
{

class ImageItem;

/**
 * @short A manager of a gallery's metadata.
 * @author Gregor Kališnik.
 */
class MetaDataManager : public QObject
{
  Q_OBJECT
  signals:
    /**
     * Insertion has just happened.
     */
    void signalInsert();

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
     * @param filename Filename of the image.
     */
    void addImage(const QString &filename);

    /**
     * Returns the metadata id of the image.
     *
     * @param filename Filename of the image.
     */
    int imageId(const QString &filename);

    /**
     * Returns the logical name of the image.
     *
     * @param id ID of the image.
     */
    QString name(int id);
    /**
     * Sets the image's logical name.
     *
     * @param name The new name.
     * @param id Image's id.
     *
     * @return @c true The change was successful.
     * @return @c false The change was unsuccessful.
     */
    bool setName(const QString &name, int id);

    /**
     * Returns the image's description.
     *
     * @param id Image's ID.
     */
    QString description(int id);
    /**
     * Sets image's description.
     *
     * @param description The new description.
     * @param id Image's ID.
     */
    void setDescription(const QString &description, int id);

    /**
     * Returns the full metadata info of the image.
     *
     * @param id Image's ID.
     */
    QMap<QString, QString> metadataInfo(int id);

    /**
     * Checks if the name exists.
     *
     * @param name Name to check.
     */
    bool checkName(const QString &name);

    /**
     * Removes the image from the metadata.
     *
     * @param id Image's ID.
     *
     * @return @c true Removal was successful.
     * @return @c false Removal was unsuccessful.
     */
    bool removePicture(int id);

    /**
     * Executes a query and returns the result.
     *
     * @param rawQuery The SQL query.
     */
    QVariant query(const QString &rawQuery);

    /**
     * Checks if the needed SQL driver is available.
     */
    static bool driverAvailable();

    ~MetaDataManager();

  protected:
    /**
     * Reimplemented method for preventing the flood of the insertion.
     */
    void timerEvent(QTimerEvent*);

  private:
    QSqlDatabase m_metadataFile;
    QString m_galleryName;

    QStringList m_pendingInsert;
    QStringList m_pendingUpdateName;
    QStringList m_pendingUpdateDescription;
    int m_timerId;

  private slots:
    /**
     * Inserts the images that are in the list.
     */
    void slotInsert();

};

}

#endif
