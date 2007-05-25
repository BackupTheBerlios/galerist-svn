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
 * @author Gregor Kališnik <gregor@unimatrix-one.org>
 */
class TransformationJob : public AbstractJob
{
    Q_OBJECT
  signals:
    /**
     * Emitted when an operation has finished.
     *
     * @see GCore#GJobs#TransformationJob#cropImage
     * @see GCore#GJobs#TransformationJob#rotateImage
     * @see GCore#GJobs#TransformationJob#blurImage
     * @see GCore#GJobs#TransformationJob#sharpenImage
     *
     * @param image The preview.
     */
    void preview(const QImage &image);
    /**
     * Emitted when a transformation has been completed and committed.
     *
     * @see GCore#GJobs#TransformationJob#cropImage
     * @see GCore#GJobs#TransformationJob#rotateImage
     * @see GCore#GJobs#TransformationJob#blurImage
     * @see GCore#GJobs#TransformationJob#sharpenImage
     *
     * @param newImage New transformed image.
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
      /** Rotate image. */
      Rotate,
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
      /** Parameter describing the number of filters to apply. */
      NumberFilter,
      /** Size parameter. */
      Size,
      /** Angle of rotation. */
      Angle
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
     * Saves the image.
     */
    void saveImage();

    /**
     * Crops the image.
     */
    void cropImage(const QRect &area);

    /**
     * Rotate image.
     *
     * @param angle Rotate image for angle.
     */
    void rotateImage(quint16 angle);

    /**
     * Blurs image.
     *
     * @param numberFilter How many blur filters to apply.
     */
    void blurImage(int numberFilter);

    /**
     * Sharpens image.
     *
     * @param numberFilter How many sharpen filters to apply.
     */
    void sharpenImage(int numberFilter);

    /**
     * Resizes the image.
     *
     * @param size The new size.
     */
    void resizeImage(const QSize &size);

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
    QDir m_path;
    ImageItem *m_item;
    bool m_imageLoaded;

    /**
     * Does the actual crop.
     *
     * @see GCore#GJobs#TransformationJob#cropImage
     */
    void doCrop();

    /**
     * Rotates the image.
     *
     * @see GCore#GJobs#TransformationJob#rotateImage
     */
    void doRotate();

    /**
     * Blurs the image.
     *
     * @see GCore#GJobs#TransformationJob#blurImage
     */
    void doBlur();

    /**
     * Sharpens the image.
     *
     * @see GCore#GJobs#TransformationJob#sharpenImage
     */
    void doSharpen();

    /**
     * Resizes the image.
     *
     * @see GCore#GJobs#TransformationJob#resizeImage
     */
    void doResize();

};

}

}

#endif
