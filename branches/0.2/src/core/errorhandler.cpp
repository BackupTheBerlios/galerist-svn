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
#include "errorhandler.h"

#include "core/data.h"

#include <QtGui/QMessageBox>

namespace GCore
{

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent)
{
  connect(this, SIGNAL(signalMessage(const QString, int)), this, SLOT(slotReporter(const QString&, int)));
}

void ErrorHandler::slotReporter(const QString &message, int type)
{
  switch (type) {
    case (Information) : {
      QMessageBox::information(0, tr("Information"), message);
      break;
    }
    case (Warning) : {
      QMessageBox::warning(0, tr("Warning"), message);
      break;
    }
    case (Critical) : {
      QMessageBox::critical(0, tr("Error"), message);
      break;
    }
    default: {
      QMessageBox::critical(0, tr("Bug in error handling"), tr("Error handler doesn't know the error type."));
      break;
    }
  }
  //reportMessage(errorMessage, type);
}

void ErrorHandler::reportMessage(const QString &meessage, int type)
{
  /*switch (type) {
    case (Information) :
    {
      QMessageBox::information(0, tr("Information"), message);
      break;
    }
    case (Warning) :
    {
      QMessageBox::warning(0, tr("Warning"), message);
      break;
    }
    case (Critical) :
    {
      QMessageBox::critical(0, tr("Error"), message);
      break;
    }
    default:
    {
      QMessageBox::critical(0, tr("Bug in error handling"), tr("Error handler doesn't know the error type."));
      break;
    }
  }*/
  emit GCore::Data::self()->getErrorHandler()->signalMessage(meessage, type);
}

ErrorHandler::~ErrorHandler()
{}


}
