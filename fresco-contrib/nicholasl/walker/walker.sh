#! /bin/sh
source ../fresco-config
$CPP $CPPFLAGS $CXXFLAGS -I$FRESCO/Berlin/include/ walker.cc -o walker $LIBFLAGS -lBerlin

