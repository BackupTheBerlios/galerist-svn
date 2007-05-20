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
#include "ui_photocontrol-resize.h"

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
    int m_currentOperation;
    bool m_saved;
    QSize m_imageSize;
    QMap<int, QVariant> m_params;

    // Layouts for pages
    Ui::PhotoControlMain m_mainPage;
    Ui::PhotoControlCrop m_cropPage;
    Ui::PhotoControlBlur m_blurPage;
    Ui::PhotoControlBlur m_sharpenPage;
    Ui::PhotoControlResize m_resizePage;

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

    /**
     * Setups and returns the control page for resize tool.
     */
    QWidget *setupResizePage();

    /**
     * Calculates the aspect ratio and fixes the parameters.
     *
     * @param heightChanged Defines if the height (true) or width (false) has been changed.
     * @param value The value that needs to be in the account.
     */
    void keepAspectRatio(bool heightChanged, int value);

  private slots:
    /**
     * Goes back to operationless existence :D.
     */
    void restore();

    /**
     * Make preperations for the switched image. This happenes in edit mode.
     *
     * @param size Size of the switched image.
     */
    void switchedImage(const QSize &size);

    /**
     * Saves the change (or better, notifies other to save the change).
     */
    void saveChanges();

    /**
     * Enables the save button and other things.
     *
     * @see GWidgets#PhotoControl#keepAspectHeight
     * @see GWidgets#PhotoControl#keepAspectWidth
     */
    void valuesChanged();

    /**
     * Keeps the aspect ratio when changing height. It's a wrapper slot.
     *
     * @see GWidgets#PhotoControl#valuesChanged
     * @see GWidgets#PhotoControl#keepAspectWidth
     */
    void keepAspectHeight(int height);

    /**
     * Keeps the aspect ratio when changing width. It's a wrapper slot.
     *
     * @see GWidgets#PhotoControl#valuesChanged
     * @see GWidgets#PhotoControl#keepAspectHeight
     */
    void keepAspectWidth(int width);

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

    /**
     * Selects the Resize transformation tool.
     */
    void selectResize();
};

}

#endif
