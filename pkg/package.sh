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
  echo "Usage: $0 <tarball|osx|osx-bundle|rpm>"
  exit 1
fi


# Make the distribution depending on what type was requested
case "$1" in
#
# Source tarball (.tar.gz)
#
"tarball")
  # Put the tarball in the current directory
  destdir=`pwd`
  srcdir=`dirname $(pwd) | sed -e "s/.*\///"`
  tarball="$destdir/$srcdir.tar.gz"
  
  echo "Creating source tarball: $tarball"  
  pushd "../.." 1>/dev/null
  filelist=`svn ls -R $srcdir/ | 
              sed -e "s/^/$srcdir\/&/" | 
              sed -e "/\/$/d" |
              sed -e "s/\n/ /"`
  tar -czf "$tarball" $filelist
  popd -1 1>/dev/null
  ;;
 
#
# OS X .dmg
#
"osx")
  cp -R ../src/vidalia/Vidalia.app ../
  srcdir="../"
  srcfiles="Vidalia.app README CREDITS CHANGELOG"
  srcfiles="$srcfiles LICENSE LICENSE-GPLV2 LICENSE-GPLV3"
  srcfiles="$srcfiles LICENSE-LGPLV3 LICENSE-OPENSSL"
  osx/builddmg.sh "$srcdir" "$srcfiles"
  ;;

#
# OS X .mpkg (Bundle)
#
"osx-bundle")
  if [ $# -ne 3 ]
  then
    echo "Usage: $0 osx-bundle <path-to-tor> <privoxy-pkg.zip>"
    exit 1
  fi
  torpath="$2"
  privoxy="$3"
  
  pushd "osx"
  ./buildmpkg.sh "$torpath" "$privoxy"
  popd
  ;;
  
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

#
# Invalid
#
*)
  echo "Unrecognized distribution type."
  ;;
esac

exit $?

