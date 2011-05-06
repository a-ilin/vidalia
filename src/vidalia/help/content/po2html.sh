#!/bin/bash
#
# Author: Runa A. Sandvik, <runa.sandvik@gmail.com>
# For The Tor Project, Inc.
#
# This is Free Software (GPLv3)
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# This script will convert translated po files back to html. 
#

### Start config ###

# Location of the translated po files. Do not add the trailing slash. 
translated="/home/runa/tor/vidalia/src/vidalia/help/content/po"

# Location of the original and translated html files. Do not add the
# trailing slash.
html="/home/runa/tor/vidalia/src/vidalia/help/content"

### End config ###

# Find po files to convert.
po=`find $translated -type f -name \*.po`

# For every po found, create and/or update the translated manpage.
for file in $po ; do

	# Validate input and write results to a log file
	validate_script="/home/runa/tor/translation/tools/validate.py"
	validate_log="validate.log"
	python "$validate_script" -i "$file" -l "$validate_log"

	# Get the basename of the file we are dealing with.
	pofile=`basename $file`

	# Strip the file for its original extension and add .html.
	htmlfile="${pofile%.*}.html"

	# Figure out which language we are dealing with.
	lang=`dirname $file | sed "s#$translated/##"`

	# The translated document is written if 80% or more of the po
	# file has been translated. Also, po4a-translate will only write
	# the translated document if 80% or more has been translated.
	# However, it will delete the translated file if less than 80%
	# has been translated. To avoid having our current, translated
	# files deleted, convert the po to a temp file first. If this
	# file was actually written, rename it.

	# Convert translated po to html.
	po4a-translate -f xhtml -m "$html/en/$htmlfile" -p "$file" -l "$html/$lang/tmp-$htmlfile" --master-charset utf-8 -L utf-8

	# Check to see if the file was written. If yes, rename it.
	if [ -e "$html/$lang/tmp-$htmlfile" ]
	then
		mv "$html/$lang/tmp-$htmlfile" "$html/$lang/$htmlfile"
	fi
done
