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
 * \file animatedpixmap.cpp
 * \version $Id$
 */

#include <QSize>
#include <QRect>

#include "animatedpixmap.h"


/** Default constructor. */
AnimatedPixmap::AnimatedPixmap()
: _frameNumber(-1)
{
  _frameTimer.setInterval(100);
  connect(&_frameTimer, SIGNAL(timeout()), this, SLOT(frameTimeout()));
}

/** Creates an animated pixmap from the specified file. */
AnimatedPixmap::AnimatedPixmap(const QString &fileName)
{
  _frameTimer.setInterval(100);
  setPixmap(QPixmap(fileName));
  connect(&_frameTimer, SIGNAL(timeout()), this, SLOT(frameTimeout()));
}

/** Starts the animation. */
void
AnimatedPixmap::start()
{
  _frameTimer.start();
  _frameNumber = 0;
}

/** Stops the animated image. */
void
AnimatedPixmap::stop()
{
  _frameTimer.stop();
}

/** Sets the duration of each animation frame to <b>frameDelay</b>. */
void
AnimatedPixmap::setFrameDelay(int frameDelay)
{
  _frameTimer.setInterval(frameDelay);
}

/** Sets the source image for the animation to <b>pixmap</b>. */
void
AnimatedPixmap::setPixmap(const QPixmap &pixmap)
{
  _pixmap = pixmap;
  _frameNumber = 0;
}

/** Returns the number of frames in the animation. */
int
AnimatedPixmap::frameCount() const
{
  return (_pixmap.width()/_pixmap.height());
}

/** Called when the current animation frame should be changed. */
void
AnimatedPixmap::frameTimeout()
{
  _frameNumber = (_frameNumber + 1) % frameCount();
  emit frameChanged(_frameNumber);  
}

/** Returns the current animation frame. */
QPixmap
AnimatedPixmap::currentFrame() const
{
  return _pixmap.copy(_frameNumber * _pixmap.height(),
                      0,
                      _pixmap.height(),
                      _pixmap.height());
}

