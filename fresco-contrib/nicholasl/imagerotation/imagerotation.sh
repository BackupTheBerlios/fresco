#! /bin/sh
source ../fresco-config
c++ -I$FRESCO/include imagerotation.cc -o imagerotation -L$FRESCO/lib $LIBFRESCO $LIBORB

