/*
   Displays an XML document in a treeview.
   The XML node content is displayed on the right, if you click on the node
   on the left.

   BSD license
   Copyright 2001 Ben Bucksch
*/

package org.bucksch.fresco.demo.html;

import javax.xml.parsers.*;
import org.w3c.dom.*;
import org.fresco.Warsaw.*;
import org.fresco.clientlib.*;

class Viewer {
  public static void main(String[] args) {
    try {
      System.out.println("Started");
		  ClientContextImpl cc = new ClientContextImpl(args);
      System.out.println("Created clientContext");

			String uri = args.length > 0 ? args[0] : "http://www.bucksch.org";

      System.out.print("Loading " + uri + "...");
			DocumentBuilderFactory parserfactory =
				                                DocumentBuilderFactory.newInstance();
			parserfactory.setIgnoringComments(true);
			parserfactory.setCoalescing(true);
			DocumentBuilder parser = parserfactory.newDocumentBuilder();
			Document doc = parser.parse(uri);
      System.out.println(" Done.");

      System.out.print("Rendering...");
			Tree tree = new Tree(doc, cc);
      System.out.println(" Done.");

      // loop
      java.lang.Object sync = new java.lang.Object();
      synchronized (sync) {
        sync.wait();
      }

    } catch (Exception e) {
      System.err.println(
            "HTML Viewer error: " + e);
      e.printStackTrace(System.out);
    }
  }
}

class Tree
{
	protected CDetails c;
	Tree(Document doc, ClientContextImpl cc)
	{
		// Create Fresco dialog (yeah!)
		Graphic detailscontainer = cc.layout.vbox();
		c = new CDetails(cc, detailscontainer);
		Graphic treecontainer = cc.layout.vbox();

		TreeImpl doctree = new TreeImpl(c.label("Document"), "Document",
																		null, null, null,
																		treecontainer, cc);
		ShowNode(doc, doctree);

		Graphic mainwindow = cc.layout.hbox();
		mainwindow.append_graphic(treecontainer);
		mainwindow.append_graphic(detailscontainer);
		cc.desktop.shell(cc.tool.group(c.background(mainwindow)));
	}
	public void ShowNode(Node node, TreeItemImpl treeitem_parent)
	{
		String name = node.getNodeName();

		ShowContent clickImpl = new ShowContent(node, c);
		Command clickRef = null;
		try { clickRef = CommandHelper.narrow(
														     c.cc.poa.servant_to_reference(clickImpl)); }
		catch(Exception e) { System.err.println(e); }

		TreeItemImpl nodeItem = new TreeItemImpl(c.label(name), name,
																						 clickRef, null, null);
		treeitem_parent.addChild(nodeItem);
		NodeList nodes = node.getChildNodes();
		for (int i = 0; i < nodes.getLength(); i++)
			ShowNode(nodes.item(i), nodeItem);
	}
}

// Provides common functions
class CDetails
{
	public ClientContextImpl cc;
	protected Graphic detailsWindow;

	public CDetails(ClientContextImpl a_cc, Graphic a_detailsWindow)
	{
		cc = a_cc;
		detailsWindow = a_detailsWindow;
	}
	public Graphic label(String text)
	{
		Unistring open_unistr = new Unistring(text);
		return cc.layout.margin(cc.tool.rgb(
		  	  					cc.text.chunk(open_unistr.toFresco()), 0., 0., 0.), 20.);
	}
	public Graphic background(Graphic foreground)
	{
		Warsaw.ToolKitPackage.FrameSpec fs = new Warsaw.ToolKitPackage.FrameSpec();
		Color col = new Color();
		col.red = 255; col.green = 255; col.blue = 255; col.alpha = 0;
		fs.foreground(col);
		return cc.tool.frame(foreground, 1., fs, true);
	}
	public void ShowDetails(Graphic windowContents)
	{
		detailsWindow.first_child_graphic().remove();
		detailsWindow.append_graphic(windowContents);
	}
}

class ShowContent extends CommandPOA
{
	public Node node;
  protected CDetails c;

	ShowContent(Node a_node, CDetails a_c)
	{
		node = a_node;
		c = a_c;
	}
	public void execute(org.omg.CORBA.Any a)
	{
		System.out.println(node.getNodeValue());
		c.ShowDetails(c.cc.text.chunk(
									new Unistring(node.getNodeValue()).toFresco()));
	}
	public void destroy() { /*XXX*/ }
}
