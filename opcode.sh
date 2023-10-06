#!/bin/bash
FILE=$1
FUN=$2

objdump --disassemble=$FUN $FILE | grep ' *[0-9a-f]:' | sed 's/ *[0-9a-f]*:\t//' | sed 's/   *.*//' | tr -d '\n' | tr -d ' '
