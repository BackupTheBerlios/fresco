/*
  This implements basic functions for Fresco clients, incl.
  the ClientContext and the establishment of the client-server connection.

  Copyright 2001-2002 Ben Bucksch
  BSD license
 */
package org.fresco.clientlib;

import org.fresco.idl.fresco.*;
import org.omg.CosNaming.*;
import org.omg.CORBA.*;
import java.io.*;

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
  public RasterKit raster = null;
  public CommandKit command = null;

  // Debugging
  public boolean verbose = true;



  public ClientConnection(String[] commandlineArgs,
                          String anAppTitle)
  {
    super(null, anAppTitle);  // poa is being set later
    // Get ORB, POA
    try {
      statusTry("Initializing CORBA ORB");
      orb = org.omg.CORBA.ORB.init(commandlineArgs, null);
      statusSuccess(null);
      statusTry("Trying to get CORBA root POA");
      poa = org.omg.PortableServer.POAHelper.narrow(
                                  orb.resolve_initial_references("RootPOA"));
      statusSuccess(null);
      statusTry("Activating POAManager");
      poa.the_POAManager().activate();
      statusSuccess(null);
    } catch (Exception e) { fatalError("Could not get initial references", e); }
    // Get NameService
    try {
      statusTry("Trying to get name service");
      org.omg.CORBA.Object nserv =
                                orb.resolve_initial_references("NameService");
      namingContext = NamingContextExtHelper.narrow(nserv);
      statusSuccess(null);
    } catch (Exception e) { statusFailure("Could not get name service", e); }

    // Get server
    Server frescoServer = null;
    try {
    	frescoServer = resolveServer(commandlineArgs);
    } catch (Exception e) { fatalError("Search for Fresco server failed", e); }

    // Connect client to server
    org.omg.CORBA.Object thisRef = null;
    try {
      thisRef = poa.servant_to_reference(this);
      statusSuccess("Connected ClientContextImpl to ORB");
    } catch (Exception e) { fatalError("Could not connect ClientContextImpl object to ORB", e); }

    try {
      serverContext = frescoServer.create_server_context(_this());
    } catch (Exception e) { fatalError("Client could not connect to server", e); }

    // Getting some kits
    /* This is mostly convience for the app developer, not really
       related to the ClientContext */
    try {
      text = TextKitHelper.narrow(resolveKit("IDL:fresco.org/Fresco/TextKit:1.0"));
      desktop = DesktopKitHelper.narrow(
                                     resolveKit("IDL:fresco.org/Fresco/DesktopKit:1.0"));
      layout = LayoutKitHelper.narrow(resolveKit("IDL:fresco.org/Fresco/LayoutKit:1.0"));
      tool = ToolKitHelper.narrow(resolveKit("IDL:fresco.org/Fresco/ToolKit:1.0"));
      widget = WidgetKitHelper.narrow(resolveKit("IDL:fresco.org/Fresco/WidgetKit:1.0"));
      figure = FigureKitHelper.narrow(resolveKit("IDL:fresco.org/Fresco/FigureKit:1.0"));
      raster = RasterKitHelper.narrow(resolveKit("IDL:fresco.org/Fresco/RasterKit:1.0"));
      command = CommandKitHelper.narrow(
                                     resolveKit("IDL:fresco.org/Fresco/CommandKit:1.0"));
      statusSuccess("Got Kits");
    } catch (Exception e) { fatalError("Could not get Kit", e); }
  }

  // Tries to get Kit "name" from server
  public org.omg.CORBA.Object resolveKit(String name)
  {
    org.fresco.idl.fresco.KitPackage.Property[] properties = {};
    return resolveKit(name, properties);
  }

  // Tries to get Kit "name" with "properties" from server
  public org.omg.CORBA.Object resolveKit(String name,
                           org.fresco.idl.fresco.KitPackage.Property[] properties)
  {
    org.omg.CORBA.Object result = null;
    try
    {
      result = serverContext.resolve(name, properties);
    }
    catch(Exception e)
    {
      //XXX should be fatal?
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

  protected Server resolveServer(String args[])
  		throws Exception
  {
  	statusTry("Trying to find Fresco server");
  	org.omg.CORBA.Object result = null;

  	// Read vars

  	String resolveType = null; // enum: "iorfile", "iorstring", "nameservice" or "corbaloc" 
  	String filePathIOR = null;
  	String serverID = null;
  	String stringifiedIOR = null;

  	// Reading vars from commandline
  	try {
  		for (int i = 0; i < args.length; i++)
  		{
  			// Not supporting short versions, to not conflict with app
  			if (args[i].equals("--lookup-method") || args[i].equals("--export-ref"))
  				resolveType = args[i + 1];
  			else if (args[i].equals("--ior-file-path"))
  				filePathIOR = args[i + 1];
  			else if (args[i].equals("--server-id"))
  				serverID = args[i + 1];
  			else if (args[i].equals("--ior-string"))
  				stringifiedIOR = args[i + 1];
  		}
  	} catch (Exception e) { fatalError("Commandline arguments malformed", e); }

  	/*
  	// Get FRESCO_DISPLAY environment variable
	if (stringifiedIOR == null)
	{
		String envvar = System.getenv("FRESCO_DISPLAY"); -- deprecated - big suckage!
		if (envvar != null)
			stringifiedIOR = envvar;
	}
	*/

	// Set defaults
	if (resolveType == null)
	{
		if (filePathIOR != null)
			resolveType = "iorfile";
		else if (stringifiedIOR != null)
			resolveType = "iorstring";
		else
			resolveType = "iorfile";
	}
	if (filePathIOR == null)
		filePathIOR = "/tmp/fresco";
	if (serverID == null)
		serverID = "FrescoServer";

	// Try to resolve server

	// string IOR
	if (resolveType.equals("iorstring"))
		result = orb.string_to_object(stringifiedIOR);
	// file IOR
	else if (resolveType.equals("iorfile") || resolveType.equals("ior"))
	{
		 File inputFile = new File(filePathIOR, serverID);
		 Reader in = new BufferedReader(new InputStreamReader(new FileInputStream(inputFile)));
		 StringBuffer content = new StringBuffer();
		 int ch;
		 while ((ch = in.read()) != -1)
		 	content.append((char) ch);

		 String ior = content.toString().trim();
		 result = orb.string_to_object(ior);
	}
	// nameservice
	else if (resolveType.equals("nameservice") || resolveType.equals("nameserver"))
		result = resolve(serverID);
	// corbaloc
	else if (resolveType.equals("corbaloc"))
		throw new Exception("corbaloc not implemented");

	// check and narrow result
	if (result == null)
		throw new NullPointerException("Got a null server reference");
	Server resultNarrow = ServerHelper.narrow(result);

	statusSuccess("Got Fresco server reference");
	return resultNarrow;
  }



  // Other convience functions

  // Create a top-level application window on the Berlin desktop
  public void mainWindow(MainController group)
  {
    desktop.shell(group, _this());
  }

  /* Create a top-level application window on the Berlin desktop
     It does not allow to modify the event management etc., but
     takes directly a Graphic to display
  */
  public void mainWindowSimple(Graphic content)
  {
    mainWindow(tool.group(content));
  }

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
      fatalError("Application " + appTitle + " terminated", e);
    }
  }



  // Debugging

  public void fatalError(String msg, Exception e)
  {
    System.err.println("ERROR: " + msg + ": " + e);
    if (verbose)
    {
      System.err.println("Stack trace:");
      e.printStackTrace(System.err);
    }
    System.exit(1);
  };
  public void statusTry(String msg)
  {
    if (verbose)
      System.out.print(msg + "... ");
  };
  public void statusSuccess(String msg)
  {
    if (verbose)
      System.out.println(msg == null ? "done." : msg);
  };
  public void statusFailure(String msg, Exception e)
  {
  	if (verbose)
  		System.out.println((msg == null ? "failed" : msg)
  									+ (e == null ? "" : " - " + e));
  };
}
