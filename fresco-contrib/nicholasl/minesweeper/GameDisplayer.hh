#ifndef __GAMEDISPLAYER__
#define __GAMEDISPLAYER__

#include <Fresco/config.hh>
#include <Fresco/resolve.hh>
#include <Fresco/Server.hh>

#include <Fresco/LayoutKit.hh>
#include <Fresco/ToolKit.hh>
#include <Fresco/WidgetKit.hh>
#include <Fresco/TextKit.hh>
#include <Fresco/Unicode.hh>
#include <Fresco/Command.hh>
#include <Fresco/Trigger.hh>
#include <Fresco/Grid.hh>
#include <Fresco/Graphic.hh>

class GameDisplayer {
public:
  GameDisplayer::GameDisplayer(Fresco::ServerContext_var _s);
  void expose(int x, int y, int type);
  //void expose_all();
  void new_field(int w, int h, Fresco::Command_ptr cmd);
  Fresco::Graphic_var graphic();
private:
  Layout::Grid_var _grid;
  // 0 is blank, 1-8 are the numbers, 9 is a mine and 10 is the flag
  Fresco::Graphic_var mines[11]; 
  Fresco::Graphic_var active_graphic;
  Fresco::LayoutKit_var layout;
  Fresco::ToolKit_var tool;
  Fresco::WidgetKit_var widget;
  Fresco::TextKit_var text;
};

#endif

