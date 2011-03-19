##
##  $Id$
## 
##  This file is part of Vidalia, and is subject to the license terms in the
##  LICENSE file, found in the top level directory of this distribution. If 
##  you did not receive the LICENSE file with this file, you may obtain it
##  from the Vidalia source package distributed by the Vidalia Project at
##  http://www.vidalia-project.net/. No part of Vidalia, including this file,
##  may be copied, modified, propagated, or distributed except according to
##  the terms described in the LICENSE file.
##


set(common_SRCS
  crypto.cpp
  file.cpp
  html.cpp
  Log.cpp
  net.cpp
  procutil.cpp
  stringutil.cpp
  TorSocket.cpp
)
qt4_wrap_cpp(common_SRCS 
  TorSocket.h
)

if(WIN32)
  set(common_SRCS ${common_SRCS}
    win32.cpp
  )
endif(WIN32)


add_library(common STATIC ${common_SRCS})
target_link_libraries(common
  ${QT_QTCORE_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
)

