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
** \file animatedpixmap.h
** \version $Id$
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

