/*
	This implements an item in a Tree-View

	Copyright 2001 Ben Bucksch
	BSD license
 */
package org.fresco.clientlib;

import org.fresco.idl.fresco.*;
import org.fresco.idl.fresco.ToolKitPackage.*;

public class TreeItemImpl
{
	public TreeItemImpl(Graphic a_description, // node that appears on screen
											String a_textlabel,  // debugging
											Command an_onClick,
											Command an_onOpen, Command an_onClose /* triggers
																								 after click on the twister */
										 )
	{
		description = a_description;
		textlabel = a_textlabel;
		onClick = an_onClick;
		onOpen = an_onOpen;
		onClose = an_onClose;
		if (onOpen != null)
			setLeafPriviledged(2/*unknown*/);
	}
	/* These functions only work before you add the item to a parent via
		 the praent's AddChild method.
		 They are provided, so you can construct a command that requires this
		 item as parameter for its instructor. This especially useful with
		 onClick to lazily add childs. */
	public void setOnClickCommand(Command c)
	{
		onClick = c;
	}
	public void setOnOpenCommand(Command c)
	{
		onOpen = c;
	  setLeafPriviledged(2/*unknown*/);
	}
	public void setOnCloseCommand(Command c)
	{
		onClose = c;
	}

	public void addChild(TreeItemImpl child)
	{
		if (child == null)  // invalid call
			return;   //XXX exception
		if (representation == null) /* not properly initialized
																	 (SetParent not called) */
			return;   //XXX exception

		setLeafPriviledged(0/*false*/);

		child.setParent(this, container, cc);
	}
	public void removeChild(TreeItemImpl child)
	{
		// XXX Not implemented
	}

	public boolean isOpen()
	{
		return m_isOpen;
	}
	public boolean getOpen()
	{
		return m_isOpen;
	}
	public void setOpen(boolean newState)
	{
		if (newState == m_isOpen)
			return;  // Nothing to do

		if (newState)
			container_parentgraphic.append_graphic(container);
		else
		{
			container_parentgraphic.first_child_graphic().remove();
			// XXX do I leak the iterator?

			// if a descandant was selected, select this
			// XXX
		}

		m_isOpen = newState;

		adjustTwister();
	}
	public int isLeaf()
	{
		return m_isLeaf;
	}
	public int getLeaf()
	{
		return m_isLeaf;
	}
	public void setLeaf(int newState)
	{
		if (isLeaf() == 0/*false*/)
			return;  //XXX Exception: protected
  		/* Rationale: If leaf is false, we know for sure that we have childs.
	  		 Don't let any anonymous guy overwrite that */
		if (newState < 0 || newState > 2)
			return;  //XXX Exception
		if (newState == getLeaf())
			return;  // Nothing to do

		setLeafPriviledged(newState);
	}
	// This is the protected/trusted version of setLeaf
	protected void setLeafPriviledged(int newState)
	{
		if (newState < 0 || newState > 2)
			return;  //XXX Exception
		if (newState == m_isLeaf)
			return;  // Nothing to do

		m_isLeaf = newState;

		adjustTwister();
	}

	// Only call, if you know that it needs change
	protected void adjustTwister()
	{
		if (twister == null)
			return;  // Not initialized
		char symbol;
		if (m_isLeaf == 0/*false*/)
		{
			if (isOpen())
				symbol = '-';
			else
				symbol = '+';
		}
		else if (m_isLeaf == 1/*true*/)
			symbol = ' ';
		else if (m_isLeaf == 2/*unknown*/)
			symbol = '?';
		else
			symbol = '#';  // must never happen
	  twister.first_child_graphic().remove();
		//twister.append_graphic(cc.layout.fixed_size(cc.text.glyph((short)symbol),
		//																								100., 80.));
		// Buggy
		twister.append_graphic(cc.layout.hfixed(cc.text.glyph((short)symbol),100.));
		//twister.append_graphic(cc.text.glyph((short)symbol));
	}

	public boolean isSelected()
	{
		return m_isSelected;
	}
	public boolean getSelected()
	{
		return m_isSelected;
	}
	public void setSelected(boolean newState)
	{
		System.out.println("selected " + newState + " " + textlabel);
		if (newState == m_isSelected)
			return;

		m_isSelected = newState;

		if (m_isSelected)
		{
	  	// Notify root
			/* Note: This *must* happen after |isSelected = newState|, otherwise
				 there might be a loop, because the root may in turn call this function
				 just to be sure (its function may be called by something else). */
			descendantGotSelected(this);
		}

		// Adjust representation
		Color col = new Color();
		col.green = 1.; col.blue = 1.;
		col.red = m_isSelected ? 0. : 1.;
		//col.alpha = m_isSelected ? 0. : 1.; -- what did intend here?
		desc_background = background(desc_graphic, col);
		//  Replace
		GraphicIterator i = desc_background_parent.first_child_graphic();
		i.remove();
		i.destroy();
		desc_background_parent.append_graphic(desc_background);
	}

