/*
 *
 * This source file is a part of the Fresco Project.
 * Copyright (C) 1999 Stefan Seefeld <stefan@fresco.org> 
 * http://www.fresco.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
 * MA 02139, USA.
 */

#include "Wallpaper.hh"
#include <Fresco/Controller.hh>
#include <Fresco/Desktop.hh>
#include <Fresco/Image.hh>
#include <Fresco/MainController.hh>
#include <Fresco/resolve.hh>
#include <Fresco/Window.hh>

using namespace Prague;
using namespace Fresco;

Wallpaper::Wallpaper(ServerContext_ptr sc, ClientContext_ptr cc,
  const std::string &fname)
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
//  const char *fname = "SimpleDesktop/wallpaper.png";
  Raster_var raster = my_raster->create_raster(fname.c_str());
//  Image_var image = my_figure->pixmap(raster);
  my_root = my_figure->pixmap(raster);
//  my_root = image;
}

Wallpaper::~Wallpaper()
{
}

void Wallpaper::run()
{
  Desktop_var desktop = my_desktop->desk();
  desktop->lock();
  Vertex p; p.x = p.y = p.z = 0.;
  Vertex s;
  //made up value until I can figure out how to get real size.
  s.x = 11500; 
  s.y = 8500;
  s.z = 0.;
  Controller_var group = my_tool->group(my_root);
  Layout::StageHandle_var handle = desktop->insert(group, p, s, 0);
  desktop->unlock();

  while (true) 
  {
    Thread::delay(Prague::Time(1000));
  }
}

