#! /bin/sh
source ../fresco-config
$CPP $CPPFLAGS $CXXFLAGS gauge.cc -o gauge $LIBFLAGS -lBerlin
