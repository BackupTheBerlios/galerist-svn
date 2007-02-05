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
#ifndef GWIDGETSNEWCOPYPAGE_H
#define GWIDGETSNEWCOPYPAGE_H

#include <QtGui/QWidget>
#include "ui_newcopypage.h"

/*#define STEP_WARNING 0
#define STEP_COPY 1
#define STEP_FINISH 2*/

namespace GDialogs
{
class NewWizard;
}

namespace GWidgets
{

/**
 * Class that represents a copy page of a wizard.
 * @short Copy page.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class NewCopyPage : public QWidget, private Ui::NewCopyPage
{
    Q_OBJECT
  public:
    /**
     * Defines the step at which it is.
     */
    enum Step {
      StepWarning,
      StepCopy
  };

    /**
     * Default constructor.
     */
    NewCopyPage(Step step, QWidget *parent = 0);

    /**
     * Default destructor.
     */
    ~NewCopyPage();

  private:
    /**
     * Hides the progress bar.
     */
    void hideProgress();
    /**
     * Hides the finish button.
     */
    void hideFinish();

    /**
     * Starts the copy process.
     */
    void startCopy();

  private slots:
    /**
     * Updates the progress bar.
     *
     * @param finished Finished number of units.
     * @param total Total number of units.
     * @param current Name of the current item.
     * @param image Thumbnail of the current photo.
     */
    void slotProgress(int finished, int total, const QString &current, const QImage &image);

    friend class GDialogs::NewWizard;

};

}

#endif
