
//
// map.h
// Editor, map.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/11/29 Waync created.
//

#pragma once

namespace good {

namespace ed {

template<class PrjT, class TileDataT>
class MapCmdDraw : public UndoCommand
{
public:
  int mMapId;
  int mTile;
  std::vector<std::pair<int,int> > mAction; // (draw pos, prev tile)
  int mSaveX, mSaveY;

  MapCmdDraw(int idMap, int tile, int x, int y) : UndoCommand(GOOD_MAPED_CMD_DRAW), mMapId(idMap), mTile(tile)
  {
    mSaveX = x, mSaveY = y;
  }

  bool exec(int x, int y)
  {
    TileDataT& map = PrjT::inst().getMap(mMapId);

    int xySel = x + y * map.mWidth;

    int tile = map.mData[xySel];
    if (mTile == tile) {
      return false;
    }

    mAction.push_back(std::pair<int,int>(xySel, tile));
    map.mData[xySel] = mTile;

    return true;
  }

  virtual bool exec()
  {
    return exec(mSaveX, mSaveY);
  }

  virtual bool undo()
  {
    TileDataT& map = PrjT::inst().getMap(mMapId);

    for (int i = (int)mAction.size() - 1; i >= 0; i--) {
      map.mData[mAction[i].first] = mAction[i].second;
    }

    return true;
  }

  virtual bool redo()
  {
    TileDataT& map = PrjT::inst().getMap(mMapId);

    for (int i = (int)mAction.size() - 1; i >= 0; i--) {
      map.mData[mAction[i].first] = mTile;
    }

    return true;
  }
};

template<class PrjT, class TileDataT>
class MapCmdDrawPattern : public UndoCommand
{
public:
  int mMapId;
  std::vector<int> mPattern;
  int mPatWidth, mPatHeight;
  std::vector<std::pair<int, std::vector<int> > > mAction; // Draw action (draw pos, prev tile pattern).
  int mSaveX, mSaveY;

  MapCmdDrawPattern(int idMap, std::vector<int> const& pat, int w, int h, int x, int y) : UndoCommand(GOOD_MAPED_CMD_DRAW_PATTERN), mMapId(idMap), mPattern(pat), mPatWidth(w), mPatHeight(h)
  {
    mSaveX = x, mSaveY = y;
  }

  bool exec(int x, int y)
  {
    TileDataT& map = PrjT::inst().getMap(mMapId);

    int xySel = x + y * map.mWidth;

    std::vector<int> tile;
    getPattern(xySel, tile);

    if (mPattern == tile) {
      return false;
    }

    mAction.push_back(std::pair<int,std::vector<int> >(xySel, tile)); // Save.
    drawPattern(xySel, mPattern);

    return true;
  }

  virtual bool exec()
  {
    return exec(mSaveX, mSaveY);
  }

  virtual bool undo()
  {
    for (int i = (int)mAction.size() - 1; i >= 0; i--) {
      drawPattern(mAction[i].first, mAction[i].second);
    }

    return true;
  }

  virtual bool redo()
  {
    for (size_t i = 0; i < mAction.size(); i++) {
      drawPattern(mAction[i].first, mPattern);
    }

    return true;
  }

  void drawPattern(int xySel, std::vector<int> const& pat)
  {
    TileDataT& map = PrjT::inst().getMap(mMapId);

    int w = map.mWidth, h = map.mHeight;
    int cx = (xySel % w) - mPatWidth / 2, cy = (xySel / w) - mPatHeight / 2;

    int idx = 0;
    for (int j = 0; j < mPatHeight; j++) {
      for (int i = 0; i < mPatWidth; i++) {
        int tile = pat[idx++];
        int x = cx + i, y = cy + j;
        if (0 <= x && w > x && 0 <= y && h > y) {
          map.mData[x + y * w] = tile;
        }
      }
    }
  }

