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

#include <QtCore/QTimer>

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QRegion>
#include <QtGui/QPolygon>
#include <QtGui/QToolTip>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QBitmap>

namespace GWidgets
{

TipLabel *TipLabel::m_tipLabel = 0;

TipLabel::TipLabel(QWidget *parent)
    : QLabel(parent, Qt::ToolTip)
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
  int width = text().count() * 7;
  qDebug(QString::number(width).toAscii().data());
  int height = 50;

  QRect desktopGeometry = QApplication::desktop()->screenGeometry();
  QPolygon triangle;
  QPen border;
  border.setWidth(3);
  border.setCosmetic(true);
  border.setCapStyle(Qt::RoundCap);
  border.setJoinStyle(Qt::RoundJoin);

  int difference = desktopGeometry.bottomRight().x() - m_pos.x();

  if (difference < width) {
    triangle << QPoint(width, height + 15) << QPoint(width - 20, height) << QPoint(width - 40, height);
    setGeometry(m_pos.x() - width, m_pos.y(), width + 4, height + 15);
  } else {
    triangle << QPoint(0, height + 15) << QPoint(20, height) << QPoint(40, height);
    setGeometry(m_pos.x(), m_pos.y(), width + 4, height + 15);
  }

  QRect rectangle(1, 1, width - 4, height - 1);
  int roundness = 70;

  QPainterPath bubblePath;
  bubblePath.addRoundRect(rectangle, roundness);
  bubblePath.addPolygon(triangle);
  bubblePath.closeSubpath();

  QBitmap mask(width + 4, height + 15);
  mask.fill();

  QPainter maskPainter(&mask);
  maskPainter.setRenderHint(QPainter::Antialiasing, true);
  maskPainter.setBrush(QColor(0, 0, 0));
  maskPainter.setPen(border);
  maskPainter.drawPath(bubblePath);
  maskPainter.end();

  QLinearGradient bubbleColor(QPointF(width / 2, 0), QPointF(width / 2, height));
  bubbleColor.setColorAt(1, QColor(150, 150, 0));
  bubbleColor.setColorAt(0.7, QColor(200, 200, 0));
  bubbleColor.setColorAt(0, QColor(255, 255, 60));

  QImage bubble(width, height + 15, QImage::Format_ARGB32_Premultiplied);

  QPainter bubblePainter(&bubble);
  bubblePainter.initFrom(this);
  bubblePainter.setRenderHint(QPainter::Antialiasing, true);
  bubblePainter.eraseRect(0, 0, width, height + 15);

  bubblePainter.strokePath(bubblePath, border);
  bubblePainter.fillPath(bubblePath, bubbleColor);

  bubblePainter.setPen(QPen(QColor(0, 0, 0)));
  bubblePainter.setBrush(QBrush(QColor(0, 0, 0)));
  bubblePainter.drawText(QRectF(0, 0, width, height), Qt::AlignCenter, text());
  bubblePainter.end();

  QPainter widgetPainter(this);
  widgetPainter.setRenderHint(QPainter::Antialiasing, true);
  widgetPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  bubblePainter.eraseRect(0, 0, width, height + 15);
  widgetPainter.drawImage(0, 0, bubble);

  setMask(mask);
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
