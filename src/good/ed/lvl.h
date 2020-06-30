
//
// lvl.h
// Editor, good level.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/01/27 Waync created.
//

#pragma once

namespace good {

namespace ed {

template<class LvlT>
class LevelCmdAddObj : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  int mIdSprite;
  int mIdMap;
  int mIdTexture;
  int mPosX, mPosY;
  unsigned int mBgColor;

  LevelCmdAddObj(int idLevel, int idSprite, int idMap, int idTexture, int x, int y) : UndoCommand(GOOD_LEVELED_CMD_ADDOBJ), mLevelId(idLevel)
  {
    mId = -1;
    mIdSprite = idSprite;
    mIdMap = idMap;
    mIdTexture = idTexture;
    mPosX = x, mPosY = y;
  }

  virtual bool exec()
  {
    return -1 != (mId = LvlT::addObj(PrjT::inst().getLevel(mLevelId), mLevelId, -1, mIdSprite, mIdMap, mIdTexture, mPosX, mPosY));
  }

  virtual bool undo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);

    mBgColor = lvl.getObj(mId).mBgColor;

    return LvlT::removeObj(lvl, mId);
  }

  virtual bool redo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);
    if (mId == LvlT::addObj(lvl, mLevelId, mId, mIdSprite, mIdMap, mIdTexture, mPosX, mPosY)) {
      lvl.getObj(mId).setBgColor(mBgColor);
      return true;
    } else {
      return false;
    }
  }
};

template<class LvlT>
class LevelCmdMoveObj : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  int mPosX, mPosY;

  LevelCmdMoveObj(int idLevel, int id, int offsetX, int offsetY) : UndoCommand(GOOD_LEVELED_CMD_MOVEOBJ), mLevelId(idLevel)
  {
    mId = id;
    PrjT::ObjectT& o = PrjT::inst().getLevel(idLevel).getObj(id);
    mPosX = o.mPosX, mPosY = o.mPosY;
    o.mPosX += offsetX, o.mPosY += offsetY;
  }

  virtual bool exec()
  {
    return true;
  }

  virtual bool undo()
  {
    return LvlT::moveObj(PrjT::inst().getLevel(mLevelId), mId, mPosX, mPosY);
  }

  virtual bool redo()
  {
    return undo();
  }
};

template<class LvlT>
class LevelCmdRemoveObj : public UndoCommand
{
public:
  int mLevelId;
  int mType;
  int mIdParent;
  int mId;
  int mOrgIdx;
  int mIdSprite;
  int mIdMap;
  int mIdTexture;
  int mPosX, mPosY;
  sw2::IntRect mDim;
  int mBgColor;
  bool mRepX, mRepY;

  LevelCmdRemoveObj(int idLevel, int id) : UndoCommand(GOOD_LEVELED_CMD_REMOVEOBJ), mLevelId(idLevel)
  {
    LvlT const& lvl = PrjT::inst().getLevel(mLevelId);
    typename LvlT::ObjectT const& o = lvl.getObj(id);

    mType = o.mType;
    mId = id;
    mIdParent = lvl.getParent(id);
    if (idLevel == mIdParent) {
      mOrgIdx = lvl.getObjIndex(id);
    } else {
      mOrgIdx = lvl.getObj(mIdParent).getObjIndex(id);
    }

    mIdSprite = o.mSpriteId;
    mIdMap = o.mMapId;
    mIdTexture = o.mTextureId;
    mPosX = o.mPosX;
    mPosY = o.mPosY;
    mDim = o.mDim;
    mBgColor = o.mBgColor;
    mRepX = o.mRepX, mRepY = o.mRepY;
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);
    if (mId != LvlT::addObj(lvl, mIdParent, mId, mIdSprite, mIdMap, mIdTexture, mPosX, mPosY)) {
      return false;
    }

    typename LvlT::ObjectT& o = lvl.getObj(mId);
    o.mType = mType;
    o.mDim = mDim;
    o.mBgColor = mBgColor;
    o.mRepX = mRepX;
    o.mRepY = mRepY;

    if (mIdParent == mLevelId) {
      lvl.mObjIdx.pop_back();
      lvl.mObjIdx.insert(lvl.mObjIdx.begin() + mOrgIdx, mId);
    } else {
      PrjT::ObjectT &o = lvl.getObj(mIdParent);
      o.mObjIdx.pop_back();
      o.mObjIdx.insert(o.mObjIdx.begin() + mOrgIdx, mId);
    }

    return true;
  }

  virtual bool redo()
  {
    return LvlT::removeObj(PrjT::inst().getLevel(mLevelId), mId);
  }
};

