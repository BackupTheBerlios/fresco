#include <Fresco/config.hh>
#include <Fresco/resolve.hh>

#include <Fresco/WidgetKit.hh>
#include <Fresco/CommandKit.hh>
#include <Fresco/DesktopKit.hh>
#include <Fresco/ToolKit.hh>
#include <Fresco/LayoutKit.hh>
#include <Fresco/GadgetKit.hh>
#include <Fresco/ImageKit.hh>
#include <Fresco/FigureKit.hh>
#include <Fresco/Trigger.hh>
#include <Fresco/Server.hh>
#include <Fresco/ClientContextImpl.hh>
#include <Fresco/Unicode.hh>
#include <Fresco/MainController.hh>
#include <Fresco/BoundedValue.hh>
#include <Fresco/Image.hh>

#include "debug.hh"

using namespace Prague;
using namespace Fresco;

int main(int argc, char **argv)
{
  CORBA::ORB_var orb;
  CosNaming::NamingContext_var context;
  PortableServer::POA_var poa;
  PortableServer::POAManager_var pman;
  ClientContextImpl *client;
  ServerContext_var server;

  try {
    orb = CORBA::ORB_init(argc, argv);
    context = resolve_init<CosNaming::NamingContext>(orb, "NameService");
    poa = resolve_init<PortableServer::POA>(orb, "RootPOA");
    pman = poa->the_POAManager();
    pman->activate();

    client = new ClientContextImpl("Debug");

    Server_var s = resolve_name<Server>(context, "IDL:fresco.org/Fresco/Server:1.0");
    server = s->create_server_context(ClientContext_var(client->_this()));
    DefaultPOA::default_POA(poa);
  } catch (CORBA::COMM_FAILURE c) {
    std::cerr << "Could not connect to the berlin server (CORBA::COMM_FAILURE)." << std::endl;
  }

  DesktopKit_var desktop = resolve_kit<DesktopKit>(server, "IDL:fresco.org/Fresco/DesktopKit:1.0");
  WidgetKit_var widget = resolve_kit<WidgetKit>(server, "IDL:fresco.org/Fresco/WidgetKit:1.0");
  ToolKit_var tool = resolve_kit<ToolKit>(server, "IDL:fresco.org/Fresco/ToolKit:1.0");
  LayoutKit_var layout = resolve_kit<LayoutKit>(server, "IDL:fresco.org/Fresco/LayoutKit:1.0");
  GadgetKit_var gadget = resolve_kit<GadgetKit>(server, "IDL:fresco.org/Fresco/GadgetKit:1.0");
  CommandKit_var command = resolve_kit<CommandKit>(server, "IDL:fresco.org/Fresco/CommandKit:1.0");
  ImageKit_var image = resolve_kit<ImageKit>(server, "IDL:fresco.org/Fresco/ImageKit:1.0");
  FigureKit_var figure = resolve_kit<FigureKit>(server, "IDL:fresco.org/Fresco/FigureKit:1.0");
  TextKit_var text = resolve_kit<TextKit>(server, "IDL:fresco.org/Fresco/TextKit:1.0");

  Raster_var raster = image->create("fresco.png");
  Image_var pixmap = figure->pixmap(raster);
  Graphic_var margin_image = layout->margin(pixmap, 50.);

  DebugConsole *console = new DebugConsole(client, server);

  DebugController *debugcontroller = new DebugController("Controller", console);
  poa->activate_object(debugcontroller);
  Controller_var x = debugcontroller->_this();
  x->body(margin_image);

  Graphic_var vbox = layout->vbox();
  vbox->append_graphic(x);
  Graphic_var glyph = text->chunk(Unicode::to_CORBA(Babylon::String("Close.")));
  ExitCommand *exit_cmd = new ExitCommand();
  Trigger_var button = widget->button(Graphic_var(tool->rgb(glyph, 0., 0., 0.)), Command_var(exit_cmd->_this()));
  vbox->append_graphic(button);

  MainController_var group = tool->group(Graphic_var(layout->align(vbox, 0., 0.)));
  group->append_controller(button);
  Window_var window = desktop->shell(group, ClientContext_var(client->_this()));
  desktop->map(window, true);

  orb->run();
}
