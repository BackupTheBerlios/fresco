#! /bin/sh
source ../fresco-config
$CPP -g -I$FRESCO/include blob.cc -o blob -L$FRESCO/lib $LIBFRESCO $LIBORB
