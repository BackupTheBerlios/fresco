#include <Fresco/config.hh>
#include <Fresco/resolve.hh>
#include <Fresco/ClientContextImpl.hh>
#include <Berlin/DefaultPOA.hh>

#include "CDPlayer.hh"

using namespace Prague;
using namespace Fresco;

int main(int argc, char **argv)
{
   Prague::GetOpt getopt(argv[0], "fresco CD player");
   getopt.add('h', "help", Prague::GetOpt::novalue, "help message");
   add_resolving_options_to_getopt(getopt);
   size_t argo = getopt.parse(argc, argv);
   argc -= argo;
   argv += argo;
 
   if (getopt.is_set('h'))
   {
     getopt.usage();
     exit(0);
   }

  try
  {
     CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
     PortableServer::POA_var poa = 
         resolve_init<PortableServer::POA>(orb, "RootPOA");
     DefaultPOA::default_POA(poa);
     PortableServer::POAManager_var pman = poa->the_POAManager();
     pman->activate();
     
     Server_var server = resolve_server(getopt, orb);
     
     ClientContextImpl *client = new ClientContextImpl("Desktop");
     ClientContext_var client_ref = client->_this();

     ServerContext_var server_context = 
         server->create_server_context(client_ref);
     
     CDPlayer *app = new CDPlayer(server_context, client_ref, poa);
     
     app->run();
     delete app;
     delete client;
  }
  catch (const CORBA::COMM_FAILURE &c)
  {
     std::cerr << "Could not connect to the display server "
               << "(CORBA::COMM_FAILURE)." << std::endl;
  }
  catch (const std::exception &e)
  {
     std::cerr << "Exception: " << e.what() << std::endl;
  }
}
