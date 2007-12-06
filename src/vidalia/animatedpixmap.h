/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2007,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file animatedpixmap.h
 * \version $Id$
 */

#ifndef _ANIMATEDPIXMAP_H
#define _ANIMATEDPIXMAP_H

#include <QTimer>
#include <QPixmap>

/** Provides an animated pixmap that can be used even if Qt was compiled 
 * without GIF support (which it is, by default) or the system doesn't have a 
 * libmng available by default (OS X, for example, usually doesn't). Animated 
 * pixmaps should have a series of square frames adjoined horizontally in a 
 * single image file. */
class AnimatedPixmap : public QObject
{
  Q_OBJECT

public:
  /** Default constructor. */
  AnimatedPixmap();
  /** Creates an animated pixmap from the specified file. */
  AnimatedPixmap(const QString &fileName);

  /** Starts the animation. */
  void start();
  /** Stops the animated image. */
  void stop();
  /** Returns the number of frames in the animation. */
  int frameCount() const;
  /** Returns the current animation frame. */
  QPixmap currentFrame() const;
  /** Sets the duration of each animation frame to <b>frameDelay</b>. */
  void setFrameDelay(int frameDelay);
  /** Sets the source image for the animation to <b>pixmap</b>. */
  void setPixmap(const QPixmap &pixmap);

signals:
  /** Emitted when the current frame has changed. <b>frameNumber</b> contains
   * the current frame number. */
  void frameChanged(int frameNumber);

private slots:
  /** Called when the current animation frame should be changed. */
  void frameTimeout();

private:
  QPixmap _pixmap;    /**< Source image for the animation frames. */
  int _frameNumber;   /**< Current animation frame number. */
  QTimer _frameTimer; /**< Timer to control the delay between frames. */
};

#endif