template<class LvlT>
class LevelCmdZorderObj : public UndoCommand
{
public:
  enum {
    MOVE_TOPMOST,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_BOTTOMMOST
  };

  int mLevelId;
  int mId;
  int mMove;
  size_t mPrevOrder;

  LevelCmdZorderObj(int idLevel, int id, int move) : UndoCommand(GOOD_LEVELED_CMD_ZORDER), mLevelId(idLevel)
  {
    mId = id;
    mMove = move;
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);

    switch (mMove)
    {
    case MOVE_TOPMOST:
      lvl.mObjIdx.pop_back();
      lvl.mObjIdx.insert(lvl.mObjIdx.begin() + mPrevOrder, mId);
      return true;

    case MOVE_UP:
      return LvlT::moveObjDown(lvl, mId);

    case MOVE_DOWN:
      return LvlT::moveObjUp(lvl, mId);

    case MOVE_BOTTOMMOST:
      lvl.mObjIdx.erase(lvl.mObjIdx.begin());
      lvl.mObjIdx.insert(lvl.mObjIdx.begin() + mPrevOrder, mId);
      return true;
    }

    return false;
  }

  virtual bool redo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);

    switch (mMove)
    {
    case MOVE_TOPMOST:
      mPrevOrder = lvl.getObjIndex(mId);
      return LvlT::moveObjTopmost(lvl, mId);

    case MOVE_UP:
      return LvlT::moveObjUp(lvl, mId);

    case MOVE_DOWN:
      return LvlT::moveObjDown(lvl, mId);

    case MOVE_BOTTOMMOST:
      mPrevOrder = lvl.getObjIndex(mId);
      return LvlT::moveObjBottommost(lvl, mId);
    }

    return false;
  }
};

template<class LvlT>
class LevelCmdSetBgColor : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  unsigned int mColor;

  LevelCmdSetBgColor(int idLevel, int id, unsigned int color) : UndoCommand(GOOD_LEVELED_CMD_SETBGCOLOR), mLevelId(idLevel)
  {
    mId = id;
    mColor = color;
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    return redo();
  }

  virtual bool redo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);
    typename LvlT::ObjectT *pObj = mLevelId == mId ? &lvl : &lvl.getObj(mId);
    unsigned int prevColor = pObj->mBgColor;

    bool ret = pObj->setBgColor(mColor);
    if (ret) {
      mColor = prevColor;
    }

    return ret;
  }
};

template<class LvlT>
class LevelCmdSetDim : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  sw2::IntRect mDim;

  LevelCmdSetDim(int idLevel, int id, int newx, int newy, int neww, int newh) : UndoCommand(GOOD_LEVELED_CMD_SETDIM), mLevelId(idLevel)
  {
    mId = id;
    mDim = sw2::IntRect(newx, newy, newx + neww, newy + newh);
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    return redo();
  }

  virtual bool redo()
  {
    typename LvlT::ObjectT& o = PrjT::inst().getLevel(mLevelId).getObj(mId);
    sw2::IntRect prevDim = o.mDim;

    bool ret = o.setDim(mDim.left, mDim.top, mDim.width(), mDim.height());
    if (ret) {
      mDim = prevDim;
    }

    return ret;
  }
};

template<class LvlT>
class LevelCmdSetProp : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  std::string mName;
  bool mVisible;
  float mRot, mScaleX, mScaleY, mAnchorX, mAnchorY;
  bool mRepX, mRepY;

  LevelCmdSetProp(int idLevel, int id, std::string const& name, bool vis, float rot, float sx, float sy, float ax, float ay, bool repx, bool repy) : UndoCommand(GOOD_LEVELED_CMD_SETPROP), mLevelId(idLevel)
  {
    mId = id;
    mName = name;
    mVisible = vis;
    mRot = rot;
    mScaleX = sx; mScaleY = sy;
    mAnchorX = ax; mAnchorY = ay;
    mRepX = repx; mRepY = repy;
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    return redo();
  }

  virtual bool redo()
  {
    typename LvlT::ObjectT& o = PrjT::inst().getLevel(mLevelId).getObj(mId);
    typename LvlT::ObjectT copy = o;

    if (PrjT::inst().rename(o, mName)) {
      mName = copy.mName;
      return true;
    }

    if (o.setVisible(mVisible)) {
      mVisible = copy.mVisible;
      return true;
    }

    if (o.setRot(mRot)) {
      mRot = copy.mRot;
      return true;
    }

    if (o.setScale(mScaleX, mScaleY)) {
      mScaleX = copy.mScaleX; mScaleY = copy.mScaleY;
      return true;
    }

    if (o.setAnchor(mAnchorX, mAnchorY)) {
      mAnchorX = copy.mAnchorX; mAnchorY = copy.mAnchorY;
      return true;
    }

    if (o.setRepeat(mRepX, mRepY)) {
      mRepX = copy.mRepX; mRepY = copy.mRepY;
      return true;
    }

    return false;
  }
};

