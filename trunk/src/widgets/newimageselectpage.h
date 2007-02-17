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

#ifndef NEWIMAGESELECTPAGE_H
#define NEWIMAGESELECTPAGE_H

#include "widgets/wizardpage.h"
#include "ui_newimageselectpage.h"

class QDir;
class QImage;

namespace GCore
{
namespace GJobs
{
class ReadJob;
}
}

namespace GWidgets
{

class NewImageSelectPage : public WizardPage, private Ui::NewImageSelectPage
{
  Q_OBJECT
  public:
    /**
     * A default constructor.
     *
     * @param parent Parent widget of this page.
     */
    NewImageSelectPage(QWidget* parent = 0);

    /**
     * A default destructor.
     */
    ~NewImageSelectPage();

  public slots:

  protected:
    /**
     * Reimplemented method.
     */
    void initialise();

    /**
     * Reimplemented method. Stores the filenames list of selected images.
     */
    void nextEvent();

    /**
     * Reimplemented method for clearing all the images from the list.
     */
    void backEvent();

    /**
     * Reimplemented method that searches the images path.
     */
    void viewEvent();

    /**
     * Reimplemented stop event. Need to stop the read job.
     */
    void stopEvent();

  protected slots:

  private:
    GCore::GJobs::ReadJob *m_job;
    QStringList m_directories;

  private slots:
    /**
     * Process the readed images.
     */
    void slotProcess(const QString &filename, const QImage &image, const QString &directory);

};

}

#endif

