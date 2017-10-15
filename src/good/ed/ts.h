
//
// ts.h
// Editor, tileset.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/08/12 Waync created.
//

#pragma once

namespace good {

namespace ed {

class Tileset : public good::Tileset
{
public:

  //
  // Store.
  //

  bool store(sw2::Ini& sec) const
  {
    sec["texture"] = mTextureId;
    sec["tileWidth"] = mTileWidth;
    sec["tileHeight"] = mTileHeight;
    sec["cxTile"] = mCxTile;
    sec["cyTile"] = mCyTile;

    return true;
  }
};

} // namespace ed

} // namespace good

// end of ts.h
