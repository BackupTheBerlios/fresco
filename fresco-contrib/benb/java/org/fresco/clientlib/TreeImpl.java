/*
	This implements a Tree-View, a widget that can display hierarchical data.

	It is implemented completely client-side and it does not itself participate
	in the scene graph (but it does insert server-side objects into the scene
	graph).

	It is itself a TreeItemImpl, so you can pass it as parent to TreeItemsImpls.
	Currently, it even displays itself, meaning that there is always exactly
	one node at the top level in the tree, and this node is (always) visible.

	Copyright 2001 Ben Bucksch
	BSD license
 */
package org.fresco.clientlib;

import org.fresco.Warsaw.*;

public class TreeImpl extends TreeItemImpl
{
	public TreeImpl(Graphic a_description, String a_textlabel,
									Command an_onClick,//See TreeItemImpl()
									Command an_onOpen, Command an_onClose,   // ditto
									Graphic parentitem_container,  // where tree to append to
								  ClientConnection a_cc)
	{
		super (a_description, a_textlabel, an_onClick, an_onOpen, an_onClose);
		Graphic pane = a_cc.layout.hbox();  // the scrolling area
		//parentitem_container.append_graphic(a_cc.widget.scrollpane(pane));
		parentitem_container.append_graphic(a_cc.tool.group(pane));
		setParent(null, pane, a_cc);
	}
	public TreeItemImpl getSelectedItem()
	{
		return selectedItem;
	}
	public void setSelectedItem(TreeItemImpl newSelection)
	{
		System.out.println("selecteditem " + newSelection.textlabel);
		if (newSelection == selectedItem)
			return;

		selectedItem.setSelected(false);
		newSelection.setSelected(true);  /* Note: with a non-compliant impl. of
																		 Selected, this might cause a loop. */
		selectedItem = newSelection;
	}
	// Only for Selected of the child. Do not use.
	// Used to notify root node about selection change
	public void descendantGotSelected(TreeItemImpl a_descendant)
	{
		System.out.println("tree desc");
		setSelectedItem(a_descendant);
	}

	protected TreeItemImpl selectedItem = this;
}
