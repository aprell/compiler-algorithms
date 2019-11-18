#!/bin/sh
# gen-make-include-file-deps.sh -- Generate Makefile fragments for
#   source-file dependency tracking
#
# Copyright © 2019  Miles Bader
#
# Author: Miles Bader <snogglethorpe@gmail.com>
# Created: 2019-11-17
#

awk '
function add_dep(dep    ,line,l)
{
    line = dep

    if (prev_line) {
	len = length (prev_line)
	if (len < 50)
	    prev_line = prev_line substr ("                                                  ", len)

	prev_line = prev_line " \\"

	print prev_line
	prev_line = 0

	line = "    " line
    } else
	line = target_prefix line

    prev_line = line
}

BEGIN {
    prev_line = 0
}

FNR == 1 {
    if (prev_line)
	print prev_line

    prev_line = 0

    if (FILENAME ~ /[.]h$/) {
	target_prefix = FILENAME "-DEPS = "
    } else {
	target = FILENAME
	sub (/[.][^.]*$/, ".o", target)
	target_prefix = target ": "

	add_dep(FILENAME)
    }
}

/^ *# *include "/ {
    sub (/^[^"]*"/, "")
    sub (/".*$/, "")
    add_dep($0 " $(" $0 "-DEPS)")
}

END {
    if (prev_line)
	print prev_line
}' "$@"
