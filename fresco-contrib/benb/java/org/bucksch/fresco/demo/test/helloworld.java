/*
  A test program putting up a little window on the Berlin server

  Copyright 2002 Ben Bucksch
  BSD license
 */
package org.bucksch.fresco.demo.test;

import org.fresco.Warsaw.*;
import org.fresco.clientlib.*;

class HelloWorld
{
  static public void main(String[] args)
  {
    try {
      ClientConnection cc = new ClientConnection(args, "Hello World");
      Graphic label = cc.text.chunk(new Unistring("Hello World!").toFresco());
      cc.desktop.shell(cc.tool.group(label), cc._this());
      cc.run();  // executes ordinary Java main loop
    } catch (Exception e) {
      System.err.println("App error: " + e);
      e.printStackTrace(System.out);
    }
  }
}
