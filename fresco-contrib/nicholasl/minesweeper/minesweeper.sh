#! /bin/sh
source ../fresco-config
omniidl -bcxx -Wba field.idl
$CPP $CPPFLAGS $CXXFLAGS -c fieldSK.cc -o fieldSK.o
$CPP $CPPFLAGS $CXXFLAGS -c fieldDynSK.cc -o fieldDynSK.o
$CPP $CPPFLAGS $CXXFLAGS -c TerrainGenerator.cc -o TerrainGenerator.o
$CPP $CPPFLAGS $CXXFLAGS -c GameDisplayer.cc -o GameDisplayer.o
$CPP $CPPFLAGS $CXXFLAGS -c GameContext.cc -o GameContext.o
$CPP $CPPFLAGS $CXXFLAGS -c minesweeper.cc -o minesweeper.o
$CPP $CPPFLAGS $CXXFLAGS -o minesweeper fieldSK.o fieldDynSK.o TerrainGenerator.o GameDisplayer.o GameContext.o minesweeper.o $LIBFLAGS
