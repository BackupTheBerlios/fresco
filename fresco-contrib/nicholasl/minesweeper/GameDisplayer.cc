#include "GameDisplayer.hh"

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

void GameDisplayer::new_field(int width, int height, Fresco::Command_ptr cmd)
{
  Layout::Grid::Index index;
  index.col = width; index.row = height;
  _grid = layout->fixed_grid(index);

  for(int i = 0; i < width; i++) {
    index.col = i;
    for (int j = 0; j < height; j++) {
      index.row = j;
      Fresco::Trigger_var mine_button = widget->button(mines[0], cmd);
      CORBA::Any any;
      any <<= (CORBA::Long)(8*i+j);
      mine_button->payload(any);
#if 0
      /* stress-test the payload code */
      any <<= (CORBA::Long)(999);
      CORBA::Any_var any2 = mine_button->payload();
      CORBA::Long x;
      any2 >>= x;
      assert(x != 999);
      assert(x == 8*i+j);
#endif
      _grid->replace(mine_button, index);
    }
  }

  Fresco::GraphicIterator_var(active_graphic->last_child_graphic())->replace(_grid);
}