	// Only for AddChild of the parent. Do not use.
	public void setParent(TreeItemImpl the_parent,
												Graphic parentitem_container,
												ClientConnection a_cc)
	{
		if (parent != null) // SetParent called more than once
			return;

		parent = the_parent;
		cc = a_cc;

		// Create Graphics for this item
		Color col = new Color();
		col.red = 1.; col.green = 1.; col.blue = 1.; col.alpha = 1.;
		representation = background(cc.layout.vbox(), col);
		Graphic text = cc.layout.hbox();
		twister = frame(cc.layout.hbox());
		adjustTwister();
		OnClick internal_onClick = new OnClick(this, onClick, cc);
		OnOpenClose internal_onOpenClose =
			                             new OnOpenClose(this, onOpen, onClose, cc);
		Command internal_onClick_Ref = null;
		Command internal_onOpenClose_Ref = null;
		try
		{
		  org.omg.CORBA.Object objRef =
			                          cc.poa.servant_to_reference(internal_onClick);
			internal_onClick_Ref = CommandHelper.narrow(objRef);

		  objRef = cc.poa.servant_to_reference(internal_onOpenClose);
			internal_onOpenClose_Ref = CommandHelper.narrow(objRef);
		}
		catch (Exception e)
		{
			System.err.println(e);
		}
		desc_graphic = cc.tool.button(description, internal_onClick_Ref);
		col.alpha = 1.;
   	desc_background = background(desc_graphic, col);
	  desc_background_parent = cc.layout.hbox();  /* dummy, so we can replace
																									 background easier */
		desc_background_parent.append_graphic(desc_background);
		text.append_graphic(cc.tool.button(twister, internal_onOpenClose_Ref));
		text.append_graphic(desc_background_parent);
		Graphic item = cc.layout.margin(cc.tool.rgb(text, 0., 0., 0.), 20.);
		representation.append_graphic(item);
		/* Note: parentitem_container is the childs-container of the parent
		   TreeItemImpl. container_parentgraphic is the parent Graphic of
			 *our* container. container_parentgraphic is needed to easily
			 add/remove container from the scene graph (i.e. show/hide). */
		container_parentgraphic = cc.layout.vbox();
		representation.append_graphic(cc.layout.lmargin(container_parentgraphic,
																										150.));

		parentitem_container.append_graphic(representation);

		container = frame(cc.layout.vbox());  // will be added to scene graph in Open
	}
	// Only for Selected of the child. Do not use.
	// Used to notify root node about selection change
	public void descendantGotSelected(TreeItemImpl a_descendant)
	{
		System.out.println("desc item " + textlabel);
		if (parent != null)
			parent.descendantGotSelected(a_descendant);
	}

  // Helper
  // Does not modify any class data
  public Graphic background(Graphic foreground, Color color)
	{
		FrameSpec fs = new FrameSpec();
		fs.foreground(color);
		return cc.tool.frame(foreground, 1., fs, true);
	}
	// debugging
  public Graphic frame(Graphic content)
	{
		return content;
		/*
		FrameSpec spec = new FrameSpec();
		spec.brightness(FrameType.outset, 0.5);
		return cc.tool.frame(content, 20., spec, false);
		// Buggy
		*/
	}

	// XXX clean up vars, especially access
	// Actual data
	protected boolean m_isSelected = false;
	protected boolean m_isOpen = false;

	public TreeItemImpl parent = null;  /* If parent == null, then assume
																						that this is the root node */
  public Graphic description;
  public String textlabel;
  protected Command onClick;
  protected Command onOpen;
  protected Command onClose;
	protected int m_isLeaf = 1/*true*/;  // enum: 0=false, 1=true, 2=unknown

	// Graphic data
	protected Graphic representation = null;
	protected Graphic twister = null;  // parent of the "+" / "-" icon
	protected Graphic desc_background;  // the background color of the text
  protected Graphic desc_background_parent; /* The Graphic that contains
                                               desc_background */
  protected Graphic desc_graphic; // the node for the text on screen
	protected Graphic container;  // room for childs
	protected Graphic container_parentgraphic; // allows easy add/rm of container

  // Helper
	protected ClientConnection cc;
}

abstract class OnEvent extends CommandPOA
{
  public TreeItemImpl item;
  public Command app_command;

	// Caching
	protected boolean haveAppCommand;

  // Helper
	protected ClientConnection cc;

	OnEvent(TreeItemImpl an_item, Command an_app_command, ClientConnection a_cc)
	{
		item = an_item;
    app_command = an_app_command;
		haveAppCommand = (app_command != null);
		cc = a_cc;
	}
  public void destroy() { /*XXX*/ }
}

class OnClick extends OnEvent
{
	OnClick(TreeItemImpl an_item, Command an_app_command, ClientConnection a_cc)
	{
		super(an_item, an_app_command, a_cc);
	}
	public void execute(org.omg.CORBA.Any a)
	{
		System.out.println("OnClick for " + item.textlabel);
    item.setSelected(true);
		if (haveAppCommand)
			app_command.execute(a);
	}
}

class OnOpenClose extends OnEvent
{
	// for onOpen, I reuse app_command from superclass
  public Command app_command_close;
	protected boolean haveAppCommandClose;

	OnOpenClose(TreeItemImpl an_item,
				 Command an_app_command_for_open,
				 Command an_app_command_for_close,
				 ClientConnection a_cc)
	{
		super(an_item, an_app_command_for_open, a_cc);
		app_command_close = an_app_command_for_close;
		haveAppCommandClose = (app_command_close != null);
	}
	public void execute(org.omg.CORBA.Any a)
	{
		System.out.println("OnOpenClose for " + item.textlabel);
		boolean wasOpen = item.getOpen();
		if (wasOpen)
		{
			// Hide childs, invoke app's onClose command
			item.setOpen(false);
			if (haveAppCommandClose)
				app_command_close.execute(a);
		}
		else
		{
			// Display childs, invoke app's onOpen command
			item.setOpen(true);
			if (haveAppCommand)  // app's Open Command
				app_command.execute(a);
			if (item.getLeaf() == 2/*unknown*/)
				    /* If the app's open Command added childs, the addChild method
							 would have set leaf to false already */
				item.setLeaf(1/*true*/);  /* assume that if the first call to the
							 app's onOpen Command didn't add any childs, subsequent ones
							 won't either. Not necessarily true, but I see no other way to
							 ever get out of the unknown state. */
		}
	}
}
