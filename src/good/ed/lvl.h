
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

enum GOOD_LEVELED_UNDO_COMMAND {
  GOOD_LEVELED_CMD_ADDOBJ,
  GOOD_LEVELED_CMD_REMOVEOBJ,
  GOOD_LEVELED_CMD_MOVEOBJ,
  GOOD_LEVELED_CMD_ZORDER,
  GOOD_LEVELED_CMD_SETBGCOLOR,
  GOOD_LEVELED_CMD_SETDIM,
  GOOD_LEVELED_CMD_SETPROP,
  GOOD_LEVELED_CMD_CHANGEPARENT,
  GOOD_LEVELED_CMD_SETSCRIPT,
  GOOD_LEVELED_CMD_SETSIZE,
  GOOD_LEVELED_CMD_SETLEVELPOS
};

namespace clvl {

template<class LvlT>
class CmdAddObj : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  int mIdSprite;
  int mIdMap;
  int mIdTexture;
  int mPosX, mPosY;
  unsigned int mBgColor;

  CmdAddObj(int idLevel, int idSprite, int idMap, int idTexture, int x, int y)
    : UndoCommand(GOOD_LEVELED_CMD_ADDOBJ), mLevelId(idLevel)
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
class CmdMoveObj : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  int mPosX, mPosY;

  CmdMoveObj(int idLevel, int id, int offsetX, int offsetY) : UndoCommand(GOOD_LEVELED_CMD_MOVEOBJ), mLevelId(idLevel)
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
class CmdRemoveObj : public UndoCommand
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

  CmdRemoveObj(int idLevel, int id) : UndoCommand(GOOD_LEVELED_CMD_REMOVEOBJ), mLevelId(idLevel)
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
class CmdZorderObj : public UndoCommand
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

  CmdZorderObj(int idLevel, int id, int move) : UndoCommand(GOOD_LEVELED_CMD_ZORDER), mLevelId(idLevel)
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
class CmdSetBgColor : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  unsigned int mColor;

  CmdSetBgColor(int idLevel, int id, unsigned int color) : UndoCommand(GOOD_LEVELED_CMD_SETBGCOLOR), mLevelId(idLevel)
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
class CmdSetDim : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  sw2::IntRect mDim;

  CmdSetDim(int idLevel, int id, int newx, int newy, int neww, int newh) : UndoCommand(GOOD_LEVELED_CMD_SETDIM), mLevelId(idLevel)
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
class CmdSetProp : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  std::string mName;
  bool mVisible;
  float mRot, mScaleX, mScaleY, mAnchorX, mAnchorY;
  bool mRepX, mRepY;

  CmdSetProp(int idLevel, int id, std::string const& name, bool vis, float rot, float sx, float sy, float ax, float ay, bool repx, bool repy)
    : UndoCommand(GOOD_LEVELED_CMD_SETPROP), mLevelId(idLevel)
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
class CmdChangeParent : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  int mOrgParentId;
  int mOrgIdx;
  int mNewParentId;

  CmdChangeParent(int idLevel, int id, int idNewParent) : UndoCommand(GOOD_LEVELED_CMD_CHANGEPARENT), mLevelId(idLevel)
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
class CmdSetScript : public UndoCommand
{
public:
  int mLevelId;
  int mId;
  std::string mScript;

  CmdSetScript(int idLevel, int id, const std::string &script) : UndoCommand(GOOD_LEVELED_CMD_SETSCRIPT), mLevelId(idLevel)
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
class CmdSetSize : public UndoCommand
{
public:
  int mLevelId;
  int mWidth, mHeight;

  CmdSetSize(int id, int w, int h) : UndoCommand(GOOD_LEVELED_CMD_SETSIZE)
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
class CmdSetLevelPos : public UndoCommand
{
public:
  int mLevelId;
  int mPosX, mPosY;

  CmdSetLevelPos(int id, int x, int y) : UndoCommand(GOOD_LEVELED_CMD_SETLEVELPOS)
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

} // namespace clvl

template<class PrjT>
class Level : public good::Level<Object<PrjT> >
{
public:
  typedef Object<PrjT> ObjectT;

  UndoImpl mUndo;

  bool mShowSnap;
  int mSnapWidth, mSnapHeight;

  Level() : mUndo(PrjT::UNDO_LEVEL), mShowSnap(true), mSnapWidth(16), mSnapHeight(16)
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

