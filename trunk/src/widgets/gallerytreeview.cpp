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

#include "gallerytreeview.h"

#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtCore/QModelIndex>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>

#include <QtCore/QtDebug>

#include "core/imagemodel.h"
#include "core/data.h"

namespace GWidgets
{

GalleryTreeView::GalleryTreeView(QWidget *parent)
    : QTreeView(parent)
{
  connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotCheckSelection(const QModelIndex&)));
}

void GalleryTreeView::contextMenuEvent(QContextMenuEvent *event)
{
  GCore::Data::self()->getGalleryContextMenu()->exec(event->globalPos());
}

void GalleryTreeView::slotDelete()
{
  QModelIndex selectedGallery = selectedIndexes().first();

  emit clicked(QModelIndex());

  // @TODO need to change this?
  if (QMessageBox::question(0, tr("Confirm deletion"), tr("Are you sure you want to delete %1?").arg(selectedGallery.data().toString()), tr("Delete"), tr("Keep"), QString(), 1, 1) == 0)
    static_cast<GCore::ImageModel*>(model())->removeGallery(selectedGallery);
}

void GalleryTreeView::slotCheckSelection(const QModelIndex &selected)
{
  clearFocus();
  if (selected.isValid()) {
    emit signalSelected(true);
  } else
    emit signalSelected(false);
}

GalleryTreeView::~GalleryTreeView()
{}

}
