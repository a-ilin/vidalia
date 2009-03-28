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

add_subdirectory(ts2po)
add_subdirectory(po2ts)

if (WIN32)
  add_subdirectory(po2nsh)
  add_subdirectory(nsh2po EXCLUDE_FROM_ALL)
  add_subdirectory(po2wxl)
  add_subdirectory(wixtool)
endif(WIN32)

