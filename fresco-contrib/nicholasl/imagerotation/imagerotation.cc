#include <Fresco/config.hh>
#include <Fresco/resolve.hh>

#include <Fresco/TextKit.hh>
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

using namespace Prague;
using namespace Fresco;

class ExitCommand : public virtual POA_Fresco::Command,
		    public virtual PortableServer::RefCountServantBase
{
 public:
  void execute(const CORBA::Any &) { exit(0); }
  void destroy() {}
};

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

    client = new ClientContextImpl("Rotated");

    Server_var s = resolve_name<Server>(context, "IDL:fresco.org/Fresco/Server:1.0");
    server = s->create_server_context(ClientContext_var(client->_this()));
  } catch (CORBA::COMM_FAILURE c) {
    std::cerr << "Could not connect to the berlin server (CORBA::COMM_FAILURE)." << std::endl;
  }

  DesktopKit_var desktop = resolve_kit<DesktopKit>(server, "IDL:fresco.org/Fresco/DesktopKit:1.0");
  TextKit_var text = resolve_kit<TextKit>(server, "IDL:fresco.org/Fresco/TextKit:1.0");
  WidgetKit_var widget = resolve_kit<WidgetKit>(server, "IDL:fresco.org/Fresco/WidgetKit:1.0");
  ToolKit_var tool = resolve_kit<ToolKit>(server, "IDL:fresco.org/Fresco/ToolKit:1.0");
  LayoutKit_var layout = resolve_kit<LayoutKit>(server, "IDL:fresco.org/Fresco/LayoutKit:1.0");
  GadgetKit_var gadget = resolve_kit<GadgetKit>(server, "IDL:fresco.org/Fresco/GadgetKit:1.0");
  CommandKit_var command = resolve_kit<CommandKit>(server, "IDL:fresco.org/Fresco/CommandKit:1.0");
  ImageKit_var image = resolve_kit<ImageKit>(server, "IDL:fresco.org/Fresco/ImageKit:1.0");
  FigureKit_var figure = resolve_kit<FigureKit>(server, "IDL:fresco.org/Fresco/FigureKit:1.0");

  Graphic_var glyph = text->chunk(Unicode::to_CORBA(Babylon::String("Hello world!")));
  ExitCommand *exit_cmd = new ExitCommand();
  Trigger_var button = widget->button(Graphic_var(tool->rgb(glyph, 0., 0., 0.)), Command_var(exit_cmd->_this()));
  Graphic_var margin_text = layout->margin(button, 50.);

  Raster_var raster = image->create("berlin-48.png");
  Image_var pixmap = figure->pixmap(raster);
  Graphic_var margin_image = layout->margin(pixmap, 50.);

  Graphic_var vb = layout->vbox();
  ToolKit::FrameSpec spec;
  spec.brightness(0.5); spec._d(ToolKit::outset);
  BoundedValue_var bv = command->bvalue(0., 360., 60, 0.01, 0.1);
  //BoundedValue_var bv = _commands->bvalue(0., 1., 0., 0.01, 0.1);
  Graphic_var rot = gadget->rotator(vb, bv, zaxis);
  bv->adjust(0.5);
  Graphic_var decorator = tool->frame(rot, 20., spec, true);
  vb->append_graphic(margin_text);
  vb->append_graphic(margin_image);
  MainController_var group = tool->group(Graphic_var(layout->align(decorator, 0., 0.)));
  group->append_controller(button);
  Window_var window = desktop->transient(group);//, ClientContext_var(client->_this()));
  Command_var mapper = desktop->map(window, true);
  CORBA::Any x; mapper->execute(x);
  while (true) Thread::delay(Prague::Time(1000));
}
