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
#ifndef GWIDGETSSEARCHBAR_H
#define GWIDGETSSEARCHBAR_H

#include <QtGui/QWidget>

class QHBoxLayout;

namespace GWidgets {

class LineEdit;

/**
 * @short A search bar for an easier way of search.
 * @author Gregor Kalisnik <gregor@podnapisi.net>
 */
class SearchBar : public QWidget
{
  Q_OBJECT
  signals:
    /**
     * Emited when SearchBar loses focus (aka gets hidden).
     */
    void focusLost();
    /**
     * Emited when filter changes.
     */
    void filterChanged(const QString &newFilter);

  public:
    /**
     * A constructor.
     *
     * @param parent Parent widget.
     */
    SearchBar(QWidget *parent = 0);

    /**
     * A destructor.
     */
    ~SearchBar();

    /**
     * Adds the selected letter and shows the bar if neccessary.
     */
    void addLetter(const QString &letter);

    /**
     * Set filter output.
     */
    void setListFilter(QObject *filterList);

  public slots:
    /**
     * Reimplemented method.
     * Hides and deletes the content.
     */
    void hide();

  protected:
    /**
     * Reimplemented method.
     * Hides the bar when Escape key is pressed.
     */
    void keyPressEvent(QKeyEvent *event);

  private:
    QHBoxLayout *m_layout;
    LineEdit *m_searchLine;

};

}

#endif
