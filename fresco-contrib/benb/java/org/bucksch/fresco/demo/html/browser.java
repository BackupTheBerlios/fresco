/*
  A very simple HTML Browser.

  Copyright 2001 Ben Bucksch
  BSD license
 */
package org.bucksch.fresco.demo.html;

import javax.xml.parsers.*;
import org.w3c.dom.*;
import org.fresco.Warsaw.*;
import org.fresco.clientlib.*;

class BrowserApp
{
  public static void main(String[] args)
  {
    try {
      System.out.println("Started");
      ClientContextImpl cc = new ClientContextImpl(args);
      System.out.println("Created clientContext");

      String uri = args.length > 0 ? args[0] : "http://www.bucksch.org";

      new BrowserWindow(cc).LoadPage(uri);

      cc.run() // loop

    } catch (Exception e) {
      System.err.println(
            "HTML Viewer error: " + e);
      e.printStackTrace(System.out);
    }
  }
}

// Functions for an external app developer who might load the browser
class Browser
{
}

class BrowserWindow
{
  protected ClientContextImpl cc;
  protected Document doc;
  protected Graphic canvas_container = null;  /* this is where we will hook
                                                 up the page. */
  protected TextBuffer urlfield;

  BrowserWindow(ClientContextImpl a_cc)
  {
    cc = a_cc;
  }
  public void LoadPage(String uri) /* throws CantLoad, CantParse */
  {
    try {

      System.out.print("Loading and parsing " + uri + "...");
      DocumentBuilderFactory parserfactory =
                              DocumentBuilderFactory.newInstance();
      parserfactory.setIgnoringComments(true);
      parserfactory.setCoalescing(true);
      DocumentBuilder parser = parserfactory.newDocumentBuilder();
      doc = parser.parse(uri);
      doc.normalize();
      System.out.println(" Done.");

      System.out.print("Rendering...");
      setUpWindow(new Renderer(doc, this, cc).GetGraphic());
      System.out.println(" Done.");

      clear_buffer(urlfield);
      urlfield.insert_string(new Unistring(uri).toFresco());

    } catch (java.io.FileNotFoundException e) {
      NotFound(e);
    } catch (java.net.UnknownHostException e) {
      NotFound(e);
    } catch (Exception e) {
      System.err.println(
            "HTML Viewer error: " + e);
      e.printStackTrace(System.out);
    }
  }
  protected void setUpWindow(Graphic page)
  {
    // Set page background, margin etc.
    org.fresco.Warsaw.ToolKitPackage.FrameSpec fs =
                              new org.fresco.Warsaw.ToolKitPackage.FrameSpec();
    Color col = new Color();
    col.red = 255; col.green = 255; col.blue = 255; col.alpha = 0;
    fs.foreground(col);
    //Graphic canvas = cc.tool.frame(cc.layout.margin(page, 100.), 1., fs,true);
    Graphic canvas = cc.tool.rgb(cc.tool.frame(cc.layout.margin(page, 100.),
                                               1., fs, true), 0., 0., 0.);

    if (canvas_container == null)
      createWindow();
    else
      canvas_container.first_child_graphic().remove();

    canvas_container.append_graphic(canvas);
  }
  protected void createWindow()
  {
    Graphic mainwin = cc.layout.vbox();
    canvas_container = cc.layout.hbox();  // member var
    mainwin.append_graphic(createURLbar());
    mainwin.append_graphic(canvas_container);
    cc.desktop.shell(cc.tool.group(mainwin));
  }
  
  // Helper functions. They do not modify any class vars
  protected Graphic createURLbar()
  {
    Graphic toolbar = cc.layout.hbox();
    try {
      urlfield = cc.command.text();
      urlfield.value();
      toolbar.append_graphic(cc.tool.text_input(
                                        cc.text.simple_viewer(urlfield),
                                        urlfield));
      toolbar.append_graphic(cc.widget.button(
                               cc.text.chunk(new Unistring("Go").toFresco()),
                               new GoCommand(urlfield, this)._this()));
    }
    catch(Exception e) { System.err.println(e); }
    return toolbar;
  }
  protected void NotFound(Exception e)
  {
      String errormsg = "ERROR: File not found:";
      System.err.println(errormsg + e);
      Graphic vbox = cc.layout.vbox();
      vbox.append_graphic(cc.text.chunk(new Unistring(errormsg).toFresco()));
      vbox.append_graphic(cc.text.chunk(new Unistring(" " + e).toFresco()));
      setUpWindow(cc.tool.rgb(vbox, 0., 0., 0.));
  }
  static public void clear_buffer(TextBuffer tb)
  {
    tb.position(0);
    tb.remove_forward(tb.size());
  }
}

class GoCommand extends CommandPOA
{
  TextBuffer urlfield;
  BrowserWindow window;
  GoCommand(TextBuffer an_urlfield, BrowserWindow a_window)
  {
    urlfield = an_urlfield;
    window = a_window;
  }
  public void execute(org.omg.CORBA.Any a)
  {
    short[] uri_uni = urlfield.value();
    String uri = new Unistring(uri_uni).toJava();
    window.LoadPage(uri);
    //window.LoadPage(new Unistring(urlfield.value()).toJava());
  }
  public void destroy() { /*XXX*/ }
}


/*

// Provides common functions
class C extends ClientContextImpl
{
  public C()
  {
    super();
  }
  public Graphic label(String text)
  {
    Unistring open_unistr = new Unistring(text);
    return cc.layout.margin(cc.tool.rgb(
                    cc.text.chunk(open_unistr.toFresco()), 0., 0., 0.), 20.);
  }
  public Graphic frame(Graphic content)
  {
    org.fresco.Warsaw.ToolKitPackage.FrameSpec spec =
                            new org.fresco.Warsaw.ToolKitPackage.FrameSpec();
    spec.brightness(org.fresco.Warsaw.ToolKitPackage.FrameType.outset, 0.5);
    return cc.tool.frame(content, 20., spec, false);
  }
}
*/
