/*
  A test program putting up a dummy tree view on the Berlin server

  Copyright 2001 Ben Bucksch
  BSD license
 */
package org.bucksch.fresco.demo.test;

import org.fresco.Warsaw.*;
import org.fresco.clientlib.*;

class DemoApp
{
  static public void main(String[] args)
  {
    try {
      System.out.println("Started");
      ClientConnection cc = new ClientConnection(args, "Demo");
      cc.statusSuccess("Created clientContext");

      Graphic detailscontainer = cc.layout.vbox();
      Graphic treecontainer = cc.layout.vbox();
      Unistring desc_unistr = new Unistring("A Tree");
      Graphic label = cc.text.chunk(desc_unistr.toFresco());
      TreeImpl list = new TreeImpl(label, "A Tree", null, null, null,
                                   treecontainer, cc);
      for (int i = 0; i < 10; i++)
      {
        desc_unistr = new Unistring("Item " + i);
        label = cc.layout.hbox();
        label.append_graphic(cc.layout.valign(cc.figure.pixmap(
                                   cc.image.create("Berlin-16.png")), 0.5));
        label.append_graphic(cc.text.chunk(desc_unistr.toFresco()));
        TreeItemImpl item = new TreeItemImpl(label, "Item " + i,
                                             null, null, null);
        list.addChild(item);
      }
      Graphic mainwindow = cc.layout.hbox();
      mainwindow.append_graphic(treecontainer);
      mainwindow.append_graphic(detailscontainer);
      cc.desktop.shell(cc.tool.group(background(cc, cc.layout.vfixed(mainwindow, 3000.))), cc._this());

      cc.run();  // loop

    } catch (Exception e) {
      System.err.println("App error: " + e);
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
    //    Graphic content = cc.tool.rgb(details, 255., 255., 255.);
    return cc.tool.frame(foreground, 1., fs, true);
  }
}
