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
  log.cpp
  net.cpp
  process.cpp
  stringutil.cpp
  torsocket.cpp
  zlibbytearray.cpp
)
qt4_wrap_cpp(common_SRCS torsocket.h)


if(USE_QSSLSOCKET)
  set(common_SRCS ${common_SRCS} torsslsocket.cpp)
  qt4_wrap_cpp(common_SRCS torsslsocket.h)
endif(USE_QSSLSOCKET)

if(WIN32)
  set(common_SRCS ${common_SRCS}
    win32.cpp
  )
endif(WIN32)


add_library(common STATIC ${common_SRCS})
target_link_libraries(common ${QT_LIBRARIES})

if(USE_QSSLSOCKET AND OPENSSL_LIBRARIES)
  target_link_libraries(common ${OPENSSL_LIBRARIES})
endif(USE_QSSLSOCKET AND OPENSSL_LIBRARIES)

