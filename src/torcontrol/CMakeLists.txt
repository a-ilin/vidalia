##
##  $Id$
## 
##  This file is part of Vidalia, and is subject to the license terms in the
##  LICENSE file, found in the top level directory of this distribution. If 
##  you did not receive the LICENSE file with this file, you may obtain it
##  from the Vidalia source package distributed by the Vidalia Project at
##  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
##  including this file, may be copied, modified, propagated, or distributed 
##  except according to the terms described in the LICENSE file.
##


set(torcontrol_SRCS 
  AddressMap.cpp
  BootstrapStatus.cpp
  Circuit.cpp
  ControlCommand.cpp
  ControlConnection.cpp
  ControlReply.cpp
  ControlSocket.cpp
  ControlMethod.cpp
  ProtocolInfo.cpp
  ReplyLine.cpp
  RouterDescriptor.cpp
  RouterStatus.cpp
  SendCommandEvent.cpp
  Stream.cpp
  tcglobal.cpp
  TorControl.cpp
  TorEvents.cpp
  TorProcess.cpp
  TorSignal.cpp
)

if (WIN32)
  set(torcontrol_SRCS ${torcontrol_SRCS} 
    TorService.cpp
  )
endif(WIN32)


add_library(torcontrol STATIC  ${torcontrol_SRCS})

## Link the executable with the appropriate Qt libraries
if (USE_QT5)
  qt5_use_modules(torcontrol
    Core
    Network
  )
else (USE_QT5)
  target_link_libraries(torcontrol
    ${QT_QTCORE_LIBRARY}
    ${QT_QTCORE_LIB_DEPENDENCIES}
    ${QT_QTNETWORK_LIBRARY}
  )
endif (USE_QT5)

target_link_libraries(torcontrol
  common
)

