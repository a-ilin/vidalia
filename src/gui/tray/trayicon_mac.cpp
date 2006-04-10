/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
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
 * \file trayicon_win.cpp
 * \version $Id$
 */

#include <QApplication>

#include "trayicon_mac.h"


/** Constructor. */
TrayIconImpl::TrayIconImpl(const QString &iconFile, const QString &toolTip)
{
  setObjectName("trayiconimpl");
  _imageRef = 0;
  _shown    = false;

  /* Add the tool tip to the structure */
  setIcon(iconFile);

  /* And give it an icon */
  setToolTip(toolTip);
}

/** Destructor */
TrayIconImpl::~TrayIconImpl()
{
  if (_shown) {
    hide();
  }
  if (_imageRef) {
    CGImageRelease(_imageRef);
  }
}

/** Callback used by CGDataProviderCreateWithData(). */
void
TrayIconImpl::releaseCallback(void *info, const void *data, size_t size)
{
  Q_UNUSED(info);
  Q_UNUSED(size);
  free((void*)data);
}

/** Load icon data from the given file and create a CGImageRef. */
CGImageRef
TrayIconImpl::createIconFromFile(FSSpec fileSpec)
{
  CGDataProviderRef provider = NULL;
  CGImageRef image = NULL;
  IconFamilyHandle iconFamily;
  
  /* Load the icon from the resource bundle */
  if (ReadIconFile(&fileSpec, &iconFamily) == noErr) {
    int size = 128 * 128 * 4;
    Handle rawBitmapdata = NewHandle(size);
    GetIconFamilyData(iconFamily, kThumbnail32BitData, rawBitmapdata);
    
    Handle rawMaskdata = NewHandle(128 * 128);
    GetIconFamilyData(iconFamily, kThumbnail8BitMask, rawMaskdata);
    
    char *data = (char *)malloc(size);
    HLock(rawBitmapdata);
    HLock(rawMaskdata);
    
    /* copy mask data into alpha channel */
    const char *mask = (const char*) *rawMaskdata;
    const char *from = (const char*) *rawBitmapdata;
    char *to = data;
    
    for (int i= 0; i < 128*128; i++) {
      from++;
      *to++ = *mask++;
      *to++ = *from++;
      *to++ = *from++;
      *to++ = *from++;
    }
    HUnlock(rawBitmapdata);
    HUnlock(rawMaskdata);
    
    DisposeHandle(rawBitmapdata);
    DisposeHandle(rawMaskdata);
    
    provider = CGDataProviderCreateWithData(NULL, data, size, releaseCallback);
    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    
    image = CGImageCreate(128,  // width
                          128,  // height
                          8,    // Bits per component
                          32,   // Bits per pixel
                          4 * 128,  // Bytes per row
                          cs,
                          kCGImageAlphaFirst,
                          provider,
                          NULL,
                          0,
                          kCGRenderingIntentDefault);
    
    CGColorSpaceRelease(cs);
    CGDataProviderRelease(provider);
  }
  return image;
}

/** Create an icon from the given filename in the application bundle. */
CGImageRef
TrayIconImpl::createIcon(const QString &iconFile)
{
  FSRef ref;
  CGImageRef image = NULL;
  
  /* Create a CFStringRef that we can use to build the resource URL */
  CFStringRef iconFileRef = CFStringCreateWithCString(NULL, qPrintable(iconFile), 
                                                      kCFStringEncodingASCII);
  
  /* Build a URL to the requested .icns in our resource bundle */
  CFURLRef url = CFBundleCopyResourceURL(CFBundleGetMainBundle(), 
                                         iconFileRef, CFSTR("icns"), NULL);
  
  /* Try to find the resource in the bundle */
  if (CFURLGetFSRef(url, &ref)) {
    FSSpec fileSpec;
    if (FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, 
                         NULL, &fileSpec, NULL) == noErr) {
      /* Found it, now create the icon from it */
      image = createIconFromFile(fileSpec);
    }
  }
  CFRelease(iconFileRef);
  CFRelease(url);
  return image;  
}

/** Show the tray icon image. */
void
TrayIconImpl::show()
{
  if (_imageRef) {
    CGContextRef ctxRef = BeginCGContextForApplicationDockTile();
    if (!ctxRef) {
      return;
    }
    SetApplicationDockTileImage(_imageRef);
    EndCGContextForApplicationDockTile(ctxRef);
    
    _shown = true;
  }  
}

/** Hide the tray icon image. */
void
TrayIconImpl::hide()
{
  _shown = false;
  
  CGContextRef ctxRef = BeginCGContextForApplicationDockTile();
  if (!ctxRef) {
    return;
  }
  RestoreApplicationDockTileImage();
  EndCGContextForApplicationDockTile(ctxRef);
}

/** Set the tray icon's tooltip. */
void
TrayIconImpl::setToolTip(const QString &toolTip)
{
  /* The dock icon doesn't have a tooltip. */
  Q_UNUSED(toolTip);
}

/** Set the tray icon's image. */
void
TrayIconImpl::setIcon(const QString &iconFile)
{
  /* If there was an image set previously, free it before getting the new one */
  if (_imageRef) {
    CGImageRelease(_imageRef);
    _imageRef = 0;
  }
  
  /* Load the icon data from the application bundle */
  _imageRef = createIcon(iconFile);
  
  /* If the icon is to be shown, put it in the dock now. */
  if (_imageRef && _shown) {
    show();
  }
}

