#include "Teapot.hh"
#include <Fresco/Controller.hh>
#include <Fresco/Desktop.hh>
#include <Fresco/Image.hh>
#include <Fresco/MainController.hh>
#include <Fresco/resolve.hh>
#include <Fresco/Window.hh>

#include "Model.hh"

using namespace Prague;
using namespace Fresco;

Teapot::Teapot(ServerContext_ptr sc, ClientContext_ptr cc)
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
    my_widget(::resolve_kit<Fresco::WidgetKit>(my_server,
      "IDL:fresco.org/Fresco/WidgetKit:1.0")),
    my_prim(::resolve_kit<Fresco::PrimitiveKit>(my_server,
      "IDL:fresco.org/Fresco/PrimitiveKit:1.0")),
    my_tool(::resolve_kit<Fresco::ToolKit>(my_server,
      "IDL:fresco.org/Fresco/ToolKit:1.0"))
    
{
}

Teapot::~Teapot()
{
}

void Teapot::run()
{
  Fresco::Mesh mesh;
  mesh.nodes.length(NUM_VERTS);
  for (int i = 0; i < NUM_VERTS; i++)
  {
    mesh.nodes[i].x = vertices[i].xyz[0];
    mesh.nodes[i].y = vertices[i].xyz[1];
    mesh.nodes[i].z = vertices[i].xyz[2];
  }
  mesh.triangles.length(NUM_FACES);
  for (int i = 0; i < NUM_FACES; i++)
  {
    mesh.triangles[i].a = faces[i].verts[0];
    mesh.triangles[i].a = faces[i].verts[1];
    mesh.triangles[i].a = faces[i].verts[2];
  }
  Graphic_var model = my_prim->geometry(mesh);
  Graphic_var root = my_prim->root(model);
  root = my_layout->align(root, 0., 0.);
  root = my_layout->fixed_size(root, 3000., 3000.);
  Controller_var group = my_tool->group(root);
  Window_var window = my_desktop->shell(group, my_client);
}

