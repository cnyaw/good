
//
// snd.h
// Sound.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2008/08/08 Waync created.
//

#pragma once

namespace good {

class Sound : public Base
{
public:

  //
  // Resource file name, relative to resource project file.
  //

  std::string mFileName;

  bool mPreload;
  bool mStream;                         // Streaming.
  bool mLoop;

  //
  // Name property.
  //

  std::string getName() const
  {
    return good::getName(*this, "snd");
  }

  //
  // Load sound.
  //

  template<class PoolT>
  bool load(PoolT& p, sw2::Ini const& ini, int id)
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(id, "snd");

    if (!ini.find(secName)) {
      return false;
    }

    sw2::Ini sec = ini[secName];

    //
    // Property.
    //

    if (!Base::load(p, sec, id)) {
      return false;
    }

    mFileName = sec["fileName"].value;
    mPreload = sec["preload"];
    mStream = sec["stream"];
    mLoop = sec["loop"];

    return true;
  }
};

} // namespace good

// end of snd.h
