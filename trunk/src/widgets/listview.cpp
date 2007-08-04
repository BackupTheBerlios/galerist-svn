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

#include "listview.h"

#include <QtCore/QModelIndex>

#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtGui/QSortFilterProxyModel>

#include <QtDebug>

#include "core/imagemodel.h"
#include "core/data.h"

#include "widgets/imagedelegate.h"

using namespace GCore;

namespace GWidgets {

ListView::ListView(QWidget *parent)
  : QListView(parent)
{
  viewport()->setAttribute(Qt::WA_Hover, true);

  setItemDelegate(new ImageDelegate(this));

  connect(this, SIGNAL(pressed(const QModelIndex&)), this, SLOT(slotSelect(const QModelIndex&)));
}

void ListView::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu;

  QAction *properties = menu.addAction(tr("Properties"));
  QAction *remove = menu.addAction(tr("Remove picture"), this, SLOT(slotRemove()));

  if (selectedIndexes().isEmpty()) {
    properties->setEnabled(false);
    remove->setEnabled(false);
  }

  menu.exec(event->globalPos());
}

void ListView::slotSelect(const QModelIndex &selectedIndex)
{
  QModelIndexList imagesList = selectedIndexes();
  QModelIndexList::const_iterator finish = imagesList.constEnd();

  for (QModelIndexList::const_iterator count = imagesList.constBegin(); count != finish; count++) {
    if (*count == selectedIndex)
      return;
  }

  clearSelection();

  setCurrentIndex(selectedIndex);
}

void ListView::slotRemove()
{
  if (QMessageBox::question(0, tr("Confirm remove"), tr("Are you sure you want to remove %1 picture/s?").arg(selectedIndexes().count()), tr("Remove"), tr("Keep"), QString(), 1, 1) == 0)
    setRootIndex(static_cast<GCore::ImageModel*> (model())->removeImages(selectedIndexes()));
}

void ListView::setRootIndex(const QModelIndex &index)
{
  QListView::setRootIndex(Data::self()->galleryProxy()->mapToSource(index));
}

void ListView::mouseReleaseEvent(QMouseEvent *event)
{
  if (selectedIndexes().isEmpty())
    emit signalSelected(false);
  else
    emit signalSelected(true);

  QListView::mouseReleaseEvent(event);
}

ListView::~ListView()
{}

}
