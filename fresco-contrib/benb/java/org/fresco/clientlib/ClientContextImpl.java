/*
  Implemention for Fresco's ClientContext interface

  Copyright 2001-2002 Ben Bucksch
  BSD license
 */
package org.fresco.clientlib;

import org.fresco.idl.fresco.*;

public class ClientContextImpl extends ClientContextPOA
{
  // Generally useful
  public org.omg.PortableServer.POA poa = null;
  public Unistring app_title = null;

  public ClientContextImpl(org.omg.PortableServer.POA a_poa,
                           String an_app_title)
  {
    poa = a_poa;
    app_title = new Unistring("Java client");
  }
  public short[] application_title()
  {
    return app_title.toFresco();
  };
  public short[] user_name()
  {
    Unistring result = new Unistring("joe");
    return result.toFresco();
    // XXX get OS user. which interface? java.lang.System.getProperty("USERNAME", "joe")? <file:///usr/local/packages/java/docs/jdkse/docs/api/java/lang/System.html#getProperty(java.lang.String)>
  };
  public void ping()
  {
    return;
  };
  public Command exit()
  {
    CommandPOA com = new ExitCommand();
    Command comRef = null;
    try
    {
      org.omg.CORBA.Object objRef = poa.servant_to_reference(com);
      comRef = CommandHelper.narrow(objRef);
    }
    catch (Exception e)
    {
      System.err.println("ERROR: Could not create exit command" + e);
      System.exit(1);
    }
    return comRef;
  };
}

class ExitCommand extends CommandPOA
{
  ExitCommand()
  {
  }
  public void execute(org.omg.CORBA.Any a)
  {
    System.exit(0);
  }
  public void destroy() { /*XXX*/ }
}
