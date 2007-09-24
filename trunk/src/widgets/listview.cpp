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
#include <QtCore/QUrl>

#include <QtGui/QApplication>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QPainter>

#include "core/imagemodel.h"
#include "core/imageitem.h"
#include "core/data.h"
#include "core/jobmanager.h"

#include "widgets/imagedelegate.h"

#include "dialogs/newgallerywizard.h"

#include <QtDebug>

using namespace GCore;
using namespace GDialogs;

namespace GWidgets {

ListView::ListView(QWidget *parent)
  : QListView(parent)
{
  viewport()->setAttribute(Qt::WA_Hover, true);

  setItemDelegate(new ImageDelegate(this));
}

ListView::~ListView()
{}

void ListView::contextMenuEvent(QContextMenuEvent *event)
{
  Data::self()->listContextMenu()->exec(event->globalPos());
}

void ListView::slotRemove()
{
  if (QMessageBox::question(0, tr("Confirm remove"), tr("Are you sure you want to remove %1 picture/s?").arg(selectedIndexes().count()), tr("Remove"), tr("Keep"), QString(), 1, 1) == 0)
    JobManager::self()->deleteImages(selectedIndexes());
}

void ListView::setRootIndex(const QModelIndex &index)
{
  if (index.model() == Data::self()->galleryProxy())
    QListView::setRootIndex(Data::self()->galleryProxy()->mapToSource(index));
  else
    QListView::setRootIndex(index);
}

void ListView::invertSelection()
{
  QModelIndexList cachedIndexes = selectedIndexes();

  selectAll();

  foreach (QModelIndex index, cachedIndexes)
    setSelection(visualRect(index), QItemSelectionModel::Deselect);
}

void ListView::paintEvent(QPaintEvent *event)
{
  QModelIndexList indexes = Data::self()->imageModel()->childs(rootIndex());
  QRect renderingRect = event->rect().translated(horizontalOffset(), verticalOffset());
  QPainter painter(viewport());

  QStyleOptionViewItem option = viewOptions();
  QStyle::State defaultState = option.state;

  foreach (QModelIndex index, indexes) {
    if (!rectForIndex(index).intersects(renderingRect))
      continue;

    option.rect = visualRect(index);
    option.state = defaultState;

    if (selectionModel()->isSelected(index)) {
      option.state |= QStyle::State_Selected;
    }

    if (index == m_hoverItem)
      option.state |= QStyle::State_MouseOver;
    else
      option.state &= ~QStyle::State_MouseOver;

    painter.save();
    itemDelegate()->paint(&painter, option, index);
    painter.restore();
  }

  if (m_rubberBandRect.isValid()) {
    QStyleOptionRubberBand opt;
    opt.initFrom(this);
    opt.shape = QRubberBand::Rectangle;
    opt.opaque = false;
    opt.rect = m_rubberBandRect;
    painter.save();
    style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
    painter.restore();
  }
}

bool ListView::viewportEvent(QEvent *event)
{
  if (event->type() == QEvent::HoverEnter || event->type() == QEvent::HoverMove) {
    QHoverEvent *hoverEvent = static_cast<QHoverEvent*>(event);
    m_hoverItem = indexAt(hoverEvent->pos());
  } else if (event->type() == QEvent::HoverLeave) {
    m_hoverItem = QModelIndex();
  }

  return QListView::viewportEvent(event);
}

void ListView::mousePressEvent(QMouseEvent *event)
{
  if (!(event->modifiers() & Qt::ControlModifier) && !(event->modifiers() & Qt::ShiftModifier) && !indexAt(event->pos()).isValid() && event->button() != Qt::RightButton)
    clearSelection();

  m_originPos = event->pos();
  m_rubberBandRect = QRect(m_originPos, QSize());

  QListView::mousePressEvent(event);
}

void ListView::mouseMoveEvent(QMouseEvent *event)
{
  m_rubberBandRect = QRect(m_originPos - QPoint(0, verticalOffset()), event->pos()).normalized();

  QListView::mouseMoveEvent(event);
}

void ListView::mouseReleaseEvent(QMouseEvent *event)
{
  m_rubberBandRect = QRect();
  m_originPos = QPoint();

  QListView::mouseReleaseEvent(event);
}

void ListView::dragEnterEvent(QDragEnterEvent *event)
{
  QList<QUrl> imageUrls = event->mimeData()->urls();
  QList<QUrl>::const_iterator end = imageUrls.constEnd();

  bool haveImages = false;

  for (QList<QUrl>::const_iterator count = imageUrls.constBegin(); count != end; count++) {
    QString path = (*count).toLocalFile();
    if (path.contains(GCore::Data::self()->supportedFormats())) {
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
    if (!path.isEmpty() && path.contains(GCore::Data::self()->supportedFormats()))
      imagePaths << path;
  }

  QString image = imagePaths.first();
  image.remove(QRegExp("^.+/"));
  QString path = imagePaths.first();
  path.remove(image);

  QStringList pictures = imagePaths;
  pictures.replaceInStrings(path, QString());

  QAction *choice = Data::self()->dropContextMenu()->exec(mapToGlobal(event->pos()));

  if (!choice)
    return;

  if (choice->data().toInt() == 0) {
    NewGalleryWizard *wizard = new NewGalleryWizard(path, pictures, this);
    wizard->show();
  } else if (choice->data().toInt() == 1) {
    Job job = JobManager::self()->addImages(rootIndex(), imagePaths);
    connect(job.jobPtr(), SIGNAL(progress(int, int, const QString&, const QImage&)), GCore::Data::self()->imageAddProgress(), SLOT(setProgress(int, int, const QString&, const QImage&)));
  }

  event->acceptProposedAction();
}

void ListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
  QModelIndexList cachedIndexes = selectedIndexes();
  if (cachedIndexes.isEmpty()) {
    emit signalSelected(false);
    emit signalOneSelected(false);
  } else {
    if (cachedIndexes.count() == 1)
      emit signalOneSelected(true);
    else
      emit signalOneSelected(false);

    emit signalSelected(true);
  }

  QListView::selectionChanged(selected, deselected);
}

}
