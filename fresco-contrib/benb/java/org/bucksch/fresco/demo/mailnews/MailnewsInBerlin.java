/*
  This implements the native (low-level) Fresco frontend for Mailnews4corba.
  Mailnews4corba is the backend of Mozilla Mailnews, wrapped into a
  CORBA interface.

  Copyright 2001 Ben Bucksch
  BSD license
 */
package org.bucksch.fresco.demo.mailnews;

import org.mozilla.Mailnews.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.fresco.Warsaw.*;
import org.fresco.clientlib.*;

class MailnewsInBerlin {
  public static void main(String[] args) {
    try {
      System.out.println("Started");
      ClientContextImpl cc = new ClientContextImpl(args);
      System.out.println("Created clientContext");
      org.omg.CORBA.Object objRef = cc.resolve("Mailnews4corba");
      System.out.println("Got Mailnews app object from naming service");
      MailnewsApp mailnews = MailnewsAppHelper.narrow(objRef);
      System.out.println("Narrowed to MailnewsApp interface");

      mailnews.sendUnsentMsgs();
      System.out.println("Invoked example function");

      System.out.println(mailnews.interfacesProperties());
      System.out.println("Invoked example function");

      // Create Fresco dialog (yeah!)
      Graphic detailscontainer = cc.layout.vbox();
      C c = new C(cc, detailscontainer);
      Graphic treecontainer = cc.layout.vbox();
      Unistring desc_unistr = new Unistring("Accounts");
      Graphic label = cc.text.chunk(desc_unistr.toFresco());
    TreeImpl accountlist = new TreeImpl(label, "Accounts", null, null, null,
                                          treecontainer, cc);
      Account[] accounts = mailnews.accounts();
      for (int i = 0; i < accounts.length; i++)
      {
        ShowAccountDetails clickImpl = new ShowAccountDetails(accounts[i], c);
        Command clickRef = CommandHelper.narrow(
                                    cc.poa.servant_to_reference(clickImpl));
        desc_unistr = new Unistring(accounts[i].name());
        label = cc.layout.hbox();
        label.append_graphic(cc.layout.valign(cc.figure.pixmap(
                                   cc.image.create("berlin-16.png")), 0.5));
        label.append_graphic(cc.text.chunk(desc_unistr.toFresco()));
        TreeItemImpl accountItem = new TreeItemImpl(label, accounts[i].name(),
                                                    clickRef,
                                                    null /*see below*/, null);
        OpenFolder openImpl = new OpenFolder(accounts[i].theIncomingServer(),
                                             accountItem, c);
        Command openRef = CommandHelper.narrow(
                                    cc.poa.servant_to_reference(openImpl));
        accountItem.setOnOpenCommand(openRef);
        accountlist.addChild(accountItem);
      }
      Graphic mainwindow = cc.layout.hbox();
      mainwindow.append_graphic(treecontainer);
      mainwindow.append_graphic(detailscontainer);
      //cc.desktop.shell(cc.tool.group(c.background(mainwindow)));
      cc.desktop.shell(cc.tool.group(c.background(cc.layout.vfixed(mainwindow, 3000.))));

      // wait for invocations
      java.lang.Object sync = new java.lang.Object();
      synchronized (sync) {
        sync.wait();
      }

    } catch (Exception e) {
      System.err.println(
            "MailnewsInBerlin error: " + e);
      e.printStackTrace(System.out);
    }
  }
}

// Provides common functions
class C
{
  public ClientContextImpl cc;
  protected Graphic detailsWindow;

  public C(ClientContextImpl a_cc, Graphic a_detailsWindow)
  {
    cc = a_cc;
    detailsWindow = a_detailsWindow;
  }
  public Graphic label(String text)
  {
    //System.out.println("3.1");
    Unistring open_unistr = new Unistring(text);
    return cc.layout.margin(cc.tool.rgb(
                    cc.text.chunk(open_unistr.toFresco()), 0., 0., 0.), 20.);
    /*
    System.out.println("3.2");
    short[] btext = open_unistr.toFresco();
    System.out.println("3.3");
    Graphic chunk = cc.text.chunk(btext);
    System.out.println("3.4");
    Graphic rgb = cc.tool.rgb(chunk, 0., 0., 0.);
    System.out.println("3.5");
    Graphic margin = cc.layout.margin(rgb, 20.);
    System.out.println("3.6");
    return margin;
    */
  }
  public Graphic background(Graphic foreground)
  {
    Warsaw.ToolKitPackage.FrameSpec fs = new Warsaw.ToolKitPackage.FrameSpec();
    Color col = new Color();
    col.red = 255; col.green = 255; col.blue = 255; col.alpha = 0;
    fs.foreground(col);
    //    Graphic content = cc.tool.rgb(details, 255., 255., 255.);
    return cc.tool.frame(foreground, 1., fs, true);
  }
  public void ShowDetails(Graphic windowContents)
  {
    detailsWindow.first_child_graphic().remove();
    // XXX leak of iterator?
    detailsWindow.append_graphic(windowContents);
  }
}

