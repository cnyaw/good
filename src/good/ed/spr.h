
//
// spr.h
// Editor, sprite.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/12/13 Waync created.
//

#pragma once

namespace good {

namespace ed {

template<class PrjT, class SpriteDataT>
class SpriteCmdInsert : public UndoCommand
{
public:
  int mSpriteId;
  size_t mIdxInsert;
  std::vector<int> mFrames;
  int mTime;

  SpriteCmdInsert(int idSpr, size_t idxInsert, std::vector<int> const& frames, int time) : UndoCommand(GOOD_SPRITEED_CMD_INSERT), mSpriteId(idSpr)
  {
    SpriteDataT const& spr = PrjT::inst().getSprite(mSpriteId);

    if (spr.mFrame.size() < idxInsert) {
      idxInsert = spr.mFrame.size();
    }

    mIdxInsert = idxInsert;
    mFrames = frames;
    mTime = time;
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    SpriteDataT& spr = PrjT::inst().getSprite(mSpriteId);

    for (size_t i = 0; i < mFrames.size(); ++i) {
      (void)SpriteDataT::removeFrame(spr, mIdxInsert);
    }

    return true;
  }

  virtual bool redo()
  {
    SpriteDataT& spr = PrjT::inst().getSprite(mSpriteId);

    for (size_t i = 0; i < mFrames.size(); ++i) {
      (void)SpriteDataT::insertFrame(spr, mIdxInsert + i, mFrames[i], mTime);
    }

    return true;
  }
};

template<class PrjT, class SpriteDataT>
class SpriteCmdRemove : public UndoCommand
{
public:
  int mSpriteId;
  size_t mIdxRemove;
  int mFrame;
  int mTime;
  bool mValid;

  SpriteCmdRemove(int idSpr, size_t idxRemove) : UndoCommand(GOOD_SPRITEED_CMD_REMOVE), mSpriteId(idSpr)
  {
    mValid = false;

    SpriteDataT const& spr = PrjT::inst().getSprite(mSpriteId);

    if (spr.mTime.size() > idxRemove) {
      mIdxRemove = idxRemove;
      mFrame = spr.mFrame[idxRemove];
      mTime = spr.mTime[idxRemove];
      mValid = true;
    }
  }

  virtual bool exec()
  {
    return mValid && redo();
  }

  virtual bool undo()
  {
    return SpriteDataT::insertFrame(PrjT::inst().getSprite(mSpriteId), mIdxRemove, mFrame, mTime);
  }

  virtual bool redo()
  {
    return SpriteDataT::removeFrame(PrjT::inst().getSprite(mSpriteId), mIdxRemove);
  }
};

template<class PrjT, class SpriteDataT>
class SpriteCmdSetTime : public UndoCommand
{
public:
  SpriteDataT& mSprite;
  size_t mIdxFrame;
  int mTime, mCount;
  bool mVaild;
  std::vector<int> mTimeSave;

  SpriteCmdSetTime(SpriteDataT& sd, size_t idxFrame, int count, int time) : UndoCommand(GOOD_SPRITEED_CMD_SETTIME), mSprite(sd)
  {
    mVaild = false;
    if (mSprite.mTime.size() > idxFrame && mSprite.mTime[idxFrame] != time) {
      mIdxFrame = idxFrame;
      mTime = time;
      mCount = count;
      for (int i = 0; i < count; i++) {
        mTimeSave.push_back(mSprite.mTime[idxFrame + i]);
        mSprite.mTime[idxFrame + i] = time;
      }
      mVaild = true;
    }
  }

  bool exec(int time)
  {
    if (mSprite.mTime[mIdxFrame] != time) {
      mTime = time;
      for (int i = 0; i < mCount; i++) {
        mSprite.mTime[mIdxFrame + i] = time;
      }
      return true;
    }
    return false;
  }

  virtual bool exec()
  {
    return mVaild;
  }

  virtual bool undo()
  {
    for (int i = 0; i < mCount; i++) {
      mSprite.mTime[mIdxFrame + i] = mTimeSave[i];
    }
    return true;
  }

  virtual bool redo()
  {
    for (int i = 0; i < mCount; i++) {
      mSprite.mTime[mIdxFrame + i] = mTime;
    }
    return true;
  }
};

template<class PrjT>
class Sprite : public good::Sprite<Tileset>
{
public:
  UndoImpl mUndo;

  Sprite() : mUndo(PrjT::UNDO_LEVEL)
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
  // Helper.
  //

  bool setLoop(bool loop)
  {
    if (loop == mLoop) {
      return false;
    }

    mLoop = loop;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setOffset(int xoffset, int yoffset)
  {
    if (xoffset == mOffsetX && yoffset == mOffsetY) {
      return false;
    }

    mOffsetX = xoffset;
    mOffsetY = yoffset;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setTime(size_t iFrame, int count, int time)
  {
    typedef SpriteCmdSetTime<PrjT, Sprite> CmdT;

    UndoCommand* pcmd = mUndo.getCurCommand();
    if (0 != pcmd && GOOD_SPRITEED_CMD_SETTIME == pcmd->getId()) {
      CmdT* pcmd3 = static_cast<CmdT*>(pcmd);
      if (pcmd3->mIdxFrame == iFrame && pcmd3->mCount == count) {
        return pcmd3->exec(time);
      }
    }

    CmdT* pcmd2 = new CmdT(*this, iFrame, count, time);
    if (mUndo.execAndAdd(pcmd2)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  //
  // Edit frame/time list.
  //

  bool insertFrame(size_t iFrame, std::vector<int> const& tiles, int time)
  {
    typedef SpriteCmdInsert<PrjT, Sprite> CmdT;
    CmdT *pcmd = new CmdT(mId, iFrame, tiles, time);
    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool removeFrame(size_t iFrame)
  {
    typedef SpriteCmdRemove<PrjT, Sprite> CmdT;
    CmdT *pcmd = new CmdT(mId, iFrame);
    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  static bool insertFrame(Sprite& sd, size_t iFrame, int tile, int time)
  {
    if (sd.mFrame.size() <= iFrame) {   // Append end.
      sd.mFrame.push_back(tile);
      sd.mTime.push_back(time);
    } else {
      sd.mFrame.insert(sd.mFrame.begin() + iFrame, tile);
      sd.mTime.insert(sd.mTime.begin() + iFrame, time);
    }

    return true;
  }

  static bool removeFrame(Sprite& sd, size_t iFrame)
  {
    if (sd.mFrame.size() <= iFrame) {
      return false;
    }

    sd.mFrame.erase(sd.mFrame.begin() + iFrame);
    sd.mTime.erase(sd.mTime.begin() + iFrame);

    return true;
  }

  //
  // Store sprite.
  //

  bool store(sw2::Ini& ini) const
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(mId, "sprite");

    sw2::Ini& sec = ini[secName];       // Insert.

    //
    // Property.
    //

    if (!mName.empty()) {
      sec["name"] = mName;
    }

    if (!mTileset.store(sec)) {
      return false;
    }

    std::vector<int> v;
    for (size_t i = 0; i < mFrame.size(); ++i) {
      v.push_back(mFrame[i]);
      v.push_back(mTime[i]);
    }

    if (!v.empty()) {
      sec["data"] = intVecToStr(v);
    }

    if (mLoop) {
      sec["loop"] = mLoop;
    }

    if (mOffsetX) {
      sec["xOffset"] = mOffsetX;
    }

    if (mOffsetY) {
      sec["yOffset"] = mOffsetY;
    }

    return true;
  }
};

} // namespace ed

} // namespace good

// end of spr.h
