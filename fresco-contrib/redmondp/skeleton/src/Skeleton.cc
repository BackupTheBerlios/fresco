#include "Skeleton.hh"
#include <Fresco/Controller.hh>
#include <Fresco/Desktop.hh>
#include <Fresco/Image.hh>
#include <Fresco/MainController.hh>
#include <Fresco/resolve.hh>
#include <Fresco/Window.hh>

using namespace Prague;
using namespace Fresco;

Skeleton::Skeleton(ServerContext_ptr sc, ClientContext_ptr cc)
  : my_server(ServerContext::_duplicate(sc)),
    my_client(ClientContext::_duplicate(cc)),
    my_command(::resolve_kit<Fresco::CommandKit>(my_server,
      "IDL:fresco.org/Fresco/CommandKit:1.0")),
    my_desktop(::resolve_kit<Fresco::DesktopKit>(my_server, 
      "IDL:fresco.org/Fresco/DesktopKit:1.0")),
    my_figure(::resolve_kit<Fresco::FigureKit>(my_server, 
      "IDL:fresco.org/Fresco/FigureKit:1.0")),
    my_gadget(::resolve_kit<Fresco::GadgetKit>(my_server, 
      "IDL:fresco.org/Fresco/GadgetKit:1.0")),
    my_layout(::resolve_kit<Fresco::LayoutKit>(my_server, 
      "IDL:fresco.org/Fresco/LayoutKit:1.0")),
    my_raster(::resolve_kit<Fresco::RasterKit>(my_server, 
      "IDL:fresco.org/Fresco/RasterKit:1.0")),
    my_text(::resolve_kit<Fresco::TextKit>(my_server,
      "IDL:fresco.org/Fresco/TextKit:1.0")),
    my_tool(::resolve_kit<Fresco::ToolKit>(my_server,
      "IDL:fresco.org/Fresco/ToolKit:1.0")),
    my_widget(::resolve_kit<Fresco::WidgetKit>(my_server,
      "IDL:fresco.org/Fresco/WidgetKit:1.0"))
{
}

Skeleton::~Skeleton()
{
}

void Skeleton::run()
{
}

