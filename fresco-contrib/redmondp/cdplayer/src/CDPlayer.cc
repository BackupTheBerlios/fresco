#include "CDPlayer.hh"

#include <Fresco/Controller.hh>
#include <Fresco/MainController.hh>
#include <Fresco/Trigger.hh>
#include <Fresco/Unicode.hh>
#include <Berlin/ObserverImpl.hh>

class SeekObserver : public ObserverImpl
{
public:
    SeekObserver(CDROM *cdrom) : _cdrom(cdrom) {}
    void update(const CORBA::Any &any)
    {
	Fresco::Coord coord;
	if (any >>= coord)
	    if (coord > 0.05)
		_cdrom->trackSeek(coord);
    }
private:
    CDROM *_cdrom;
};

class CDCommand : public virtual POA_Fresco::Command,
		  public virtual PortableServer::RefCountServantBase
{
public:
    CDCommand() {}
    CDCommand(CDROM *cdrom) : _cdrom(cdrom) {}
    
    virtual void execute(const CORBA::Any &) {}
    void destroy()
    {
	PortableServer::POA_var poa = _default_POA();
	PortableServer::ObjectId *oid = poa->servant_to_id(this);
	poa->deactivate_object(*oid);
	delete oid;
    }
protected:

    CDROM *_cdrom;
};

class StartPlay : public CDCommand
{
public:
    StartPlay(CDROM *cdrom) : CDCommand(cdrom) {}
    virtual void execute(const CORBA::Any &) { _cdrom->play(); }
};

class PausePlay : public CDCommand
{
public:
    PausePlay(CDROM *cdrom) : CDCommand(cdrom) {}
    virtual void execute(const CORBA::Any &) { _cdrom->pause(); }
};

class StopPlay : public CDCommand
{
public:
    StopPlay(CDROM *cdrom) : CDCommand(cdrom) {}
    virtual void execute(const CORBA::Any &) { _cdrom->stop(); }
};

class NextTrack : public CDCommand
{
public:
    NextTrack(CDROM *cdrom) : CDCommand(cdrom) {}
    virtual void execute(const CORBA::Any &) { _cdrom->next(); }
};

class PrevTrack : public CDCommand
{
public:
    PrevTrack(CDROM *cdrom) : CDCommand(cdrom) {}
    virtual void execute(const CORBA::Any &) { _cdrom->prev(); }
};

class Eject : public CDCommand
{
public:
    Eject(CDROM *cdrom) : CDCommand(cdrom) {}
    virtual void execute(const CORBA::Any &) { _cdrom->eject(); }
};

using namespace Prague;
using namespace Fresco;
using namespace Babylon;

CDPlayer::CDPlayer(ServerContext_ptr sc, ClientContext_ptr cc,
  PortableServer::POA_ptr poa) :
  my_server(ServerContext::_duplicate(sc)),
  my_client(ClientContext::_duplicate(cc)),
  my_poa(PortableServer::POA::_duplicate(poa)),
  
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
  std::cerr << "CD player is initializing..." << std::endl;
  try 
  {
    my_cdrom = new CDROM;
  }
  catch (...)
  {
    std::cerr << "the cdrom could not be accessed" << std::endl;
    exit(0);    
  }
    
  make_root_graphic();
}

CDPlayer::~CDPlayer()
{
  delete my_cdrom;
}

