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
#include <Fresco/BoundedValue.hh>

#include <Prague/Sys/Timer.hh>

#include <sys/time.h>
#include <time.h>

using namespace Prague;
using namespace Fresco;

class ExitCommand : public virtual POA_Fresco::Command,
                    public virtual PortableServer::RefCountServantBase
{
 public:
  void execute(const CORBA::Any &) { exit(0); }
  void destroy() {}
};

class Update : public virtual Prague::Timer::Notifier
{
public:
  Update::Update(Graphic_var g, TextKit_var t)
    : glyph(g), text(t) {}
  ~Update() {}
  void notify()
  {
    Prague::Time now = Prague::Time::currentTime();
    time_t t = time(0);
    Graphic_var p = text->chunk(Unicode::to_CORBA(Babylon::String(asctime(localtime(&t)))));
    glyph->body(p);
    glyph->need_resize();
    std::cerr << ".";
  }
private:
  Graphic_var glyph;
  TextKit_var text;
};

int main(int argc, char **argv)
{
  CORBA::ORB_var orb;
  PortableServer::POA_var poa;
  PortableServer::POAManager_var pman;
  ClientContextImpl *client;
  ServerContext_var server;

  try {
    Prague::GetOpt getopt(argv[0], "Fresco Time");
    add_resolving_options_to_getopt(getopt);
    orb = CORBA::ORB_init(argc, argv);
    poa = resolve_init<PortableServer::POA>(orb, "RootPOA");
    pman = poa->the_POAManager();
    pman->activate();

    client = new ClientContextImpl("Time");

    Server_var s = resolve_server(getopt, orb);
    server = s->create_server_context(ClientContext_var(client->_this()));
  } catch (CORBA::COMM_FAILURE c) {
    std::cerr << "Could not connect to the berlin server (CORBA::COMM_FAILURE)." << std::endl;
    exit(0);
  } catch (const std::exception &e) {
    std::cerr << "Failed by exception: " << e.what() << std::endl;
    exit(0);
  } catch (...) {
    std::cerr << "Failed by exception. Have a debugger break on __throw."
              << std::endl;
    exit(0);
  }

  DesktopKit_var desktop = resolve_kit<DesktopKit>(server, "IDL:fresco.org/Fresco/DesktopKit:1.0");
  TextKit_var text = resolve_kit<TextKit>(server, "IDL:fresco.org/Fresco/TextKit:1.0");
  WidgetKit_var widget = resolve_kit<WidgetKit>(server, "IDL:fresco.org/Fresco/WidgetKit:1.0");
  ToolKit_var tool = resolve_kit<ToolKit>(server, "IDL:fresco.org/Fresco/ToolKit:1.0");
  LayoutKit_var layout = resolve_kit<LayoutKit>(server, "IDL:fresco.org/Fresco/LayoutKit:1.0");
  Graphic_var glyph = text->chunk(Unicode::to_CORBA(Babylon::String("Quit")));
  ExitCommand *exit_cmd = new ExitCommand();
  Trigger_var button = widget->button(Graphic_var(tool->rgb(glyph, 0., 0., 0.)), Command_var(exit_cmd->_this()));
  Graphic_var margin_text = layout->margin(button, 50.);

  Graphic_var time = tool->rgb(Graphic::_nil(), 0, 0, 0);

  Graphic_var hb = layout->hbox();
  ToolKit::FrameSpec spec;
  spec.brightness(0.5); spec._d(ToolKit::outset);
  Graphic_var decorator = tool->frame(hb, 20., spec, true);
  hb->append_graphic(time);
  hb->append_graphic(margin_text);
  MainController_var group = tool->group(Graphic_var(layout->align(decorator, 0., 0.)));
  group->append_controller(button);
  Window_var window = desktop->shell(group, ClientContext_var(client->_this()));
  desktop->map(window, true);

  Update *update = new Update(time, text);
  Prague::Timer *timer = new Prague::Timer(update);
  timer->start(Prague::Time(0), Prague::Time(1000));

  orb->run();
}
