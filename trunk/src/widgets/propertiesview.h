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
#ifndef GWIDGETSPROPERTIESVIEW_H
#define GWIDGETSPROPERTIESVIEW_H

#include <QtGui/QWidget>

#include "ui_propertiesview.h"

#include <QtCore/QModelIndex>

namespace GWidgets
{

/**
 * @short A view for showing photo properties.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class PropertiesView : public QWidget
{
    Q_OBJECT
  signals:
    void closed(const QModelIndex &index);

  public:
    /**
     * A constructor.
     */
    PropertiesView(QWidget *parent = 0);

    /**
     * A destructor.
     */
    ~PropertiesView();

  protected:
    /**
     * Reimplemented method for implementing "open image in external viewer".
     */
    void mouseReleaseEvent(QMouseEvent *event);
    
  public slots:
    void setCurrentIndex(const QModelIndex &index);

  private:
    Ui::PropertiesView ui;
    QModelIndex m_currentIndex;
    QString m_oldName;
    QString m_oldDescription;
    short m_rotation;

    void updateData();
    void updateNavigation();

  private slots:
    void next();
    void previous();
    void close();
    void save();
    void enableSave();
    void rotateCW();
    void rotateCCW();

};

}

#endif
