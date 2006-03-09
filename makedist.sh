#!/bin/bash
#
# Builds distribution packages for various platforms.


# Check for proper script arguments
if [ "$#" -ne 1 ]
then
  echo "Usage: $1 <tarball|win32|osx>"
  exit 1
fi

# Make the distribution depending on what type was requested
case "$1" in
"tarball")
  echo "Creating source tarball..."
  srcdir=`pwd | sed -e "s/.*\///"`
  pushd ".." 1>/dev/null
  `svn ls -R "$srcdir"/ | 
     sed -e "s/^/$srcdir\/&/" | 
     sed -e "/\/$/d" | 
     tar -cz -T - -f "$srcdir".tar.gz`
  popd 1>/dev/null
  ;;
  
"osx")
  echo "Unimplemented"
  ;;

"win32")
  echo "Unimplemented"
  ;;

*)
  echo "Unrecognized distribution type."
  ;;
esac

exit $?

