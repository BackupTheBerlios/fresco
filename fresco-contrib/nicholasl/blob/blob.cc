#include <Fresco/config.hh>
#include <Fresco/resolve.hh>

#include <Fresco/TextKit.hh>
#include <Fresco/WidgetKit.hh>
#include <Fresco/CommandKit.hh>
#include <Fresco/DesktopKit.hh>
#include <Fresco/ToolKit.hh>
#include <Fresco/LayoutKit.hh>
#include <Fresco/CommandKit.hh>
#include <Fresco/Trigger.hh>
#include <Fresco/Server.hh>
#include <Fresco/ClientContextImpl.hh>
#include <Fresco/Unicode.hh>
#include <Fresco/MainController.hh>
#include <Fresco/DrawTraversal.hh>
#include <Fresco/PickTraversal.hh>
#include <Fresco/DrawingKit.hh>
#include <Fresco/Graphic.hh>
#include <Berlin/GraphicImpl.hh>
#include <Fresco/Region.hh>

#include <stdlib.h>
#include <math.h>

#include <sys/types.h>
#include <unistd.h>

using namespace Prague;
using namespace Fresco;

class Blob : public GraphicImpl
{
public:
  Blob() {}
  ~Blob() {}
  virtual void request(Fresco::Graphic::Requisition &requisition);
  virtual void draw(Fresco::DrawTraversal_ptr);
};

void Blob::request(Fresco::Graphic::Requisition &requisition)
{
 requisition.x.defined = true;
 requisition.x.natural = requisition.x.maximum = requisition.x.minimum = 1000.;
 requisition.x.align = 0.;
 requisition.y.defined = true;
 requisition.y.natural = requisition.y.maximum = requisition.y.minimum = 1000.;
 requisition.y.align = 0.;
}

void Blob::draw(DrawTraversal_ptr traversal)
{
  DrawingKit_var drawing = traversal->drawing();
  drawing->save();
  //std::cout << "client: " << getpid() << std::endl;
  Color green = {0., 1., 0., 1.};
  drawing->foreground(green);
  drawing->surface_fillstyle(DrawingKit::solid);
  //drawing->clipping(Region::_nil());
  int stepings = 3+(int)(12.*rand()/RAND_MAX);
  Path b;
  b.shape = non_convex;
  b.nodes.length(stepings);
  for (int i = 0; i < stepings; i++) {
    float theta = 2*3.14159*i/stepings;
    float radius = 100.+400.*rand()/RAND_MAX;
    b.nodes[i].x = radius*cos(theta);
    b.nodes[i].y = radius*sin(theta);
#if 0
    cout << radius << "cis(" << theta << ") -> ("
         << b.nodes[i].x << "," << b.nodes[i].y << ")" << endl;
#endif
    b.nodes[i].x += 500.;
    b.nodes[i].y += 500.;
  }
  drawing->draw_path(b);
  drawing->restore();
}

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

    client = new ClientContextImpl("The Blob");

    Server_var s = resolve_name<Server>(context, "IDL:fresco.org/Fresco/Server:1.0");
    server = s->create_server_context(ClientContext_var(client->_this()));
    DefaultPOA::default_POA(poa);
  } catch (CORBA::COMM_FAILURE c) {
    std::cerr << "Could not connect to the berlin server (CORBA::COMM_FAILURE)." << std::endl;
  }

  DesktopKit_var desktop = resolve_kit<DesktopKit>(server, "IDL:fresco.org/Fresco/DesktopKit:1.0");
  TextKit_var text = resolve_kit<TextKit>(server, "IDL:fresco.org/Fresco/TextKit:1.0");
  WidgetKit_var widget = resolve_kit<WidgetKit>(server, "IDL:fresco.org/Fresco/WidgetKit:1.0");
  ToolKit_var tool = resolve_kit<ToolKit>(server, "IDL:fresco.org/Fresco/ToolKit:1.0");
  LayoutKit_var layout = resolve_kit<LayoutKit>(server, "IDL:fresco.org/Fresco/LayoutKit:1.0");
  CommandKit_var command = resolve_kit<CommandKit>(server, "IDL:fresco.org/Fresco/CommandKit:1.0");
  Graphic_var glyph = text->chunk(Unicode::to_CORBA(Babylon::String("Exit.")));

  ExitCommand *exit_cmd = new ExitCommand();
  Trigger_var button = widget->button(Graphic_var(tool->rgb(glyph, 0., 0., 0.)), Command_var(exit_cmd->_this()));
  Graphic_var margin_text = layout->margin(button, 50.);

  Blob *blob = new Blob();
  poa->activate_object(blob);

  Graphic_var vb = layout->vbox();
  ToolKit::FrameSpec spec;
  spec.brightness(0.5); spec._d(ToolKit::outset);
  Graphic_var decorator = tool->frame(vb, 20., spec, true);
  vb->append_graphic(blob->_this());
  vb->append_graphic(margin_text);
  MainController_var group = tool->group(Graphic_var(layout->align(decorator, 0., 0.)));
  group->append_controller(button);
  Window_var window = desktop->shell(group, ClientContext_var(client->_this()));
  desktop->map(window, true);
  while (true) Thread::delay(Prague::Time(1000));
}
