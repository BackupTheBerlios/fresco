/*
	This implements basic functions for Fresco clients, incl.
  the ClientContext and the establishment of the client-server connection.

	Copyright 2001-2002 Ben Bucksch
	BSD license
 */
package org.fresco.clientlib;

import org.fresco.Warsaw.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;

public class ClientConnection extends ClientContextImpl
{
	// Generally useful
	public ORB orb = null;
	// poa inherited
	public NamingContextExt namingContext = null;

	// For implementing Fresco clients
	// For convience
	public ServerContext serverContext = null;
  public TextKit text = null;
  public DesktopKit desktop = null;
  public LayoutKit layout = null;
  public ToolKit tool = null;
  public WidgetKit widget = null;
  public FigureKit figure = null;
  public CommandKit command = null;
  public ImageKit image = null;

  // Internal
  protected boolean verbose = true;

	public ClientConnection(String[] commandline_args,
                          String an_app_title)
	{
    super(null, an_app_title);  // poa is being sat later
		try
		{
			statusTry("Initializing CORBA ORB");
			orb = org.omg.CORBA.ORB.init(commandline_args, null);
			statusSuccess(null);
			statusTry("Trying to get CORBA root POA");
			poa = org.omg.PortableServer.POAHelper.narrow(
                                  orb.resolve_initial_references("RootPOA"));
			statusSuccess(null);
			statusTry("Activating POAManager");
		  poa.the_POAManager().activate();
			statusSuccess(null);
			statusTry("Trying to get name service");
      org.omg.CORBA.Object nserv =
                                orb.resolve_initial_references("NameService");
			namingContext = NamingContextExtHelper.narrow(nserv);
			statusSuccess(null);
		}
		catch (Exception e)
		{
			fatalError("Could not get initial references", e);
		}

		// Get server
  	Server berlin_server = null;
		try
		{
  		org.omg.CORBA.Object objRef = resolve("IDL:Warsaw/Server:1.0");
      if (objRef == null)
        fatalError("Got null ref for server from nameservice", null);
  		statusSuccess("Got Fresco server object from naming service");
  		berlin_server = ServerHelper.narrow(objRef);
      statusSuccess("Narrowed to Server interface");
		}
		catch (Exception e)
		{
			fatalError("Could not get server", e);
		}

		// Connect client to server
		org.omg.CORBA.Object thisRef = null;
		try
		{
			thisRef = poa.servant_to_reference(this);
			statusSuccess("Connected ClientContextImpl to ORB");
		}
		catch (Exception e)
		{
			fatalError("Could not connect ClientContextImpl object to ORB", e);
		}

		try
		{
			serverContext = berlin_server.create_server_context(_this());
		}
		catch (Exception e)
		{
			fatalError("Client could not connect to server", e);
		}

		// Getting some kits
		/* This is mostly convience for the app developer, not really
			 related to the ClientContext */
		try
		{
			text = TextKitHelper.narrow(resolve_kit("IDL:Warsaw/TextKit:1.0"));
			desktop = DesktopKitHelper.narrow(
                                     resolve_kit("IDL:Warsaw/DesktopKit:1.0"));
			layout = LayoutKitHelper.narrow(resolve_kit("IDL:Warsaw/LayoutKit:1.0"));
			tool = ToolKitHelper.narrow(resolve_kit("IDL:Warsaw/ToolKit:1.0"));
			widget = WidgetKitHelper.narrow(resolve_kit("IDL:Warsaw/WidgetKit:1.0"));
			figure = FigureKitHelper.narrow(resolve_kit("IDL:Warsaw/FigureKit:1.0"));
			command = CommandKitHelper.narrow(
                                     resolve_kit("IDL:Warsaw/CommandKit:1.0"));
			image = ImageKitHelper.narrow(resolve_kit("IDL:Warsaw/ImageKit:1.0"));
      statusSuccess("Got Kits");
		}
		catch (Exception e)
		{
		  fatalError("Could not get Kit", e);
		}
	}

	// Tries to get Kit "name" from server
	public org.omg.CORBA.Object resolve_kit(String name)
	{
		org.fresco.Warsaw.KitPackage.Property[] properties = {};
		return resolve_kit(name, properties);
	}

	// Tries to get Kit "name" with "properties" from server
	public org.omg.CORBA.Object resolve_kit(String name,
													 org.fresco.Warsaw.KitPackage.Property[] properties)
	{
		org.omg.CORBA.Object result = null;
		try
		{
			result = serverContext.resolve(name, properties);
		}
		catch(Exception e)
		{
			if (properties == null)
				fatalError("Could not get Kit \"" + name + "\"", e);
			else
				fatalError("Could not get Kit \"" + name +
                   "\" with properties \"" + properties + "\"", e);
		}
		return result;
	}

	// Tries to get object "name" (on the top-level) from the NameService
	public org.omg.CORBA.Object resolve(String name)
	{
		NameComponent nc = new NameComponent(name, "Object");
		NameComponent path[] = {nc};
		org.omg.CORBA.Object objRef = null;
		try
		{
			objRef = namingContext.resolve(path);
		}
		catch (Exception e)
		{
			fatalError("Cound not get object with name \"" + name +
                 "\" from NameService", e);
		}
		return objRef;
	}

  // Other convience functions

  // Run ordinary Java main loop
  public void run()
  {
		try
		{
      java.lang.Object sync = new java.lang.Object();
      synchronized (sync)
      {
        sync.wait();
      }
		}
		catch (Exception e)
		{
			fatalError("Application " + app_title + " terminated", e);
		}
  }

  // Debugging
  public void fatalError(String msg, Exception e)
  {
    System.err.println("ERROR: " + msg + ": " + e);
    if (verbose)
    {
      System.err.println("Stack trace:");
      e.printStackTrace(System.out);
    }
    System.exit(1);
  };
  public void statusTry(String msg)
  {
    if (verbose)
      System.err.print(msg + "... ");
  };
  public void statusSuccess(String msg)
  {
    if (verbose)
      System.err.println(msg == null ? "done." : msg);
  };
}
