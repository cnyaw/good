
//
// spr.h
// Good sprite.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/12/13 Waync created.
//

#pragma once

namespace good {

template<class TilesetT>
class Sprite : public Base
{
public:

  //
  // Frame.
  //

  std::vector<int> mFrame;

  //
  // Display interval of each frame, # frame.
  //

  std::vector<int> mTime;

  //
  // Loop animation.
  //

  bool mLoop;

  //
  // Tileset.

  TilesetT mTileset;

  //
  // Offset.
  //

  int mOffsetX, mOffsetY;

  //
  // Name property.
  //

  std::string getName() const
  {
    return good::getName(*this, "sprite");
  }

  //
  // Load sprite.
  //

  template<class PoolT>
  bool load(PoolT& p, sw2::Ini const& ini, int id)
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(id, "sprite");

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

    if (!mTileset.load(sec)) {
      return false;
    }

    std::vector<int> v;
    assignListFromString(sec["data"].value, v);

    if (0 != (v.size() % 2)) {
      return false;
    }

    for (size_t i = 0; i < v.size() / 2; ++i) {
      mFrame.push_back(v[2 * i + 0]);
      mTime.push_back(v[2 * i + 1]);
    }

    mLoop = sec["loop"];

    mOffsetX = sec["xOffset"];
    mOffsetY = sec["yOffset"];

    return true;
  }
};

} // namespace good

// end of spr.h
