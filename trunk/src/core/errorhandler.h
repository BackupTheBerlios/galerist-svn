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
#ifndef GCOREERRORHANDLER_H
#define GCOREERRORHANDLER_H

#include <QtCore/QObject>

namespace GCore
{

/**
 * @short Handles errors.
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class ErrorHandler : public QObject
{
    Q_OBJECT
  signals:
    /**
     * Signals the message to show.
     *
     * @param message Message to show.
     * @param type of the message.
     *
     * @see ErrorHandler#reportMessage
     * @see ErrorHandler#slotReporter
     */
    void signalMessage(const QString &message, int type);

  public:
    enum ErrorType
    {
      /**
       * It's an information.
       */
      Information,
      /**
       * It's a warning.
       */
      Warning,
      /**
       * It's a critical error.
       */
      Critical
  };

    /**
     * A constructor.
     *
     * @param parent Parent of the message widget.
     */
    ErrorHandler(QObject *parent = 0);

    /**
     * Method for showing the message.
     *
     * @param message String that will be shown to the user.
     * @param type Type of the message.
     *
     * @see ErrorHandler#signalMessage
     * @see ErrorHandler#slotReporter
     */
    static void reportMessage(const QString &message, int type);

    /**
     * A constructor.
     */
    ~ErrorHandler();

  public slots:
    /**
     * Slot that shows the message.
     *
     * @param message Message that will be shown.
     * @param type Type of the message.
     *
     * @see ErrorHandler#reportMessage
     * @see ErrorHandler#signalMessage
     */
    void slotReporter(const QString &message, int type);

};

}

#endif
