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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QtGui/QApplication>
#include <QtCore/QTranslator>
#include <QtCore/QTextCodec>

#include "mainwindow.h"

#include "core/data.h"

#include "widgets/imageaddprogress.h"

int main(int argc, char *argv[])
{
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  QApplication app(argc, argv);
  app.setOrganizationName("Unimatrix-One");
  app.setApplicationName("Goya");
  GCore::Data::self()->setValue(GCore::Data::AppVersion, "0.3.0");
  GCore::Data::self()->setValue(GCore::Data::AppBranch, "unstable");
  GCore::Data::self()->setValue(GCore::Data::SupportedFormats, QRegExp("gif|jpg|jpeg|png"));

  QTranslator translation;
  translation.load(GCore::Data::self()->value(GCore::Data::TranslationName).toString(), GCore::Data::self()->value(GCore::Data::TranslationPath).toString());

  app.installTranslator(&translation);

  MainWindow *mainWindow = new MainWindow();
  mainWindow->show();

  GCore::Data::self()->setValue(GCore::Data::MainWindow, QVariant::fromValue<QWidget*>(mainWindow));
  GCore::Data::self()->setValue(GCore::Data::ImageAddProgress, QVariant::fromValue<QWidget*>(new GWidgets::ImageAddProgress(mainWindow)));

  return app.exec();
}
