#ifndef _CDPlayer_hh
#define _CDPlayer_hh

#include <Fresco/resolve.hh>
#include <Fresco/BoundedValue.hh>
#include <Fresco/ClientContextImpl.hh>
#include <Fresco/Graphic.hh>
#include <Fresco/Server.hh>

#include <Fresco/CommandKit.hh>
#include <Fresco/DesktopKit.hh>
#include <Fresco/FigureKit.hh>
#include <Fresco/GadgetKit.hh>
#include <Fresco/LayoutKit.hh>
#include <Fresco/RasterKit.hh>
#include <Fresco/TextKit.hh>
#include <Fresco/ToolKit.hh>
#include <Fresco/WidgetKit.hh>

#include "CDROM.hh"

class CDPlayer
{
  CDROM *my_cdrom;
  Fresco::ServerContext_var my_server;
  Fresco::ClientContext_var my_client;
  PortableServer::POA_var my_poa;
  Fresco::BoundedValue_var my_pos;
  Fresco::Graphic_var my_root;

  Fresco::CommandKit_var my_command;
  Fresco::DesktopKit_var my_desktop;
  Fresco::FigureKit_var my_figure;
  Fresco::GadgetKit_var my_gadget;
  Fresco::LayoutKit_var my_layout;
  Fresco::RasterKit_var my_raster;
  Fresco::TextKit_var my_text;
  Fresco::ToolKit_var my_tool;
  Fresco::WidgetKit_var my_widget;

  void make_root_graphic();
  void pulse();
public:
  CDPlayer(Fresco::ServerContext_ptr, Fresco::ClientContext_ptr, 
    PortableServer::POA_ptr poa);
  ~CDPlayer();

  void run();    
};

#endif
