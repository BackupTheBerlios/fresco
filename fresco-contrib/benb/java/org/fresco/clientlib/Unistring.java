/*
	This implements basic functions for Fresco clients.

	Copyright 2001 Ben Bucksch
	BSD license
 */
package org.fresco.clientlib;

import org.fresco.Warsaw.*;

public class Unistring
{
	String content = null;

	public Unistring() {}
	public Unistring(java.lang.String str_java)
	{
		content = str_java;
	}
	public Unistring(short[] str_fresco)
	{
		content = "";
		for (int i = 0; i < str_fresco.length; i++)
			content += (char)str_fresco[i];
	}
	public String toJava()
	{
		return content;
	}
	public short[] toFresco()
	{
		short[] str_fresco = new short[content.length()];
		for (int i = 0; i < content.length(); i++)
			str_fresco[i] = (short)content.charAt(i);
		return str_fresco;
	}
}
