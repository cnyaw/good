/*
  goodJniView.java
  good player Android platforma implementation.

  Copyright (c) 2014 Waync Cheng.
  All Rights Reserved.

  2014/7/6 Waync created
 */

package weilican.good;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class goodJniView extends GLSurfaceView
{
  private static int KEYS_LBUTTON = 1 << 7;
  public static int KEYS_ESCAPE = 1 << 9;

  public static int mKeys = 0;
  static float mPosX = 0;
  static float mPosY = 0;

  static public Activity activity;

  public goodJniView(Context context)
  {
    super(context);
    setRenderer(new Renderer());
  }

  @Override
  public boolean onTouchEvent(MotionEvent e) {

    switch (e.getAction())
    {
      case MotionEvent.ACTION_DOWN:
        // Fall throgh.
      case MotionEvent.ACTION_MOVE:
        mKeys |= KEYS_LBUTTON;
        mPosX = e.getX();
        mPosY = e.getY();
        return true;
      default:
        mKeys &= ~KEYS_LBUTTON;
    }

    return false;
  }

  private static class Renderer implements GLSurfaceView.Renderer
  {
    long fps = (long)(1000/60.0f);
    long startTime = System.currentTimeMillis();
    public void onDrawFrame(GL10 gl)
    {
      try {
        long endTime = System.currentTimeMillis();
        long dt = endTime - startTime;
        if (dt < fps) {
          Thread.sleep(fps - dt);
        }
        startTime = System.currentTimeMillis();
        if (goodJniLib.step(mKeys, mPosX, mPosY)) {
          activity.finish();
        }
      } catch (Exception e) {
      }
    }

    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
      goodJniLib.init(width, height);
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
    }
  }
}
