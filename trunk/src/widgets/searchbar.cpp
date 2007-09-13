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
#include "searchbar.h"

#include "widgets/lineedit.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QIcon>

namespace GWidgets
{

SearchBar::SearchBar(QWidget *parent)
    : QWidget(parent),
    m_timerId(0)
{
  m_layout = new QHBoxLayout(this);

  QPushButton *button = new QPushButton(QIcon(":/images/cancel.png"), "", this);
  button->setFlat(true);
  m_layout->addWidget(button);

  QLabel *label = new QLabel(tr("Search:"), this);
  m_layout->addWidget(label);

  m_searchLine = new LineEdit(this);
  m_layout->addWidget(m_searchLine);

  m_layout->addStretch();

  m_layout->setContentsMargins(0, 0, 0, 0);

  setLayout(m_layout);

  setContentsMargins(0, 0, 0, 0);

  setFocusProxy(m_searchLine);

  hide();

  connect(m_searchLine, SIGNAL(textChanged(const QString&)), this, SIGNAL(filterChanged(const QString&)));
  connect(m_searchLine, SIGNAL(textChanged(const QString&)), this, SLOT(checkSearch(const QString&)));
  connect(button, SIGNAL(clicked()), this, SLOT(hide()));
}

SearchBar::~SearchBar()
{}

void SearchBar::addLetter(const QString &letter)
{
  setFocus();
  m_searchLine->setText(m_searchLine->text() + letter);

  show();
}

void SearchBar::hide()
{
  emit focusLost();
  m_searchLine->clear();
  QWidget::hide();
}

void SearchBar::setListView(QWidget *filterList)
{
  connect(this, SIGNAL(focusLost()), filterList, SLOT(slotRetakeFocus()));
  connect(this, SIGNAL(filterChanged(const QString&)), filterList, SLOT(setFilter(const QString&)));
}

void SearchBar::keyPressEvent(QKeyEvent * event)
{
  if (event->key() == Qt::Key_Escape) {
    clearFocus();
    hide();
  }
  QWidget::keyPressEvent(event);
}

void SearchBar::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == m_timerId) {
    killTimer(m_timerId);
    m_timerId = 0;
    hide();
  }
}

void SearchBar::checkSearch(const QString &text)
{
  if (text.isEmpty()) {
    m_timerId = startTimer(2000);
  } else {
    killTimer(m_timerId);
    m_timerId = 0;
  }
}

}
