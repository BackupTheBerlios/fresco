#! /bin/sh
source ../fresco-config
c++ -g -I$FRESCO/include blob.cc -o blob -L$FRESCO/lib $LIBFRESCO $LIBORB
