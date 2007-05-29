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

#include "photodescription.h"

#include <QtCore/QPointF>

#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QtGui/QTextDocument>
#include <QtGui/QPainter>

#include "core/imagemodel.h"

#include "widgets/photoview.h"
#include "widgets/textedit.h"

#include "widgets/photowidgets/photoitem.h"

namespace GWidgets
{

namespace GPhotoWidgets
{

PhotoDescription::PhotoDescription(QGraphicsItem *parent, QGraphicsScene *scene, QGraphicsView *view)
    : QGraphicsTextItem(parent, scene),
    m_view(view)
{
  m_editor = new TextEdit(view);
  m_editor->hide();
  m_editor->setMinimumWidth(PhotoItem::ItemWidth - 30);
  m_editor->setMaximumWidth(PhotoItem::ItemWidth - 30);
  m_editor->setMaximumHeight(PhotoItem::ItemHeight - 20);

  setTextWidth(PhotoItem::ItemWidth - 30);

  connect(m_editor, SIGNAL(editingFinished(const QString&)), this, SLOT(slotHide(const QString&)));
  connect(m_editor, SIGNAL(editingCanceled()), this, SLOT(slotCancelEditing()));
}

void PhotoDescription::setText(const QString &text)
{
  m_text = text;
  m_text.remove("<i>").remove("</i>");

  m_editor->setPlainText(m_text);

  QGraphicsTextItem::setHtml("<i>" + QString(m_text).replace('\n', "<br>") + "</i>");
}

void PhotoDescription::setEdit()
{
  m_editor->move(QPoint(m_view->mapFromScene(mapToScene(mapFromParent(15, 160)))));
  m_editor->show();
  m_editor->setFocus();
}

void PhotoDescription::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  painter->save();
  document()->drawContents(painter, QRectF(0, 0, PhotoItem::ItemWidth - 30, 30));
  painter->restore();
}

void PhotoDescription::mousePressEvent(QGraphicsSceneMouseEvent*)
{
  setEdit();
  m_view->scene()->clearSelection();
  parentItem()->setSelected(true);
}

void PhotoDescription::slotHide(const QString &description)
{
  m_editor->hide();
  emit editingFinished(description);
  setText(description);
}

void PhotoDescription::slotCancelEditing()
{
  m_editor->hide();
  m_editor->setPlainText(m_text);
}

PhotoDescription::~PhotoDescription()
{
  delete m_editor;
}

}

}
