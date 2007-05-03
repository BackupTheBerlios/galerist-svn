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
#ifndef GWIDGETSPHOTOCONTROL_H
#define GWIDGETSPHOTOCONTROL_H

#include <QtGui/QWidget>
#include <QtCore/QVariant>

#include "ui_photocontrol-main.h"
#include "ui_photocontrol-crop.h"
#include "ui_photocontrol-blur.h"

class QStackedWidget;
class QPushButton;

namespace GWidgets
{

class PhotoView;

/**
 * @short A control panel for PhotoView.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class PhotoControl : public QWidget
{
    Q_OBJECT
  signals:
    /**
     * Emitted when Save button of the operation is pressed.
     *
     * @param operation Which operation has been saved (or need to be saved).
     * @param params Parameters for the selected operation.
     */
    void saveChange(int operation, const QMap<int, QVariant> &params);

    /**
     * Emitted upon value change.
     *
     * @param operation Which operation has been saved (or need to be saved).
     * @param params Parameters for the selected operation.
     */
    void valuesChange(int operation, const QMap<int, QVariant> &params);

    /**
     * Emitted when an operation has been selected.
     */
    void operationSelected(int operation);

    /**
     * Emitted whenn operation has been canceled.
     */
    void cancelOperation(int operation);

  public:
    /**
     * List of operations that this PhotoControl can do.
     */
    enum Operation
    {
      /** Used for defining operationless state. */
      NoOperation,
      /** Crop operation. */
      Crop,
      /** Blur operation. */
      Blur,
      /** Sharpen operation. */
      Sharpen
    };

    /**
     * List of possible parameter types.
     */
    enum ParameterType
    {
      /** Parameter describing an area. */
      Area,
      /** Parameter describing the number of repeating operation. */
      RepeatNumber
    };

    /**
     * Just a constructor.
     */
    PhotoControl(QWidget *parent = 0);

    /**
     * A destructor.
     */
    ~PhotoControl();

    /**
     * Connects to the PhotoView.
     */
    void connectView(PhotoView *view);

  public slots:
    /**
     * Enables or enables the next/back button. Depends on the signal.
     */
    void checkNavigation(int newLocation, int totalImages);

  private:
    QStackedWidget *m_controlPanel;
    Operation m_currentOperation;
    bool m_saved;
    QMap<int, QVariant> m_params;

    // Layouts for pages
    Ui::PhotoControlMain m_mainPage;
    Ui::PhotoControlCrop m_cropPage;
    Ui::PhotoControlBlur m_blurPage;
    Ui::PhotoControlBlur m_sharpenPage;

    /**
     * Setups and returns the main control page.
     */
    QWidget *setupMainPage();

    /**
     * Setups and returns the control page for cropping tool.
     */
    QWidget *setupCropControl();

    /**
     * Setups and returns the control page for blur tool.
     */
    QWidget *setupBlurPage();

    /**
     * Setups and returns the control page for sharpen tool.
     */
    QWidget *setupSharpenPage();

  private slots:
    /**
     * Goes back to operationless existence :D.
     */
    void restore();

    /**
     * Saves the change (or better, notifies other to save the change).
     */
    void saveChanges();

    /**
     * Enables the save button.
     */
    void valuesChanged();

    /**
     * Requests a preview.
     */
    void requestPreview();

    /**
     * Executed when the crop tool is selected.
     */
    void selectCrop();

    /**
     * Selects the Blur transformation tool.
     */
    void selectBlur();

    /**
     * Selects the Sharpen transformation tool.
     */
    void selectSharpen();
};

}

#endif
