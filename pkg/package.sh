#!/bin/bash
#
# Builds distribution packages for various platforms.


# Check for proper script arguments
if [ "$#" -ne 1 ]
then
  echo "Usage: $0 <tarball|win32|osx>"
  exit 1
fi


# Make the distribution depending on what type was requested
case "$1" in
"tarball")
  # Put the tarball in the current directory
  destdir=`pwd`
  srcdir=`dirname $(pwd) | sed -e "s/.*\///"`
  tarball="$destdir/$srcdir.tar.gz"
  
  echo "Creating source tarball: $tarball"  
  pushd "../.." 1>/dev/null
  svn ls -R $srcdir/ | 
     sed -e "s/^/$srcdir\/&/" | 
     sed -e "/\/$/d" | 
     tar -cz -T - -f "$tarball"
  popd -1 1>/dev/null
  ;;
  
"osx")
  dmg=`dirname $(pwd) | sed -e "s/.*\///"`.dmg
  srcdir="../"
  srcfiles="bin README AUTHORS LICENSE COPYING CHANGELOG"
  osx/builddmg.sh "$srcdir" "$srcfiles" "$dmg"
  ;;

"win32")
  echo "Unimplemented"
  ;;

*)
  echo "Unrecognized distribution type."
  ;;
esac

exit $?

