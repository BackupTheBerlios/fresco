#include <Fresco/Command.hh>
#include "GameContext.hh"

GameContext::GameContext(GameDisplayer *display)
  : _gd(display), field(0), map(0)
{
}

GameContext::~GameContext()
{
  if (field) free(field);
  if (map) free (map);
}

void GameContext::new_field(int w, int h)
{
  _width = w;
  _height = h;

  SelectMine *sm = new SelectMine(_gd, this);
  Fresco::Command_var sm_cmd = Fresco::Command_var(sm->_this());

  if (field) free(field);
  field = cook_terrain(_width, _height, 10);
  if (map) free(map);
  map = (int*)calloc(sizeof(int), _width * _height);
  _gd->new_field(_width, _height, sm_cmd);
}

bool GameContext::is_exposed(int x, int y)
{
  return *(map+x+_width*y) == 1;
}

void GameContext::set_exposed(int x, int y)
{
  *(map+x+_width*y) = 1;
}

void GameContext::expose(int x, int y)
{
  vector<int> need_expose;
  need_expose.push_back(x*_width+y);
  while (need_expose.size()) {
    int coord = need_expose.back();
    need_expose.pop_back();
    int coord_x = (int)floor(coord/_width);
    int coord_y = coord-(coord_x*_width);
    expose2(coord_x, coord_y, &need_expose);
  }
}

void GameContext::expose2(int x, int y, vector<int> *need_expose) {
  if (is_exposed(x, y)) { return;}

  int i = field[x*_width+y];
  _gd->expose(x, y, i);
  set_exposed(x, y);
  if (i == 0) {
    if (x > 0) {
      if (!is_exposed(x-1, y))
	need_expose->push_back((x-1)*_width+y);
      if (y > 0)
	if (!is_exposed(x-1, y-1))
	  need_expose->push_back((x-1)*_width+(y-1));
      if (y < _height-1)
	if (!is_exposed(x-1, y+1))
	  need_expose->push_back((x-1)*_width+(y+1));
    }
    
    if (y > 0)
      if (!is_exposed(x, y-1))
	need_expose->push_back(x*_width+(y-1));
    if (y < _height-1)
      if (!is_exposed(x, y+1))
	need_expose->push_back(x*_width+(y+1));
    
    if (x < _width-1) {
      if (!is_exposed(x+1, y))
	need_expose->push_back((x+1)*_width+y);
      if (y > 0)
	if (!is_exposed(x+1, y-1))
	  need_expose->push_back((x+1)*_width+(y-1));
      if (y < _height-1)
	if (!is_exposed(x+1, y+1))
	  need_expose->push_back((x+1)*_width+(y+1));
    }
  }
}
