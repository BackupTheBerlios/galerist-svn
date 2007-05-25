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

#ifndef GWIDGETSLINEEDIT_H_
#define GWIDGETSLINEEDIT_H_

#include <QtGui/QLineEdit>

namespace GWidgets
{

/**
 * Class that represents a line edit with validation capabilities.
 * @short Line edit.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class LineEdit : public QLineEdit
{
    Q_OBJECT
  signals:
    /**
     * Signals end of editing.
     *
     * @param text New text.
     */
    void editingFinished(const QString &text);
    /**
     * Signals end of editing. A canceled one.
     */
    void editingCanceled();

  public:
    /**
     * Additional types of lineedit.
     */
    enum Types
    {
      /** Default lineedit without any verification and auto completion. */
      Default,
      /** Lineedit with verification module */
      WithVerify,
      /** Lineedit optimised for selecting files. Includes verification, auto completion and in Windows conversion from / to \. (Maybe with browse button?) */
      FileSelector
  };
    /**
     * Default constructor.
     * @param parent Parent widget.
     * @param type Type of the line edit.
     */
    LineEdit(QWidget *parent = 0, Types type = Default);

    /**
     * Sets validity Changes the color and image.
     *
     * @param valid Valid or not.
     * @param reason Reason why it's valid or invalid. Not used!
     * @param firstRun Set to true only when the dialog is created. Later use is with false (or default).
     */
    void setValidity(bool valid, const QString &reason = QString(), bool firstRun = false);

    /**
     * Checks if the text is valid.
     *
     * @return @c true It's valid.
     * @return @c false It's invalid.
     */
    bool isValid();

    /**
     * Sets the type of the lineedit.
     * @param type Type of the lineedit.
     */
    void setType(Types type);

    /**
     * Default destructor.
     */
    ~LineEdit();

  protected:
    /**
     * Overloaded method for defining behaviour on paint event.
     *
     * @param event Event itself.
     */
    void paintEvent(QPaintEvent *event);
    /**
     * Overloaded method for defining behaviour on key press.
     *
     * @param event Event itself.
     */
    void keyPressEvent(QKeyEvent *event);

    /**
     * Defines what to do on hide event.
     *
     * @param event Event itself.
     */
    void hideEvent(QHideEvent *event);

  private:
    bool m_valid;
    bool m_canceling;
    Types m_type;

  private slots:
    /**
     * Emits the signal.
     */
    void slotEmit();
    /**
     * Translates from unix file path to Windows file path ('/' => '\').
     * @param path Path to translate.
     */
    void slotTranslate();

};

}

#endif
