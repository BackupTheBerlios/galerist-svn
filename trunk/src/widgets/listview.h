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
#ifndef GWIDGETSLISTVIEW_H_
#define GWIDGETSLISTVIEW_H_

#include <QtGui/QListView>

class QContextMenuEvent;
class QAction;
class QEvent;
class QMouseEvent;

namespace GWidgets
{
class ListView : public QListView
{
    Q_OBJECT
  signals:
    void signalSelected(bool selected);

  public:
    ListView(QWidget *parent);

    ~ListView();

  public slots:
    void slotRemove();
    void setRootIndex(const QModelIndex &index);

  protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

  private:
};

}

#endif
