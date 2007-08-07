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
#include "propertiesview.h"

#include <QtCore/QUrl>
#include <QtCore/QProcess>

#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QPixmap>
#include <QtGui/QDesktopServices>
#include <QtGui/QMouseEvent>

#include "core/data.h"
#include "core/imagemodel.h"
#include "core/imageitem.h"

using namespace GCore;

namespace GWidgets
{

PropertiesView::PropertiesView(QWidget *parent)
    : QWidget(parent)
{
  ui.setupUi(this);

  ui.nameEdit->setType(LineEdit::WithInternalVerify);
  ui.nameEdit->setValidationMethod(LineEdit::InvalidStatesDefined);
  ui.nameEdit->setErrorMessage(tr("Please specify a new name."));
}

PropertiesView::~PropertiesView()
{}

void PropertiesView::mouseReleaseEvent(QMouseEvent *event)
{
  if (childAt(event->pos()) == ui.photo && event->button() != Qt::RightButton) {
    QString imageEditor = Data::self()->imageEditor();
    QString imageFilePath = m_currentIndex.data(ImageModel::ImageFilepathRole).toString();
    if (imageEditor.isEmpty()) {
      QUrl photoUrl(imageFilePath);
      QDesktopServices::openUrl(photoUrl);
    } else {
      QStringList args;
      args << imageFilePath;
      QProcess::startDetached(imageEditor, args);
    }
    event->ignore();
  } else {
    QWidget::mouseReleaseEvent(event);
  }
}

void PropertiesView::setCurrentIndex(const QModelIndex &index)
{
  m_currentIndex = index;

  updateData();
}

void PropertiesView::updateData()
{
  if (!m_currentIndex.isValid())
    return;

  ui.nameEdit->setText(m_currentIndex.data(ImageModel::ImageNameRole).toString());
  ui.descriptionEdit->setText(m_currentIndex.data(ImageModel::ImageDescriptionRole).toString());

  QStringList invalidValues = Data::self()->imageModel()->imagesNames(m_currentIndex.parent());
  invalidValues.removeAll(ui.nameEdit->text());
  ui.nameEdit->setInvalidValues(invalidValues);

  QPixmap pixmap = QPixmap::fromImage(m_currentIndex.data(ImageModel::ImagePictureRole).value<QImage>()).scaled(512, 512, Qt::KeepAspectRatioByExpanding);
  ui.photo->setFixedSize(pixmap.size());
  ui.photo->setPixmap(pixmap);
}

}
