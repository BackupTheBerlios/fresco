/*$Id: DebugConsole.cc,v 1.2 2002/06/08 20:26:16 nicholasl Exp $
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

#include "DebugConsole.hh"

using namespace Prague;
using namespace Fresco;

DebugConsole::DebugConsole(ClientContextImpl *client, Fresco::ServerContext_var server) {
  _client = ClientContext_var(client->_this());
  _server = server;
  _text = resolve_kit<TextKit>(server, "IDL:fresco.org/Fresco/TextKit:1.0");
  _tool = resolve_kit<ToolKit>(server, "IDL:fresco.org/Fresco/ToolKit:1.0");
  _widget = resolve_kit<WidgetKit>(server, "IDL:fresco.org/Fresco/WidgetKit:1.0");
  _layout = resolve_kit<LayoutKit>(server, "IDL:fresco.org/Fresco/LayoutKit:1.0");
  _desktop = resolve_kit<DesktopKit>(server, "IDL:fresco.org/Fresco/DesktopKit:1.0");

  Trigger_var close_trigger = close_button();

  body_text = _layout->vbox();
  append_text("DebugConsole loaded.");

  Graphic_var window_content = _layout->vbox();
  Controller_var page = _widget->scrollable(body_text);
  window_content->append_graphic(_layout->flexible_fill(page));
  window_content->append_graphic(close_trigger);

  MainController_var group = _tool->group(window_content);
  group->append_controller(page);
  group->append_controller(close_trigger);
  Window_var window = _desktop->shell(group, _client);
  _desktop->map(window, true);
}

inline void DebugConsole::append_text(const char *c) {
  body_text->append_graphic(_text->chunk(Unicode::to_CORBA(Babylon::String(c))));
}

Fresco::Trigger_var DebugConsole::close_button() {
  Graphic_var glyph = _text->chunk(Unicode::to_CORBA(Babylon::String("Close.")));
  ExitCommand *command = new ExitCommand();
  Trigger_var button = _widget->button(Graphic_var(_tool->rgb(glyph, 0., 0., 0.)), Command_var(command->_this()));
  return button;
}

void DebugConsole::start_event(const char *id, const char *name, const char *ext)
{
  char c[100];
  snprintf(c, 100, "%s: %s", id, name);
  //std::cout << c << std::endl;
  append_text(c);
}

void DebugConsole::d(CORBA::Any a) {
  //std::cout << "-- " << a << " --  ";
}

void DebugConsole::end_event(const char *id)
{
  //std::cout << std::endl;
}

bool DebugConsole::is_enabled(const char *id, const char *name)
{
  return true;
}
