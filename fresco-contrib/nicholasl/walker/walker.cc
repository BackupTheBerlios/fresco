#include <Fresco/config.hh>
#include <Fresco/resolve.hh>

#include <Fresco/Desktop.hh>

#include <Fresco/Server.hh>
#include <Fresco/ClientContextImpl.hh>
#include <Fresco/Unicode.hh>

#include <Fresco/WidgetKit.hh>
#include <Fresco/CommandKit.hh>
#include <Fresco/DesktopKit.hh>
#include <Fresco/TextKit.hh>
#include <Fresco/ToolKit.hh>
#include <Fresco/LayoutKit.hh>

#include <Berlin/DrawTraversalImpl.hh>
#include <Berlin/TraversalImpl.hh>
#include <Berlin/DrawingKitBase.hh>
#include <Berlin/KitImpl.hh>
#include <Berlin/Color.hh>

#include <string>
#include <iostream>

CORBA::ORB_var orb;

class NullDrawingKit : public DrawingKitBase, public KitImpl
{
public:
  NullDrawingKit(const std::string &, const Fresco::Kit::PropertySeq &);
  virtual ~NullDrawingKit();
  virtual KitImpl *clone(const Fresco::Kit::PropertySeq &);

  virtual void transformation(Fresco::Transform_ptr t) {}
  virtual Fresco::Transform_ptr transformation() { return Fresco::Transform::_nil();}
  virtual void clipping(Fresco::Region_ptr r) {}
  virtual Fresco::Region_ptr clipping() { return Fresco::Region::_nil();}
  virtual Fresco::Color foreground() { return Fresco::Color();}
  virtual Fresco::Color lighting() { return Fresco::Color();}
  virtual Fresco::Coord point_size() { return 1;}
  virtual Fresco::Coord line_width() { return 1;}
  virtual Fresco::DrawingKit::Endstyle line_endstyle() { return Fresco::DrawingKit::butt;}
  virtual Fresco::DrawingKit::Fillstyle surface_fillstyle() { return Fresco::DrawingKit::solid;}
  virtual Fresco::Raster_ptr texture() { return Fresco::Raster::_nil();}

  virtual void set_transformation(Fresco::Transform_ptr) {}
  virtual void set_clipping(Fresco::Region_ptr) {}
  virtual void set_foreground(const Fresco::Color &) {}
  virtual void set_lighting(const Fresco::Color &) {}
  virtual void set_point_size(Fresco::Coord) {}
  virtual void set_line_width(Fresco::Coord) {}
  virtual void set_line_endstyle(Fresco::DrawingKit::Endstyle) {}
  virtual void set_surface_fillstyle(Fresco::DrawingKit::Fillstyle) {}
  virtual void set_texture(Fresco::Raster_ptr) {}

  virtual void set_font_size(CORBA::ULong) {}
  virtual void set_font_weight(CORBA::ULong) {}
  virtual void set_font_family(const Fresco::Unistring &) {}
  virtual void set_font_subfamily(const Fresco::Unistring &) {}
  virtual void set_font_fullname(const Fresco::Unistring &) {}
  virtual void set_font_style(const Fresco::Unistring &) {}
  virtual void set_font_attribute(const Fresco::NVPair &) {}

  virtual CORBA::ULong font_size() { return 12;}
  virtual CORBA::ULong font_weight() { return 1;}
  virtual Fresco::Unistring *font_family() {}// return new Unistring(Unicode::to_CORBA(Babylon::String("Times Roman"))); }
  virtual Fresco::Unistring *font_subfamily() {}// return new Unistring();}
  virtual Fresco::Unistring *font_fullname() {}// return new Unistring(Unicode::to_CORBA(Babylon::String("Times Roman")));}
  virtual Fresco::Unistring *font_style() {}// return new Unistring(Unicode::to_CORBA(Babylon::String("Regular")));}
  virtual Fresco::DrawingKit::FontMetrics font_metrics() { return Fresco::DrawingKit::FontMetrics();}
  virtual Fresco::DrawingKit::GlyphMetrics glyph_metrics(Fresco::Unichar) { return Fresco::DrawingKit::GlyphMetrics();}
  virtual CORBA::Any *get_font_attribute(const Fresco::Unistring &) { return new CORBA::Any();}

  virtual Fresco::Coord resolution(Fresco::Axis) { return 0.1;}

  virtual void draw_path(const Fresco::Path &) {}
  virtual void draw_rectangle(const Fresco::Vertex &, const Fresco::Vertex &) {}
  virtual void draw_quadric(const Fresco::DrawingKit::Quadric, Fresco::Coord, Fresco::Coord) {}
  virtual void draw_ellipse(const Fresco::Vertex &, const Fresco::Vertex &) {}
  virtual void draw_image(Fresco::Raster_ptr) {}
  virtual void allocate_char(Fresco::Unichar, Fresco::Graphic::Requisition &) {}
  virtual void draw_char(Fresco::Unichar) {}
  virtual void allocate_text(const Fresco::Unistring &, Fresco::Graphic::Requisition &) {}
  virtual void draw_text(const Fresco::Unistring &) {}
  virtual void copy_drawable(Fresco::Drawable_ptr, Fresco::PixelCoord, Fresco::PixelCoord, Fresco::PixelCoord, Fresco::PixelCoord) {}

