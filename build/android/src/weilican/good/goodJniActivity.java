/*
  goodJniActivity.java
  Good player Android.

  Copyright (c) 2015 Waync Cheng.
  All Rights Reserved.

  2015/6/17 Waync created
 */

package weilican.good;

import android.app.Activity;
import android.os.Bundle;
import android.content.Intent;
import android.net.Uri;
import android.view.KeyEvent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import java.io.*;

public class goodJniActivity extends Activity
{
  static
  {
    System.loadLibrary("good");
  }

  final int ACTIVITY_CHOOSE_FILE = 1;

  static goodJniActivity thisActivity;
  goodJniView mView;

  static native void setcwd(String path);
  static native void appDestroy();
  static native void appPause();

  SoundManager snd = new SoundManager();

  @Override protected void onCreate(Bundle b)
  {
    super.onCreate(b);
    mView = new goodJniView(getApplication());
    goodJniView.activity = this;
    setContentView(mView);

    thisActivity = this;

    File dataDir = getFilesDir();
    setcwd(dataDir.toString());         // Set default current working path.

    DoChooseFile();
  }

  protected void DoChooseFile() {

    //
    // Choose file.
    //

    Intent chooseFile;
    Intent intent;
    chooseFile = new Intent(Intent.ACTION_GET_CONTENT);
    chooseFile.addCategory(Intent.CATEGORY_OPENABLE);
    chooseFile.setType("*/*");
    intent = Intent.createChooser(chooseFile, "Choose a good project file");
    startActivityForResult(intent, ACTIVITY_CHOOSE_FILE);
  }

  static public void doIntEvent(int i) {
    thisActivity.handleIntEvent(i);
  }

  protected void handleIntEvent(int i) {
  }

  @Override
  protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    if (ACTIVITY_CHOOSE_FILE != requestCode) {
      return;
    }

    if (RESULT_OK == resultCode) {
      sndStopAll();
      Uri uri = data.getData();
      String filePath = goodJniLib.getPath(thisActivity, uri);
      if (filePath.startsWith("/file")) {
        filePath = filePath.substring(5);
      }
      File file = new File(filePath);
      setcwd(file.getParent());         // Change cwd to selected path.
      goodJniLib.create(filePath, getAssets());
    } else {
      finish();
    }
  }

  @Override
  public boolean onKeyDown(int keyCode, KeyEvent event)
  {
    if (KeyEvent.KEYCODE_BACK == keyCode) {
      goodJniView.mKeys |= goodJniView.KEYS_ESCAPE;
      return true;
    }
    return super.onKeyDown(keyCode, event);
  }

  @Override
  public boolean onKeyUp(int keyCode, KeyEvent event)
  {
    goodJniView.mKeys = 0;
    return false;
  }

  @Override protected void onPause()
  {
    super.onPause();
    mView.onPause();
    snd.pauseAll();
    appPause();
  }

  @Override protected void onResume()
  {
    super.onResume();
    mView.onResume();
    snd.resumeAll();
  }

  @Override protected void onDestroy()
  {
    appDestroy();
    super.onDestroy();
  }

  static public void doOpenUrl(String url)
  {
    Uri uri = Uri.parse(url);
    Intent it = new Intent(Intent.ACTION_VIEW, uri);
    thisActivity.startActivity(it);
  }

  //
  // Image support.
  //

  static public int[] imgFromBitmap(Bitmap b) {
    if (null == b) {
      return null;
    }
    int pixels[] = new int[2 + b.getWidth() * b.getHeight()];
    pixels[0] = b.getWidth();
    pixels[1] = b.getHeight();
    b.getPixels(pixels, 2, b.getWidth(), 0, 0, b.getWidth(), b.getHeight());
    for (int y = 0; y < pixels[1]; y++) {
      for (int x = 0; x < pixels[0]; x++) {
        int index = y * pixels[0] + x + 2;
        int pixel = pixels[index];
        int A = (pixel >> 24) & 0xff;
        int R = (pixel >> 16) & 0xff;
        int G = (pixel >> 8) & 0xff;
        int B = pixel & 0xff;
        pixels[index] = (A << 24) | (B << 16) | (G << 8) | R;
      }
    }
    return pixels;
  }

  static public int[] imgGetImage(byte stream[]) {
    BitmapFactory.Options opt = new BitmapFactory.Options();
    opt.inScaled = false;
    Bitmap b = BitmapFactory.decodeByteArray(stream, 0, stream.length, opt);
    return imgFromBitmap(b);
  }

  static public int[] imgGetCharImage(int size, int ch, boolean bAntiAlias) {
    String text = String.valueOf((char)ch);
    Paint paint = new Paint();
    paint.setAntiAlias(bAntiAlias);
    paint.setTextSize(size);
    paint.setColor(Color.WHITE);
    paint.setTextAlign(Paint.Align.LEFT);
    float baseline = -paint.ascent();
    int width = (int) (paint.measureText(text) + 0.5f);
    int height = (int) (baseline + paint.descent() + 0.5f);
    Bitmap b = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    Canvas canvas = new Canvas(b);
    canvas.drawText(text, 0, baseline, paint);
    return imgFromBitmap(b);
  }

  //
  // Sound support.
  //

  static public boolean sndAddSound(int idRes, byte stream[]) {
    return thisActivity.snd.addSound(idRes, stream);
  }

  static public int sndGetSound(int idRes) {
    return thisActivity.snd.getSound(idRes);
  }

  static public void sndRelease(int id) {
    thisActivity.snd.releaseSound(id);
  }

  static public boolean sndIsPlaying(int id) {
    return thisActivity.snd.isSoundPlaying(id);
  }

  static public boolean sndIsLooping(int id) {
    return thisActivity.snd.isSoundLooping(id);
  }

  static public void sndSetLoop(int id, boolean loop) {
    thisActivity.snd.setSoundLooping(id, loop);
  }

  static public void sndPlay(int id) {
    thisActivity.snd.playSound(id);
  }

  static public void sndStop(int id) {
    thisActivity.snd.stopSound(id);
  }

  static public void sndPause(int id) {
    thisActivity.snd.pauseSound(id);
  }

  static public void sndStopAll() {
    thisActivity.snd.stopAll();
  }
}
