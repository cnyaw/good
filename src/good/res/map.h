
//
// map.h
// Good map.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/11/29 Waync created.
//

#pragma once

namespace good {

template<class TilesetT>
class Map : public Base
{
public:

  //
  // Map size(in tiles).
  //

  int mWidth, mHeight;

  //
  // Tile data, width * height tiles.
  //

  std::vector<int> mData;

  //
  // Tileset.
  //

  TilesetT mTileset;

  //
  // Name property.
  //

  std::string getName() const
  {
    return good::getName(*this, "map");
  }

  //
  // Load tile map.
  //

  template<class PoolT>
  bool load(PoolT& p, sw2::Ini const& ini, int id)
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(id, "map");

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

    mWidth = sec["width"];
    mHeight = sec["height"];

    if (!mTileset.load(sec)) {
      return false;
    }

    //
    // Data.
    //

    mData.resize(mWidth * mHeight);

    std::stringstream ss1(sec["data"].value), ss2, ss3;

    if (!sw2::Util::unbase64(ss1, ss2)) {
      return false;
    }

    if (!sw2::Util::unzip(ss2, ss3)) {
      return false;
    }

    std::string str(ss3.str());

    ::memcpy(&mData[0], str.data(), str.length());

    return true;
  }
};

} // namespace good

// end of map.h