void CDPlayer::make_root_graphic()
{
    Graphic_var hbox1 = my_layout->hbox();
    Graphic_var hbox2 = my_layout->hbox();
    Graphic_var hbox3 = my_layout->hbox();
    Graphic_var hbox4 = my_layout->hbox();
    Graphic_var vbox1 = my_layout->vbox();
    Graphic_var vbox2 = my_layout->vbox();
    Graphic_var vbox3 = my_layout->vbox();
		
    ToolKit::FrameSpec spec;
    spec.brightness(0.5); spec._d(ToolKit::inset);
//    Graphic_var frame = my_tool->frame(hbox1,0.1,spec,true);
    my_root = my_tool->frame(hbox1,0.1,spec,true);

    hbox1->append_graphic(vbox1);
  
    vbox1->append_graphic(my_tool->rgb(
			    my_tool->frame(hbox3,10.0,spec,true),
			    0.0,0.75,0.0)
			);

//    SeekObserver *seek = new SeekObserver(my_cdrom);
//    my_poa->activate_object(seek);
//    my_pos = my_command->bvalue(0.0, 1.0, 0.0, 0.05, 0.05);
//    my_pos->attach(seek->_this());
//    vbox1->append_graphic(Graphic_var(my_widget->slider(my_pos, xaxis)));
    vbox1->append_graphic(hbox2);

    hbox3->append_graphic(vbox2);
    hbox3->append_graphic(vbox3);
	
  vbox3->append_graphic(my_layout->margin_flexible(
                          my_tool->rgb(
			    my_text->chunk(
			      Unicode::to_CORBA(String("Fresco CD Player"))
			    ), 
			    0, 0, 0
			  ),
			50, 500, 50));
			
    vbox3->append_graphic(hbox4);
    hbox4->append_graphic(my_layout->hfill());
//    Graphic_var exitGlyph =
//	my_layout->margin_flexible(my_tool->rgb(my_text->chunk(Unicode::to_CORBA(Babylon::String("Exit"))), 0, 0, 0), 100, 500, 100);

    Fresco::Path path; 
    path.nodes.length(3);
    path.shape = convex;
    const float d = 100.0;

    path.nodes[0].z = 0.0; path.nodes[1].z = 0.0; path.nodes[2].z = 0.0;

    path.nodes[0].x = 0.0; path.nodes[0].y = 0.0; 
    path.nodes[1].x = d;   path.nodes[1].y = d/2.;
    path.nodes[2].x = 0.0; path.nodes[2].y = d;

    Graphic_var playGroup = my_figure->group();
    playGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->polygon(path),0.5)),0.0,0.0,0.0));
    Graphic_var playGlyph = my_layout->margin_flexible(my_figure->fitter(playGroup), 100, 500, 100);

    Graphic_var stopGroup = my_figure->group();
    stopGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->rectangle(0.0,0.0,d,d),0.5)),0.0,0.0,0.0));
    Graphic_var stopGlyph = my_layout->margin_flexible(my_figure->fitter(stopGroup), 100, 500, 100);

    Graphic_var pauseGroup = my_figure->group();
    pauseGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->rectangle(0.0,0.0,d/3.,d),0.5)),0.0,0.0,0.0));
    pauseGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->rectangle(d*(2./3.),0.0,d,d),0.5)),0.0,0.0,0.0));
    Graphic_var pauseGlyph = my_layout->margin_flexible(my_figure->fitter(pauseGroup), 100, 500, 100);

    path.nodes[0].x = 0.0;  path.nodes[0].y = d/2.;
    path.nodes[1].x = d/2.; path.nodes[1].y = 0.0;
    path.nodes[2].x = d;    path.nodes[2].y = d/2.;

    Graphic_var ejectGroup = my_figure->group();
    ejectGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->polygon(path),0.5)),0.0,0.0,0.0));
    ejectGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->rectangle(0.0,d*(2./3.),d,d),0.5)),0.0,0.0,0.0));
    Graphic_var ejectGlyph = my_layout->margin_flexible(my_figure->fitter(ejectGroup), 100, 500, 100);
	       
    path.nodes[0].x = 0.0;       path.nodes[0].y = 0.0;
    path.nodes[1].x = d*(2./3.); path.nodes[1].y = d/2.;
    path.nodes[2].x = 0.0;       path.nodes[2].y = d;

    Graphic_var nextGroup = my_figure->group();
    nextGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->polygon(path),0.5)),0.0,0.0,0.0));
    nextGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->rectangle(d*(2./3.),0.0,d,d),0.5)),0.0,0.0,0.0));
    Graphic_var nextGlyph = my_layout->margin_flexible(my_figure->fitter(nextGroup), 100, 500, 100);
		   
    path.nodes[0].x = d/3.; path.nodes[0].y = d/2.;
    path.nodes[1].x = d;    path.nodes[1].y = 0.0;
    path.nodes[2].x = d;    path.nodes[2].y = d;

    Graphic_var prevGroup = my_figure->group();
    prevGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->polygon(path),0.5)),0.0,0.0,0.0));
    prevGroup->append_graphic(my_tool->rgb(Graphic_var(my_tool->alpha(my_figure->rectangle(0.0,0.0,d/3.,d),0.5)),0.0,0.0,0.0));
    Graphic_var prevGlyph = my_layout->margin_flexible(my_figure->fitter(prevGroup), 100, 500, 100);

    PrevTrack *prevCommand = new PrevTrack(my_cdrom);
    Trigger_var prev = my_widget->button(prevGlyph, Command_var(prevCommand->_this()));
    hbox2->append_graphic(prev);
  
    StartPlay *startCommand = new StartPlay(my_cdrom);
    Trigger_var start = my_widget->button(playGlyph,Command_var(startCommand->_this()));
    hbox2->append_graphic(start);

    PausePlay *pauseCommand = new PausePlay(my_cdrom);
    Trigger_var pause = my_widget->button(pauseGlyph,Command_var(pauseCommand->_this()));
    hbox2->append_graphic(pause);
  
    StopPlay *stopCommand = new StopPlay(my_cdrom);
    Trigger_var stop = my_widget->button(stopGlyph, Command_var(stopCommand->_this()));
    hbox2->append_graphic(stop);
  
    NextTrack *nextCommand = new NextTrack(my_cdrom);
    Trigger_var next = my_widget->button(nextGlyph, Command_var(nextCommand->_this()));
    hbox2->append_graphic(next);

    Eject *ejectCommand = new Eject(my_cdrom);
    Trigger_var eject = my_widget->button(ejectGlyph, Command_var(ejectCommand->_this()));
    hbox2->append_graphic(eject);

//    AboutApp *aboutCommand = new AboutApp;
//    Trigger_var aboutTrig = my_widget->button(aboutGlyph, Command_var(aboutCommand->_this()));
    
//    my_root = my_tool->group(frame);
}

void CDPlayer::pulse()
{
    // probably isn't best to simply set value but what the heck
    my_pos->value(my_cdrom->trackProgress());
}

void CDPlayer::run()
{
  std::cerr << "Running..." << std::endl;

  Controller_var c = my_tool->group(my_root);

  Window_var window = my_desktop->shell(c, my_client);

  while (true) 
  {
    Thread::delay(Prague::Time(1000));
  }

}
