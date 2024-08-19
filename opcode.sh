#!/bin/bash
f=$1
fun=$2

objdump --disassemble=$fun $f

objdump --disassemble=$fun $f | grep ' *[0-9a-f]:' | sed 's/ *[0-9a-f]*:\t//' | sed 's/   *.*//' | tr -d '\n' | tr -d ' '

# objdump --disassemble="$fun" "$f" |
	# sed '
		# /^ *[0-9a-f]:/!d
		# s/.*\t\(.*\)\t.*/\1/
		# s/[ \t]//g
	# ' | paste -s -d ''

# objdump --disassemble="$fun" "$f" |
# 	sed '
# 		/^ *[0-9a-f]:/!d
# 		:l
# 		s/.*\t\(.*\)\t.*/\1/
# 		s/[ \t]//g
# 		x; G
# 		s/\n//
# 		$p
# 		h; d
# 		t l
# 	'

