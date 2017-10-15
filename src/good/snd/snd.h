
//
// snd.h
// Audio interface.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/07/22 Waync created.
//

#pragma once

namespace good {

namespace snd {

template<class SoundT>
class Sound
{
public:

  static SoundT isExist(std::string const& name);

  static SoundT getSound(std::string const& name, bool bBgm);

  static SoundT getSound(std::string const& name, bool bBgm, std::string const& stream);

  static void releaseSound(SoundT& snd);

  static float getSoundVolumen();

  static void setSoundVolume(float vol);

  static float getMusicVolume();

  static void setMuiscVolume(float vol);

  bool isValid() const;

  bool isPlaying() const;

  void play();

  void stop();

  void pause();

  bool isLoop() const;

  void setLoop(bool loop);
};

} // namespace snd

} // namespace good

// end of snd.h
