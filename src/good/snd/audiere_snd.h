
//
// audiere_snd.h
// Audiere sound manager implementation.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/11/01 Waync created.
//

#pragma once

#include "audiere.h"

namespace good {

namespace snd {

class AudiereSoundResourceItem
{
public:

  audiere::SoundEffectPtr sfx;
  audiere::OutputStreamPtr bgm;

  AudiereSoundResourceItem() : sfx(0), bgm(0)
  {
  }

  void free()
  {
    if (sfx) {
      sfx->stop();
      sfx = 0;
    } else if (bgm) {
      bgm->stop();
      bgm = 0;
    }
  }

  bool isValid() const
  {
    return sfx || bgm;
  }

  bool isPlaying() const
  {
    if (bgm) {
      return bgm->isPlaying();
    } else {
      return false;
    }
  }

  void play()
  {
    if (sfx) {
      sfx->play();
    } else if (bgm) {
      bgm->play();
    }
  }

  void stop()
  {
    if (sfx) {
      sfx->stop();
    } else if (bgm) {
      bgm->stop();
    }
  }

  void pause()
  {
    //
    // Not support.
    //
  }

  bool isLoop() const
  {
    if (bgm) {
      return bgm->getRepeat();
    } else {
      return false;
    }
  }

  void setLoop(bool loop)
  {
    if (bgm) {
      bgm->setRepeat(loop);
    }
  }

  void setVolume(float vol)
  {
    if (sfx) {
      sfx->setVolume(vol);
    } else if (bgm) {
      bgm->setVolume(vol);
    }
  }
};

class AudiereSoundResource
{
  AudiereSoundResource() : device(0), volSnd(.8f), volMus(.8f)
  {
    device = audiere::OpenDevice();
    if (0 == device) {
      SW2_TRACE_ERROR("[snd][audiere] open sound device failed!");
    }
  }

  audiere::AudioDevicePtr device;

  float volSnd, volMus;

  std::map<std::string, int> snd;       // ID map to items.
  sw2::ObjectPool<AudiereSoundResourceItem> items;

public:

  static AudiereSoundResource& inst()
  {
    static AudiereSoundResource i;
    return i;
  }

  ~AudiereSoundResource()
  {
    free();
    device = 0;
  }

  void free()
  {
    for (int i = items.first(); -1 != i; i = items.next(i)) {
      items[i].free();
    }

    items.clear();
    snd.clear();
  }

  bool isExist(std::string const& name) const
  {
    return snd.end() != snd.find(name);
  }

  int getSound(std::string const& name, bool bBgm)
  {
    if (0 == device) {
      return -1;
    }

    std::map<std::string, int>::iterator it = snd.find(name);
    if (snd.end() != it) {
      items[it->second].setVolume(volSnd);
      return it->second;
    }

    std::ifstream ifs(name.c_str(), std::ios::binary);
    if (!ifs) {
      SW2_TRACE_ERROR("open sound file %s failed", name.c_str());
      return -1;
    }

    ifs.seekg(0, std::ios_base::end);
    int lenstream = (int)ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    std::string str;
    str.resize(lenstream);

    ifs.read((char*)str.data(), lenstream);

    return getSound(name, bBgm, str);
  }

  int getSound(std::string const& name, bool bBgm, std::string const& stream)
  {
    if (0 == device) {
      return -1;
    }

    std::map<std::string, int>::iterator it = snd.find(name);
    if (snd.end() != it) {
      items[it->second].setVolume(volSnd);
      return it->second;
    }

    audiere::FilePtr fp = audiere::CreateMemoryFile(
                                     (void*)stream.data(),
                                     (int)stream.size());
    if (0 == fp) {
      SW2_TRACE_ERROR("create sound %s memory stream failed", name.c_str());
      return -1;
    }

    int id = -1;

    if (bBgm) {

      audiere::OutputStreamPtr bgm = audiere::OpenSound(device, fp, true);
      if (0 == bgm) {
        SW2_TRACE_ERROR("open streaming sound %s failed", name.c_str());
        return -1;
      }

      id = items.alloc();
      if (-1 != id) {
        bgm->setVolume(volMus);
        items[id].bgm = bgm;
      }

    } else {

      audiere::SoundEffectPtr sfx = audiere::OpenSoundEffect(
                                               device,
                                               fp,
                                               audiere::MULTIPLE);
      if (0 == sfx) {
        SW2_TRACE_ERROR("open sound effect %s failed", name.c_str());
        return -1;
      }

      id = items.alloc();
      if (-1 != id) {

        sfx->setVolume(volSnd);
        items[id].sfx = sfx;

        snd[name] = id;
      }
    }

    return id;
  }

