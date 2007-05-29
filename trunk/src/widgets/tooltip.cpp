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

/**
 * @short A bubble like label. "Floating" of course.
 * @author Gregor KaliÅ¡nik <gregor@unimatrix-one.org>
 */
class TipLabel : public QWidget
{
  public:
    /**
     * A default constructor.
     */
    TipLabel(QWidget *parent = 0);

    /**
     * Overloaded method for moving the widget. It's a wrapper method, that saves the new location.
     *
     * @param pos Position to move to.
     */
    void move(const QPoint &pos);

    /**
     * Method for setting the text.
     *
     * @param text Text to show.
     */
    void setText(const QString &text);

    /**
     * Returns the current text.
     *
     * @return The text.
     */
    QString text();

    /**
     * A destructor.
     */
    ~TipLabel();

  public:

  protected:
    /**
     * Reimplemented method.
     * Defines it's own painting event.
     */
    void paintEvent(QPaintEvent*);

    /**
     * Reimplemented method.
     * Hides the TipLabel.
     */
    void timerEvent(QTimerEvent *event);

  private:
    QPoint m_pos;
    static TipLabel *m_tipLabel;
    QString m_text;
    QImage m_bubble;
    int m_timerId;

    /**
     * Deletes the label.
     */
    void close();

    /**
     * Draws the bubble to the m_bubble image.
     *
     * @see GWidgets#TipLabel#createBubblePath
     * @see GWidgets#TipLabel#setMask
     */
    void paintBubble();
    /**
     * Creates the bubble path.
     *
     * @param width Width of the bubble.
     * @param heigh Height of the bubble.
     *
     * @return Bubble in a QPainterPath.
     *
     * @see GWidgets#TipLabel#paintBubble
     * @see GWidgets#TipLabel#setMask
     */
    QPainterPath createBubblePath(int width, int height);
    /**
     * Overloaded method. It's a wrapper method, that paints the mask and sets it.
     *
     * @param width Width of the bubble (not mask).
     * @param height Height of the bubble (not mask).
     * @param path The bubble path.
     * @param pen Pen which will draw the border.
     *
     * @see GWidgets#TipLabel#paintBubble
     * @see GWidgets#TipLabel#createBubblePath
     */
    void setMask(int width, int height, const QPainterPath &path, const QPen &pen);

    friend class ToolTip;

};

TipLabel *TipLabel::m_tipLabel = 0;

TipLabel::TipLabel(QWidget *parent)
    : QWidget(parent, Qt::ToolTip),
    m_timerId(0)
{}

void TipLabel::move(const QPoint &pos)
{
  m_pos = pos;
  QWidget::move(pos);
}

void TipLabel::setText(const QString &text)
{
  m_text = text;
  m_timerId = startTimer(text.count() * 100);
  paintBubble();
}

QString TipLabel::text()
{
  return m_text;
}

TipLabel::~TipLabel()
{}

void TipLabel::paintEvent(QPaintEvent*)
{
  QPainter widgetPainter(this);
  widgetPainter.drawImage(0, 0, m_bubble);
}

void TipLabel::timerEvent(QTimerEvent *event)
{
  killTimer(event->timerId());

  if (event->timerId() == m_timerId) {
    close();
    m_timerId = 0;
  }
}

void TipLabel::close()
{
  delete m_tipLabel;
  m_tipLabel = 0;
}

void TipLabel::paintBubble()
{
  int width = text().count() * 7;
  int height = 50;

  QPen border;
  border.setWidth(3.0);
  border.setCosmetic(true);
  border.setCapStyle(Qt::RoundCap);
  border.setJoinStyle(Qt::RoundJoin);
  border.setColor(QColor(134, 201, 239));

  QPainterPath bubblePath = createBubblePath(width, height);

  setMask(width, height, bubblePath, border);

  QLinearGradient bubbleColor(QPointF(width / 2, 0), QPointF(width / 2, height));
  bubbleColor.setColorAt(0, QColor(228, 241, 247));
  bubbleColor.setColorAt(0.5, QColor(240, 249, 254));
  bubbleColor.setColorAt(1, QColor(228, 241, 247));

  m_bubble = QImage(width, height + 15, QImage::Format_ARGB32_Premultiplied);

  QPainter bubblePainter(&m_bubble);
  bubblePainter.initFrom(this);
  bubblePainter.setRenderHint(QPainter::Antialiasing, true);
  bubblePainter.eraseRect(0, 0, width, height + 15);

  bubblePainter.strokePath(bubblePath, border);
  bubblePainter.fillPath(bubblePath, bubbleColor);

  bubblePainter.setPen(QPen(QColor(0, 0, 0)));
  bubblePainter.setBrush(QBrush(QColor(0, 0, 0)));
  bubblePainter.drawText(QRectF(0, 0, width, height), Qt::AlignCenter, text());
  bubblePainter.end();
}

QPainterPath TipLabel::createBubblePath(int width, int height)
{
  QRect desktopGeometry = QApplication::desktop()->screenGeometry();
  QPolygon triangle;
  int difference = desktopGeometry.bottomRight().x() - m_pos.x();

  if (difference < width) {
    triangle << QPoint(width, height + 15) << QPoint(width - 20, height) << QPoint(width - 40, height);
    setGeometry(m_pos.x() - width, m_pos.y(), width + 4, height + 15);
  } else {
    triangle << QPoint(0, height + 15) << QPoint(20, height) << QPoint(40, height);
    setGeometry(m_pos.x(), m_pos.y(), width + 4, height + 15);
  }

  QRect rectangle(2, 1, width - 4, height - 1);
  int roundness = 70;

  QPainterPath outputPath;
  outputPath.addRoundRect(rectangle, roundness);
  outputPath.addPolygon(triangle);
  outputPath.closeSubpath();

  return outputPath;
}

void TipLabel::setMask(int width, int height, const QPainterPath &path, const QPen &pen)
{
  QBitmap mask(width + 4, height + 15);
  mask.fill();

  QPen maskBorder(pen);
  maskBorder.setWidth(maskBorder.width() + 1.0);

  QPainter maskPainter(&mask);
  maskPainter.setRenderHint(QPainter::Antialiasing, true);
  maskPainter.setBrush(QColor(0, 0, 0));
  maskPainter.setPen(maskBorder);
  maskPainter.drawPath(path);
  maskPainter.end();

  QWidget::setMask(mask);
}

// Implementation of ToolTip begins here

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
}


}
