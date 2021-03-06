#include <Fresco/config.hh>
#include <Fresco/resolve.hh>

#include <Fresco/TextKit.hh>
#include <Fresco/WidgetKit.hh>
#include <Fresco/CommandKit.hh>
#include <Fresco/DesktopKit.hh>
#include <Fresco/ToolKit.hh>
#include <Fresco/LayoutKit.hh>
#include <Fresco/Trigger.hh>
#include <Fresco/Server.hh>
#include <Fresco/ClientContextImpl.hh>
#include <Fresco/Unicode.hh>
#include <Fresco/MainController.hh>
#include <Fresco/DrawTraversal.hh>
#include <Fresco/PickTraversal.hh>
#include <Fresco/DrawingKit.hh>
#include <Fresco/Graphic.hh>
#include <Fresco/Region.hh>
#include <Fresco/BoundedValue.hh>
#include <Fresco/View.hh>

#include <Berlin/GraphicImpl.hh>
#include <Berlin/ObserverImpl.hh>
#include <Berlin/ViewImpl.hh>

#include <stdlib.h>
#include <math.h>

#include <sys/types.h>
#include <unistd.h>

using namespace Prague;
using namespace Fresco;

class Gauge : public virtual ViewImpl, public GraphicImpl
{
public:
  Gauge(Fresco::BoundedValue_ptr v);
  ~Gauge();
  virtual void request(Fresco::Graphic::Requisition &requisition);
  virtual void draw(Fresco::DrawTraversal_ptr);
  virtual void update(const CORBA::Any &any);
private:
  Fresco::BoundedValue_var _value;
};

Gauge::Gauge(Fresco::BoundedValue_ptr v)
  : _value(Fresco::BoundedValue::_duplicate(v))
{
  DefaultPOA::_default_POA()->activate_object(this);
  _value->attach(Observer_var(this->_this()));
}

Gauge::~Gauge()
{
  _value->detach(Observer_var(this->_this()));
}

void Gauge::update(const CORBA::Any &any) {
  need_redraw();
}

void Gauge::request(Fresco::Graphic::Requisition &requisition)
{
 requisition.x.defined = true;
 requisition.x.minimum = 2000.;
 requisition.x.natural = 2000.;
 //requisition.x.maximum = 0.;
 requisition.x.align = 0.;
 requisition.y.defined = true;
 requisition.y.minimum = 50.;
 requisition.y.natural = 50.;
 //requisition.y.maximum = 0.;
 requisition.y.align = 0.;
}

void Gauge::draw(DrawTraversal_ptr traversal)
{
  Region_var allocation = traversal->current_allocation();  
  Vertex upper, lower;
  allocation->bounds(upper, lower);
  DrawingKit_var drawing = traversal->drawing();
  drawing->save();
  drawing->surface_fillstyle(DrawingKit::solid);
  Color bg = {1., 1., 1., 1.};
  Color fg = {0., 0., 1., 1.};
  drawing->foreground(bg);
  drawing->draw_rectangle(upper, lower);
  lower.x *= (_value->value()-_value->lower())/(_value->upper()-_value->lower());
  drawing->foreground(fg);
  drawing->draw_rectangle(upper, lower);
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

    client = new ClientContextImpl("Linear Gauge");

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

  Fresco::BoundedValue_var value = command->bvalue(0., 1., 0., 0.01, 0.1);
  Gauge *gauge = new Gauge(value);
  ToolKit::FrameSpec spec;
  spec.brightness(0.5); spec._d(ToolKit::inset);
  Graphic_var frame = tool->frame(gauge->_this(), 20., spec, true);

  Graphic_var vb = layout->vbox();
  //vb->append_graphic(layout->vfill());
  //vb->append_graphic(layout->hfill());
  vb->append_graphic(frame);
  //vb->append_graphic(layout->vfill());
  vb->append_graphic(margin_text);
  spec._d(ToolKit::outset);
  Graphic_var decorator = tool->frame(vb, 20., spec, true);
  MainController_var group = tool->group(Graphic_var(layout->align(decorator, 0., 0.)));
  group->append_controller(button);
  Window_var window = desktop->shell(group, ClientContext_var(client->_this()));
  //orb->run();

  bool increment = true;
  while (true) {
    if (increment) { value->forward(); } else { value->backward(); }
    if (value->value() >= value->upper()) { increment = false; }
    if (value->value() <= value->lower()) { increment = true; }
    Thread::delay(Prague::Time(99));
    gauge->need_redraw();
  }
}
