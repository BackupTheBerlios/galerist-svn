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
#include "imageaddprogress.h"

#include <QtCore/QTimer>

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

namespace GWidgets
{

ImageAddProgress::ImageAddProgress(QWidget *parent)
    : QWidget(parent, Qt::Popup)
{
  setupUi(this);

  QPoint position;
  QRect desktop = QApplication::desktop()->screenGeometry();
  position.setY(desktop.bottom() - 200);
  position.setX(desktop.right() - 300);
  move(position);

  hide();
}

ImageAddProgress::~ImageAddProgress()
{}

void ImageAddProgress::setProgress(int finished, int total, const QString &currentName, const QImage &currentPixmap)
{
  show();
  progressBar->setMaximum(total);
  progressBar->setValue(finished);
  nameLabel->setText(currentName);
  imageLabel->setPixmap(QPixmap::fromImage(currentPixmap).scaled(64, 64, Qt::KeepAspectRatio));

  if (finished == total) {
    nameLabel->setText(tr("Copying of pictures is successful."));
    QTimer::singleShot(1000, this, SLOT(hide()));
  }
}

}
