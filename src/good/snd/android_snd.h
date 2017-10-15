
//
// android_snd.h
// Android sound manager implementation.
//
// Copyright (c) 2015 Waync Cheng.
// All Rights Reserved.
//
// 2015/6/11 Waync created.
//

#pragma once

#include <map>
#include <string>

#include <jni.h>

extern JNIEnv *tmpEnv;

namespace good {

namespace snd {

class AndroidSound
{
public:

  int s;

  typedef std::map<std::string, std::pair<std::string, int> > SoundMap; // <name,<stream,id>> id is a serial number.

  static SoundMap& getSoundMap()
  {
    static SoundMap m;
    return m;
  }

  static jbyteArray getSoundStream(int idRes)
  {
    const SoundMap &m = getSoundMap();
    SoundMap::const_iterator it = m.begin();
    for (; m.end() != it; ++it) {
      if (it->second.second == idRes) {
        const std::string &s = it->second.first;
        jbyteArray barr = tmpEnv->NewByteArray(s.size());
        tmpEnv->SetByteArrayRegion(barr, 0, s.size(), (const jbyte*)s.c_str());
        return barr;
      }
    }
    return (jbyteArray)0;
  }

  explicit AndroidSound(int snd) : s(snd)
  {
  }

  static bool isExist(std::string const& name)
  {
    const SoundMap& m = getSoundMap();
    return m.end() != m.find(name);
  }

  static AndroidSound getSound(std::string const& name, bool bBgm)
  {
    return AndroidSound(-1);
  }

  static AndroidSound getSound(std::string const& name, bool bBgm, std::string const& stream)
  {
    if (!tmpEnv) {
      return AndroidSound(-1);
    }

    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    SoundMap& m = getSoundMap();
    SoundMap::const_iterator it = m.find(name);
    if (m.end() == it) {

      //
      // Add new sound stream resource.
      //

      int idRes = m.size();
      jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndAddSound", "(I[B)Z");
      if (mid) {
        jbyteArray barr = tmpEnv->NewByteArray(stream.size());
        tmpEnv->SetByteArrayRegion(barr, 0, stream.size(), (const jbyte*)stream.c_str());

        if (!tmpEnv->CallStaticBooleanMethod(cls, mid, idRes, barr)) {
          return AndroidSound(-1);
        }
      }

      m[name] = std::make_pair(stream, idRes);
      it = m.find(name);
    }

    //
    // Get new sound instance.
    //

    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndGetSound", "(I)I");
    if (mid) {
      return AndroidSound(tmpEnv->CallStaticIntMethod(cls, mid, it->second.second));
    } else {
      return AndroidSound(-1);
    }
  }

  static void releaseSound(AndroidSound snd)
  {
    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndRelease", "(I)V");
    if (mid) {
      tmpEnv->CallStaticVoidMethod(cls, mid, snd.s);
    }
  }

  static float getSoundVolume()
  {
    return .0f;
  }

  static void setSoundVolume(float vol)
  {
  }

  static float getMusicVolume()
  {
    return .0f;
  }

  static void setMusicVolume(float vol)
  {
  }

  bool isValid() const
  {
    return 0 < s;
  }

  bool isPlaying() const
  {
    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndIsPlaying", "(I)Z");
    if (mid) {
      return tmpEnv->CallStaticBooleanMethod(cls, mid, s);
    }
    return false;
  }

  void play()
  {
    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndPlay", "(I)V");
    if (mid) {
      tmpEnv->CallStaticVoidMethod(cls, mid, s);
    }
  }

  void stop()
  {
    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndStop", "(I)V");
    if (mid) {
      tmpEnv->CallStaticVoidMethod(cls, mid, s);
    }
  }

  void pause()
  {
    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndPause", "(I)V");
    if (mid) {
      tmpEnv->CallStaticVoidMethod(cls, mid, s);
    }
  }

  bool isLoop() const
  {
    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndIsLooping", "(I)Z");
    if (mid) {
      return tmpEnv->CallStaticBooleanMethod(cls, mid, s);
    }
    return false;
  }

  void setLoop(bool loop)
  {
    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndSetLoop", "(IZ)V");
    if (mid) {
      tmpEnv->CallStaticVoidMethod(cls, mid, s, loop);
    }
  }
};

} // namespace snd

} // namespace good

// end of android_snd.h
