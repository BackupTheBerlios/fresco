#include <Fresco/Command.hh>
#include "GameContext.hh"

GameContext::GameContext(GameDisplayer *display)
  : _gd(display)
{
  field = (int*)malloc(64 * sizeof(int));
  *field = 0;
  *(field+1) = 0;
  *(field+2) = 1;
  *(field+3) = 9;
  *(field+4) = 1;
  *(field+5) = 0;
  *(field+6) = 2;
  *(field+7) = 9;

  *(field+8) = 0;
  *(field+9) = 0;
  *(field+10) = 1;
  *(field+11) = 1;
  *(field+12) = 1;
  *(field+13) = 0;
  *(field+14) = 3;
  *(field+15) = 9;

  *(field+16) = 0;
  *(field+17) = 0;
  *(field+18) = 0;
  *(field+19) = 0;
  *(field+20) = 0;
  *(field+21) = 1;
  *(field+22) = 3;
  *(field+23) = 9;

  *(field+24) = 0;
  *(field+25) = 0;
  *(field+26) = 0;
  *(field+27) = 0;
  *(field+28) = 0;
  *(field+29) = 1;
  *(field+30) = 9;
  *(field+31) = 2;

  *(field+32) = 0;
  *(field+33) = 0;
  *(field+34) = 0;
  *(field+35) = 0;
  *(field+36) = 1;
  *(field+37) = 2;
  *(field+38) = 2;
  *(field+39) = 1;

  *(field+40) = 1;
  *(field+41) = 1;
  *(field+42) = 0;
  *(field+43) = 1;
  *(field+44) = 2;
  *(field+45) = 9;
  *(field+46) = 1;
  *(field+47) = 0;

  *(field+48) = 9;
  *(field+49) = 2;
  *(field+50) = 0;
  *(field+51) = 1;
  *(field+52) = 9;
  *(field+53) = 3;
  *(field+54) = 2;
  *(field+55) = 1;

  *(field+56) = 9;
  *(field+57) = 2;
  *(field+58) = 0;
  *(field+59) = 1;
  *(field+60) = 1;
  *(field+61) = 2;
  *(field+62) = 9;
  *(field+63) = 1;

  memset(map, 0, sizeof(bool) * 8 * 8);
}

GameContext::~GameContext()
{
  free(field);
}

void GameContext::new_field(int w, int h)
{
  _width = w;
  _height = h;

  SelectMine *sm = new SelectMine(_gd, this);
  Fresco::Command_var sm_cmd = Fresco::Command_var(sm->_this());

  _gd->new_field(_width, _height, sm_cmd);
}

bool GameContext::is_exposed(int x, int y)
{
  return map[x][y];
}

void GameContext::set_exposed(int x, int y)
{
  map[x][y] = true;
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
  std::cerr << "(" << x << "," << y << ")" << std::endl;
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
