/*
  Rendering "engine". Provides impl. for elements and class Renderer, which
  switchboards to them.
  You just call Renderer.GetGraphic() (after constructing Renderer
  for your DOM node) and get a complete scene graph for that
  subtree of the DOM tree.

  Copyright 2001 Ben Bucksch
  BSD license
 */
package org.bucksch.fresco.demo.html;

import javax.xml.parsers.*;
import org.w3c.dom.*;
import org.fresco.Warsaw.*;
import org.fresco.clientlib.*;

// root of rendering class hierarchy
abstract class Element
{
  protected ClientContextImpl cc;
  protected Node node;
  protected BrowserWindow window = null;

  /* virtual functions */
  public Element(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    cc = a_cc;
    node = a_node;
    window = bw;
  }

  /* Useful to let subclasses add decorators. */
  /* Default impl.. May well be overwritten. */
  public Graphic GetGraphic()
  {
    return GetChilds();
  }
  abstract public Graphic GetChilds();

  /* Iterates the child nodes of node_parent (which is represented on the
     screen by graphic_parent) and adds them to the graphic_parent */
  public void ProcessChilds(Graphic graphic_parent)
  {
    NodeList nodes = node.getChildNodes();
    for (int i = 0; i < nodes.getLength(); i++)
      graphic_parent.append_graphic(AddNode(nodes.item(i)));
  }
  /* Checks the type ("name") of node, invokes the appropriate function
     and returns the representation */
  public Graphic AddNode(Node a_node)
  {
    return new Renderer(node, window, cc).AddNode(a_node);
  }
}



/* Implement flow, block, inline */
/* Note that e.g. InlinesElement is an element, which can only contain inlines.
   (Logically - nothing in the InlinesElement enforces that atm.)
   It is *not* necessarily an inline element itself.
   An example for an InlinesElement is <p>. */

/* Creates a Graphic, whose childs can be both blocks and inlines.
   "Flow" is the word the HTML spec uses for that. */
