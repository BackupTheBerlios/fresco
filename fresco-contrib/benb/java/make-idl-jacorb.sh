#adapt
METAIDL=/usr/src/berlin/fresco-contrib/benb/idl/fresco-meta.idl
IDLROOT=/usr/src/berlin/cvs-source/Fresco-IDL/share/idl/
JAVAROOT=/usr/src/java/corba/

PACKAGES="-i2jpackage Fresco:org.fresco.idl.fresco -i2jpackage Widget:org.fresco.idl.widget -i2jpackage Layout:org.fresco.idl.layout -i2jpackage Figure:org.fresco.idl.figure -i2jpackage Primitive:org.fresco.idl.primitive -i2jpackage GGI:org.fresco.idl.ggi -i2jpackage Unidraw:org.fresco.idl.unidraw"

rm $JAVAROOT/org/fresco/idl/* -rf
idl -sloppy_forward -DJacORB -all -d $JAVAROOT -I$IDLROOT $PACKAGES $METAIDL
find $JAVAROOT/org/fresco/idl/ -name "*.java" | xargs javac
