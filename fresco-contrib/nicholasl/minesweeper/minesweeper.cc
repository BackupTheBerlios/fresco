#include <Fresco/config.hh>
#include <Fresco/resolve.hh>
#include "minesweeper.hh"

void SelectMine::execute(const CORBA::Any &a) {
  int x, y;
  CORBA::Long val;
  a >>= val;
  x = (int)floor(val/8.);
  y = val-(x*8);
  _context->expose(x, y);
}

class RestartGame : public virtual POA_Fresco::Command,
		    public virtual PortableServer::RefCountServantBase
{
public:
  RestartGame::RestartGame(GameContext *game) : _game(game) {}
  void RestartGame::execute(const CORBA::Any &) {
    _game->new_field(8, 8);
  }
  void destroy() {}
private:
  GameContext *_game;
};

int main(int argc, char **argv)
{
  Prague::GetOpt getopt(argv[0], "Fresco Minesweeper");
  getopt.add('h', "help", Prague::GetOpt::novalue, "help message");
  add_resolving_options_to_getopt(getopt);
  getopt.parse(argc, argv);
  if (getopt.is_set('h')) { getopt.usage(); exit(0); }

  CORBA::ORB_var orb;
  PortableServer::POA_var poa;
  PortableServer::POAManager_var pman;
  ClientContextImpl *client;
  Fresco::ServerContext_var server;

  try {
    orb = CORBA::ORB_init(argc, argv);
    poa = resolve_init<PortableServer::POA>(orb, "RootPOA");
    pman = poa->the_POAManager();
    pman->activate();

    client = new ClientContextImpl("Fresco Minesweeper");

    server = resolve_server(getopt, orb)->create_server_context(client->_this());
  } catch (CORBA::COMM_FAILURE c) {
    std::cerr << "Could not connect to the berlin server (CORBA::COMM_FAILURE)." << std::endl;
  }

  Fresco::DesktopKit_var desktop = resolve_kit<Fresco::DesktopKit>(server, "IDL:fresco.org/Fresco/DesktopKit:1.0");
  Fresco::ToolKit_var tool = resolve_kit<Fresco::ToolKit>(server, "IDL:fresco.org/Fresco/ToolKit:1.0");
  Fresco::LayoutKit_var layout = resolve_kit<Fresco::LayoutKit>(server, "IDL:fresco.org/Fresco/LayoutKit:1.0");
  Fresco::TextKit_var text = resolve_kit<Fresco::TextKit>(server, "IDL:fresco.org/Fresco/TextKit:1.0");
  Fresco::WidgetKit_var widget = resolve_kit<Fresco::WidgetKit>(server, "IDL:fresco.org/Fresco/WidgetKit:1.0");

  Fresco::Graphic_var vbox = layout->vbox();
  Fresco::ToolKit::FrameSpec spec;
  spec.brightness(0.5); spec._d(Fresco::ToolKit::outset);
  Fresco::Graphic_var window_frame = tool->frame(vbox, 20., spec, true);

  Fresco::Graphic_var vb = layout->vbox();

  GameDisplayer *display = new GameDisplayer(server);
  GameContext *game = new GameContext(display);

  RestartGame *restart_cmd = new RestartGame(game);
  Fresco::Graphic_var hbox = layout->hbox();
  Fresco::Graphic_var restart = tool->rgb(text->chunk(Unicode::to_CORBA(Babylon::String(":-)"))), 0., 0., 0.);
  Fresco::Trigger_var restart_button = widget->button(restart, (Fresco::Command_var)restart_cmd->_this());
  hbox->append_graphic(layout->hfill());
  hbox->append_graphic(layout->margin(restart_button, 20));
  hbox->append_graphic(layout->hfill());

  Fresco::MainController_var group = tool->group(Fresco::Graphic_var(layout->align(window_frame, 0., 0.)));

  CORBA::Any dummy; restart_cmd->execute(dummy);

  vbox->append_graphic(hbox);
  vbox->append_graphic(display->graphic());

  Fresco::Window_var window = desktop->shell(group, Fresco::ClientContext_var(client->_this()));
  orb->run();
}
