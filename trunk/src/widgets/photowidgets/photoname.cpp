/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÅ¡nik                                 *
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

#include "photoname.h"

#include <QtCore/QPointF>

#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>

#include "core/imagemodel.h"

#include "widgets/photoview.h"
#include "widgets/lineedit.h"

#include "widgets/photowidgets/photoitem.h"

namespace GWidgets
{

namespace GPhotoWidgets
{

PhotoName::PhotoName(QGraphicsItem *parent, QGraphicsScene *scene, QGraphicsView *view)
    : QObject(0),
    QGraphicsSimpleTextItem(parent, scene),
    m_view(view),
    m_editing(false)
{
  m_editor = new LineEdit(view, LineEdit::WithVerify);
  m_editor->hide();
  m_editor->setMinimumWidth(PhotoItem::ItemWidth - 30);
  m_editor->setMaximumWidth(PhotoItem::ItemWidth - 30);

  connect(m_editor, SIGNAL(textChanged(const QString&)), this, SLOT(slotCheck(const QString&)));
  connect(m_editor, SIGNAL(editingFinished(const QString&)), this, SLOT(slotHide(const QString&)));
  connect(m_editor, SIGNAL(editingCanceled()), this, SLOT(slotCancel()));
}

void PhotoName::setText(const QString &text)
{
  bool truncate = false;
  QString name = text;
  int length = name.length();
  int width = length * 6;

  do {
    length = name.length();
    width = length * 6;

    if (width >= ((PhotoItem::ItemWidth - 30) - (6 * 3))) {
      name.chop(3);
      truncate = true;
    }
  } while (width >= ((PhotoItem::ItemWidth - 30) - (6 * 3)));

  if (truncate) {
    name.truncate(length);
    name += "...";
  }

  setPos(((PhotoItem::ItemWidth - 30) / 2) - (width / 2), 140);

  m_text = text;
  m_layout.setText(name);
  m_layout.beginLayout();
  QTextLine line = m_layout.createLine();
  line.setLineWidth(PhotoItem::ItemWidth - 30);
  m_layout.endLayout();

  m_editor->setText(text);

  QGraphicsSimpleTextItem::setText(name);
}

void PhotoName::setEdit()
{
  m_editing = true;
  m_editor->move(QPoint(m_view->mapFromScene(mapToScene(mapFromParent(15, 140)))));
  m_editor->show();
  m_editor->setFocus();
  slotCheck(m_text);
}

void PhotoName::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  painter->save();
  painter->setPen(QPen());
  painter->setFont(QFont());

  m_layout.draw(painter, QPointF(0, 0));

  painter->restore();
}

void PhotoName::mousePressEvent(QGraphicsSceneMouseEvent*)
{
  setEdit();
  m_view->scene()->clearSelection();
  parentItem()->setSelected(true);
}

void PhotoName::slotCheck(const QString &name)
{
  if (!m_editing)
    return;

  PhotoView *view = static_cast<PhotoView*>(m_view);
  if (name.isEmpty())
    m_editor->setValidity(false, tr("Enter a name."));
  else if (static_cast<GCore::ImageModel*>(view->model())->exists(name, view->rootIndex()))
    if (name != m_text)
      m_editor->setValidity(false, tr("Picture with this name allready exists!"));
    else
      m_editor->setValidity(false, tr("Please change the name in order to be a valid change!"));
  else
    m_editor->setValidity(true);
}

void PhotoName::slotHide(const QString &name)
{
  m_editor->hide();

  if (m_editor->isValid()) {
    emit editingFinished(name);
    setText(name);
  } else {
    m_editor->setText(m_text);
  }

  m_editing = false;
}

void PhotoName::slotCancel()
{
  m_editing = false;
  m_editor->hide();
  m_editor->setText(m_text);
}

PhotoName::~PhotoName()
{
  delete m_editor;
}

}

}
