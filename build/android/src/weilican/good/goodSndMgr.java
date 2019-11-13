/*
  goodSndMgr.java
  Good player Android sound manager.

  Copyright (c) 2019 Waync Cheng.
  All Rights Reserved.

  2019/11/13 Waync created
*/

package weilican.good;

import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.util.Base64;

class SoundManager {
  String sndRes[] = new String[64];
  MediaPlayer snd[] = new MediaPlayer[64];

  synchronized boolean addSound(int idRes, byte stream[]) {
    if (0 > idRes) {
      return false;
    }

    int idSlot = internalGetIdRes(idRes);
    if (-1 == idSlot) {
      return false;
    }

    try {
      String base64Stream = Base64.encodeToString(stream, Base64.DEFAULT);
      sndRes[idRes] = "data:audio/amr;base64," + base64Stream;
      return true;
    } catch (Exception e) {
      return false;
    }
  }

  synchronized int getFreeSoundPoolIndex() {

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

  synchronized int getSound(int idRes) {
    if (0 <= idRes && sndRes.length > idRes) {

      if (null == sndRes[idRes]) {
        if (!addSound(idRes, goodJniLib.getSound(idRes))) {
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
        mp.setDataSource(sndRes[idRes]);
        mp.prepare();
        setOnCompletionListener(mp, idSnd);
        mp.start();
        snd[idSnd] = mp;
        return idSnd;
      } catch (Exception e) {
      }
    }

    return -1;
  }

  void setOnCompletionListener(MediaPlayer mp, final int idSnd) {
    mp.setOnCompletionListener(new OnCompletionListener() {
      @Override
      public void onCompletion(MediaPlayer mp) {
        releaseSound(idSnd);
      }
    });
  }

  synchronized int internalGetIdRes(int idRes) {

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
      String newSndRes[] = new String[2 * sndRes.length];
      for (int i = 0; i < sndRes.length; i++) {
        newSndRes[i] = sndRes[i];
      }
      sndRes = newSndRes;
    }

    return idRes;
  }

  synchronized boolean isSoundLooping(int id) {
    if (sndValid(id)) {
      return snd[id].isLooping();
    } else {
      return false;
    }
  }

  synchronized boolean isSoundPlaying(int id) {
    if (sndValid(id)) {
      return snd[id].isPlaying();
    } else {
      return false;
    }
  }

  synchronized void pauseAll() {
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

  synchronized void pauseSound(int id) {
    if (sndValid(id)) {
      snd[id].pause();
    }
  }

  synchronized void playSound(int id) {
    if (sndValid(id)) {
      snd[id].start();
    }
  }

  synchronized void releaseSound(int id) {
    if (sndValid(id)) {
      snd[id].release();
      snd[id] = null;
    }
  }

  synchronized void resumeAll() {
    for (int i = 0; i < snd.length; i++) {
      if (null != snd[i]) {
        snd[i].start();
      }
    }
  }

  synchronized void setSoundLooping(int id, boolean loop) {
    if (sndValid(id)) {
      snd[id].setLooping(loop);
    }
  }

  synchronized boolean sndValid(int id) {
    return 0 <= id && snd.length > id && null != snd[id];
  }

  synchronized void stopAll() {
    for (int i = 0; i < snd.length; i++) {
      if (null != snd[i]) {
        snd[i].release();
        snd[i] = null;
      }
    }
  }

  synchronized void stopSound(int id) {
    if (sndValid(id)) {
      snd[id].stop();
    }
  }
}
