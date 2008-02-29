/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file animatedpixmap.cpp
** \version $Id$
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

