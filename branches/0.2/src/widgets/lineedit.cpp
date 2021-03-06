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

#include "lineedit.h"

#include <QtCore/QPoint>
#include <QtCore/QDir>

#include <QtGui/QPalette>
#include <QtGui/QColor>
#include <QtGui/QToolTip>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QCompleter>


#include "core/data.h"

#include "widgets/tooltip.h"

namespace GWidgets
{

LineEdit::LineEdit(QWidget *parent, Types type)
    : QLineEdit(parent),
    m_valid(false),
    m_canceling(false),
    m_type(type)
{
  // Wrapper slot, so we emit the data with it. No need subclassing keyPressEvent() method
  connect(this, SIGNAL(editingFinished()), this, SLOT(slotEmit()));

  // Setup the type
  setType(m_type);
}

void LineEdit::paintEvent(QPaintEvent *event)
{
  QLineEdit::paintEvent(event);
  int spacing = 3;

  // We don't activate the verification function if the lineedit is a default one
  if (m_type == Default)
    return;

  QPainter painter(this);

  QImage image;
  if (!m_valid)
    image = QImage(":/images/input_wrong.png");
  else
    image = QImage(":/images/input_ok.png");

  painter.drawImage(QRect(width() - (image.width() + spacing), (height() / 2) - (image.height() / 2), image.width(), image.height()), image);
}

void LineEdit::setValidity(bool valid, const QString &reason, bool firstRun)
{
  m_valid = valid;

  if (!firstRun)
    ToolTip::showMessage(reason, this, mapToGlobal(QPoint(0, 0)));
}

bool LineEdit::isValid()
{
  return m_valid;
}

void LineEdit::setType(Types type)
{
  m_type = type;

  if (m_type == FileSelector || m_type == WithVerify) {
    setStyleSheet(QString(":enabled { padding-right: %1; }").arg(18));
  }

  // If this lineedit is a file selector, then we activate the translation function.
  if (m_type == FileSelector) {
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTranslate()));
    setCompleter(GCore::Data::self()->getDirCompleter());
  } else {
    disconnect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTranslate()));
    setCompleter(0);
  }
}

void LineEdit::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Escape) {
    m_canceling = true;
    emit editingCanceled();
    m_canceling = false;
    event->ignore();
    return;
  }

  QLineEdit::keyPressEvent(event);
}

void LineEdit::hideEvent(QHideEvent *event)
{
  ToolTip::showMessage(QString(), this);

  QLineEdit::hideEvent(event);
}

void LineEdit::slotEmit()
{
  if (!m_canceling)
    emit editingFinished(text());
}

void LineEdit::slotTranslate()
{
  setText(QDir::toNativeSeparators(text()));
}

LineEdit::~LineEdit()
{
  ToolTip::showMessage(QString(), this);
}

}
