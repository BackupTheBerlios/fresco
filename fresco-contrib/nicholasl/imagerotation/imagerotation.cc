#include <Warsaw/config.hh>
#include <Warsaw/resolve.hh>

#include <Warsaw/TextKit.hh>
#include <Warsaw/WidgetKit.hh>
#include <Warsaw/CommandKit.hh>
#include <Warsaw/DesktopKit.hh>
#include <Warsaw/ToolKit.hh>
#include <Warsaw/LayoutKit.hh>
#include <Warsaw/GadgetKit.hh>
#include <Warsaw/CommandKit.hh>
#include <Warsaw/ImageKit.hh>
#include <Warsaw/FigureKit.hh>
#include <Warsaw/Trigger.hh>
#include <Warsaw/Server.hh>
#include <Warsaw/ClientContextImpl.hh>
#include <Warsaw/Unicode.hh>
#include <Warsaw/MainController.hh>
#include <Warsaw/BoundedValue.hh>
#include <Warsaw/Image.hh>

using namespace Prague;
using namespace Warsaw;

class ExitCommand : public virtual POA_Warsaw::Command,
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
    assert(orb);
    context = resolve_init<CosNaming::NamingContext>(orb, "NameService");
    assert(context);
    poa = resolve_init<PortableServer::POA>(orb, "RootPOA");
    assert(poa);
    pman = poa->the_POAManager();
    assert(pman);
    pman->activate();

    client = new ClientContextImpl("Test");
    assert(client);

    Server_var s = resolve_name<Server>(context, "IDL:Warsaw/Server:1.0");
    assert(s);
    server = s->create_server_context(ClientContext_var(client->_this()));
    assert(server);
  } catch (CORBA::COMM_FAILURE c) {
    std::cerr << "Could not connect to the berlin server (CORBA::COMM_FAILURE)." << std::endl;
  }

  DesktopKit_var desktop = resolve_kit<DesktopKit>(server, "IDL:Warsaw/DesktopKit:1.0");
  assert(desktop);
  TextKit_var text = resolve_kit<TextKit>(server, "IDL:Warsaw/TextKit:1.0");
  assert(text);
  WidgetKit_var widget = resolve_kit<WidgetKit>(server, "IDL:Warsaw/WidgetKit:1.0");
  assert(widget);
  ToolKit_var tool = resolve_kit<ToolKit>(server, "IDL:Warsaw/ToolKit:1.0");
  assert(tool);
  LayoutKit_var layout = resolve_kit<LayoutKit>(server, "IDL:Warsaw/LayoutKit:1.0");
  assert(layout);
  GadgetKit_var gadget = resolve_kit<GadgetKit>(server, "IDL:Warsaw/GadgetKit:1.0");
  assert(gadget);
  CommandKit_var command = resolve_kit<CommandKit>(server, "IDL:Warsaw/CommandKit:1.0");
  assert(command);
  ImageKit_var image = resolve_kit<ImageKit>(server, "IDL:Warsaw/ImageKit:1.0");
  assert(image);
  FigureKit_var figure = resolve_kit<FigureKit>(server, "IDL:Warsaw/FigureKit:1.0");
  assert(figure);

  Graphic_var glyph = text->chunk(Unicode::to_CORBA(Babylon::String("Hello world!")));
  assert(glyph);
  ExitCommand *exit_cmd = new ExitCommand();
  assert(exit_cmd);
  Trigger_var button = widget->button(Graphic_var(tool->rgb(glyph, 0., 0., 0.)), Command_var(exit_cmd->_this()));
  assert(button);
  Graphic_var margin_text = layout->margin(button, 50.);
  assert(margin_text);

  Raster_var raster = image->create("berlin-48.png");
  Image_var pixmap = figure->pixmap(raster);
  Graphic_var margin_image = layout->margin(pixmap, 50.);

  Graphic_var vb = layout->vbox();
  assert(vb);
  ToolKit::FrameSpec spec;
  spec.brightness(0.5); spec._d(ToolKit::outset);
  BoundedValue_var bv = command->bvalue(0., 360., 60, 0.01, 0.1);
  //BoundedValue_var bv = _commands->bvalue(0., 1., 0., 0.01, 0.1);
  Graphic_var rot = gadget->rotator(vb, bv, zaxis);
  bv->adjust(0.5);
  Graphic_var decorator = tool->frame(rot, 20., spec, true);
  assert(decorator);
  vb->append_graphic(margin_text);
  vb->append_graphic(margin_image);
  MainController_var group = tool->group(Graphic_var(layout->align(decorator, 0., 0.)));
  assert(group);
  group->append_controller(button);
  Window_var window = desktop->shell(group, ClientContext_var(client->_this()));
  assert(window);
  desktop->map(window, true);
  while (true) Thread::delay(Prague::Time(1000));
}
