##
##  $Id: $
## 
##  This file is part of Vidalia, and is subject to the license terms in the
##  LICENSE file, found in the top level directory of this distribution. If 
##  you did not receive the LICENSE file with this file, you may obtain it
##  from the Vidalia source package distributed by the Vidalia Project at
##  http://www.vidalia-project.net/. No part of Vidalia, including this file,
##  may be copied, modified, propagated, or distributed except according to
##  the terms described in the LICENSE file.
##

add_definitions(-DSTATICLIB -DNDEBUG)

set(miniupnpc_SRCS
  igd_desc_parse.c
  minisoap.c
  miniupnpc.c
  miniwget.c
  minixml.c
  upnpcommands.c
  upnperrors.c
  upnpreplyparse.c
)
if (NOT WIN32)
  set(miniupnpc_SRCS ${miniupnpc_SRCS} minissdpc.c)
endif(NOT WIN32)

add_library(miniupnpc STATIC ${miniupnpc_SRCS})