  bool setSize(int newWidth, int newHeight)
  {
    clvl::CmdSetSize<Level>* pcmd;
    pcmd = new clvl::CmdSetSize<Level>(mId, newWidth, newHeight);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool setShowSnap(bool b)
  {
    if (b == mShowSnap) {
      return false;
    }

    mShowSnap = b;

    PrjT::inst().mModified = true;

    return true;
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

  //
  // Modify object property.
  //

  bool setObjBgColor(int idObj, unsigned int color)
  {
    clvl::CmdSetBgColor<Level>* pcmd;
    pcmd = new clvl::CmdSetBgColor<Level>(mId, idObj, color);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool setObjDim(int idObj, int newx, int newy, int neww, int newh)
  {
    clvl::CmdSetDim<Level>* pcmd;
    pcmd = new clvl::CmdSetDim<Level>(mId, idObj, newx, newy, neww, newh);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool setObjProp(int idObj, std::string const& name, bool vis, float rot, float sx, float sy, float ax, float ay, bool repx, bool repy)
  {
    clvl::CmdSetProp<Level>* pcmd;
    pcmd = new clvl::CmdSetProp<Level>(mId, idObj, name, vis, rot, sx, sy, ax, ay, repx, repy);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
  }

  bool setObjScript(int idObj, const std::string &script)
  {
    clvl::CmdSetScript<Level>* pcmd;
    pcmd = new clvl::CmdSetScript<Level>(mId, idObj, script);

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

  int addObj(int idSprite, int idMap, int idTexture, int x, int y)
  {
    clvl::CmdAddObj<Level>* pcmd;
    pcmd = new clvl::CmdAddObj<Level>(mId, idSprite, idMap, idTexture, x, y);

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

    clvl::CmdRemoveObj<Level>* pcmd;
    pcmd = new clvl::CmdRemoveObj<Level>(mId, id);

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
      clvl::CmdMoveObj<Level>* pcmd;
      pcmd = new clvl::CmdMoveObj<Level>(mId, ids[i], offsetX, offsetY);

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
    clvl::CmdSetLevelPos<Level>* pcmd;
    pcmd = new clvl::CmdSetLevelPos<Level>(mId, posX, posY);

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

      clvl::CmdSetDim<Level>* pcmd;
      pcmd = new clvl::CmdSetDim<Level>(mId, ids[i], o.mDim.left, o.mDim.top, neww, newh);

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
    return doZorderObj(id, clvl::CmdZorderObj<Level>::MOVE_BOTTOMMOST);
  }

  bool moveObjDown(int id)
  {
    return doZorderObj(id, clvl::CmdZorderObj<Level>::MOVE_DOWN);
  }

  bool moveObjTopmost(int id)
  {
    return doZorderObj(id, clvl::CmdZorderObj<Level>::MOVE_TOPMOST);
  }

  bool moveObjUp(int id)
  {
    return doZorderObj(id, clvl::CmdZorderObj<Level>::MOVE_UP);
  }

  bool doZorderObj(int id, int move)
  {
    clvl::CmdZorderObj<Level>* pcmd;
    pcmd = new clvl::CmdZorderObj<Level>(mId, id, move);

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
  bool doAlignObjects(std::vector<int> const& ids, int type)
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
      case 0:                           // Align left.
        offsetX = rcPivot.left - rc.left;
        break;
      case 1:                           // Align right.
        offsetX = rcPivot.right - rc.right;
        break;
      case 2:                           // Align top.
        offsetY = rcPivot.top - rc.top;
        break;
      case 3:                           // Align bottom.
        offsetY = rcPivot.bottom - rc.bottom;
        break;
      }

      clvl::CmdMoveObj<Level>* pcmd;
      pcmd = new clvl::CmdMoveObj<Level>(mId, ids[i], offsetX, offsetY);

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
    return doAlignObjects<ImgT>(ids, 0);
  }

  template<class ImgT>
  bool alignRight(std::vector<int> const& ids)
  {
    return doAlignObjects<ImgT>(ids, 1);
  }

  template<class ImgT>
  bool alignTop(std::vector<int> const& ids)
  {
    return doAlignObjects<ImgT>(ids, 2);
  }

  template<class ImgT>
  bool alignBottom(std::vector<int> const& ids)
  {
    return doAlignObjects<ImgT>(ids, 3);
  }

  //
  // Hierarchy.
  //

  bool changeParent(int idObj, int idNewParent)
  {
    clvl::CmdChangeParent<Level>* pcmd;
    pcmd = new clvl::CmdChangeParent<Level>(mId, idObj, idNewParent);

    if (mUndo.execAndAdd(pcmd)) {
      PrjT::inst().mModified = true;
      return true;
    }

    return false;
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

    if (-1 != idTexture && 0xff != idObj && 0xff != idMap && !prj.mRes.isTex(idTexture)) {
      SW2_TRACE_ERROR("Add sprite object failed, texture is not exist!");
      return false;
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

    if (0xff == idSprite && 0xff == idTexture && 0xff == idMap) {
      o.mType = PrjT::ObjectT::TYPE_DUMMY;
    } else if (0 <= idSprite) {
      o.mType = PrjT::ObjectT::TYPE_SPRITE;
    } else if (0 <= idTexture) {
      o.mType = PrjT::ObjectT::TYPE_TEXBG;
    } else if (0 <= idMap) {
      o.mType = PrjT::ObjectT::TYPE_MAPBG;
    } else {
      o.mType = PrjT::ObjectT::TYPE_COLBG;
    }

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

    return true;
  }
};

} // namespace ed

} // namespace good

// end of lvl.h
