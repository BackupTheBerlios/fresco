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
  protected Unistring appTitle = null;

  public ClientContextImpl(org.omg.PortableServer.POA aPOA,
                           String anAppTitle)
  {
    poa = aPOA;
    appTitle = new Unistring(anAppTitle);
  }
  public short[] application_title()
  {
    return appTitle.toFresco();
  };
  public short[] user_name()
  {
    String result = System.getProperty("user.name", "joe");
    return new Unistring(result).toFresco();
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
