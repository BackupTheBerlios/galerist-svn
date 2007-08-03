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

#include <QtCore/QUrl>

#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMenu>

#include "core/data.h"
#include "core/imageitem.h"
#include "core/imagemodel.h"

#include "dialogs/newgallerywizard.h"

using namespace GCore;
using namespace GDialogs;

namespace GWidgets
{

ListView::ListView(QWidget *parent)
    : QListView(parent)
{}

ListView::~ListView()
{}

void ListView::dragEnterEvent(QDragEnterEvent *event)
{
  QList<QUrl> imageUrls = event->mimeData()->urls();
  QList<QUrl>::const_iterator end = imageUrls.constEnd();

  bool haveImages = false;

  for (QList<QUrl>::const_iterator count = imageUrls.constBegin(); count != end; count++) {
    QString path = (*count).toLocalFile();
    if (path.contains(Data::self()->value(Data::SupportedFormats).toRegExp())) {
      haveImages = true;
      break;
    }
  }

  if (haveImages)
    event->acceptProposedAction();
}

void ListView::dragMoveEvent(QDragMoveEvent *event)
{
  event->acceptProposedAction();
}

void ListView::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

void ListView::dropEvent(QDropEvent *event)
{
  const QMimeData *mimeData = event->mimeData();

  QStringList imagePaths;
  QList<QUrl> imageUrls = mimeData->urls();
  QList<QUrl>::const_iterator end = imageUrls.constEnd();

  for (QList<QUrl>::const_iterator count = imageUrls.constBegin(); count != end; count++) {
    QString path = (*count).toLocalFile();
    if (!path.isEmpty() && path.contains(Data::self()->value(Data::SupportedFormats).toRegExp()))
      imagePaths << path;
  }

  QString image = imagePaths.first();
  image.remove(QRegExp("^.+/"));
  QString path = imagePaths.first();
  path.remove(image);

  QStringList pictures = imagePaths;
  pictures.replaceInStrings(path, QString());

  QMenu dropdownMenu(tr("Dropped images"), this);
  QAction *newDestination = dropdownMenu.addAction(QIcon(":/images/new.png"), tr("New gallery"));
  newDestination->setStatusTip(tr("Create a new gallery with the dropped in images."));
  newDestination->setParent(Data::self()->value(Data::MainWindow).value<QWidget*>());
  QAction *existing = dropdownMenu.addAction(QIcon(":/images/add-existing.png"), tr("Existing gallery"));
  existing->setStatusTip(tr("Insert the dropped in images into the selected gallery."));
  existing->setParent(Data::self()->value(Data::MainWindow).value<QWidget*>());

  if (rootIndex().data(ImageModel::ImageTypeRole).toInt() != ImageItem::Gallery)
    existing->setEnabled(false);

  QAction *choice = dropdownMenu.exec(mapToGlobal(event->pos()));

  if (choice == newDestination) {
    NewGalleryWizard *wizard = new NewGalleryWizard(path, pictures, this);
    wizard->show();
  } else if (choice == existing) {
    connect(static_cast<ImageModel*>(Data::self()->value(Data::ImageModel).value<QObject*>())->addImages(rootIndex(), path, pictures), SIGNAL(signalProgress(int, int, const QString&, const QImage&)), Data::self()->value(Data::ImageAddProgress).value<QObject*>(), SLOT(setProgress(int, int, const QString&, const QImage&)));
  }

  event->acceptProposedAction();
}


}
