#!/bin/sh

# Usage
#Edit the path vars in the Path section below and then run it where you
#extracted it, e.g. |./make.sh|.
#
#The IDL compilation is commented out, because there's already a package with
#precompiled stubs/skeletons. Uncomment it, if you want to generate them
#yourself.
#The J2EE stuff is commented out, too, because it is non-free.
#The Mailnews client won't work for you, because it depends on a patched
#Mozilla tree and some experimental OOUI code. Contact me, if you are seriously
#interested. The Java code will probably still be a good read anyway, even if
#you can't run it.
#
#You see that there're only 2 commands left - that's all that should be needed
#to do for you to get the little demo app running.
#
#(empty,
# see below)
#
# Paths
IDLDIR=/usr/src/berlin/cvs-source/idl
IDLJAVA=/usr/package/java/jdk_2_1_4_0/bin/idlj
#CLASSPATH_BASE=/usr/share/kaffe/Klasses.jar
#CLASSPATH_J2EE = /usr/local/j2sdkee1.3/lib/j2ee.jar
#
#(do not add anything here
# to avoid CVS merge conflicts with local pathname changes)
#
#For readability (you usually execute these commands manually), I use
#variables only for pathnames. Parameter names change anyway, if you use
#other compilers / runtime environments.
PACKAGES="-pkgPrefix Warsaw org.fresco -pkgPrefix Figure org.fresco -pkgPrefix GGI org.fresco -pkgPrefix Layout org.fresco -pkgPrefix Primitive org.fresco -pkgPrefix Unidraw org.fresco -pkgPrefix Widget org.fresco"

# Warsaw
#$IDLJAVA -fclient -emitAll -I$IDLDIR $PACKAGES $IDLDIR/Warsaw/meta-all.idl
#$IDLJAVA -fclient -fserver -emitAll -I$IDLDIR $PACKAGES $IDLDIR/Warsaw/meta-callback.idl
#(We need skeletons only for the latter interfaces)

#jikes -classpath $CLASSPATH_BASE:$CLASSPATH_ORB:. org/bucksch/fresco/demo/test/demo.java
#jikes -classpath $(CLASSPATH_BASE):$(CLASSPATH_ORB):$(CLASSPATH_J2EE):. org/bucksch/fresco/demo/html/viewer.java

# Run
#kaffe -addclasspath $CLASSPATH_ORB:. org.bucksch.fresco.demo.test.DemoApp
#kaffe -addclasspath $(CLASSPATH_ORB):$(CLASSPATH_J2EE):. org.bucksch.fresco.demo.html.Viewer http://yourserver/test/xhtml.xml
