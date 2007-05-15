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
#ifndef GCORE_GJOBSTRANSFORMATIONJOB_H
#define GCORE_GJOBSTRANSFORMATIONJOB_H

#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtCore/QDir>

#include "core/jobs/abstractjob.h"

namespace Magick
{
class Image;
}

namespace GCore
{

class ImageItem;

namespace GJobs
{

/**
 * @short Transformation of images like crop, resize, etc.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class TransformationJob : public AbstractJob
{
    Q_OBJECT
  signals:
    /**
     * Emitted when an operation has finished.
     *
     * @see GCore#GJobs#TransformationJob#cropImage
     *
     * @param image The preview.
     */
    void preview(const QImage &image);
    /**
     * Emitted when a transformation has been completed and committed.
     *
     * @see GCore#GJobs#TransformationJob#cropImage
     */
    void completed(const QImage &newImage);
  public:
    /**
     * List of operations that this PhotoControl can do.
     */
    enum Operation
    {
      /** Used for defining operationless state. */
      NoOperation,
      /** Load the image. */
      LoadImage,
      /** Saves the image. */
      SaveImage,
      /** Crop operation. */
      Crop,
      /** Blur operation. */
      Blur,
      /** Sharpen operation. */
      Sharpen,
      /** Resize operation. */
      Resize
    };

    /**
     * List of possible parameter types.
     */
    enum ParameterType
    {
      /** Parameter describing an area. */
      Area,
      /** Parameter describing the number of repeating operation. */
      RepeatNumber,
      /** New size after resize. */
      ResizeSize
    };
    
    /**
     * A constructor.
     */
    TransformationJob(ImageItem *parent = 0);

    /**
     * A destructor.
     */
    ~TransformationJob();

    /**
     * Loads the image.
     */
    void loadImage();

    /**
     * Closes the image.
     *
     * @param save Save upon close.
     */
    void closeImage(bool save);

    /**
     * Saves the image.
     */
    void saveImage();

    /**
     * Crops the image.
     */
    void cropImage(const QRect &area);

  protected:
    /**
     * Reimplemented method.
     * Defines the loop.
     */
    void job();

  private:
    Magick::Image *m_image;
    int m_operation;
    QMap<int, QVariant> m_params;
    bool m_operationDone;
    QDir m_path;
    ImageItem *m_item;
    bool m_imageLoaded;

    /**
     * Does the actual crop.
     *
     * @see GCore#GJobs#TransformationJob#cropImage
     */
    void doCrop();

};

}

}

#endif