class MixedElement extends Element
{
  public MixedElement(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public Graphic GetChilds()
  {
    Graphic container = cc.layout.vbox(); // XXXXXXX!!!!
    ProcessChilds(container);
    return container;
  }
}
/* Creates a Graphic, whose childs can only be blocks (e.g. <ol>). */
class BlocksElement extends Element
{
  public BlocksElement(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public Graphic GetChilds()
  {
    Graphic container = cc.layout.vbox();
    ProcessChilds(container);
    return container;
  }
}
/* Creates a Graphic, whose childs can only be inlines (e.g. <em>, #text). */
class InlinesElement extends Element
{
  public InlinesElement(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public Graphic GetChilds()
  {
    //Graphic container = cc.layout.hbox(); // XXXXXXX!!!!
    Graphic container = cc.layout.vbox(); // XXXXXXX!!!!
    ProcessChilds(container);
    return container;
  }
}
class VboxElement extends Element
{
  public VboxElement(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public Graphic GetChilds()
  {
    Graphic container = cc.layout.vbox();
    ProcessChilds(container);
    return container;
  }
}
class HboxElement extends Element
{
  public HboxElement(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public Graphic GetChilds()
  {
    Graphic container = cc.layout.hbox();
    ProcessChilds(container);
    return container;
  }
}
/* Cannot contain further childs */
class LeafElement extends Element
{
  public LeafElement(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public Graphic GetChilds()
  {
    // XXX Exception
    return cc.layout.vbox();
  }
}



// Unexpected element
class NullElement extends MixedElement
{
  protected boolean ignore = false;
  public NullElement(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
    String value = node.getNodeValue();
    if (node.getNodeName() == "#text"
        //value == "" || value == " " - doesn't work
        && (
             value.length() == 0 ||
             (
               value.length() == 1
               && value.charAt(0) == ' '
             )
           )
       )
      ignore = true;

    if (!ignore)
    {
      System.err.println("ERROR: unexpected <" + node.getNodeName() +
                         "> element");
      for (Node i = node.getParentNode(); i != null; i = i.getParentNode())
        System.err.println("  child of <" + i.getNodeName() + ">");
    }
  }
  public Graphic GetGraphic()
  {
    if (!ignore)
    {
      System.err.println("  Value: <" + node.getNodeValue() + ">");
      return cc.layout.vbox(); /* dummy - don't output content of unexpected
                                  element. Probably violates spec -> should
                                  be changed. */
    }
    else
      return cc.layout.vbox(); // dummy
  }
}

// <ul>, <ol>, etc. and <li>
class List extends VboxElement
{
  public List(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public void ProcessChilds(Graphic graphic_parent)
  {
    NodeList nodes = node.getChildNodes();
    int no = 0;
    for (int i = 0; i < nodes.getLength(); i++)
    {
      Node item = nodes.item(i);
      if (item.getNodeName().toLowerCase() == "li")
        graphic_parent.append_graphic(AddNode(item, ++no));
        //graphic_parent.append_graphic(new C(cc).frame(AddNode(item, ++no)));
      else
        graphic_parent.append_graphic(
                           new NullElement(item, window, cc).GetGraphic());
    }
  }
  public Graphic AddNode(Node a_node, int no)
  {
    Graphic li = cc.layout.hbox();

    // Bullet
    String parent = node.getNodeName().toLowerCase();
    String bullet = "#";
    if (parent == "ul")
      bullet = "*";
    else if (parent == "ol")
      bullet = String.valueOf(no) + ".";
    Unistring bullet_uni = new Unistring(bullet);
    /*
    li.append_graphic(cc.layout.rmargin(cc.text.chunk(bullet_uni.toFresco()),
                                        150.));
                                           // XXX use fixed instead of margin
    */
    //li.append_graphic(new C(cc).frame(cc.text.chunk(bullet_uni.toFresco())));
    li.append_graphic(cc.text.chunk(bullet_uni.toFresco()));

    // Content
    li.append_graphic(new MixedElement(a_node, window, cc).GetGraphic());
    //Graphic mixed = new MixedElement(a_node, window, cc).GetGraphic();
    //li.append_graphic(new C(cc).frame(mixed));
    //li.append_graphic(new CDetails(cc, null).label("foo"));

    return li;
  }
}

// <table>, <tr>, <td> etc.
class Table extends VboxElement
{
  public Table(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public void ProcessChilds(Graphic graphic_parent)
  {
    /* Since Fresco doesn't support a dynamically resizable Grid (I think),
       we need 2 passes: One that counts the number of cells per
       row (to allocate the right#sized Grid) and one that fills the Grid.
       Note that this fixed nature of the Grid prevents us from progressive
       rendering, i.e. displaying the data as soo as we get it from the
       network. :-( */

    final boolean skipUnexpected = true;  // ignore non-tr/td/th elements?

    // Determine size of Grid
    int noOfRows = 0;  // number of rows
    int noOfCells = 0;  // number of rows
    NodeList nodes = node.getChildNodes();
    for (int i = 0; i < nodes.getLength(); i++)
    {
      Node item = nodes.item(i);
      if (item.getNodeName().toLowerCase() == "tr" || !skipUnexpected)
      {
        noOfRows++;
        int noOfCellsHere = 0;
        NodeList nodes2 = item.getChildNodes();
        for (int j = 0; j < nodes2.getLength(); j++)
        {
          String nodename = nodes2.item(j).getNodeName().toLowerCase();
          if (nodename == "td")
            noOfCellsHere++;
          else if (nodename == "th")
            noOfCellsHere++;
          else if (!skipUnexpected)
            noOfCellsHere++;
        }
        if (noOfCellsHere > noOfCells)
          noOfCells = noOfCellsHere;
      }
    }

    // Create Grid
    Layout.GridPackage.Index size = new Layout.GridPackage.Index();
    size.row = noOfRows;
    size.col = noOfCells;
    Layout.Grid grid = cc.layout.fixed_grid(size);

    int cur_row = 0;  /* This is != i, because we might skip some nodes,
                         e.g. empty #text nodes. At the end, cur_row should
                         be == noOfRows. */
    for (int i = 0; i < nodes.getLength(); i++)
    {
      int cur_col = 0;  // ditto
      Node item = nodes.item(i);
      if (item.getNodeName().toLowerCase() == "tr")
      {
        NodeList nodes2 = item.getChildNodes();
        for (int j = 0; j < nodes2.getLength(); j++)
        {
          Node item2 = nodes2.item(j);
          String nodename = item2.getNodeName().toLowerCase();
          Layout.GridPackage.Index pos = new Layout.GridPackage.Index();
          pos.row = cur_row;
          pos.col = cur_col;
          if (nodename == "td")
          {
            grid.replace(AddTDNode(item2), pos);
            cur_col++;
          }
          else if (nodename == "th")
          {
            grid.replace(AddTDNode(item2), pos);
            cur_col++;
          }
          else if (!skipUnexpected)
          {
            grid.replace(new NullElement(item, window, cc).GetGraphic(), pos);
            cur_col++;
          }
        }
        cur_row++;
      }
      else if (!skipUnexpected)
      {
        Layout.GridPackage.Index pos = new Layout.GridPackage.Index();
        pos.row = cur_row;
        pos.col = 0;
        grid.replace(new NullElement(item, window, cc).GetGraphic(), pos);
        cur_row++;
      }
    }
    graphic_parent.append_graphic(grid);
  }
  public Graphic AddTDNode(Node a_node)
  {
    return frame(cc.layout.margin(
                      new MixedElement(a_node, window, cc).GetGraphic(), 20.));
  }
  public Graphic AddTHNode(Node a_node)
  {
    // XXX bold, halign
    return frame(cc.layout.margin(
                      new MixedElement(a_node, window, cc).GetGraphic(), 20.));
  }
  // does not modify class vars
  public Graphic frame(Graphic content)
  {
    org.fresco.Warsaw.ToolKitPackage.FrameSpec spec =
                             new org.fresco.Warsaw.ToolKitPackage.FrameSpec();
    spec.brightness(org.fresco.Warsaw.ToolKitPackage.FrameType.outset, 0.5);
    return cc.tool.frame(content, 20., spec, false);
  }
}

// <h1>, <h2>, ...
class HeadingElement extends InlinesElement
{
  protected int order;
  public HeadingElement(Node a_node, BrowserWindow bw, ClientContextImpl a_cc,
                        int an_order /* 1=h1, 2=h2, ... */)
  {
    super(a_node, bw, a_cc);
    order = an_order;
  }
  public Graphic GetGraphic()
  {
    return cc.layout.lmargin(GetChilds(), 300.);
    //return cc.layout.halign(GetChilds(), 0.5); // XXX produces odd graphics
  }
}

// <a href>
class AnchorElement extends InlinesElement
{
  public AnchorElement(Node a_node, BrowserWindow bw,
                       ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public Graphic GetGraphic()
  {
    // set up click command
    Command clickRef = null;
    org.w3c.dom.Element elem = (org.w3c.dom.Element)node;
    CommandPOA clickImpl = new AnchorClick(elem.getAttribute("href"), window);
    try { clickRef = CommandHelper.narrow(
                                    cc.poa.servant_to_reference(clickImpl)); }
    catch(Exception e) { System.err.println(e); }

    // set up representation
    org.fresco.Warsaw.ToolKitPackage.FrameSpec spec =
                             new org.fresco.Warsaw.ToolKitPackage.FrameSpec();
    spec.brightness(org.fresco.Warsaw.ToolKitPackage.FrameType.outset, 0.5);
    return cc.tool.button(cc.tool.rgb(cc.tool.frame(
                   GetChilds(), 30., spec, false), 0., 0., 255.), clickRef);
    //return cc.tool.button(cc.tool.rgb(GetChilds(), 0., 0., 255.), clickRef);
  }
}
class AnchorClick extends CommandPOA
{
  String uri;
  BrowserWindow window;
  AnchorClick(String a_uri, BrowserWindow a_window)
  {
    uri = a_uri;
    window = a_window;
  }
  public void execute(org.omg.CORBA.Any a)
  {
    window.LoadPage(uri);
  }
  public void destroy() { /*XXX*/ }
}

// #text
class TextElement extends LeafElement
{
  public TextElement(Node a_node,
                     BrowserWindow bw, // should not be needed
                     ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public Graphic GetGraphic()
  {
    String value = node.getNodeValue();
    if (node.getNodeName() == "#text"
        //value == "" || value == " " - doesn't work
        && (
             value.length() == 0 ||
             (
               value.length() == 1
               && value.charAt(0) == ' '
             )
           )
       )
      return cc.layout.hbox();
    else
    {
      final int max_width = 40;
      Graphic paragraph = cc.layout.vbox();
      String rest = value;
      for (; rest.length() > max_width; )
      {
        int lineend = max_width;
        for (; rest.charAt(lineend) != ' '; lineend--)
          ;
        paragraph.append_graphic(GetLine(rest.substring(0, lineend)));
        rest = rest.substring(lineend + 1, rest.length() - 1);
      }
      paragraph.append_graphic(GetLine(rest));
      return paragraph;
    }
  }
  protected Graphic GetLine(String line)
  {
    return cc.tool.rgb(cc.text.chunk(new Unistring(line).toFresco()),
                       0., 0., 0.);
    //return cc.text.chunk(new Unistring(node.getNodeValue()).toFresco());
    //return new C(cc).frame(cc.text.chunk(
    //                     new Unistring(node.getNodeValue()).toFresco()));
  }
}


/* Processes elementes (can handle inline, block and flowed childs).
   Mostly switchboarding.
   Use this, if you don't know / care about the type of the node. It will
   invoke the right impl.. */
/* The processing of some childs is defered to special classes.
   The processing of some other childs is done here, for simplicity. */
public class Renderer extends Element
{
  public Renderer(Node a_node, BrowserWindow bw, ClientContextImpl a_cc)
  {
    super(a_node, bw, a_cc);
  }
  public Graphic GetChilds()
  {
    return null;
  }
  public Graphic GetGraphic()
  {
    return AddNode(node);
  }

