/*$Id: DebugConsole.hh,v 1.2 2002/06/08 20:26:16 nicholasl Exp $
 *
 * This source file is a part of the Berlin Project.
 * Copyright (C) 2002 Nick Lewycky <nicholas@mxc.ca>
 * http://www.berlin-consortium.org
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
#ifndef _DebugConsole_hh
#define _DebugConsole_hh

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
#include <Fresco/Graphic.hh>

class ExitCommand : public virtual POA_Fresco::Command,
		    public virtual PortableServer::RefCountServantBase
{
public:
  void execute(const CORBA::Any &) { exit(0); }
  void destroy() {}
};

class DebugConsole : public virtual PortableServer::RefCountServantBase
{
public:
  DebugConsole(ClientContextImpl *client, Fresco::ServerContext_var server);
  ~DebugConsole() {}
  void start_event(const char *id, const char *name, const char *extended_name);
  void end_event(const char *id);
  bool is_enabled(const char *id, const char *name);
  Fresco::Trigger_var close_button();
  void d(CORBA::Any);
private:
  std::string buf;
  Fresco::Graphic_var body_text;
  inline void append_text(const char *c);
  Fresco::TextKit_var _text;
  Fresco::ToolKit_var _tool;
  Fresco::WidgetKit_var _widget;
  Fresco::LayoutKit_var _layout;
  Fresco::DesktopKit_var _desktop;
  Fresco::ClientContext_var _client;
  Fresco::ServerContext_var _server;
};

#endif
