/*$Id: DebugConsole.hh,v 1.1 2002/05/16 13:54:47 nicholasl Exp $
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

#include <Warsaw/config.hh>
#include <Warsaw/resolve.hh>

#include <Warsaw/TextKit.hh>
#include <Warsaw/WidgetKit.hh>
#include <Warsaw/CommandKit.hh>
#include <Warsaw/DesktopKit.hh>
#include <Warsaw/ToolKit.hh>
#include <Warsaw/LayoutKit.hh>
#include <Warsaw/Trigger.hh>
#include <Warsaw/Server.hh>
#include <Warsaw/ClientContextImpl.hh>
#include <Warsaw/Unicode.hh>
#include <Warsaw/MainController.hh>
#include <Warsaw/Graphic.hh>

class ExitCommand : public virtual POA_Warsaw::Command,
		    public virtual PortableServer::RefCountServantBase
{
public:
  void execute(const CORBA::Any &) { exit(0); }
  void destroy() {}
};

class DebugConsole : public virtual PortableServer::RefCountServantBase
{
public:
  DebugConsole(ClientContextImpl *client, Warsaw::ServerContext_var server);
  ~DebugConsole() {}
  void start_event(const char *id, const char *name, const char *extended_name);
  void end_event(const char *id);
  bool is_enabled(const char *id, const char *name);
  Warsaw::Trigger_var close_button();
  void d(CORBA::Any);
private:
  std::string buf;
  Warsaw::Graphic_var body_text;
  inline void append_text(const char *c);
  Warsaw::TextKit_var _text;
  Warsaw::ToolKit_var _tool;
  Warsaw::WidgetKit_var _widget;
  Warsaw::LayoutKit_var _layout;
  Warsaw::DesktopKit_var _desktop;
  Warsaw::ClientContext_var _client;
  Warsaw::ServerContext_var _server;
};

#endif
