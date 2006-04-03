#!/bin/bash
#

# Make sure enough arguments were supplied
if [ "$#" -ne 3 ]
then
  echo "Usage: $0 <srcdir> <files to include> <outfile.dmg>"
  exit 1
fi

# Make sure the source directory exists
srcdir=$1
if [ ! -d "$srcdir" ]
then
  echo "Specified source directory doesn't exit: $srcdir"
  exit 1
fi
echo "Source Directory: $srcdir"

# Get the rest of the command-line arguments
filelist=$2
echo "Files to copy: $filelist"
dmg=$3
echo "Destination Disk Image: $dmg"
volname=`echo "$dmg" | sed -e "s/.*\///" | sed -e "s/.dmg//"`
echo "Volume Name: $volname"

# Create the disk image
hdiutil create $dmg -size 20m -fs HFS+ -volname "$volname"

# Mount the image and get the device name
mntdir="dist.mnt"
hdiutil attach "$dmg" -noautoopen -quiet -mountpoint "$mntdir"
dev_handle=`hdiutil info | grep "$mntdir" | grep "Apple_HFS" | awk '{print $1}'`

# Copy over the specified files into the disk image
for file in $filelist
do
  echo "Copying $srcdir/$file to $mntdir/"
  ditto -rsrc "$srcdir/$file" "$mntdir/"
done

# Unmount the volumen
hdiutil detach $dev_handle

# Replace the image with a compressed image
hdiutil convert $dmg -format UDZO -o $dmg.udzo.dmg
rm -f $dmg
mv $dmg.udzo.dmg $dmg

