#! /bin/sh
#source ../fresco-config
#CPP="g++-3.2"
#$CPP $CPPFLAGS $CXXFLAGS painters.cc -o painters $LIBFLAGS
c++ -D_REENTRANT -I/home/nicholas/Fresco/Prague/include -I/home/nicholas/Fresco/Prague/include -I/home/nicholas/Fresco/Babylon/include -I/home/nicholas/Fresco/Babylon/include -I/home/nicholas/Fresco/include painters.cc -o painters -L/home/nicholas/Fresco/lib -lFresco -lomnithread -ltcpwrapGK -lomniORB3 -lomniDynamic3 -lpthread -L/home/nicholas/Fresco/Babylon/lib -lBabylon -L/home/nicholas/Fresco/Prague/lib -lPrague -ldl -lpthread -lutil