  void getPattern(int xySel, std::vector<int>& pat)
  {
    TileDataT const& map = PrjT::inst().getMap(mMapId);

    int w = map.mWidth, h = map.mHeight;
    int cx = (xySel % w) - mPatWidth / 2, cy = (xySel / w) - mPatHeight / 2;

    pat.resize(mPattern.size());

    int idx = 0;
    for (int j = 0; j < mPatHeight; j++) {
      for (int i = 0; i < mPatWidth; i++) {
        int x = cx + i, y = cy + j;
        if (0 > x || w <= x || 0 > y || h <= y) {
          pat[idx++] = 0;
        } else {
          pat[idx++] = map.mData[x + y * w];
        }
      }
    }
  }
};

template<class PrjT, class TileDataT>
class MapCmdFill : public UndoCommand
{
public:
  int mMapId;
  int mTile;
  int mPosX, mPosY;
  std::vector<int> mSaveData;
  int mSaveX, mSaveY;

  MapCmdFill(int idMap, int tile, int x, int y) : UndoCommand(GOOD_MAPED_CMD_FILL), mMapId(idMap), mTile(tile)
  {
    mSaveX = x, mSaveY = y;
  }

  bool exec(int x, int y)
  {
    TileDataT const& map = PrjT::inst().getMap(mMapId);

    mPosX = x, mPosY = y;
    mSaveData = map.mData;              // Save whole tiledata.

    int depth = 0;
    fill_i(x, y, mTile, map.mData[x + map.mWidth * y], depth);

    return true;
  }

  virtual bool exec()
  {
    return exec(mSaveX, mSaveY);
  }

  virtual bool undo()
  {
    PrjT::inst().getMap(mMapId).mData = mSaveData;

    return true;
  }

  virtual bool redo()
  {
    TileDataT const& map = PrjT::inst().getMap(mMapId);

    int depth = 0;
    fill_i(mPosX, mPosY, mTile, map.mData[mPosX + map.mWidth * mPosY], depth);

    return true;
  }

  //
  // Helper.
  //

  void fill_i(int x, int y, int fill, int border, int& depth)
  {
    TileDataT& map = PrjT::inst().getMap(mMapId);

    int xy = x + map.mWidth * y;

    if (0 > x || 0 > y || map.mWidth <= x || map.mHeight <= y) {
      return;
    }

    if (fill == map.mData[xy] || border != map.mData[xy]) {
      return;
    }

    if (1024 <= depth) {                // Limit to avoid stack overflow.
      return;
    }

    map.mData[xy] = fill;

    depth += 1;
    fill_i(x + 1, y, fill, border, depth);
    fill_i(x - 1, y, fill, border, depth);
    fill_i(x, y + 1, fill, border, depth);
    fill_i(x, y - 1, fill, border, depth);
    depth -= 1;
  }
};

template<class PrjT>
class Map : public good::Map<Tileset>
{
public:
  std::vector<GridLine> mVertGrid, mHorzGrid;

  UndoImpl mUndo;

  bool mDrawFlag;                       // Flag.

  Map() : mUndo(PrjT::UNDO_LEVEL), mDrawFlag(true)
  {
  }

  //
  // Release undo command resource.
  //

  void clear()
  {
    mUndo.clear();
  }

  //
  // Helper.
  //

  bool setGrid(std::vector<GridLine> const& vgrid, std::vector<GridLine> const& hgrid)
  {
    if (vgrid == mVertGrid && hgrid == mHorzGrid) {
      return false;
    }

    mVertGrid = vgrid;
    mHorzGrid = hgrid;

    PrjT::inst().mModified = true;

    return true;
  }

  //
  // Draw.
  //

  void beginDraw()
  {
    //
    // Call before draw(mouse down).
    //

    mDrawFlag = false;
  }

  void endDraw()
  {
    //
    // Call after draw(mouse up).
    //

    mDrawFlag = false;
  }

