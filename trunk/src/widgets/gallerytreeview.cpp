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

#include "gallerytreeview.h"

#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtCore/QModelIndex>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include "core/imagemodel.h"
#include "core/data.h"
#include "core/jobmanager.h"

#include "widgets/imageaddprogress.h"

using namespace GCore;

namespace GWidgets
{

GalleryTreeView::GalleryTreeView(QWidget *parent)
    : QTreeView(parent)
{
  connect(this, SIGNAL(pressed(const QModelIndex&)), this, SLOT(checkSelection(const QModelIndex&)));
}

void GalleryTreeView::contextMenuEvent(QContextMenuEvent *event)
{
  static_cast<QMenu*>(Data::self()->value(Data::GalleryContextMenu).value<QWidget*>())->exec(event->globalPos());
}

void GalleryTreeView::slotDelete()
{
  QModelIndex selectedGallery = Data::self()->galleryProxy()->mapToSource(selectedIndexes().first());

  //emit clicked(QModelIndex());

  if (QMessageBox::question(0, tr("Confirm deletion"), tr("Are you sure you want to delete %1?").arg(selectedGallery.data(ImageModel::ImageNameRole).toString()), tr("Delete"), tr("Keep"), QString(), 1, 1) == 0) {
    QString job = JobManager::self()->deleteGallery(selectedGallery);
    connect(JobManager::self()->job(job), SIGNAL(progress(int, int, const QString&, const QImage&)), Data::self()->imageAddProgress(), SLOT(setProgress(int, int, const QString &, const QImage &)));
    JobManager::self()->startJob(job);
  }
}

void GalleryTreeView::checkSelection(const QModelIndex &selected)
{
  clearFocus();
  collapseAll();

  if (selected.isValid()) {
    emit signalSelected(true);

    // Expand the whole tree up
    for (QModelIndex count = selected; count.isValid(); count = count.parent())
      expand(count);
  } else {
    emit signalSelected(false);
  }
}

GalleryTreeView::~GalleryTreeView()
{}

}
