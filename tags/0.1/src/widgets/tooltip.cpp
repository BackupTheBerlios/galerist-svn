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
#include "tooltip.h"

#include "core/data.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QRegion>
#include <QtGui/QPolygon>
#include <QtCore/QTimer>
#include <QtGui/QToolTip>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

namespace GWidgets
{

TipLabel *TipLabel::m_tipLabel = 0;

TipLabel::TipLabel(QWidget *parent)
    : QLabel(parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint)
{}

void TipLabel::move(const QPoint &pos)
{
  m_pos = pos;
  QWidget::move(pos);
}

void TipLabel::setText(const QString &text)
{
  m_text = text;
}

QString TipLabel::text()
{
  return m_text;
}

void TipLabel::paintEvent(QPaintEvent*)
{

  int width = text().count() * 8;
  int height = 50;

  QRect desktopGeometry = QApplication::desktop()->screenGeometry();
  QPolygon triangle;

  int difference = desktopGeometry.bottomRight().x() - m_pos.x();

  if (difference < width) {
    triangle << QPoint(width, 60) << QPoint(width - 20, 20) << QPoint(width - 40, 30);
    setGeometry(m_pos.x() - width, m_pos.y(), width, height + 50);
  } else {
    triangle << QPoint(0, 60) << QPoint(20, 20) << QPoint(40, 30);
    setGeometry(m_pos.x(), m_pos.y(), width, height + 50);
  }

  QRegion bubble = QRegion(0, 0, width, height, QRegion::Ellipse).united(QRegion(triangle, Qt::WindingFill));

  QPainterPath path;
  path.addRegion(bubble);

  QPainter painter(this);
  painter.setPen(QPen(QColor(255, 255, 0)));
  painter.setBrush(QColor(255, 255, 0));
  painter.drawPath(path);

  painter.setPen(QPen(QColor(0, 0, 0)));
  painter.setBrush(QBrush(QColor(0, 0, 0)));
  painter.drawText(QRectF(0, 0, width, height), Qt::AlignCenter, text());

  setMask(bubble);
}

void TipLabel::slotClose()
{
  delete m_tipLabel;
  m_tipLabel = 0;
}

TipLabel::~TipLabel()
{}

void ToolTip::showMessage(const QString &text, QWidget *parent, const QPoint &pos)
{
  if (!GCore::Data::self()->getEnableToolTips()) {
    QToolTip::showText(pos, text, parent);
    return;
  }

  if (TipLabel::m_tipLabel) {
    if (TipLabel::m_tipLabel->text() == text)
      return;

    delete TipLabel::m_tipLabel;
    TipLabel::m_tipLabel = 0;
  }

  if (text.isEmpty())
    return;

  TipLabel::m_tipLabel = new TipLabel(parent);
  TipLabel::m_tipLabel->setText(text);
  TipLabel::m_tipLabel->move(pos - QPoint(-15, 60));
  TipLabel::m_tipLabel->show();

  QTimer::singleShot(text.count() * 100, TipLabel::m_tipLabel, SLOT(slotClose()));
}


}
