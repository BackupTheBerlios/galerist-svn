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
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class ImageModel : public QAbstractItemModel
{
    Q_OBJECT
  signals:
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

    ImageModel(const QString &path, QObject *parent = 0);
    ~ImageModel();

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void setName(const QString &name, const QModelIndex &index);
    void setDescription(const QString &description, const QModelIndex &index);

    bool exists(const QString &name, const QModelIndex &index);

    QIcon fileIcon(const QModelIndex &item) const;
    /**
     * Adds photo t the queue for thumbing.
     */
    void queueIconMake(const QModelIndex &item);

    bool refresh(ImageItem *root = 0);
    void reconstruct();

    QObject *addImages(const QModelIndex &parent, const QString &sourcePath, const QStringList &fileNames = QStringList());
    QObject *createGallery(const QString &name, const QStringList &fileNames, const QString &sourcePath, const QModelIndex &parent = QModelIndex());

    bool removeGallery(const QModelIndex &index);
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

    void stopCopy();

  protected:
    void timerEvent(QTimerEvent*);

  private:
    void setupModelData(const QString &path) const;
    void processPath(const QDir &path, ImageItem *root = 0) const;

    QString m_path;
    mutable GCore::GJobs::ReadJob *m_currentJob;
    //QModelIndex m_currentThumbnail;
    GCore::GJobs::CopyJob *m_currentCopyJob;
    QModelIndex m_currentCopyParent;
    bool m_delete;
    QList<QModelIndex> m_pendingUpdate;

    ImageItem *m_rootItem;

  private slots:
    void slotProcess(const QString &fileName);
    void slotRemoveJob();
    void slotChange(const QModelIndex &item);
    void slotRemoveCopyJob();
    void slotRefresh();

};

}

#endif
