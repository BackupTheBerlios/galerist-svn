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

#ifndef GWIDGETSTEXTEDIT_H_
#define GWIDGETSTEXTEDIT_H_

#include <QtGui/QTextEdit>

namespace GWidgets
{

/**
 * Class for text edits used in PhotoView.
 * @short Text edit just for PhotoView.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class TextEdit : public QTextEdit
{
    Q_OBJECT
  signals:
    /**
     * Signals taht the editing has been finished.
     *
     * @param text The new text.
     */
    void editingFinished(const QString &text);
    /**
     * Signals that the editing has been canceled.
     */
    void editingCanceled();

  public:
    /**
     * Default constructor.
     */
    TextEdit(QWidget *parent = 0);

    /**
     * Default destructor.
     */
    ~TextEdit();

  protected:
    /**
     * Overloaded method for defining what to do when it gets pressed.
     *
     * @param event The event :).
     */
    void keyPressEvent(QKeyEvent *event);
    /**
     * Overloaded method for defining what to do when focus goes away.
     */
    void focusOutEvent(QFocusEvent*);

  private:
    //QTextEdit *m_textEdit;
    //QPushButton *m_submit;
    //QPushButton *m_cancel;
    QString m_previous;

  private slots:
    //void slotAccept();
};

}

#endif
