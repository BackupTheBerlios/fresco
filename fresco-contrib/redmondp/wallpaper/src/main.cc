 /*
 *
 * This source file is a part of the Fresco Project.
 * Copyright (C) 1999, 2000 Stefan Seefeld <stefan@fresco.org> 
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

#include <Fresco/config.hh>
#include <Fresco/resolve.hh>
#include <Fresco/ClientContextImpl.hh>
#include <Berlin/DefaultPOA.hh>

#include "Wallpaper.hh"

using namespace Prague;
using namespace Fresco;

int main(int argc, char **argv)
{
   Prague::GetOpt getopt(argv[0], "fresco wallpaper");
   getopt.add('h', "help", Prague::GetOpt::novalue, "help message");
   getopt.add('r', "raster", Prague::GetOpt::mandatory, 
    "use raster relative to {berlin-prefix/share/Berlin/PNG/ "
    "(default: berlin-128.png))");
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
     
     ClientContextImpl *client = new ClientContextImpl("Wallpaper");
     ClientContext_var client_ref = client->_this();

     ServerContext_var server_context = 
         server->create_server_context(client_ref);

     std::string value;
     if (getopt.is_set("raster"))
      getopt.get("raster", &value);
     else 
      value = "berlin-128.png";
     Wallpaper *app = new Wallpaper(server_context, client_ref, value);
     
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
