#!/bin/bash
#
#  $Id$
# 
#  Vidalia is distributed under the following license:
#
#  Copyright (C) 2006,  Matt Edman, Justin Hipple
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
#  02110-1301, USA.
#################################################################

# Builds distribution packages for various platforms.


# Check for proper script arguments
if [ "$#" -eq 0 ]
then
  echo "Usage: $0 <tarball|win32|osx|osx-bundle>"
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
  filelist="$srcdir/configure $filelist"
  tar -czf "$tarball" $filelist
  popd -1 1>/dev/null
  ;;
 
#
# OS X .dmg
#
"osx")
  srcdir="../"
  srcfiles="Vidalia.app README AUTHORS LICENSE COPYING CHANGELOG"
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
#  Windows .exe installer
#
"win32")
  echo "Unimplemented"
  ;;

#
# Invalid
#
*)
  echo "Unrecognized distribution type."
  ;;
esac

exit $?

