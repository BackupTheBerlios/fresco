#! /bin/sh
source ../fresco-config
$CPP -g -I$FRESCO/include debug.cc DebugConsole.cc -o debug -L$FRESCO/lib $LIBFRESCO $LIBORB
