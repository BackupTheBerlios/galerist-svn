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
#ifndef GWIDGETSIMAGEADDPROGRESS_H
#define GWIDGETSIMAGEADDPROGRESS_H

#include <QtGui/QWidget>
#include "ui_imageaddprogress.h"

namespace GWidgets
{

/**
 * Class for showing the progress of the adding photos to the existing gallery.
 * @short Class with a progress bar etc.
 * @author Gregor Kališ¡nik <gregor@podnapisi.net>
 */
class ImageAddProgress : public QWidget, private Ui::ImageAddProgress
{
    Q_OBJECT
  public:
    /**
     * Default constructor.
     *
     * @param parent Pointer to the parent widget.
     */
    ImageAddProgress(QWidget *parent = 0);

    /**
     * Default destructor.
     */
    ~ImageAddProgress();

  public slots:
    /**
     * Method for setting progress.
     *
     * @param finished Number of finished units (unit = photo).
     * @param total Number of total units (unit = photo).
     * @param currentPixmap The thumbnail of the current processed photo.
     */
    void setProgress(int finished, int total, const QString &currentName, const QImage &currentPixmap);

  private:
    
  private slots:

};

}

#endif
