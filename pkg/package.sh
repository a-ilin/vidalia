#!/bin/bash
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

# Builds distribution packages for various platforms.

# Check for proper script arguments
if [ "$#" -eq 0 ]
then
  echo "Usage: $0 <rpm>"
  exit 1
fi

# Make the distribution depending on what type was requested
case "$1" in
#
#  rpm package
#
"rpm")
  if [ $# -ne 2 ]
  then
     echo "Usage: $0 rpm <path-to-vidalia-source-tarball>"
     exit 1
  fi
  vidalia_source_path="$2"
  pushd rpm
  rpmbuild -ba --define "_sourcedir $vidalia_source_path" vidalia.spec
  popd
  ;;

# Invalid
#
*)
  echo "Unrecognized distribution type."
  ;;
esac

exit $?
