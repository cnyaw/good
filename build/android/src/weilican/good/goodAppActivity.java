/*
  goodAppActivity.java
  good player Android platforma implementation.

  Copyright (c) 2014 Waync Cheng.
  All Rights Reserved.

  2014/7/6 Waync created
 */

package weilican.good;

import android.os.Bundle;

public class goodAppActivity extends goodJniActivity
{
  @Override protected void onCreate(Bundle b)
  {
    super.onCreate(b);
  }

  public static native void setVStickMode(boolean bEnable);
}
