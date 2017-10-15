/*
  goodJniLib.java
  good player Android platforma implementation.

  Copyright (c) 2014 Waync Cheng.
  All Rights Reserved.

  2014/7/6 Waync created
 */

package weilican.good;

import android.content.res.AssetManager;

public class goodJniLib
{
  public static native void create(String file, AssetManager mgr);
  public static native void init(int width, int height);
  public static native boolean step(int keys, float mousex, float mousey);
  public static native byte[] getSound(int idRes);
}
