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
#include "inputzoomdialog.h"

#include "widgets/photoview.h"

#include "core/data.h"

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

namespace GWidgets
{

QString InputZoomDialog::m_result = QString();

InputZoomDialog::InputZoomDialog(float currentZoom, QWidget *parent)
    : QDialog(parent)
{
  setWindowTitle(tr("Select zoom level"));

  setMaximumSize(200, 100);
  setMinimumSize(200, 100);

  // Our main layout
  QVBoxLayout *main = new QVBoxLayout(this);

  // The label, so that user knows what to do
  QLabel *label = new QLabel(tr("Enter zoom level"), this);
  main->addWidget(label);

  // The input area
  m_zoomLevel = new QComboBox(this);
  m_zoomLevel->setEditable(true);
  main->addWidget(m_zoomLevel);

  // Layout for buttons
  QHBoxLayout *buttons = new QHBoxLayout(this);

  // Lil' streaching
  buttons->addStretch();

  // The accept button
  m_accept = new QPushButton(QIcon(":/images/input_ok.png"), QString(), this);
  m_accept->setFlat(true);
  m_accept->setFocusPolicy(Qt::NoFocus);
  buttons->addWidget(m_accept);

  // The reject button
  m_reject = new QPushButton(QIcon(":/images/input_wrong.png"), QString(), this);
  m_reject->setFlat(true);
  m_reject->setFocusPolicy(Qt::NoFocus);
  buttons->addWidget(m_reject);

  // Again some streachin'
  //main->addStretch();

  // We add the button layout
  main->addLayout(buttons);

  // Make the main layout
  setLayout(main);

  // We add our currently selected zoom level
  if (currentZoom < 50)
    m_zoomLevel->addItem(QString::number(currentZoom) + "%");
  m_zoomLevel->addItem("50%");
  if (currentZoom < 100 && currentZoom > 50)
    m_zoomLevel->addItem(QString::number(currentZoom) + "%");
  m_zoomLevel->addItem("100%");
  if (currentZoom < 150 && currentZoom > 100)
    m_zoomLevel->addItem(QString::number(currentZoom) + "%");
  m_zoomLevel->addItem("150%");
  if (currentZoom > 150)
    m_zoomLevel->addItem(QString::number(currentZoom) + "%");

  // Set as current
  m_zoomLevel->setCurrentIndex(m_zoomLevel->findText(QString::number(currentZoom) + "%"));

  // Connecting slots
  connect(m_accept, SIGNAL(clicked()), this, SLOT(accept()));
  connect(m_zoomLevel->lineEdit(), SIGNAL(returnPressed()), this, SLOT(accept()));
  connect(m_reject, SIGNAL(clicked()), this, SLOT(reject()));
}

float InputZoomDialog::getZoomLevel(float currentZoom, QWidget *parent)
{
  InputZoomDialog *dialog = new InputZoomDialog(currentZoom, parent);

  if (dialog->exec() == QDialog::Accepted)
    return InputZoomDialog::m_result.remove("%").toFloat() / 100;

  return currentZoom;
}

void InputZoomDialog::accept()
{
  InputZoomDialog::m_result = m_zoomLevel->currentText();

  QDialog::accept();
}

}
