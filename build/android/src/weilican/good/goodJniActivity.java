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
import android.media.MediaPlayer;
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
    sndPauseAll();
    appPause();
  }

  @Override protected void onResume()
  {
    super.onResume();
    mView.onResume();
    sndResumeAll();
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

  File sndRes[] = new File[64];
  MediaPlayer snd[] = new MediaPlayer[64];

  static public boolean sndAddSound(int idRes, byte stream[]) {
    return thisActivity.doAddSound(idRes, stream);
  }

  int internalGetIdRes(int idRes) {

    //
    // Check existance.
    //

    if (0 <= idRes && sndRes.length > idRes) {
      if (null == sndRes[idRes]) {
        return idRes;
      } else {
        return -1;
      }
    }

    //
    // Grow pool.
    //

    while (sndRes.length < idRes) {
      File newSndRes[] = new File[2 * sndRes.length];
      for (int i = 0; i < sndRes.length; i++) {
        newSndRes[i] = sndRes[i];
      }
      sndRes = newSndRes;
    }

    return idRes;
  }

  boolean doAddSound(int idRes, byte stream[]) {
    if (0 > idRes) {
      return false;
    }

    int idSlot = internalGetIdRes(idRes);
    if (-1 == idSlot) {
      return false;
    }

    try {
      File tempFile = File.createTempFile("goodtmp", ".tmp");
      tempFile.deleteOnExit();
      FileOutputStream out = new FileOutputStream(tempFile);
      out.write(stream);
      out.close();
      sndRes[idRes] = tempFile;
      return true;
    } catch (Exception e) {
      return false;
    }
  }

  static public int sndGetSound(int idRes) {
    return thisActivity.doGetSound(idRes);
  }

  int getFreeSoundPoolIndex() {

    //
    // Free stop playing sound.
    //

    for (int i = 0; i < snd.length; i++) {
      if (null != snd[i] && !snd[i].isPlaying() && !snd[i].isLooping()) {
        snd[i].release();
        snd[i] = null;
      }
    }

    //
    // Find free entity.
    //

    for (int i = 0; i < snd.length; i++) {
      if (null == snd[i]) {
        return i;
      }
    }

    //
    // Double size pool.
    //

    MediaPlayer newSnd[] = new MediaPlayer[2 * snd.length];
    for (int i = 0; i < snd.length; i++) {
      newSnd[i] = snd[i];
    }
    snd = newSnd;

    return snd.length / 2;
  }

  int doGetSound(int idRes) {
    if (0 <= idRes && sndRes.length > idRes) {

      if (null == sndRes[idRes]) {
        if (!doAddSound(idRes, goodJniLib.getSound(idRes))) {
          return -1;
        }
      }

      int idSnd = getFreeSoundPoolIndex();
      if (-1 == idSnd) {
        return -1;
      }

      //
      // Init media player.
      //

      try {
        MediaPlayer mp = new MediaPlayer();
        FileInputStream fis = new FileInputStream(sndRes[idRes]);
        mp.setDataSource(fis.getFD());
        fis.close();
        mp.prepare();
        mp.start();
        snd[idSnd] = mp;
        return idSnd;
      } catch (Exception e) {
      }
    }

    return -1;
  }

  static public void sndRelease(int id) {
    thisActivity.doReleaseSound(id);
  }

  void doReleaseSound(int id) {
    if (0 <= id && snd.length > id && null != snd[id]) {
      snd[id].release();
      snd[id] = null;
    }
  }

  static public boolean sndIsPlaying(int id) {
    return thisActivity.doIsSoundPlaying(id);
  }

  boolean doIsSoundPlaying(int id) {
    if (0 <= id && snd.length > id && null != snd[id]) {
      return snd[id].isPlaying();
    } else {
      return false;
    }
  }

  static public boolean sndIsLooping(int id) {
    return thisActivity.doIsSoundLooping(id);
  }

  boolean doIsSoundLooping(int id) {
    if (0 <= id && snd.length > id && null != snd[id]) {
      return snd[id].isLooping();
    } else {
      return false;
    }
  }

  static public void sndSetLoop(int id, boolean loop) {
    thisActivity.doSetSoundLooping(id, loop);
  }

  void doSetSoundLooping(int id, boolean loop) {
    if (0 <= id && snd.length > id && null != snd[id]) {
      snd[id].setLooping(loop);
    }
  }

  static public void sndPlay(int id) {
    thisActivity.doPlaySound(id);
  }

  void doPlaySound(int id) {
    if (0 <= id && snd.length > id && null != snd[id]) {
      snd[id].start();
    }
  }

  static public void sndStop(int id) {
    thisActivity.doStopSound(id);
  }

  void doStopSound(int id) {
    if (0 <= id && snd.length > id && null != snd[id]) {
      snd[id].stop();
    }
  }

  static public void sndPause(int id) {
    thisActivity.doPauseSound(id);
  }

  void doPauseSound(int id) {
    if (0 <= id && snd.length > id && null != snd[id]) {
      snd[id].pause();
    }
  }

  static public void sndStopAll() {
    thisActivity.doSndStopAll();
  }

  void doSndStopAll() {
    for (int i = 0; i < snd.length; i++) {
      if (null != snd[i]) {
        snd[i].release();
        snd[i] = null;
      }
    }
  }

  void sndPauseAll() {
    for (int i = 0; i < snd.length; i++) {
      if (null != snd[i]) {
        if (snd[i].isLooping()) {
          snd[i].pause();
        } else {
          snd[i].release();
          snd[i] = null;
        }
      }
    }
  }

  void sndResumeAll() {
    for (int i = 0; i < snd.length; i++) {
      if (null != snd[i]) {
        snd[i].start();
      }
    }
  }
}
