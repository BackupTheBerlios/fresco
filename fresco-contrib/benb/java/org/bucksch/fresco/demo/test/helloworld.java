/*
	A test program putting up a little window on the Berlin server

	Copyright 2001 Ben Bucksch
	BSD license
 */
package org.bucksch.fresco.demo.test;

import org.fresco.Warsaw.*;
import org.fresco.clientlib.*;

class HelloWorld {
  static public void main(String[] args) {
    try {
      System.out.println("Started");
		  ClientConnection cc = new ClientConnection(args, "Hello World");
      cc.statusSuccess("Created clientContext");
			cc.desktop.shell(cc.tool.group(
                    cc.text.chunk(new Unistring("Hello World!").toFresco())),
                       cc._this());

			Graphic label = cc.text.chunk(new Unistring("Hello World!").toFresco());
			Graphic mainwindow = cc.layout.vbox();
			mainwindow.append_graphic(label);
			cc.desktop.shell(cc.tool.group(mainwindow), cc._this());
      //			cc.desktop.shell(cc.tool.group(
      //   background(cc, cc.layout.vfixed(mainwindow, 3000.))), cc._this());

      cc.run();  // executes ordinary Java main loop

    } catch (Exception e) {
      System.err.println("HTML Viewer error: " + e);
      e.printStackTrace(System.out);
    }
  }
	static public Graphic background(ClientConnection cc, Graphic foreground)
	{
		org.fresco.Warsaw.ToolKitPackage.FrameSpec fs =
                             new org.fresco.Warsaw.ToolKitPackage.FrameSpec();
		Color col = new Color();
		col.red = 255; col.green = 255; col.blue = 255; col.alpha = 0;
		fs.foreground(col);
		return cc.tool.frame(foreground, 1., fs, true);
	}
}
