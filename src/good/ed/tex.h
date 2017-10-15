
//
// tex.h
// Editor, texture.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/02/25 Waync created.
//

#pragma once

namespace good {

namespace ed {

template<class PrjT>
class Texture : public good::Texture
{
public:

  //
  // Release undo command resource.
  //

  void clear()
  {
  }

  //
  // Store texture.
  //

  bool store(sw2::Ini& ini) const
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(mId, "tex");

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

    return true;
  }
};

} // namespace ed

} // namespace good

// end of tex.h
