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
#ifndef Wallpaper_hh
#define Wallpaper_hh

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


class Wallpaper
{
  Fresco::ServerContext_var my_server;
  Fresco::ClientContext_var my_client;
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

public:
  Wallpaper(Fresco::ServerContext_ptr, Fresco::ClientContext_ptr, 
    const std::string &);
  ~Wallpaper();

  void run();
};

#endif
