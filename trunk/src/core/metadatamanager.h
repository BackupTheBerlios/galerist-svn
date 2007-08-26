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
#include <QtCore/QMutex>

//#include <QtSql>

namespace GCore
{

class ImageItem;

/**
 * @short A manager for the manifest file.
 * @author Gregor Kališnik.
 */
class MetaDataManager : public QObject
{
    Q_OBJECT

  public:
    /**
     * A constructor.
     */
    MetaDataManager();

    /**
     * Checks if the needed SQL driver is available.
     */
    static bool driverAvailable();

    static MetaDataManager *self();

    ImageItem *registerGallery(const QString &name, int parentId = -1);
    ImageItem *registerImage(const QString &name, int galleryId);

    void unregisterGallery(int id, int parentId);
    void unregisterImage(int id, int galleryId);

    int galleryId(const QString &name, int parentId = -1);
    int imageId(const QString &name, int parentId);

    QString fileName(int id) const;

    QString galleryName(int id) const;
    QString imageName(int id) const;

    QStringList imageList(int galleryId) const;
    QStringList galleryList() const;

    bool setGalleryName(int id, const QString &name, const QString &path) const;
    void setImageName(int id, const QString &name) const;

    QString imageDescription(int id) const;

    void setDescription(int id, const QString &description) const;

    bool galleryExists(const QString &fileName, int parentId = -1);
    bool galleryExists(int galleryId);
    bool imageExists(const QString &name, int galleryId);

    ImageItem *readManifest();

  private:
    static MetaDataManager *m_self;

    inline void imageList(ImageItem *gallery) const;

    mutable QMutex m_locker;

};

}

#endif