class ShowAccountDetails extends CommandPOA
{
  Account account;
  C c;
  ShowAccountDetails(Account an_account, C a_c)
  {
    account = an_account;
    c = a_c;
  }
  public void execute(org.omg.CORBA.Any a)
  {
    Graphic details = c.cc.layout.hbox();
    Graphic labels = c.cc.layout.vbox();
    Graphic vars = c.cc.layout.vbox();
    details.append_graphic(labels);
    details.append_graphic(vars);

    labels.append_graphic(c.label("Name"));
    labels.append_graphic(c.label("Type"));
    labels.append_graphic(c.label("Login"));

    IncomingServer server = account.theIncomingServer();
    vars.append_graphic(c.label(server.name()));
    ServerType type = server.type();
    String type_str = "";
    if (type == ServerType.pop3)
      type_str = "POP3";
    else if (type == ServerType.imap)
      type_str = "IMAP";
    else if (type == ServerType.nntp)
      type_str = "NNTP";
    else if (type == ServerType.none)
      type_str = "none";
    else if (type == ServerType.other)
      type_str = "other";
    else
      type_str = "unexpected";
    vars.append_graphic(c.label(type_str));
    vars.append_graphic(c.label(server.username() + "@" + server.host()));
    c.ShowDetails(c.background(details));
  }
  public void destroy() { /*XXX*/ }
}

class OpenFolder extends CommandPOA
{
  public FolderContainer folder;
  public TreeItemImpl item;
  protected C c;
  protected boolean done = false;

  OpenFolder(FolderContainer a_folder, TreeItemImpl an_item, C a_c)
  {
    folder = a_folder;
    item = an_item;
    c = a_c;
  }
  public void execute(org.omg.CORBA.Any a)
  {
    if (done)
      return;

    FolderContainer[] folders = folder.containedFolders();
    for (int i = 0; i < folders.length; i++)
    {
      Command clickRef = null;
      Folder msgfolder = FolderHelper.narrow(folders[i]);
      System.out.println(folders[i].name() + " is a msgfolder: " +
                         msgfolder != null);
      if (msgfolder != null)
      {
        ShowMsgs clickImpl = new ShowMsgs(msgfolder, c);
        try { clickRef = CommandHelper.narrow(
                                 c.cc.poa.servant_to_reference(clickImpl)); }
        catch(Exception e) { System.err.println(e); }
      }
      Unistring desc_unistr = new Unistring(folders[i].name());
      Graphic label = c.cc.layout.hbox();
      label.append_graphic(c.cc.layout.valign(c.cc.figure.pixmap(
                                    c.cc.image.create("berlin-16.png")), 0.5));
      label.append_graphic(c.cc.text.chunk(desc_unistr.toFresco()));
      TreeItemImpl folderItem = new TreeItemImpl(label, folders[i].name(),
                                                 clickRef,
                                                 null /*see below*/, null);
      OpenFolder openImpl = new OpenFolder(folders[i], folderItem, c);
      Command openRef = null;
      try { openRef = CommandHelper.narrow(
                                 c.cc.poa.servant_to_reference(openImpl)); }
      catch(Exception e) { System.err.println(e); }
      folderItem.setOnOpenCommand(openRef);
      item.addChild(folderItem);
    }
    done = true;
  }
  public void destroy() { /*XXX*/ }
}

class ShowMsgs extends CommandPOA
{
  public Folder folder;
  protected C c;

  ShowMsgs(Folder a_folder, C a_c)
  {
    folder = a_folder;
    c = a_c;
  }
  public void execute(org.omg.CORBA.Any a)
  {
    Graphic list = c.cc.layout.vbox();
    list.append_graphic(c.label("Messages (Subjects)"));

    Message[] msgs = folder.containedMsgs();
    for (int i = 0; i < msgs.length; i++)
    {
      //      list.append_graphic(c.label(msgs[i].headPart().subject()));
      Graphic label = c.label(msgs[i].headPart().subject());

      ShowMsg clickImpl = new ShowMsg(msgs[i], c);
      Command clickRef = null;
      try { clickRef = CommandHelper.narrow(
                                 c.cc.poa.servant_to_reference(clickImpl)); }
      catch(Exception e) { System.err.println(e); }
      list.append_graphic(c.cc.tool.button(label, clickRef));
    }
    c.ShowDetails(c.background(list));
  }
  public void destroy() { /*XXX*/ }
}

class ShowMsg extends CommandPOA
{
  public Message msg;
  protected C c;

  ShowMsg(Message a_msg, C a_c)
  {
    msg = a_msg;
    c = a_c;
  }
  public void execute(org.omg.CORBA.Any a)
  {
    msg.bodyPart().text();
    /*
    System.out.println("click on msg");
    Body body = msg.bodyPart();
    System.out.println("have body");
    body.text();
    System.out.println("got text");
    */
  }
  public void destroy() { /*XXX*/ }
}
