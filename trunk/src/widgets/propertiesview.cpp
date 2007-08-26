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
#include "core/metadatamanager.h"

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

  connect(ui.nextButton, SIGNAL(clicked()), this, SLOT(next()));
  connect(ui.previousButton, SIGNAL(clicked()), this, SLOT(previous()));
  connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(save()));
  connect(ui.rotateCWButton, SIGNAL(clicked()), this, SLOT(rotateCW()));
  connect(ui.rotateCCWButton, SIGNAL(clicked()), this, SLOT(rotateCCW()));

  connect(ui.nameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(enableSave()));
  connect(ui.descriptionEdit, SIGNAL(textChanged()), this, SLOT(enableSave()));
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
  updateNavigation();
}

void PropertiesView::updateData()
{
  if (!m_currentIndex.isValid())
    return;

  QString name = m_currentIndex.data(ImageModel::ImageNameRole).toString();
  QString description = m_currentIndex.data(ImageModel::ImageDescriptionRole).toString();

  m_oldName = name;
  m_oldDescription = description;
  m_rotation = 0;

  QStringList invalidValues = MetaDataManager::self()->imageList(m_currentIndex.parent().data(ImageModel::IdRole).toInt());
  invalidValues.removeAll(name);
  ui.nameEdit->setInvalidValues(invalidValues);

  ui.nameEdit->setText(name);
  ui.descriptionEdit->setText(description);

  QPixmap pixmap = QPixmap::fromImage(m_currentIndex.data(ImageModel::ImagePictureRole).value<QImage>());

  if (pixmap.height() > 512 || pixmap.width() > 512)
    pixmap = pixmap.scaled(512, 512, pixmap.height() > pixmap.width() ? Qt::KeepAspectRatio : Qt::KeepAspectRatioByExpanding);

  ui.photo->setPixmap(pixmap);
}

void PropertiesView::updateNavigation()
{
  int row = m_currentIndex.row();
  ui.previousButton->setDisabled(row <= 0);
  ui.nextButton->setEnabled(m_currentIndex.sibling(row + 1, 0).isValid());
}

void PropertiesView::next()
{
  setCurrentIndex(m_currentIndex.sibling(m_currentIndex.row() + 1, 0));
}

void PropertiesView::previous()
{
  setCurrentIndex(m_currentIndex.sibling(m_currentIndex.row() - 1, 0));
}

void PropertiesView::close()
{
  emit closed(m_currentIndex);
}

void PropertiesView::save()
{
  QAbstractItemModel *model = const_cast<QAbstractItemModel*> (m_currentIndex.model());

  QString name = ui.nameEdit->text();
  QString description = ui.descriptionEdit->toPlainText();

  model->setData(m_currentIndex, name, Qt::EditRole);
  model->setData(m_currentIndex, description, ImageModel::ImageDescriptionRole);
  model->setData(m_currentIndex, m_rotation, ImageModel::ImageRotateCW);

  m_oldName = name;
  m_oldDescription = description;
  m_rotation = 0;

  ui.saveButton->setEnabled(false);
}

void PropertiesView::enableSave()
{
  bool enable = ui.nameEdit->isValid() && (ui.nameEdit->text() != m_oldName || ui.descriptionEdit->toPlainText() != m_oldDescription || m_rotation);
  ui.saveButton->setEnabled(enable);
}

void PropertiesView::rotateCW()
{
  QPixmap pixmap = QPixmap::fromImage(m_currentIndex.data(ImageModel::ImageRotateCW).value<QImage>());

  pixmap = pixmap.scaled(512, 512, pixmap.height() > pixmap.width() ? Qt::KeepAspectRatio : Qt::KeepAspectRatioByExpanding);
  ui.photo->setPixmap(pixmap);

  m_rotation += 90;
  if (m_rotation >= 360)
    m_rotation -= 360;

  enableSave();
}

void PropertiesView::rotateCCW()
{
  QPixmap pixmap = QPixmap::fromImage(m_currentIndex.data(ImageModel::ImageRotateCCW).value<QImage>());

  pixmap = pixmap.scaled(512, 512, pixmap.height() > pixmap.width() ? Qt::KeepAspectRatio : Qt::KeepAspectRatioByExpanding);
  ui.photo->setPixmap(pixmap);

  m_rotation -= 90;
  if (m_rotation < 0)
    m_rotation += 360;

  enableSave();
}

}