  bool draw(int x, int y, int tile)
  {
    typedef MapCmdDraw<PrjT, Map> CmdT;
    if (!mDrawFlag) {
      CmdT* pcmd = new CmdT(mId, tile, x, y);
      if (mUndo.execAndAdd(pcmd)) {
        PrjT::inst().mModified = true;
        mDrawFlag = true;
        return true;
      }
      return false;
    } else {
      UndoCommand* pcmd = mUndo.getCurCommand();
      if (0 == pcmd || GOOD_MAPED_CMD_DRAW != pcmd->getId()) {
        return false;
      }
      return static_cast<CmdT*>(pcmd)->exec(x, y);
    }
  }

  bool draw(int x, int y, std::vector<int> const& pattern, int w, int h)
  {
    typedef MapCmdDrawPattern<PrjT, Map> CmdT;
    if (!mDrawFlag) {
      CmdT* pcmd = new CmdT(mId, pattern, w, h, x, y);
      if (mUndo.execAndAdd(pcmd)) {
        PrjT::inst().mModified = true;
        mDrawFlag = true;
        return true;
      }
      return false;
    } else {
      UndoCommand* pcmd = mUndo.getCurCommand();
      if (0 == pcmd || GOOD_MAPED_CMD_DRAW_PATTERN != pcmd->getId()) {
        return false;
      }
      return static_cast<CmdT*>(pcmd)->exec(x, y);
    }
  }

  bool fill(int x, int y, int fill)
  {
    typedef MapCmdFill<PrjT, Map> CmdT;
    if (!mDrawFlag) {
      CmdT* pcmd = new CmdT(mId, fill, x, y);
      if (mUndo.execAndAdd(pcmd)) {
        PrjT::inst().mModified = true;
        mDrawFlag = true;
        return true;
      }
      return false;
    } else {
      UndoCommand* pcmd = mUndo.getCurCommand();
      if (0 == pcmd || GOOD_MAPED_CMD_FILL != pcmd->getId()) {
        return false;
      }
      return static_cast<CmdT*>(pcmd)->exec(x, y);
    }
  }

  //
  // Undo/redo support.
  //

  bool canRedo()
  {
    return mUndo.canRedo();
  }

  bool canUndo()
  {
    return mUndo.canUndo();
  }

  bool redo()
  {
    if (mUndo.redo()) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool undo()
  {
    if (mUndo.undo()) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  //
  // Load/save tile map.
  //

  template<class PoolT>
  bool load(PoolT& p, sw2::Ini const& ini, int id) // Override.
  {
    //
    // Load default.
    //

    if (!good::Map<Tileset>::load(p, ini, id)) {
      return false;
    }

    std::string secName = good::getSecName(mId, "map");

    sw2::Ini sec = ini[secName];

    //
    // Grids.
    //

    if (!loadGrid(sec, "vgrid", mWidth, mVertGrid) ||
        !loadGrid(sec, "hgrid", mHeight, mHorzGrid)) {
      return false;
    }

    return true;
  }

  bool store(sw2::Ini& ini) const
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(mId, "map");

    (void)ini[secName];                 // Insert.

    //
    // Property.
    //

    {
      sw2::Ini& sec = ini[secName];

      if (!mName.empty()) {
        sec["name"] = mName;
      }

      sec["width"] = mWidth;
      sec["height"] = mHeight;

      if (!mTileset.store(sec)) {
        return false;
      }
    }

    //
    // Data.
    //

    {
      sw2::Ini& sec = ini[secName];

      std::stringstream ss1;
      ss1.write((const char*)&mData[0], (int)mData.size() * sizeof(int));

      std::stringstream ss2;
      if (!sw2::Util::zip(ss1, ss2, 9)) {
        return false;
      }

      std::string is2(ss2.str()), os3;
      if (!sw2::Util::base64(is2, os3)) {
        return false;
      }

      sec["data"] = os3;
    }

    //
    // Grid.
    //

    if (!storeGrid(ini, secName, "vgrid", mVertGrid) ||
        !storeGrid(ini, secName, "hgrid", mHorzGrid)) {
      return false;
    }

    return true;
  }
};

} // namespace ed

} // namespace good

// end of map.h
