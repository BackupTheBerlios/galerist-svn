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
#include <QtCore/QFileInfo>

#include <QtGui/QPalette>
#include <QtGui/QColor>
#include <QtGui/QToolTip>
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QCompleter>

#include "core/data.h"

#include "widgets/tooltip.h"

using namespace GCore;

namespace GWidgets
{

LineEdit::LineEdit(QWidget *parent, Types type)
    : QLineEdit(parent),
    m_validationMethod(ValidStatesDefined),
    m_inputAttribute(Readable),
    m_valid(false),
    m_canceling(false),
    m_testing(false),
    m_needExisting(true),
    m_type(type)
{
  // Wrapper slot, so we emit the data with it. No need subclassing keyPressEvent() method
  connect(this, SIGNAL(editingFinished()), this, SLOT(slotEmit()));

  // Checking slot
  connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(checkValidity(const QString&)));

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
  else if (!m_tested && m_testing)
    image = QImage(":/images/unknown.png");
  else
    image = QImage(":/images/input_ok.png");

  painter.drawImage(QRect(width() - (image.width() + spacing), (height() / 2) - (image.height() / 2), image.width(), image.height()), image);
}

void LineEdit::setValidity(bool valid, const QString &reason, bool tested)
{
  m_valid = valid;
  m_tested = tested;

  if (isVisible())
    ToolTip::showMessage(reason, this, mapToGlobal(QPoint(0, 0)));

  emit validityChanged(valid);

  update();
}

bool LineEdit::isValid() const
{
  return m_valid;
}

void LineEdit::setType(Types type)
{
  m_type = type;

  if (m_type != Default) {
    setStyleSheet(QString(":enabled { padding-right: %1; }").arg(18));
  }

  // If this lineedit is a file selector, then we activate the translation function.
  if (m_type == FileSelector) {
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTranslate()));
    setCompleter(static_cast<QCompleter*>(Data::self()->value(Data::FileCompleter).value<QObject*>()));
  } else if (m_type == DirSelector) {
    setCompleter(static_cast<QCompleter*>(Data::self()->value(Data::DirCompleter).value<QObject*>()));
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

void LineEdit::focusOutEvent(QFocusEvent *event)
{
  ToolTip::showMessage(QString(), this);

  QLineEdit::focusOutEvent(event);
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

void LineEdit::setErrorMessage(const QString &message)
{
  m_errMessage = message;
}

void LineEdit::setValidMessage(const QString &message)
{
  m_validMessage = message;
}

void LineEdit::addValidValues(const QStringList &values)
{
  m_validValues << values;
}

void LineEdit::addValidValue(const QString &value)
{
  m_validValues << value;
}

void LineEdit::addInvalidValues(const QStringList &values)
{
  m_invalidValues << values;
}

void LineEdit::addInvalidValue(const QString &value)
{
  m_invalidValues << value;
}

void LineEdit::setNeedTest(bool test)
{
  m_testing = test;
}

void LineEdit::setValidationMethod(ValidationMethods method)
{
  m_validationMethod = method;
}

void LineEdit::setValidAttribute(Attributes attribute)
{
  m_inputAttribute = attribute;
}

void LineEdit::setNeedExisting(bool need)
{
  m_needExisting = need;
}

void LineEdit::checkValidity(const QString &text)
{
  // We don't use internal verify with WithVerify and Default types
  if (m_type == WithVerify || m_type == Default)
    return;

  QStringList comperisonList;
  bool validationResult;


  switch (m_validationMethod) {
    case (ValidStatesDefined) : {
      comperisonList = m_validValues;
      validationResult = true;
      break;
    }
    case (InvalidStatesDefined) : {
      comperisonList = m_invalidValues;
      validationResult = false;
    }
  }

  // We check for valid/invalid values first
  if (comperisonList.contains(text)) {
    setValidity(validationResult, validationResult ? m_validMessage : m_errMessage, !m_testing);
    return;
  } else if (m_type == WithInternalVerify) {
    setValidity(!validationResult, !validationResult ? m_validMessage : m_errMessage, !m_testing);
    return;
  }

  QFileInfo info(text);
  bool isDir = info.isDir();
  bool isFile = info.isFile();

  switch (m_type) {
    case (DirSelector) : {
      if (!isDir && m_needExisting) {
        setValidity(false, isFile ? tr("This is a file.") : QString());
        return;
      }
      break;
    }
    case (FileSelector) : {
      if (!isFile && m_needExisting) {
        setValidity(false, isDir ? tr("This is a directory.") : QString());
        return;
      }
      break;
    }
    default:
      break;
  }

  if (!m_needExisting && !info.exists())
    info = QFileInfo(info.dir().absolutePath());

  switch (m_inputAttribute) {
    case (Readable) : {
      if (!info.isReadable()) {
        setValidity(false, tr("Directory or file is not readable!"));
        return;
      }
      break;
    }
    case (Writable) : {
      if (!info.isWritable()) {
        setValidity(false, tr("Directory or file is not writable!"));
        return;
      }
      break;
    }
    case (Executable) : {
      if (!info.isExecutable()) {
        setValidity(false, tr("File is not executable!"));
        return;
      }
      break;
    }
  }

  setValidity(true, m_validMessage, !m_testing);
}

}
