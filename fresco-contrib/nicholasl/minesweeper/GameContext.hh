#ifndef __GAMECONTEXT__
#define __GAMECONTEXT__

class GameContext;

#include "GameDisplayer.hh"
#include "minesweeper.hh"

class GameContext {
public:
  GameContext::GameContext(GameDisplayer *);
  GameContext::~GameContext();
  void new_field(int width, int height);
  void recurse(int x, int y);
  CORBA::Boolean is_exposed(int x, int y);
private:
  CORBA::Boolean map[8][8];
  GameDisplayer *_gd;
  int _width;
  int _height;
  int *field;
};

#endif
