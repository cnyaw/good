
//
// snd.h
// Editor, sound.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/08/08 Waync created.
//

#pragma once

namespace good {

namespace ed {

template<class PrjT>
class Sound : public good::Sound
{
public:

  //
  // Release undo command resource.
  //

  void clear()
  {
  }

  //
  // Set property.
  //

  bool setLoop(bool bLoop)
  {
    if (bLoop == mLoop) {
      return false;
    }

    mLoop = bLoop;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setPreload(bool bPreload)
  {
    if (bPreload == mPreload) {
      return false;
    }

    mPreload = bPreload;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setStream(bool bStream)
  {
    if (bStream == mStream) {
      return false;
    }

    mStream = bStream;

    PrjT::inst().mModified = true;

    return true;
  }

  //
  // Store texture.
  //

  bool store(sw2::Ini& ini) const
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(mId, "snd");

    sw2::Ini& sec = ini[secName];       // Insert.

    //
    // Property.
    //

    if (!mName.empty()) {
      sec["name"] = mName;
    }

    if (!mFileName.empty()) {
      sec["fileName"] = mFileName;
    }

    if (mPreload) {
      sec["preload"] = mPreload;
    }

    if (mStream) {
      sec["stream"] = mStream;
    }

    if (mLoop) {
      sec["loop"] = mLoop;
    }

    return true;
  }
};

} // namespace ed

} // namespace good

// end of snd.h