template<class LvlT>
class LevelCmdChangeParent : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  int mOrgParentId;
  int mOrgIdx;
  int mNewParentId;

  LevelCmdChangeParent(int idLevel, int id, int idNewParent) : UndoCommand(GOOD_LEVELED_CMD_CHANGEPARENT), mLevelId(idLevel)
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);

    mId = id;
    mNewParentId = idNewParent;
    mOrgParentId = lvl.getParent(id);
    if (idLevel == mOrgParentId) {
      mOrgIdx = lvl.getObjIndex(id);
    } else {
      mOrgIdx = lvl.getObj(mOrgParentId).getObjIndex(id);
    }
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);

    if (!LvlT::changeParent(lvl, mId, mOrgParentId)) {
      return false;
    }

    if (mOrgParentId == mLevelId) {
      lvl.mObjIdx.pop_back();
      lvl.mObjIdx.insert(lvl.mObjIdx.begin() + mOrgIdx, mId);
    } else {
      PrjT::ObjectT &o = lvl.getObj(mOrgParentId);
      o.mObjIdx.pop_back();
      o.mObjIdx.insert(o.mObjIdx.begin() + mOrgIdx, mId);
    }

    return true;
  }

  virtual bool redo()
  {
    return LvlT::changeParent(PrjT::inst().getLevel(mLevelId), mId, mNewParentId);
  }
};

template<class LvlT>
class LevelCmdSetScript : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  std::string mScript;

  LevelCmdSetScript(int idLevel, int id, const std::string &script) : UndoCommand(GOOD_LEVELED_CMD_SETSCRIPT), mLevelId(idLevel)
  {
    mId = id;
    mScript = script;
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    return redo();
  }

  virtual bool redo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);
    typename LvlT::ObjectT *pObj = mLevelId == mId ? &lvl : &lvl.getObj(mId);
    std::string prevScript = pObj->mScript;

    bool ret = pObj->setScript(mScript);
    if (ret) {
      mScript = prevScript;
    }

    return ret;
  }
};

template<class LvlT>
class LevelCmdSetSize : public UndoCommand
{
public:
  int mLevelId;
  int mWidth, mHeight;

  LevelCmdSetSize(int id, int w, int h) : UndoCommand(good::ed::GOOD_LEVELED_CMD_SETSIZE)
  {
    mLevelId = id;
    mWidth = w;
    mHeight = h;
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    return redo();
  }

  virtual bool redo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);
    int prevW = lvl.mWidth, prevH = lvl.mHeight;
    bool ret = LvlT::setSize(lvl, mWidth, mHeight);
    if (ret) {
      mWidth = prevW, mHeight = prevH;
    }
    return ret;
  }
};

template<class LvlT>
class LevelCmdSetLevelPos : public UndoCommand
{
public:
  int mLevelId;
  int mPosX, mPosY;

  LevelCmdSetLevelPos(int id, int x, int y) : UndoCommand(GOOD_LEVELED_CMD_SETLEVELPOS)
  {
    mLevelId = id;
    mPosX = x;
    mPosY = y;
  }

  virtual bool exec()
  {
    return redo();
  }

  virtual bool undo()
  {
    return redo();
  }

  virtual bool redo()
  {
    LvlT& lvl = PrjT::inst().getLevel(mLevelId);
    int prevX = lvl.mPosX, prevY = lvl.mPosY;
    bool ret = lvl.setPos(mPosX, mPosY);
    if (ret) {
      mPosX = prevX, mPosY = prevY;
    }
    return ret;
  }
};

template<class PrjT>
class Level : public good::Level<Object<PrjT> >
{
public:
  enum ALIGNMENT {
    ALIGN_LEFT = 0,
    ALIGN_RIGHT,
    ALIGN_TOP,
    ALIGN_BOTTOM
  };

  enum ADDTOOL {
    TOOL_MOVE,
    TOOL_REMOVE,
    TOOL_ADDCOLBG,
    TOOL_ADDTEXBG,
    TOOL_ADDMAPBG,
    TOOL_ADDSPRITE,
    TOOL_ADDDUMMY,
    TOOL_ADDLVLOBJ
  };

  typedef Object<PrjT> ObjectT;

  UndoImpl mUndo;

