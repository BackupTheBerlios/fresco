#include "GameDisplayer.hh"
#include "field.hh"

GameDisplayer::GameDisplayer(Fresco::ServerContext_var server)
{
  layout = resolve_kit<Fresco::LayoutKit>(server, "IDL:fresco.org/Fresco/LayoutKit:1.0");
  tool = resolve_kit<Fresco::ToolKit>(server, "IDL:fresco.org/Fresco/ToolKit:1.0");
  widget = resolve_kit<Fresco::WidgetKit>(server, "IDL:fresco.org/Fresco/WidgetKit:1.0");
  text = resolve_kit<Fresco::TextKit>(server, "IDL:fresco.org/Fresco/TextKit:1.0");

  active_graphic = layout->hbox();
  active_graphic->append_graphic(layout->align(Fresco::Graphic::_nil(), 0., 0.));

  Fresco::Coord x_size = 200.;
  Fresco::Coord y_size = 200.;
  mines[0] = layout->fixed_size(Fresco::Graphic::_nil(), x_size, y_size);
  mines[1] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("1"))), 0., 0., 1.), 0., 0.), x_size, y_size);
  mines[2] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("2"))), 0., 0.545, 0.), 0., 0.), x_size, y_size);
  mines[3] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("3"))), 1., 0., 0.), 0., 0.), x_size, y_size);
  mines[4] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("4"))), 0., 0., 0.545), 0., 0.), x_size, y_size);
  mines[5] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("5"))), 0.545, 0., 0.), 0., 0.), x_size, y_size);
  mines[6] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("6"))), 0.545, 0.545, 0.), 0., 0.), x_size, y_size);
  mines[7] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("7"))), 0.545, 0., 0.545), 0., 0.), x_size, y_size);
  mines[8] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("8"))), 0., 0., 0.), 0., 0.), x_size, y_size);
  mines[9] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("*"))), 0., 0., 0.), 0., 0.), x_size, y_size);
  mines[10] = layout->fixed_size(layout->align(tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String("f"))), 0., 0., 0.), 0., 0.), x_size, y_size);
}

Fresco::Graphic_var GameDisplayer::graphic()
{
  return active_graphic;
}

void GameDisplayer::expose(int x, int y, int type)
{
  Layout::Grid::Index i;
  i.col = x; i.row = y;
  _grid->replace(mines[type], i);
}

void GameDisplayer::force_update() { // XXX
  _grid->need_redraw();
}

void GameDisplayer::new_field(int width, int height, Fresco::Command_ptr cmd)
{
  Layout::Grid::Index index;
  index.col = width; index.row = height;
  _grid = layout->fixed_grid(index);

  for(int x = 0; x < width; x++) {
    index.col = x;
    for (int y = 0; y < height; y++) {
      index.row = y;
      Fresco::Trigger_var mine_button = widget->button(mines[0], cmd);
      CORBA::Any any;
      Minesweeper::Index *p = new Minesweeper::Index;
      p->x = x; p->y = y;
      any <<= p;
      mine_button->payload(any);
      _grid->replace(mine_button, index);
    }
  }

  Fresco::GraphicIterator_var(active_graphic->last_child_graphic())->replace(_grid);
}