  public Graphic AddNode(Node a_node)
  {
    String name = a_node.getNodeName().toLowerCase();
    //System.out.println(name);

    if (name == "#text")
      return new TextElement(a_node, window, cc).GetGraphic();
    else if (name == "#document")
      return new VboxElement(a_node, window, cc).GetGraphic();

    else if (name == "html")
      return new VboxElement(a_node, window, cc).GetGraphic();
    else if (name == "head")
      return new VboxElement(a_node, window, cc).GetGraphic();
    else if (name == "title")
      return new InlinesElement(a_node, window, cc).GetGraphic();
    else if (name == "body")
      return new MixedElement(a_node, window, cc).GetGraphic();

    else if (name == "div")
      return new MixedElement(a_node, window, cc).GetGraphic();
    else if (name == "span")
      return new InlinesElement(a_node, window, cc).GetGraphic();

    else if (name == "p")
      return ElementP(a_node);

    else if (name == "ul")
      return new List(a_node, window, cc).GetGraphic();
    else if (name == "ol")
      return new List(a_node, window, cc).GetGraphic();
    else if (name == "li")
      return new NullElement(a_node, window, cc).GetGraphic();
           // already consumed by List

    else if (name == "table")
      return new Table(a_node, window, cc).GetGraphic();
    else if (name == "tr")
      return new NullElement(a_node, window, cc).GetGraphic();
           // already consumed by Table
    else if (name == "th")
      return new NullElement(a_node, window, cc).GetGraphic();
           // already consumed by Table
    else if (name == "td")
      return new NullElement(a_node, window, cc).GetGraphic();
           // already consumed by Table

    else if (name == "h1")
      return new HeadingElement(a_node, window, cc, 1).GetGraphic();
    else if (name == "h2")
      return new HeadingElement(a_node, window, cc, 2).GetGraphic();
    else if (name == "h3")
      return new HeadingElement(a_node, window, cc, 3).GetGraphic();
    else if (name == "h4")
      return new HeadingElement(a_node, window, cc, 4).GetGraphic();
    else if (name == "h5")
      return new HeadingElement(a_node, window, cc, 5).GetGraphic();
    else if (name == "h6")
      return new HeadingElement(a_node, window, cc, 6).GetGraphic();

    else if (name == "em")
      return ElementEM(a_node);
    else if (name == "strong")
      return ElementSTRONG(a_node);

    else if (name == "a")
      return new AnchorElement(a_node, window, cc).GetGraphic();
    else
      return new NullElement(a_node, window, cc).GetGraphic();

    //else if (name == "")
  }


  /* The element("tag", "node-name")-specific functions follow.
     They return a graphic for the node (plus sub-nodes, if appropriate). */
  public Graphic ElementEM(Node a_node)
  {
    return new InlinesElement(a_node, window, cc).GetGraphic();
  }
  public Graphic ElementSTRONG(Node a_node)
  {
    return new InlinesElement(a_node, window, cc).GetGraphic();
  }
  public Graphic ElementP(Node a_node)
  {
    return cc.layout.vmargin(
                     new InlinesElement(a_node, window, cc).GetGraphic(),
                     100.);
  }
}