  bool mShowSnap;
  int mSnapWidth, mSnapHeight;

  bool mShowLine;
  std::vector<GridLine> mVertGrid, mHorzGrid;

  int mTool;
  int mAddSpr, mAddMap, mAddTex;        // Sel add obj param.
  unsigned int mAddCol;

  Level() : mUndo(PrjT::UNDO_LEVEL), mShowSnap(true), mSnapWidth(16), mSnapHeight(16), mShowLine(true), mTool(TOOL_MOVE)
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
  // Modify property.
  //

  int getSnapHeight() const
  {
    return mSnapHeight;
  }

  int getSnapWidth() const
  {
    return mSnapWidth;
  }

  bool isShowLine() const
  {
    return mShowLine;
  }

  bool isShowSnap() const
  {
    return mShowSnap;
  }

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

  bool setSize(int newWidth, int newHeight)
  {
    LevelCmdSetSize<Level>* pcmd;
    pcmd = new LevelCmdSetSize<Level>(mId, newWidth, newHeight);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool setSnapSize(int newWidth, int newHeight)
  {
    if (newWidth == mSnapWidth && newHeight == mSnapHeight) {
      return false;
    }

    mSnapWidth = newWidth;
    mSnapHeight = newHeight;

    PrjT::inst().mModified = true;

    return true;
  }

  void toggleShowLine()
  {
    mShowLine = !mShowLine;
    PrjT::inst().mModified = true;
  }

  void toggleShowSnap()
  {
    mShowSnap = !mShowSnap;
    PrjT::inst().mModified = true;
  }

  //
  // Modify object property.
  //

  bool setObjBgColor(int idObj, unsigned int color)
  {
    LevelCmdSetBgColor<Level>* pcmd;
    pcmd = new LevelCmdSetBgColor<Level>(mId, idObj, color);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool setObjDim(int idObj, int newx, int newy, int neww, int newh)
  {
    LevelCmdSetDim<Level>* pcmd;
    pcmd = new LevelCmdSetDim<Level>(mId, idObj, newx, newy, neww, newh);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool setObjProp(int idObj, std::string const& name, bool vis, float rot, float sx, float sy, float ax, float ay, bool repx, bool repy)
  {
    LevelCmdSetProp<Level>* pcmd;
    pcmd = new LevelCmdSetProp<Level>(mId, idObj, name, vis, rot, sx, sy, ax, ay, repx, repy);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool setObjScript(int idObj, const std::string &script)
  {
    LevelCmdSetScript<Level>* pcmd;
    pcmd = new LevelCmdSetScript<Level>(mId, idObj, script);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  //
  // Add/remove obj.
  //

  int getParent(int id) const
  {
    std::map<int, ObjectT>::const_iterator it = mObj.begin();
    for (; mObj.end() != it; ++it) {
      typename const ObjectT &o = it->second;
      for (size_t i = 0; i < o.mObjIdx.size(); i++) {
        if (o.mObjIdx[i] == id) {
          return it->first;
        }
      }
    }
    return mId;
  }

  bool isParentVisible(int id) const
  {
    while (true) {
      id = getParent(id);
      if (mId == id) {
        return mVisible;
      }
      if (!getObj(id).mVisible) {
        return false;
      }
    }
    return true;
  }

  int addObj(int idSprite, int idMap, int idTexture, int x, int y)
  {
    LevelCmdAddObj<Level>* pcmd;
    pcmd = new LevelCmdAddObj<Level>(mId, idSprite, idMap, idTexture, x, y);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return pcmd->mId;
    }

    //
    // Return id; -1 failed.
    //

    return -1;
  }

  bool removeObj(std::vector<int> const& ids)
  {
    int tag = mUndo.mTag;

    for (size_t i = 0; i < ids.size(); ++i) {
      if (!recursiveRemoveObj(ids[i], tag)) {
        if (0 < i) {
          mUndo.undo();
        }
        return false;
      }
    }

    PrjT::inst().mModified = true;

    return true;
  }

  bool recursiveRemoveObj(int id, int tag)
  {
    PrjT::ObjectT &o = getObj(id);

    for (int i = (int)o.mObjIdx.size() - 1; 0 <= i; i--) {
      if (!recursiveRemoveObj(o.mObjIdx[i], tag)) {
        return false;
      }
    }

    LevelCmdRemoveObj<Level>* pcmd;
    pcmd = new LevelCmdRemoveObj<Level>(mId, id);

    if (!mUndo.execAndAdd(pcmd)) {
      return false;
    }

    pcmd->mTag = tag;

    return true;
  }

  bool copyObj(std::vector<int> const& selObjs, std::vector<int> &newObjs)
  {
    int tag = mUndo.mTag;

    for (size_t i = 0; i < selObjs.size(); i++) {
      PrjT::ObjectT const& o = getObj(selObjs[i]);

      int id = addObj(o.mSpriteId, o.mMapId, o.mTextureId, o.mPosX, o.mPosY);
      if (-1 == id) {
        continue;
      }

      mUndo.getCurCommand()->mTag = tag; // Set as the same undo group.

      PrjT::ObjectT& o2 = getObj(id);
      o2 = o;                           // Duplicate obj o to new obj o2.
      o2.mId = id;                      // Set new obj o2's id.

      newObjs.push_back(id);
    }

    return !newObjs.empty();
  }

  //
  // Move obj.
  //

  bool moveObj(std::vector<int> const& ids, int offsetX, int offsetY)
  {
    int tag = mUndo.mTag;

    for (size_t i = 0; i < ids.size(); ++i) {
      LevelCmdMoveObj<Level>* pcmd;
      pcmd = new LevelCmdMoveObj<Level>(mId, ids[i], offsetX, offsetY);

      if (!mUndo.execAndAdd(pcmd)) {
        if (0 < i) {
          mUndo.undo();
        }
        return false;
      }

      pcmd->mTag = tag;
    }

    PrjT::inst().mModified = true;

    return true;
  }

  bool setLevelPos(int posX, int posY)
  {
    LevelCmdSetLevelPos<Level>* pcmd;
    pcmd = new LevelCmdSetLevelPos<Level>(mId, posX, posY);

    if (!mUndo.execAndAdd(pcmd)) {
      return false;
    }

    PrjT::inst().mModified = true;

    return true;
  }

  //
  // Resize obj.
  //

  bool resizeObj(std::vector<int> const& ids, int deltaX, int deltaY)
  {
    bool changed = false;
    int tag = mUndo.mTag;

    for (size_t i = 0; i < ids.size(); ++i) {

      PrjT::ObjectT const& o = getObj(ids[i]);

      if (PrjT::ObjectT::TYPE_COLBG != o.mType && PrjT::ObjectT::TYPE_TEXBG != o.mType) {
        continue;
      }

      sw2::IntRect rc;
      PrjT::inst().getObjDim<ImgT>(o, rc);

      int neww = rc.width() + deltaX, newh = rc.height() + deltaY;
      if (0 >= neww || 0 >= newh) {
        continue;
      }

      LevelCmdSetDim<Level>* pcmd;
      pcmd = new LevelCmdSetDim<Level>(mId, ids[i], o.mDim.left, o.mDim.top, neww, newh);

      if (!mUndo.execAndAdd(pcmd)) {
        if (0 < i) {
          mUndo.undo();
        }
        return false;
      }

      pcmd->mTag = tag;
      changed = true;
    }

    if (!changed) {
      return false;
    }

    PrjT::inst().mModified = true;

    return true;
  }

  //
  // Change zorder.
  //

  bool canMoveObjBottommost(int id) const
  {
    return canMoveObjDown(id);
  }

  bool canMoveObjDown(int id) const
  {
    return getObjIndex(id) != 0;
  }

  bool canMoveObjTopmost(int id) const
  {
    return canMoveObjUp(id);
  }

  bool canMoveObjUp(int id) const
  {
    return getObjIndex(id) != mObjIdx.size() - 1;
  }

  bool moveObjBottommost(int id)
  {
    return doZorderObj(id, LevelCmdZorderObj<Level>::MOVE_BOTTOMMOST);
  }

  bool moveObjDown(int id)
  {
    return doZorderObj(id, LevelCmdZorderObj<Level>::MOVE_DOWN);
  }

  bool moveObjTopmost(int id)
  {
    return doZorderObj(id, LevelCmdZorderObj<Level>::MOVE_TOPMOST);
  }

  bool moveObjUp(int id)
  {
    return doZorderObj(id, LevelCmdZorderObj<Level>::MOVE_UP);
  }

  bool doZorderObj(int id, int move)
  {
    LevelCmdZorderObj<Level>* pcmd;
    pcmd = new LevelCmdZorderObj<Level>(mId, id, move);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  //
  // Alignment.
  //

  template<class ImgT>
  bool doAlignObjs(std::vector<int> const& ids, int type)
  {
    //
    // Type: 0(left), 1(right), 2(top), 3(bottom).
    //

    if (1 >= ids.size()) {
      return false;
    }

    //
    // Get pivot obj boundary rect for reference.
    //

    PrjT &prj = PrjT::inst();

    sw2::IntRect rcPivot;
    prj.getObjDim<ImgT>(getObj(ids.back()), rcPivot);

    //
    // Move objects.
    //

    int tag = mUndo.mTag;

    for (size_t i = 0; i < ids.size() - 1; i++) {

      PrjT::ObjectT const& o = getObj(ids[i]);

      sw2::IntRect rc;
      prj.getObjDim<ImgT>(o, rc);

      int offsetX = 0, offsetY = 0;

      switch (type)
      {
      case ALIGN_LEFT:
        offsetX = rcPivot.left - rc.left;
        break;
      case ALIGN_RIGHT:
        offsetX = rcPivot.right - rc.right;
        break;
      case ALIGN_TOP:
        offsetY = rcPivot.top - rc.top;
        break;
      case ALIGN_BOTTOM:
        offsetY = rcPivot.bottom - rc.bottom;
        break;
      }

      LevelCmdMoveObj<Level>* pcmd;
      pcmd = new LevelCmdMoveObj<Level>(mId, ids[i], offsetX, offsetY);

      if (!mUndo.execAndAdd(pcmd)) {
        if (0 < i) {
          mUndo.undo();
        }
        return false;
      }

      pcmd->mTag = tag;
    }

    prj.mModified = true;

    return true;
  }

  template<class ImgT>
  bool alignLeft(std::vector<int> const& ids)
  {
    return doAlignObjs<ImgT>(ids, ALIGN_LEFT);
  }

  template<class ImgT>
  bool alignRight(std::vector<int> const& ids)
  {
    return doAlignObjs<ImgT>(ids, ALIGN_RIGHT);
  }

  template<class ImgT>
  bool alignTop(std::vector<int> const& ids)
  {
    return doAlignObjs<ImgT>(ids, ALIGN_TOP);
  }

  template<class ImgT>
  bool alignBottom(std::vector<int> const& ids)
  {
    return doAlignObjs<ImgT>(ids, ALIGN_BOTTOM);
  }

  //
  // Hierarchy.
  //

  bool changeParent(int idObj, int idNewParent)
  {
    LevelCmdChangeParent<Level>* pcmd;
    pcmd = new LevelCmdChangeParent<Level>(mId, idObj, idNewParent);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  //
  // Add tool.
  //

  bool isAddColorTool() const
  {
    return TOOL_ADDCOLBG == mTool;
  }

  bool isAddDummyTool() const
  {
    return TOOL_ADDDUMMY == mTool;
  }

  bool isAddLevelobjTool() const
  {
    return TOOL_ADDLVLOBJ == mTool;
  }

  bool isAddMapTool() const
  {
    return TOOL_ADDMAPBG == mTool;
  }

  bool isAddSpriteTool() const
  {
    return TOOL_ADDSPRITE == mTool;
  }

  bool isAddTexTool() const
  {
    return TOOL_ADDTEXBG == mTool;
  }

  bool isMoveTool() const
  {
    return TOOL_MOVE == mTool;
  }

  bool isRemoveTool() const
  {
    return TOOL_REMOVE == mTool;
  }

  void setAddColorTool(unsigned int color)
  {
    mAddMap = mAddTex = mAddSpr = -1;
    mAddCol = color;
    mTool = TOOL_ADDCOLBG;
  }

  void setAddDummyTool()
  {
    mAddMap = mAddTex = mAddSpr = 0xff;
    mAddCol = 0xff0000ff;
    mTool = TOOL_ADDDUMMY;
  }

  void setAddLevelObjTool(int id)
  {
    mAddMap = mAddTex = 0xfe;
    mAddSpr = id;
    mAddCol = 0xff0000ff;
    mTool = TOOL_ADDLVLOBJ;
  }

  void setMoveTool()
  {
    mTool = TOOL_MOVE;
  }

  void setRemoveTool()
  {
    mTool = TOOL_REMOVE;
  }

  void setToolByObjId(int id)
  {
    if (-1 >= id) {
      return;
    }

    mAddMap = mAddTex = mAddSpr = -1;
    mAddCol = 0xff0000ff;

    PrjT::LevelT& lvl = PrjT::inst().getLevel(mId);
    const PrjT::ObjectT &o = lvl.getObj(id);

    switch (o.mType)
    {
    case PrjT::ObjectT::TYPE_SPRITE:
      mAddSpr = o.mSpriteId;
      mTool = TOOL_ADDSPRITE;
      break;
    case PrjT::ObjectT::TYPE_COLBG:
      mAddCol = o.mBgColor;
      mTool = TOOL_ADDCOLBG;
      break;
    case PrjT::ObjectT::TYPE_TEXBG:
      mAddTex = o.mTextureId;
      mTool = TOOL_ADDTEXBG;
      break;
    case PrjT::ObjectT::TYPE_MAPBG:
      mAddMap = o.mMapId;
      mTool = TOOL_ADDMAPBG;
      break;
    case PrjT::ObjectT::TYPE_DUMMY:
      mAddMap = mAddTex = mAddSpr = 0xff;
      mTool = TOOL_ADDDUMMY;
      break;
    case PrjT::ObjectT::TYPE_LVLOBJ:
      mAddMap = mAddTex = 0xfe;
      mAddSpr = o.getLevelObjId();
      mTool = TOOL_ADDLVLOBJ;
      break;
    }
  }

  void setToolByResId(int id)
  {
    const PrjT::ResT &res = PrjT::inst().mRes;
    if (res.isSprite(id)) {
      mAddMap = mAddTex = -1;
      mAddCol = 0xff0000ff;
      mAddSpr = id;
      mTool = TOOL_ADDSPRITE;
    } else if (res.isTex(id)) {
      mAddMap = mAddSpr = -1;
      mAddCol = 0xff0000ff;
      mAddTex = id;
      mTool = TOOL_ADDTEXBG;
    } else if (res.isMap(id)) {
      mAddTex = mAddSpr = -1;
      mAddCol = 0xff0000ff;
      mAddMap = id;
      mTool = TOOL_ADDMAPBG;
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
  // Implementation.
  //

  static int addObj(Level& lvl, int idParent, int idObj, int idSprite, int idMap, int idTexture, int x, int y)
  {
    PrjT& prj = PrjT::inst();

    int type = -1;
    if (0xff == idSprite && 0xff == idTexture && 0xff == idMap) {
      type = PrjT::ObjectT::TYPE_DUMMY;
    } else if (0xfe == idTexture && 0xfe == idMap) {
      type = PrjT::ObjectT::TYPE_LVLOBJ;
    } else if (0 <= idSprite) {
      type = PrjT::ObjectT::TYPE_SPRITE;
    } else if (0 <= idTexture) {
      type = PrjT::ObjectT::TYPE_TEXBG;
    } else if (0 <= idMap) {
      type = PrjT::ObjectT::TYPE_MAPBG;
    } else {
      type = PrjT::ObjectT::TYPE_COLBG;
    }

    if (PrjT::ObjectT::TYPE_TEXBG == type && !prj.mRes.isTex(idTexture)) {
      SW2_TRACE_ERROR("Add sprite object failed, texture is not exist!");
      return -1;
    }

    int id = -1;

    if (-1 == idObj) {                  // Not specified.
      id = prj.mRes.mId.alloc();
      if (-1 == id) {
        return -1;
      }
    } else {                            // Sepcified obj id.
      id = prj.mRes.mId.alloc(idObj);
      assert(idObj == id);
    }

    PrjT::ObjectT o;
    o.mId = id;
    o.mVisible = true;
    o.mSpriteId = idSprite;
    o.mMapId = idMap;
    o.mTextureId = idTexture;
    o.mPosX = x;
    o.mPosY = y;
    o.mBgColor = 0xffffff;
    o.mRepX = o.mRepY = false;
    o.mRot = .0f;
    o.mScaleX = o.mScaleY = 1.0f;
    o.mAnchorX = o.mAnchorY = .0f;
    o.mDim.setEmpty();
    o.mType = type;

    lvl.mObj[id] = o;
    if (idParent == lvl.mId) {
      lvl.mObjIdx.push_back(id);
    } else {
      lvl.getObj(idParent).mObjIdx.push_back(id);
    }

    prj.mModified = true;

    return id;
  }

  static bool moveObj(Level& lvl, int id, int& oldX, int& oldY)
  {
    std::map<int, ObjectT>::iterator it = lvl.mObj.find(id);
    if (lvl.mObj.end() == it) {
      return false;
    }

    ObjectT& o = it->second;

    int x = o.mPosX, y = o.mPosY;
    (void)o.setPos(oldX, oldY);

    oldX = x, oldY = y;

    return true;
  }

  static bool removeObj(Level& lvl, int id)
  {
    int idParent = lvl.getParent(id);

    if (lvl.mId == idParent) {
      if (!PrjT::inst().removeResource(lvl.mObj, lvl.mObjIdx, id)) {
        return false;
      }
    } else {
      std::map<int, ObjectT>::iterator it = lvl.mObj.find(idParent);
      if (lvl.mObj.end() == it) {
        return false;
      }
      if (!PrjT::inst().removeResource(lvl.mObj, it->second.mObjIdx, id)) {
        return false;
      }
    }

    PrjT::inst().mModified = true;

    return true;
  }

  static bool changeParent(Level& lvl, int id, int idNewParent)
  {
    int idParent = lvl.getParent(id);
    if (lvl.mId == idParent) {
      lvl.mObjIdx.erase(lvl.mObjIdx.begin() + lvl.getObjIndex(id));
    } else {
      ObjectT& p = lvl.getObj(idParent);
      p.mObjIdx.erase(p.mObjIdx.begin() + p.getObjIndex(id));
    }

    ObjectT& o = lvl.getObj(id);
    int OffsetX = o.mPosX, OffsetY = o.mPosY;
    while (idParent != lvl.mId) {       // Get local pos of the obj in lvl.
      ObjectT& p = lvl.getObj(idParent);
      OffsetX += p.mPosX;
      OffsetY += p.mPosY;
      idParent = lvl.getParent(idParent);
    }

    if (lvl.mId == idNewParent) {
      lvl.mObjIdx.push_back(id);
    } else {
      lvl.getObj(idNewParent).mObjIdx.push_back(id);
    }

    while (idNewParent != lvl.mId) {    // Get and adjust new local pos of the obj in lvl.
      ObjectT& p = lvl.getObj(idNewParent);
      OffsetX -= p.mPosX;
      OffsetY -= p.mPosY;
      idNewParent = lvl.getParent(idNewParent);
    }

    o.mPosX = OffsetX;
    o.mPosY = OffsetY;

    PrjT::inst().mModified = true;

    return true;
  }

  static bool moveObjBottommost(Level& lvl, int id)
  {
    int i = lvl.getObjIndex(id);
    if (-1 != i) {
      lvl.mObjIdx.erase(lvl.mObjIdx.begin() + i);
      lvl.mObjIdx.insert(lvl.mObjIdx.begin(), id);
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  static bool moveObjDown(Level& lvl, int id)
  {
    int i = lvl.getObjIndex(id);
    if (-1 != i) {
      std::swap(lvl.mObjIdx[i - 1], lvl.mObjIdx[i]);
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  static bool moveObjTopmost(Level& lvl, int id)
  {
    int i = lvl.getObjIndex(id);
    if (-1 != i) {
      lvl.mObjIdx.erase(lvl.mObjIdx.begin() + i);
      lvl.mObjIdx.push_back(id);
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  static bool moveObjUp(Level& lvl, int id)
  {
    int i = lvl.getObjIndex(id);
    if (-1 != i) {
      std::swap(lvl.mObjIdx[i + 1], lvl.mObjIdx[i]);
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  static bool setSize(Level& lvl, int w, int h)
  {
    if (lvl.mWidth == w && lvl.mHeight == h) {
      return false;
    }
    lvl.mWidth = w;
    lvl.mHeight = h;
    PrjT::inst().mModified = true;
    return true;
  }

  //
  // Store level.
  //

  template<class PoolT>
  bool load(PoolT& p, sw2::Ini const& ini, int id) // Override.
  {
    //
    // Load default.
    //

    if (!good::Level<ObjectT>::load(p, ini, id)) {
      return false;
    }

    std::string secName = good::getSecName(mId, "level");

    sw2::Ini sec = ini[secName];

    //
    // Snap settings.
    //

    if (sec.find("showSnap")) {
      mShowSnap = sec["showSnap"];
    }

    if (sec.find("snapWidth")) {
      mSnapWidth = sec["snapWidth"];
      mSnapHeight = sec["snapHeight"];
    } else {
      mSnapWidth = mSnapHeight = 16;
    }

    //
    // Grids.
    //

    if (sec.find("showLine")) {
      mShowSnap = sec["showLine"];
    }

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

    std::string secName = good::getSecName(mId, "level");

    if (!ObjectT::store_i(ini, secName, mObj)) {
      return false;
    }

    sw2::Ini& sec = ini[secName];

    //
    // Property.
    //

    if (PrjT::inst().mRes.mWidth != mWidth) {
      sec["width"] = mWidth;
    }
    if (PrjT::inst().mRes.mHeight != mHeight) {
      sec["height"] = mHeight;
    }

    if (!mShowSnap) {
      sec["showSnap"] = mShowSnap;
    }

    if (16 != mSnapWidth || 16 != mSnapHeight) {
      sec["snapWidth"] = mSnapWidth;
      sec["snapHeight"] = mSnapHeight;
    }

    if (!mShowLine) {
      sec["showLine"] = mShowLine;
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

// end of lvl.h
