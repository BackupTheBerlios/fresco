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

#include <Berlin/TraversalImpl.hh>

CORBA::ORB_var orb;

class DebugTraversal : public virtual TraversalImpl
{
public:
  DebugTraversal(Fresco::Graphic_ptr g, Fresco::Region_ptr r,
		 Fresco::Transform_ptr t)
    : TraversalImpl(g, r, t)
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
  //std::cout << s << " -> " << orb->object_to_string(child) << std::endl;
  std::cout << "[push]" << std::endl;
  child->traverse(_this());
  std::cout << "[pop]" << std::endl;
}

void DebugTraversal::visit(Fresco::Graphic_ptr g)
{
  //s = orb->object_to_string(g);
  //std::cout << s << std::endl;
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
}

int main(int argc, char **argv)
{
  //  CORBA::ORB_var orb;
  CosNaming::NamingContext_var context;
  PortableServer::POA_var poa;
  PortableServer::POAManager_var pman;
  ClientContextImpl *client;
  Fresco::ServerContext_var server;

  try {
    orb = CORBA::ORB_init(argc, argv);
    context = resolve_init<CosNaming::NamingContext>(orb, "NameService");
    poa = resolve_init<PortableServer::POA>(orb, "RootPOA");
    pman = poa->the_POAManager();
    pman->activate();

    client = new ClientContextImpl("Walker");

    Fresco::Server_var s = resolve_name<Fresco::Server>(context, "IDL:fresco.org/Fresco/Server:1.0");
    server = s->create_server_context(Fresco::ClientContext_var(client->_this()));
    DefaultPOA::default_POA(poa);
  } catch (CORBA::COMM_FAILURE c) {
    std::cerr << "Could not connect to the berlin server (CORBA::COMM_FAILURE)." << std::endl;
  }

  Fresco::Graphic_var g = Fresco::Desktop::_narrow(server->get_singleton("IDL:fresco.org/Fresco/Desktop:1.0"));

  DebugTraversal *dt = new DebugTraversal(Fresco::Graphic::_nil(),
					  Fresco::Region::_nil(),
					  Fresco::Transform::_nil());

  poa->activate_object(dt);
  g->traverse(dt->_this());

  orb->shutdown(true);
}