  void releaseSound(int s)
  {
    if (!items.isUsed(s)) {
      return;
    }

    AudiereSoundResourceItem& snd = items[s];
    if (snd.sfx) {
      return;
    }

    snd.bgm = 0;

    items.free(s);
  }

  float getSoundVolume() const
  {
    return volSnd;
  }

  void setSoundVolume(float vol)
  {
    if (.0f <= vol && 1.0f >= vol) {

      volSnd = vol;

      for (int i = items.first(); -1 != i; i = items.next(i)) {
        AudiereSoundResourceItem& snd = items[i];
        if (snd.sfx) {
          snd.setVolume(vol);
        }
      }
    }
  }

  float getMusicVolume() const
  {
    return volMus;
  }

  void setMuiscVolume(float vol)
  {
    if (.0f <= vol && 1.0f >= vol) {

      volMus = vol;

      for (int i = items.first(); -1 != i; i = items.next(i)) {
        AudiereSoundResourceItem& snd = items[i];
        if (snd.bgm) {
          snd.setVolume(vol);
        }
      }
    }
  }

  bool isValid(int s) const
  {
    return items.isUsed(s);
  }

  bool isPlaying(int s) const
  {
    if (isValid(s)) {
      return items[s].isPlaying();
    } else {
      return false;
    }
  }

  void play(int s)
  {
    if (isValid(s)) {
      items[s].play();
    }
  }

  void stop(int s)
  {
    if (isValid(s)) {
      items[s].stop();
    }
  }

  void pause(int s)
  {
    if (isValid(s)) {
      items[s].pause();
    }
  }

  bool isLoop(int s) const
  {
    if (isValid(s)) {
      return items[s].isLoop();
    } else {
      return false;
    }
  }

  void setLoop(int s, bool loop)
  {
    if (isValid(s)) {
      items[s].setLoop(loop);
    }
  }
};

class AudiereSound
{
public:

  int s;

  explicit AudiereSound(int snd) : s(snd)
  {
  }

  static bool isExist(std::string const& name)
  {
    return AudiereSoundResource::inst().isExist(name);
  }

  static AudiereSound getSound(std::string const& name, bool bBgm)
  {
    return AudiereSound(AudiereSoundResource::inst().getSound(name, bBgm));
  }

  static AudiereSound getSound(std::string const& name, bool bBgm, std::string const& stream)
  {
    return AudiereSound(AudiereSoundResource::inst().getSound(
                                                       name,
                                                       bBgm, stream));
  }

  static void releaseSound(AudiereSound snd)
  {
    AudiereSoundResource::inst().releaseSound(snd.s);
  }

  static float getSoundVolume()
  {
    return AudiereSoundResource::inst().getSoundVolume();
  }

  static void setSoundVolume(float vol)
  {
    AudiereSoundResource::inst().setSoundVolume(vol);
  }

  static float getMusicVolume()
  {
    return AudiereSoundResource::inst().getMusicVolume();
  }

  static void setMusicVolume(float vol)
  {
    AudiereSoundResource::inst().setMuiscVolume(vol);
  }

  bool isValid() const
  {
    return AudiereSoundResource::inst().isValid(s);
  }

  bool isPlaying() const
  {
    return AudiereSoundResource::inst().isPlaying(s);
  }

  void play()
  {
    AudiereSoundResource::inst().play(s);
  }

  void stop()
  {
    AudiereSoundResource::inst().stop(s);
  }

  void pause()
  {
    AudiereSoundResource::inst().pause(s);
  }

  bool isLoop() const
  {
    return AudiereSoundResource::inst().isLoop(s);
  }

  void setLoop(bool loop)
  {
    AudiereSoundResource::inst().setLoop(s, loop);
  }
};

} // namespace snd

} // namespace good

// end of audiere_snd.h
