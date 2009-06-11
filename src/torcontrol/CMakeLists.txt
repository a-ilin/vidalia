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
  AddressMap.cpp
  BootstrapStatus.cpp
  Circuit.cpp
  ClientStatusEvent.cpp
  ControlCommand.cpp
  ControlConnection.cpp
  ControlReply.cpp
  ControlSocket.cpp
  DangerousPortEvent.cpp
  DangerousVersionEvent.cpp
  GeneralStatusEvent.cpp
  LogEvent.cpp
  OrConnEvent.cpp
  ProtocolInfo.cpp
  ReplyLine.cpp
  RouterDescriptor.cpp
  RouterStatus.cpp
  SendCommandEvent.cpp
  ServerStatusEvent.cpp
  Stream.cpp
  tcglobal.cpp
  TorControl.cpp
  TorEvents.cpp
  TorProcess.cpp
  TorSignal.cpp
)
qt4_wrap_cpp(torcontrol_SRCS
  ControlConnection.h
  ControlSocket.h
  TorControl.h
  TorEvents.h
  TorProcess.h
)

if (WIN32)
  set(torcontrol_SRCS ${torcontrol_SRCS} 
    TorService.cpp
  )
  qt4_wrap_cpp(torcontrol_SRCS
    TorService.h
  )
endif(WIN32)


add_library(torcontrol STATIC  ${torcontrol_SRCS})
target_link_libraries(torcontrol
  ${QT_QTCORE_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  common
)

