/*
  This implements basic functions for Fresco clients.

  Copyright 2001-2004 Ben Bucksch
  BSD license
 */
package org.fresco.clientlib;

public class Unistring
{
  String content = null;

  public Unistring() {}
  public Unistring(java.lang.String javaStr)
  {
    content = javaStr;
  }
  public Unistring(short[] frescoStr)
  {
    content = "";
    for (int i = 0; i < frescoStr.length; i++)
      content += (char)frescoStr[i];
  }
  public String toJava()
  {
    return content;
  }
  public String toString()
  {
  	return content;
  }
  public short[] toFresco()
  {
    short[] frescoStr = new short[content.length()];
    for (int i = 0; i < content.length(); i++)
      frescoStr[i] = (short)content.charAt(i);
    return frescoStr;
  }
}
