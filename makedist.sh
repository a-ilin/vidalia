#!/bin/bash
#
# Builds distribution packages for various platforms.


# Check for proper script arguments
if [ "$#" -ne 1 ]
then
  echo "Usage: $1 <tarball|win32|osx>"
  exit 1
fi

srcdir=`pwd | sed -e "s/.*\///"`

# Make the distribution depending on what type was requested
case "$1" in
"tarball")
  echo "Creating source tarball..."
  pushd ".." 1>/dev/null
  svn ls -R $srcdir/ | 
     sed -e "s/^/$srcdir\/&/" | 
     sed -e "/\/$/d" | 
     tar -cz -T - -f $srcdir.tar.gz
  popd 1>/dev/null
  ;;
  
"osx")
  dmg=$srcdir.dmg
  echo "Creating OS X Disk Image: $dmg"
  mntdir="dist.mnt"
  pushd ".." 1>/dev/null
  hdiutil create $dmg -size 13m -fs HFS+ -volname "$srcdir"
  hdiutil attach "$dmg" -noautoopen -quiet -mountpoint "$mntdir"
  dev_handle=`hdiutil info | grep "$mntdir" | grep "Apple_HFS" | awk '{print $1}'`
  ditto -rsrc $srcdir/bin $mntdir/
  hdiutil detach $dev_handle
  hdiutil convert $dmg -format UDZO -o $srcdir.udzo.dmg
  rm -f $dmg
  mv $srcdir.udzo.dmg $dmg
  popd 1>/dev/null
  ;;

"win32")
  echo "Unimplemented"
  ;;

*)
  echo "Unrecognized distribution type."
  ;;
esac

exit $?

