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
##  This file incorporates work covered by the following copyright and
##  permission notice:
##
##  Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
##
##  Redistribution and use in source and binary forms, with or without
##  modification, are permitted provided that the following conditions are met:
##
##   * Redistributions of source code must retain the above copyright notice,
##     this list of conditions and the following disclaimer.
##
##   * Redistributions in binary form must reproduce the above copyright notice,
##     this list of conditions and the following disclaimer in the documentation
##     and/or other materials provided with the distribution.
##
##   * Neither the names of the copyright owners nor the names of its
##     contributors may be used to endorse or promote products derived from this
##     software without specific prior written permission.
##
##  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
##  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
##  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
##  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
##  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
##  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
##  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
##  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
##  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
##  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
##  POSSIBILITY OF SUCH DAMAGE.
##

## Tries to find the required OpenSSL libraries. Once done this will define
## the variable OPENSSL_LIBRARIES.

message(STATUS "Looking for OpenSSL libraries")
if (MSVC)
  # /MD and /MDd are the standard values - if somone wants to use
  # others, the libnames have to change here too
  # use also ssl and ssleay32 in debug as fallback for openssl < 0.9.8b

  find_library(OPENSSL_LIBSSL_DEBUG
    NAMES ssleay32MDd ssl ssleay32
    PATHS ${OPENSSL_LIBRARY_DIR}
  )
  find_library(OPENSSL_LIBSSL_RELEASE
    NAMES ssleay32MD ssl ssleay32
    PATHS ${OPENSSL_LIBRARY_DIR}
  )
  find_library(OPENSSL_LIBCRYPTO_DEBUG
    NAMES libeay32MDd crypto libeay32
    PATHS ${OPENSSL_LIBRARY_DIR}
  )
  find_library(OPENSSL_LIBCRYPTO_RELEASE
    NAMES libeay32MD crypto libeay32
    PATHS ${OPENSSL_LIBRARY_DIR}
  )
   
  if (MSVC_IDE)
    if (NOT OPENSSL_LIBSSL_DEBUG OR NOT OPENSSL_LIBSSL_RELEASE)
      message(FATAL_ERROR
	      "Could not find the debug and release version of libssl.")
    endif(NOT OPENSSL_LIBSSL_DEBUG OR NOT OPENSSL_LIBSSL_RELEASE)
    if (NOT OPENSSL_LIBCRYPTO_DEBUG OR NOT OPENSSL_LIBCRYPTO_RELEASE)
      message(FATAL_ERROR
	      "Could not find the debug and release version of libcrypto.")
    endif(NOT OPENSSL_LIBCRYPTO_DEBUG OR NOT OPENSSL_LIBCRYPTO_RELEASE)

    set(OPENSSL_LIBSSL
      optimized ${OPENSSL_LIBSSL_RELEASE} 
      debug ${OPENSSL_LIBSSL_DEBUG})
    set(OPENSSL_LIBCRYPTO
      optimized ${OPENSSL_LIBCRYPTO_RELEASE}
      debug ${OPENSSL_LIBCRYPTO_DEBUG})
  else(MSVC_IDE)
    string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_TOLOWER)
    if (CMAKE_BUILD_TYPE_TOLOWER MATCHES debug)
      set(OPENSSL_LIBSSL ${OPENSSL_LIBSSL_DEBUG})
      set(OPENSSL_LIBCRYPTO ${OPENSSL_LIBCRYPTO_DEBUG})
    else(CMAKE_BUILD_TYPE_TOLOWER MATCHES debug)
      set(OPENSSL_LIBSSL ${OPENSSL_LIBSSL_RELEASE})
      set(OPENSSL_LIBCRYPTO ${OPENSSL_LIBCRYPTO_RELEASE})
    endif(CMAKE_BUILD_TYPE_TOLOWER MATCHES debug)
  endif(MSVC_IDE)
  mark_as_advanced(OPENSSL_LIBSSL_DEBUG   OPENSSL_LIBCRYPTO_DEBUG
                   OPENSSL_LIBSSL_RELEASE OPENSSL_LIBCRYPTO_RELEASE)
else(MSVC)
  find_library(OPENSSL_LIBSSL NAMES ssl ssleay32)
  find_library(OPENSSL_LIBCRYPTO NAMES crypto libeay32)
endif(MSVC)


if (OPENSSL_LIBSSL AND OPENSSL_LIBCRYPTO)
  set(OPENSSL_LIBRARIES ${OPENSSL_LIBSSL} ${OPENSSL_LIBCRYPTO})
  mark_as_advanced(OPENSSL_LIBSSL OPENSSL_LIBCRYPTO)
  mark_as_advanced(OPENSSL_LIBRARIES)
  message(STATUS "Looking for OpenSSL libraries - found")
else (OPENSSL_LIBSSL AND OPENSSL_LIBCRYPTO)
  if (NOT OPENSSL_LIBSSL)
    message(FATAL_ERROR "Could not find libssl.")
  else (NOT OPENSSL_LIBSSL)
    message(FATAL_ERROR "Could not find libcrypto.")
  endif (NOT OPENSSL_LIBSSL)
endif(OPENSSL_LIBSSL AND OPENSSL_LIBCRYPTO)

