/****************************************************************
*  $Id$
* 
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

#include "dock.h" 

CGImageRef getImageRef(const char *icns);


/** Sets the current applications dock icon to the specified .icns file in the
 * application's resource bundle. */
void 
setDockIcon(const char *dockIcon) {
  /* Load the dock icon from the application's resource bundle */
  CGImageRef image = getImageRef(dockIcon);
  
  if (image != NULL) {
    // without the following two statements SetApplicationDockTileImage would not work
    CGrafPtr p= BeginQDContextForApplicationDockTile();
    if (p == NULL)
      return;
    EndQDContextForApplicationDockTile(p);
    
    /* Attempt to set the dock icon */
    OSStatus status = SetApplicationDockTileImage(image);
    if (status == noErr) {
      EventRecord event;
      for (;;) {
        Boolean gotEvent = WaitNextEvent(everyEvent, &event, 32767L, nil);
        if (gotEvent) {
          switch (event.what) {
            case kHighLevelEvent:
              AEProcessAppleEvent(&event);
              break;
          }
          break;
        }
      }
    }
    CGImageRelease(image);
  }
}

/** Callback used by CGDataProviderCreateWithData(). */
void 
releaseData(void *info, const void *data, size_t size) {
  free((void*)data);
}

/** Loads an image from a .icns file in the applications bundle. */
CGImageRef 
getImageRef(const char *icns)
{
  OSStatus status;
  FSRef ref;
  IconFamilyHandle iconFamily;
  CGDataProviderRef provider = NULL;
  CGImageRef image = NULL;

  /* Create a CFStringRef that we can use to build the resource URL */
  CFStringRef iconFile = CFStringCreateWithCString(NULL, icns, 
                                                   kCFStringEncodingASCII);
  
  /* Build a URL to the requested .icns in our resource bundle */
  CFURLRef url = CFBundleCopyResourceURL(CFBundleGetMainBundle(), 
                                         iconFile, CFSTR("icns"), NULL);

  /* Try to find the resource in the bundle */
  if (CFURLGetFSRef(url, &ref)) {
    FSSpec fileSpec;
    status = FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, &fileSpec, NULL);
    if (status == noErr) {
      /* Load the icon from the resource bundle */
      status = ReadIconFile(&fileSpec, &iconFamily);
      if (status == noErr) {
        int size = 128 * 128 * 4;
         Handle rawBitmapdata = NewHandle(size);
         status = GetIconFamilyData(iconFamily, kThumbnail32BitData, rawBitmapdata);
         
         Handle rawMaskdata = NewHandle(128 * 128);
         status = GetIconFamilyData(iconFamily, kThumbnail8BitMask, rawMaskdata);
            
         char *data = (char *)malloc(size);
         HLock(rawBitmapdata);
         HLock(rawMaskdata);
            
         /* copy mask data into alpha channel */
         const char *mask = (const char*) *rawMaskdata;
         const char *from = (const char*) *rawBitmapdata;
         char *to = data;
           
         for (int i= 0; i < 128*128; i++) {
           from++;
           *to++= *mask++;
           *to++= *from++;
           *to++= *from++;
           *to++= *from++;
         }
         HUnlock(rawBitmapdata);
         HUnlock(rawMaskdata);
         
         DisposeHandle(rawBitmapdata);
         DisposeHandle(rawMaskdata);
            
         provider = CGDataProviderCreateWithData(NULL, data, size, releaseData);
         CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
            
         image = CGImageCreate(128,  // width
                               128,  // height
                                 8,  // Bits per component
                                32,  // Bits per pixel
                                 4 * 128,  // Bytes per row
                                 cs,
                                 kCGImageAlphaFirst,
                                 provider,
                                 NULL,
                                 0,
                                 kCGRenderingIntentDefault);
            
         CGColorSpaceRelease(cs);
      }
    }
  }
  return image;
}

/** Restores the dock icon to the application's icon. */
void
restoreDockIcon()
{
  RestoreApplicationDockTileImage();
}

