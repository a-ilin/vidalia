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


set(torcontrol_SRCS 
  addressmap.cpp
  bootstrapstatus.cpp
  circuit.cpp
  clientstatusevent.cpp
  controlcommand.cpp
  controlconnection.cpp
  controlreply.cpp
  controlsocket.cpp
  dangerousportevent.cpp
  dangerousversionevent.cpp
  generalstatusevent.cpp
  logevent.cpp
  orconnevent.cpp
  protocolinfo.cpp
  replyline.cpp
  routerdescriptor.cpp
  routerstatus.cpp
  sendcommandevent.cpp
  serverstatusevent.cpp
  stream.cpp
  tcglobal.cpp
  torcontrol.cpp
  torevents.cpp
  torprocess.cpp
  torsignal.cpp
)
qt4_wrap_cpp(torcontrol_SRCS
  controlconnection.h
  controlsocket.h
  torcontrol.h
  torevents.h
  torprocess.h
)

if (WIN32)
  set(torcontrol_SRCS ${torcontrol_SRCS} 
    torservice.cpp
  )
  qt4_wrap_cpp(torcontrol_SRCS
    torservice.h
  )
endif(WIN32)


add_library(torcontrol STATIC  ${torcontrol_SRCS})
target_link_libraries(torcontrol ${QT_LIBRARIES} common)

