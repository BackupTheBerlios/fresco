#! /bin/sh
source ../fresco-config
$CPP -I$FRESCO/include imagerotation.cc -o imagerotation -L$FRESCO/lib $LIBFRESCO $LIBORB

