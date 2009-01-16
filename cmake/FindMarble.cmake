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
##  This file incorporates work from the KDE Marble project, and is covered by 
##  the following copyright and permission notice:
##
##  Redistribution and use in source and binary forms, with or without
##  modification, are permitted provided that the following conditions are
##  met:
##
##   * Redistributions of source code must retain the above copyright notice,
##     this list of conditions and the following disclaimer.
##
##   * Redistributions in binary form must reproduce the above copyright 
##     notice, this list of conditions and the following disclaimer in the 
##     documentation and/or other materials provided with the distribution.
##
##   * Neither the names of the copyright owners nor the names of its
##     contributors may be used to endorse or promote products derived from 
##     this software without specific prior written permission.
##
##  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
##  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
##  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
##  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
##  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
##  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
##  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
##  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
##  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
##  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
##  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##


if (MARBLE_INCLUDE_DIR AND MARBLE_DATA_DIR AND MARBLE_LIBRARIES)
  set(MARBLE_FIND_QUIETLY TRUE)
endif(MARBLE_INCLUDE_DIR AND MARBLE_DATA_DIR AND MARBLE_LIBRARIES)

find_path(MARBLE_INCLUDE_DIR
  NAMES MarbleMap.h
  PATH_SUFFIXES marble
)

find_path(MARBLE_DATA_DIR
  NAMES srtm.dgml
  PATH_SUFFIXES maps/earth/srtm
)
if (MARBLE_DATA_DIR)
  message(STATUS "Using Marble data from ${MARBLE_DATA_DIR}")
endif(MARBLE_DATA_DIR)

find_library(MARBLE_LIBRARIES
  NAMES marblewidget
  PATHS ${MARBLE_LIBRARY_DIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(marble DEFAULT_MSG 
  MARBLE_INCLUDE_DIR
  MARBLE_LIBRARIES
)
