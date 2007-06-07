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
    /**
     * Validity changed.
     */
    void validityChanged(bool valid);

  public:
    /**
     * Additional types of lineedit.
     */
    enum Types
    {
      /** Default lineedit without any verification and auto completion. */
      Default,
      /** LineEdit with verification module */
      WithVerify,
      /** LineEdit with internal checking enabled. */
      WithInternalVerify,
      /** LineEdit optimised for selecting files. Includes verification and auto completion. */
      FileSelector,
      /** LineEdit like FileSelector, but doesn't complete with files. */
      DirSelector
  };

    /**
     * Validation methods.
     */
    enum ValidationMethods
    {
      /** Valid states are defined (Default). */
      ValidStatesDefined,
      /** Invalid states are defined. */
      InvalidStatesDefined
    };

    /**
     * Directory and file attributes.
     */
    enum Attributes
    {
      /** For valid directory/file it has to be readable (Default). */
      Readable,
      /** For valid directory/file it has to be writable. */
      Writable,
      /** For valid directory/file it has to be executable. */
      Executable
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
     * @param tested Is the input tested?
     */
    void setValidity(bool valid, const QString &reason = QString(), bool tested = false);

    /**
     * Checks if the text is valid.
     *
     * @return @c true It's valid.
     * @return @c false It's invalid.
     */
    bool isValid() const;

    /**
     * Sets the type of the lineedit.
     * @param type Type of the lineedit.
     */
    void setType(Types type);

    /**
     * Default destructor.
     */
    ~LineEdit();

    /**
     * Sets the message that will be shown upon invalid data. Works only when you select WithVerify type.
     *
     * @param message The error message.
     *
     * @see setValidMessage
     */
    void setErrorMessage(const QString &message);

    /**
     * Sets the message that will be shown when valid data gets inputed.
     *
     * @param message The "valid" message.
     *
     * @see setErrorMessage
     */
    void setValidMessage(const QString &message);

    /**
     * Adds aditional valid values to the existing list. Works only when you select WithVerify type.
     *
     * @param values Valid values.
     */
    void addValidValues(const QStringList &values);

    /**
     * Adds an aditional valid value.
     *
     * @param value A valid Value.
     *
     * @see addValidValues
     */
    void addValidValue(const QString &value);

    void addInvalidValues(const QStringList &values);
    void addInvalidValue(const QString &value);

    /**
     * Sets the need of testing the data.
     *
     * @param test Defines if it needs to get tested or not.
     */
    void setNeedTest(bool test);

    void setValidationMethod(ValidationMethods method);
    void setValidAttribute(Attributes attribute);
    void setNeedExisting(bool need);

  protected:
    /**
     * Reimplemented method for defining behaviour on paint event.
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
     * Reimplemented method.
     * Hides the bubble when loses focus.
     */
    void focusOutEvent(QFocusEvent *event);

  private:
    ValidationMethods m_validationMethod;
    Attributes m_inputAttribute;
    bool m_valid;
    bool m_canceling;
    bool m_testing;
    bool m_tested;
    bool m_needExisting;
    Types m_type;
    QString m_errMessage;
    QString m_validMessage;
    QStringList m_validValues;
    QStringList m_invalidValues;

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

    /**
     * Checks the text with the defined "true" values.
     *
     * @param text Text that needs to be verified.
     */
    void checkValidity(const QString &text);

};

}

#endif
