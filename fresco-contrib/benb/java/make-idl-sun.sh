#adapt
METAIDL=/usr/src/berlin/fresco-contrib/benb/idl/fresco-meta.idl
IDLROOT=/usr/src/berlin/cvs-source/Fresco-IDL/share/idl/
JAVAROOT=/usr/src/java/corba/
IDLCOMPILER=/usr/lib/java2/bin/idlj

PACKAGES="-pkgTranslate Fresco org.fresco.idl.fresco -pkgTranslate Widget org.fresco.idl.widget -pkgTranslate Layout org.fresco.idl.layout -pkgTranslate Figure org.fresco.idl.figure -pkgTranslate Primitive org.fresco.idl.primitive -pkgTranslate GGI org.fresco.idl.ggi -pkgTranslate Unidraw org.fresco.idl.unidraw"

rm $JAVAROOT/org/fresco/idl/* -rf
$IDLCOMPILER -D SunJava -emitAll -keep -td $JAVAROOT -i $IDLROOT $PACKAGES $METAIDL
find $JAVAROOT/org/fresco/idl/ -name "*.java" | xargs javac -sourcepath $JAVAROOT