  virtual void start_traversal(Fresco::Traversal_ptr) {}
  virtual void finish_traversal() {}

  virtual void flush() {}
};

NullDrawingKit::NullDrawingKit(const std::string &id, const Fresco::Kit::PropertySeq &p)
  : KitImpl(id, p) {}

NullDrawingKit::~NullDrawingKit() {}

KitImpl *NullDrawingKit::clone(const Fresco::Kit::PropertySeq &p)
{
  NullDrawingKit *kit = new NullDrawingKit(repo_id(), p);
  kit->init();
  return kit;
}

class DebugTraversal : public virtual DrawTraversalImpl
{
public:
  DebugTraversal(Fresco::Graphic_ptr g, Fresco::Region_ptr r,
		 Fresco::Transform_ptr t, Fresco::DrawingKit_var d)
    : DrawTraversalImpl(g, r, t, d)
  {}
  virtual ~DebugTraversal() {}
  void traverse_child(Fresco::Graphic_ptr, Fresco::Tag, Fresco::Region_ptr,
                      Fresco::Transform_ptr);
  void DebugTraversal::visit(Fresco::Graphic_ptr);
  Fresco::Traversal::order direction() { return Fresco::Traversal::up; }
  CORBA::Boolean ok() { return true; }
  CORBA::Boolean intersects_allocation() { return true; }
  CORBA::Boolean intersects_region(Fresco::Region_ptr) { return true; }
};

void DebugTraversal::traverse_child(Fresco::Graphic_ptr child, Fresco::Tag tag,
                                    Fresco::Region_ptr region,
                                    Fresco::Transform_ptr transform)
{
  std::cout << "[push]" << std::endl;
  child->traverse(_this());
  std::cout << "[pop]" << std::endl;
}

void DebugTraversal::visit(Fresco::Graphic_ptr g)
{
  std::cout << "[graphic] " << orb->object_to_string(g) << std::endl;

  Fresco::Graphic::Requisition r;
  g->request(r);
  std::cout << "preserve_aspect: " << r.preserve_aspect << std::endl;
  std::cout << "=X= ";
  if (r.x.defined) {
    std::cout << r.x.minimum << " <= " << r.x.natural << " <= " << r.x.maximum;
    std::cout << " @" << r.x.align;
  } else {
    std::cout << "not defined.";
  }
  std::cout << std::endl;

  std::cout << "=Y= ";
  if (r.y.defined) {
    std::cout << r.y.minimum << " <= " << r.y.natural << " <= " << r.y.maximum;
    std::cout << " @" << r.y.align;
  } else {
    std::cout << "not defined.";
  }
  std::cout << std::endl;

  std::cout << "=Z= ";
  if (r.z.defined) {
    std::cout << r.z.minimum << " <= " << r.z.natural << " <= " << r.z.maximum;
    std::cout << " @" << r.z.align;
  } else {
    std::cout << "not defined.";
  }
  std::cout << std::endl;
  g->draw(_this());
}

int main(int argc, char **argv)
{
  //CORBA::ORB_var orb;
  CosNaming::NamingContext_var context;
  PortableServer::POA_var poa;
  PortableServer::POAManager_var pman;
  ClientContextImpl *client;
  Fresco::ServerContext_var server;

  try {
    Prague::GetOpt getopt(argv[0], "Fresco Scene Graph Dumping Debugger.");
    add_resolving_options_to_getopt(getopt);
    orb = CORBA::ORB_init(argc, argv);
    poa = resolve_init<PortableServer::POA>(orb, "RootPOA");
    pman = poa->the_POAManager();
    pman->activate();

    client = new ClientContextImpl("Walker");

    Fresco::Server_var s = resolve_server(getopt, orb);
    server = s->create_server_context(Fresco::ClientContext_var(client->_this()));
    DefaultPOA::default_POA(poa);
  } catch (CORBA::COMM_FAILURE c) {
    std::cerr << "Could not connect to the berlin server (CORBA::COMM_FAILURE)." << std::endl;
  }

  Fresco::Graphic_var g = Fresco::Desktop::_narrow(server->get_singleton("IDL:fresco.org/Fresco/Desktop:1.0"));

  Fresco::Kit::PropertySeq props;

  NullDrawingKit *ndk = new NullDrawingKit(std::string(), props);

  poa->activate_object(ndk);

  DebugTraversal *dt = new DebugTraversal(Fresco::Graphic::_nil(),
					  Fresco::Region::_nil(),
					  Fresco::Transform::_nil(),
					  Fresco::DrawingKit_var(ndk->_this()));

  poa->activate_object(dt);
  g->traverse(dt->_this());

  orb->shutdown(true);
}
