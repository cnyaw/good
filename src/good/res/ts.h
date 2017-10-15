
//
// ts.h
// Tileset.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/01/13 Waync created.
//

#pragma once

namespace good {

class Tileset
{
public:

  //
  // Tileset.
  //

  int mTextureId;

  //
  // Tile size.
  //

  int mTileWidth, mTileHeight;

  //
  // Number tiles in vertical and vertical.
  //

  int mCxTile, mCyTile;

  //
  // Load tileset property.
  //

  bool load(sw2::Ini& sec)
  {
    mTextureId = sec["texture"];
    mTileWidth = sec["tileWidth"];
    mTileHeight = sec["tileHeight"];
    mCxTile = sec["cxTile"];
    mCyTile = sec["cyTile"];

    return true;
  }
};

} // namespace good

// end of ts.h
