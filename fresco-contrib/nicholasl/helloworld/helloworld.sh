#! /bin/sh
c++ -I$FRESCO/include helloworld.cc -o helloworld -L$FRESCO/lib $LIBFRESCO $LIBORB
