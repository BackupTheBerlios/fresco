#! /bin/sh
source ../fresco-config
$CPP -I$FRESCO/include helloworld.cc -o helloworld -L$FRESCO/lib $LIBFRESCO $LIBORB
