#ifndef Teapot_hh
#define Teapot_hh

#include <Fresco/ClientContextImpl.hh>
#include <Fresco/Graphic.hh>
#include <Fresco/Server.hh>

#include <Fresco/CommandKit.hh>
#include <Fresco/DesktopKit.hh>
#include <Fresco/FigureKit.hh>
#include <Fresco/GadgetKit.hh>
#include <Fresco/LayoutKit.hh>
#include <Fresco/PrimitiveKit.hh>
#include <Fresco/TextKit.hh>
#include <Fresco/ToolKit.hh>
#include <Fresco/WidgetKit.hh>


class Teapot
{
  Fresco::ServerContext_var my_server;
  Fresco::ClientContext_var my_client;
  Fresco::Graphic_var my_root;
  Fresco::CommandKit_var my_command;
  Fresco::DesktopKit_var my_desktop;
  Fresco::FigureKit_var my_figure;
  Fresco::GadgetKit_var my_gadget;
  Fresco::LayoutKit_var my_layout;
  Fresco::TextKit_var my_text;
  Fresco::ToolKit_var my_tool;
  Fresco::WidgetKit_var my_widget;
  Fresco::PrimitiveKit_var my_prim;
public:
  Teapot(Fresco::ServerContext_ptr, Fresco::ClientContext_ptr);
  ~Teapot();

  void run();
};

#endif
