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
  void expose(int x, int y);
private:
  bool is_exposed(int x, int y);
  void set_exposed(int x, int y);
  void expose2(int x, int y, vector<int> *to_expose);
  bool map[8][8];
  GameDisplayer *_gd;
  int _width;
  int _height;
  int *field;
};

#endif
