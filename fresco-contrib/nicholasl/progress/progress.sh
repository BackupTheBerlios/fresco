#! /bin/sh
source ../fresco-config
$CPP $CPPFLAGS $CXXFLAGS progress.cc -o progress $LIBFLAGS -lBerlin
