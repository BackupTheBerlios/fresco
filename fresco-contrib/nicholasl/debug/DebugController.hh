/*$Id: DebugController.hh,v 1.1 2002/05/16 13:54:47 nicholasl Exp $
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
#ifndef _DebugController_hh
#define _DebugController_hh

#include <Warsaw/config.hh>
#include <Warsaw/Input.hh>
#include <Warsaw/PickTraversal.hh>
#include <Warsaw/DrawTraversal.hh>
#include <Berlin/ControllerImpl.hh>
#include <Berlin/RefCountVar.hh>

#include "DebugConsole.hh"

#define ENTER() do { _console->start_event(_name, __FUNCTION__, __PRETTY_FUNCTION__); } while(0)
#define EXIT() do { _console->end_event(_name); } while(0)
#define IS_ENABLED() _console->is_enabled(_name, __FUNCTION__)
#define DUMP(x) do { CORBA::Any a; a <<= x; _console->d(a); } while(0)

class DebugController : public virtual POA_Warsaw::Controller,
                        public ControllerImpl
{
public:
  DebugController::DebugController(char *name, DebugConsole *console) : ControllerImpl(false) { _name = name; _console = console; }
  ~DebugController() {}
  // Disable debugging output during a traversal. This will have to work some day.
  void draw(Warsaw::DrawTraversal_ptr t) {
    //ENTER();
    //DUMP(t);
    //if (IS_ENABLED()) ControllerImpl::draw(t);
    //EXIT();
    ControllerImpl::draw(t);
  }
  void pick(Warsaw::PickTraversal_ptr t) {
    //ENTER();
    //DUMP(t);
    //if (IS_ENABLED()) ControllerImpl::pick(t);
    //EXIT();
    ControllerImpl::pick(t);
  }
  void press(Warsaw::PickTraversal_ptr t, const Warsaw::Input::Event &e) {
    ENTER();
    DUMP(t);
    DUMP(&e);
    if (IS_ENABLED()) ControllerImpl::press(t, e);
    EXIT();
  }
  void drag(Warsaw::PickTraversal_ptr t, const Warsaw::Input::Event &e) {
    ENTER();
    DUMP(t);
    DUMP(&e);
    if (IS_ENABLED()) ControllerImpl::drag(t, e);
    EXIT();
  }
  void release(Warsaw::PickTraversal_ptr t, const Warsaw::Input::Event &e) {
    ENTER();
    DUMP(t);
    DUMP(&e);
    if (IS_ENABLED()) ControllerImpl::release(t, e);
    EXIT();
  }
  CORBA::Boolean handle_positional(Warsaw::PickTraversal_ptr t, const Warsaw::Input::Event &e) {
    ENTER();
    DUMP(t);
    DUMP(&e);
    if (IS_ENABLED()) ControllerImpl::handle_positional(t, e);
    EXIT();
  }
  CORBA::Boolean handle_non_positional(const Warsaw::Input::Event &e) {
    ENTER();
    DUMP(&e);
    if (IS_ENABLED()) ControllerImpl::handle_non_positional(e);
    EXIT();
  }

private:
  inline void trace(const char *func, const char *extended, vector<CORBA::Any>) {
    printf("%s: %s\n", _name, func); }
  char         *_name;
  DebugConsole *_console;
};

#endif
