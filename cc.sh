#!/bin/bash
which clang
if [ $? -ne 0 ]; then
  echo "clang compiler is not found"
  exit 1
fi

OUTDIR="bin"
mkdir -p $OUTDIR

OUTNAME="$OUTDIR/$(basename $1 .c)"
CC_FLAGS="-O1 -g -fno-omit-frame-pointer -std=gnu11 "
CC_FLAGS+=$DEV_FLAGS

COMMAND="clang $1 $CC_FLAGS -o $OUTNAME"
echo $COMMAND
$COMMAND
