#ifndef __MINESWEEPER__
#define __MINESWEEPER__

#include <Fresco/config.hh>
#include <Fresco/resolve.hh>
#include <Fresco/Server.hh>

#include <Fresco/CommandKit.hh>
#include <Fresco/DesktopKit.hh>
#include <Fresco/ToolKit.hh>
#include <Fresco/LayoutKit.hh>
#include <Fresco/Trigger.hh>
#include <Fresco/ClientContextImpl.hh>
#include <Fresco/Unicode.hh>
#include <Fresco/MainController.hh>
#include <Fresco/Grid.hh>

#include <math.h>

class SelectMine;

#include "GameDisplayer.hh"
#include "GameContext.hh"

class SelectMine : public virtual POA_Fresco::Command,
		   public virtual PortableServer::RefCountServantBase
{
public:
  SelectMine::SelectMine(GameDisplayer *display, GameContext *context) 
    : _display(display), _context(context) {}
  void execute(const CORBA::Any &a);
  void destroy() {}
private:
  void recurse(int x, int y);
  GameDisplayer   *_display;
  GameContext     *_context;
};

#endif
