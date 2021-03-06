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

/**
 * @short Core parts of Goya.
 */
namespace GCore
{
/**
 * @short "Working" part of Goya (threaded).
 */
namespace GJobs
  {}

/**
 * @short Part of Goya that utilises network.
 */
namespace GNetwork
  {}
}

/**
 * @short Dialogs used by Goya.
 */
namespace GDialogs
  {}

/**
 * @short Widgets used by MainWindow and dialogs in GDialogs.
 */
namespace GWidgets
{
/**
 * @short Widgets used by NewGalleryWizard.
 */
namespace GWizard
  {}
/**
 * A special kind of widgets that are used inside a PhotoView and can't be used in normal Qt dialogs.
 * @short Widgets used in PhotoView.
 */
namespace GPhotoWidgets
  {}
}


