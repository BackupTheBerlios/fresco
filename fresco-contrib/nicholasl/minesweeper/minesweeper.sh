#! /bin/sh
source ../fresco-config
$CPP $CPPFLAGS $CXXFLAGS -c TerrainGenerator.cc -o TerrainGenerator.o
$CPP $CPPFLAGS $CXXFLAGS -c GameDisplayer.cc -o GameDisplayer.o
$CPP $CPPFLAGS $CXXFLAGS -c GameContext.cc -o GameContext.o
$CPP $CPPFLAGS $CXXFLAGS -c minesweeper.cc -o minesweeper.o
$CPP $CPPFLAGS $CXXFLAGS -o minesweeper TerrainGenerator.o GameDisplayer.o GameContext.o minesweeper.o $LIBFLAGS
