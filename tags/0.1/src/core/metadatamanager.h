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

class MetaDataManager : public QObject
{
    Q_OBJECT
  signals:
    void signalFailed(const QString &errorMessage, int type);
    void signalInsert();

  public:
    MetaDataManager(const QString &galleryPath, QObject *parent = 0);

    void addImage(const QString &filename);

    int imageId(const QString &filename);

    QString name(int id);
    bool setName(const QString &name, int id);

    QString description(int id);
    void setDescription(const QString &description, int id);

    QMap<QString, QString> metadataInfo(int id);

    bool checkName(const QString &name);

    bool removePicture(int id);

    QVariant query(const QString &rawQuery);

    static bool driverAvailable();

    ~MetaDataManager();

  protected:
    void timerEvent(QTimerEvent*);

  private:
    QSqlDatabase m_metadataFile;
    QString m_galleryName;

    QStringList m_pendingInsert;
    QStringList m_pendingUpdateName;
    QStringList m_pendingUpdateDescription;
    int m_timerId;

  private slots:
    void slotInsert();

};

}

#endif
