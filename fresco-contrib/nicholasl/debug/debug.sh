#! /bin/sh
source ../fresco-config
$CPP $CPPFLAGS $CXXFLAGS debug.cc DebugConsole.cc -o debug $LIBFLAGS -lBerlin
