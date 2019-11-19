#!/bin/sh
# set-creation-dates.sh -- Update header comment creation dates
#
# Copyright © 2019  Miles Bader
#
# Author: Miles Bader <snogglethorpe@gmail.com>
# Created: 2019-11-17
#

for file in "$@"; do
    cdate=`git log --follow --date=short --pretty=tformat:%ad "$file" | tail -1`
    if ! test "$cdate"; then
	cdate=`date +"%Y-%m-%d"`
    fi

    fdate=`sed -n -e '/[^ \t].*Created:/s/.*: *//p' -e '/^ *$/q' "$file"`

    if test "$fdate" && test "$cdate" && test "$fdate" != "$cdate"; then
	sed "s@$fdate@$cdate@" < "$file" > "$file.new" && mv -v -b "$file.new" "$file"
    fi
done
