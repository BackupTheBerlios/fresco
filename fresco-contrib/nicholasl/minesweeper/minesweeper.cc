#include "minesweeper.hh"

void SelectMine::execute(const CORBA::Any &a) {
  int x, y;
  CORBA::Long val;
  a >>= val;
  x = (int)floor(val/8.);
  y = val-(x*8);
  _context->recurse(x, y);
}

int main(int argc, char **argv)
{
  CORBA::ORB_var orb;
  CosNaming::NamingContext_var context;
  PortableServer::POA_var poa;
  PortableServer::POAManager_var pman;
  ClientContextImpl *client;
  Fresco::ServerContext_var server;

  try {
    orb = CORBA::ORB_init(argc, argv);
    context = resolve_init<CosNaming::NamingContext>(orb, "NameService");
    poa = resolve_init<PortableServer::POA>(orb, "RootPOA");
    pman = poa->the_POAManager();
    pman->activate();

    client = new ClientContextImpl("Fresco Minesweeper");

    Fresco::Server_var s = resolve_name<Fresco::Server>(context, "IDL:fresco.org/Fresco/Server:1.0");
    server = s->create_server_context(Fresco::ClientContext_var(client->_this()));
  } catch (CORBA::COMM_FAILURE c) {
    std::cerr << "Could not connect to the berlin server (CORBA::COMM_FAILURE)." << std::endl;
  }

  Fresco::DesktopKit_var desktop = resolve_kit<Fresco::DesktopKit>(server, "IDL:fresco.org/Fresco/DesktopKit:1.0");
  Fresco::ToolKit_var tool = resolve_kit<Fresco::ToolKit>(server, "IDL:fresco.org/Fresco/ToolKit:1.0");
  Fresco::LayoutKit_var layout = resolve_kit<Fresco::LayoutKit>(server, "IDL:fresco.org/Fresco/LayoutKit:1.0");

  Fresco::Graphic_var vbox = layout->vbox();
  Fresco::ToolKit::FrameSpec spec;
  spec.brightness(0.5); spec._d(Fresco::ToolKit::outset);
  Fresco::Graphic_var window_frame = tool->frame(vbox, 20., spec, true);

  Fresco::Graphic_var vb = layout->vbox();
  Fresco::MainController_var group = tool->group(Fresco::Graphic_var(layout->align(window_frame, 0., 0.)));

  GameDisplayer *display = new GameDisplayer(server);
  GameContext *game = new GameContext(display);

  game->new_field(8, 8);
  vbox->append_graphic(display->graphic());

  Fresco::Window_var window = desktop->shell(group, Fresco::ClientContext_var(client->_this()));
  orb->run();
}
