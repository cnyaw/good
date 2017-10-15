
//
// openal_snd.h
// OpenAL sound manager implementation.
//
// Copyright (c) 2010 Waync Cheng.
// All Rights Reserved.
//
// 2010/05/17 Waync created.
//

#pragma once

namespace good {

namespace snd {

class ALSound
{
public:

  int s;

  explicit ALSound(int snd) : s(snd)
  {
  }

  static bool isExist(std::string const& name)
  {
    return false;
  }

  static ALSound getSound(std::string const& name, bool bBgm)
  {
    return ALSound(-1);
  }

  static ALSound getSound(std::string const& name, bool bBgm, std::string const& stream)
  {
    return ALSound(-1);
  }

  static void releaseSound(ALSound snd)
  {
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
    return false;
  }

  bool isPlaying() const
  {
    return false;
  }

  void play()
  {
  }

  void stop()
  {
  }

  void pause()
  {
  }

  bool isLoop() const
  {
    return false;
  }

  void setLoop(bool loop)
  {
  }
};

} // namespace snd

} // namespace good

// end of openal_snd.h
