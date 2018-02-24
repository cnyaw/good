
//
// api.h
// Script api.
//
// Copyright (c) 2016 Waync Cheng.
// All Rights Reserved.
//
// 2016/7/1 Waync created.
//

#pragma once

bool addChild(int idParent, int idChild, int index)
{
  if (!mActors.isUsed(idChild)) {
    return false;
  }

  if (!mActors.isUsed(idParent)) {
    idParent = mRoot;
  }

  ActorT &p = mActors[idParent], &c = mActors[idChild];

  if (-1 != c.mParent) {
    ActorT& cp = mActors[c.mParent];
    cp.removeChild(idChild);
  }

  p.addChild(idChild, index);

  mDirty = true;

  return true;
}

void callPackage(const char* name)
{
  if (name) {

    //
    // Save current running project.
    //

    mPackageStack.push_back(mRes.mFileName);

    //
    // Now, set new package as active project. Assume new package is at the
    // same folder
    //

    mCallStack.push_back(name);
  }
}

int cloneObj(int idObj)
{
  int idNew = -1;
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    idNew = a.clone_i();
    if (-1 != idNew) {
      mActors[a.mParent].addChild(idNew);
      mDirty = true;
    }
  }
  return idNew;
}

void doLuaScript(const char *str, ...)
{
  va_list va;
  va_start(va, str);
  if (str && mLua) {
    char buf[1024];
    vsnprintf(buf, sizeof(buf), str, va);
    if (luaL_dostring(mLua, buf)) {
      char const *strErr = lua_tostring(mLua, -1);
      trace("%s\n", strErr);
    }
  }
  va_end(va);
}

void drawImage(int x, int y, int texId, int srcx, int srcy, int srcw, int srch, unsigned int color, float rot = .0f, float scalex = 1.0f, float scaley = 1.0f) const
{
  ImgT img = getImage(mRes.getTex(texId).mFileName);
  ((T*)this)->gx.drawImage(x, y, img, srcx, srcy, srcw, srch, color, rot, scalex, scaley);
  mDirty = true;
}

void drawText(int x, int y, char const *utf8text, int size, unsigned int color) const
{
  if (0 == utf8text) {
    return;
  }

  size = sw2::clamp(size, GOOD_MIN_TEXT_SIZE, GOOD_MAX_TEXT_SIZE);

  std::vector<int> unicode;
  sw2::Util::utf8ToU16(utf8text, unicode);

  int xoffset = 0;
  for (size_t i = 0; i < unicode.size(); i++) {
    ImgT img = getImage(size, unicode[i]);
    if (img.isValid()) {
      ((T*)this)->gx.drawImage(x + xoffset, y, img, 0, 0, img.getWidth(), img.getHeight(), color, .0f, 1.0f, 1.0f);
      xoffset += img.getWidth();
    } else {
      xoffset += 10;
    }
  }

  mDirty = true;
}

void exit()
{
  if (GOOD_LOGO_NAME == mRes.mName) {
    exitPackage();
  } else {
    mExit = true;
  }
}

void fillRect(int x, int y, int w, int h, unsigned int color, float rot = .0f, float scalex = 1.0f, float scaley = 1.0f) const
{
  ((T*)this)->gx.fillSolidColor(x, y, w, h, color, rot, scalex, scaley);
  mDirty = true;
}

int findChild(int idObj, const char* name) const
{
  if (name) {
    if (mActors.isUsed(idObj)) {
      ActorT const &o = mActors[idObj];
      for (size_t i = 0; i < o.mChild.size(); ++i) {
        int idChild = o.mChild[i];
        if (mActors[idChild].mResName == name) {
          return idChild;
        }
      }
    }
  }
  return -1;
}

int genDummy(int idParent, const char* script)
{
  int newid = allocActor();
  if (-1 == newid) {
    return -1;
  }

  ActorT& a = mActors[newid];
  a.create(newid, ActorT::TYPES_DUMMY, -1);

  if (!mActors.isUsed(idParent)) {
    idParent = mRoot;
  }

  mActors[idParent].addChild(newid);

  if (script) {
    a.setScript(script);
    a.OnCreate();
  }

  mDirty = true;

  return newid;
}

int genObj(int idParent, int idRes, char const *script)
{
  int newid = -1;

  if (!mActors.isUsed(idParent)) {
    idParent = mRoot;
  }

  if (mRes.isLevel(idRes)) {
    if (-1 != mRoot) {
      mActors[mRoot].free();
#ifdef GOOD_SUPPORT_STGE
      resetStge();
#endif
      mRoot = -1;
    }
    newid = createLevel(idRes);
    if (-1 != newid) {
      mRoot = newid;
    }
  } else if (mRes.isTex(idRes) || mRes.isMap(idRes) || mRes.isSprite(idRes) || 0 >= idRes/*color bg*/) {
    newid = allocActor();
    if (-1 != newid) {
      ActorT& a = mActors[newid];
      if (mRes.isTex(idRes)) {
        a.create(newid, ActorT::TYPES_TEXBG, idRes);
      } else if (mRes.isMap(idRes)) {
        a.create(newid, ActorT::TYPES_MAPBG, idRes);
      } else if (mRes.isSprite(idRes)) {
        a.create(newid, ActorT::TYPES_SPRITE, idRes);
        a.setSprite(idRes);
      } else {
        a.create(newid, ActorT::TYPES_COLBG, -1);
        a.mBgColor = 0xffff0000;
        a.mDim = sw2::IntRect(0,0,32,32);
      }
      mActors[idParent].addChild(newid);
    }
  } else {
    std::map<int, LevelT>::const_iterator it;
    for (it = mRes.mLevel.begin(); mRes.mLevel.end() != it; ++it) {
      if (it->second.isObj(idRes)) {
        newid = createObj(idParent, it->second, idRes, false, 0, 0); // Gen child obj of a lvl.
        break;
      }
    }
  }

  if (-1 != newid) {
    mDirty = true;
    if (script) {
      ActorT& a = mActors[newid];
      a.setScript(script);
      a.OnCreate();
    }
  }

  return newid;
}

int dupRes(char const *pPkgName, ResT &Res, int idRes)
{
  std::map<std::string, std::map<int, int> >::const_iterator itExtPkg = mExternalResMap.find(pPkgName);
  if (mExternalResMap.end() != itExtPkg) {
    const std::map<int, int> &ResMap = itExtPkg->second;
    std::map<int, int>::const_iterator itExtResId = ResMap.find(idRes);
    if (ResMap.end() != itExtResId) {
      return itExtResId->second;
    }
  }

  int idTmpRes = -1;
  if (Res.isTex(idRes)) {
    idTmpRes = mRes.mId.alloc();
    assert(-1 != idTmpRes);
    mRes.mTexIdx.push_back(idTmpRes);
    mRes.mTex[idTmpRes] = Res.getTex(idRes);
    mExternalResMap[pPkgName][idRes] = idTmpRes;
  } else if (Res.isSprite(idRes)) {
    const SpriteT &spr = Res.getSprite(idRes);
    int idTmpTex = mRes.mId.alloc();
    assert(-1 != idTmpTex);
    mRes.mTexIdx.push_back(idTmpTex);
    mRes.mTex[idTmpTex] = Res.getTex(spr.mTileset.mTextureId);
    mExternalResMap[pPkgName][spr.mTileset.mTextureId] = idTmpTex; // Tex of the sprite.
    idTmpRes = mRes.mId.alloc();
    assert(-1 != idTmpRes);
    mRes.mSpriteIdx.push_back(idTmpRes);
    mRes.mSprite[idTmpRes] = spr;
    mRes.mSprite[idTmpRes].mTileset.mTextureId = idTmpTex;
    mExternalResMap[pPkgName][idRes] = idTmpRes;
  } else if (Res.isMap(idRes)) {
    const MapT &m = Res.getMap(idRes);
    int idTmpTex = mRes.mId.alloc();
    assert(-1 != idTmpTex);
    mRes.mTexIdx.push_back(idTmpTex);
    mRes.mTex[idTmpTex] = Res.getTex(m.mTileset.mTextureId);
    mExternalResMap[pPkgName][m.mTileset.mTextureId] = idTmpTex; // Tex of the map.
    idTmpRes = mRes.mId.alloc();
    assert(-1 != idTmpRes);
    mRes.mMapIdx.push_back(idTmpRes);
    mRes.mMap[idTmpRes] = m;
    mRes.mMap[idTmpRes].mTileset.mTextureId = idTmpTex;
    mExternalResMap[pPkgName][idRes] = idTmpRes;
  }
  return idTmpRes;
}

int genObjEx(char const *pPkgName, int idParent, int idRes, char const *script)
{
  if (0 == pPkgName) {
    return -1;
  }

  std::map<std::string, std::map<int, int> >::const_iterator itExtPkg = mExternalResMap.find(pPkgName);
  if (mExternalResMap.end() != itExtPkg) {
    const std::map<int, int> &ResMap = itExtPkg->second;
    std::map<int, int>::const_iterator itExtResId = ResMap.find(idRes);
    if (ResMap.end() != itExtResId) {
      return genObj(idParent, itExtResId->second, script);
    }
  }

  std::string ResName(pPkgName);
  std::replace(ResName.begin(), ResName.end(), '\\', '/');

  std::string path = ResName.substr(0, ResName.find_last_of('/') + 1);
  if (path.empty()) {
    path = "./";
  }

  if (!addFileSystem(path)) {
    return -1;
  }

  std::string name = ResName.substr(ResName.find_last_of('/') + 1);
  if (isGoodArchive(name) && !name.empty() && mFileSys.end() == mFileSys.find(name)) {
    std::stringstream ss;
    if (!loadFile(name, ss) || !addFileSystem(name, ss)) {
      return -1;
    }
  }

  std::stringstream ss;
  if (!loadFile(decidePrjName(name), ss)) {
    return -1;
  }

  ResT Res;
  if (!Res.load(ss)) {
    return -1;
  }

  if (Res.isLevel(idRes)) {
    return -1;                          // Gen lvl is invalid.
  }

  int idTmpRes = dupRes(pPkgName, Res, idRes);
  if (-1 != idTmpRes) {
    return genObj(idParent, idTmpRes, script);
  } else {
    std::map<int, LevelT>::const_iterator it;
    for (it = Res.mLevel.begin(); Res.mLevel.end() != it; ++it) {
      if (it->second.isObj(idRes)) {
        if (!mActors.isUsed(idParent)) {
          idParent = mRoot;
        }
        int newid = createObj(idParent, it->second, idRes, false, pPkgName, &Res); // Gen child obj of a lvl.
        if (-1 != newid) {
          mDirty = true;
          if (script) {
            ActorT& a = mActors[newid];
            a.setScript(script);
            a.OnCreate();
          }
        }
        return newid;
      }
    }
    return -1;
  }
}

int genTextObj(int idParent, char const *utf8text, int size, char const *script)
{
  if (0 == utf8text) {
    return -1;
  }

  int newid = genDummy(idParent, 0);
  if (-1 == newid) {
    return -1;
  }

  size = sw2::clamp(size, GOOD_MIN_TEXT_SIZE, GOOD_MAX_TEXT_SIZE);

  std::vector<int> unicode;
  sw2::Util::utf8ToU16(utf8text, unicode);

  float xoffset = .0f;
  for (size_t i = 0; i < unicode.size(); i++) {
    int chid = allocActor();
    if (-1 == chid) {
      break;
    }
    mDirty = true;
    ActorT& a = mActors[chid];
    a.create(chid, ActorT::TYPES_TEXBG, -1);
    a.mPosX = xoffset;
    a.mImg = getImage(size, unicode[i]);
    if (a.mImg.isValid()) {
      xoffset += a.mImg.getWidth();
    } else {
      xoffset += 10;
    }
    mActors[newid].addChild(chid);
  }

  if (script) {
    ActorT& a = mActors[newid];
    a.setScript(script);
    a.OnCreate();
  }

  return newid;
}

void getAnchor(int idObj, float &x, float &y) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    x = a.mAnchorX;
    y = a.mAnchorY;
  }
}

bool getAntiAlias() const
{
  return mAntiAlias;
}

unsigned int getBgColor(int idObj) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    return a.mBgColor;
  } else {
    return 0;
  }
}

int getChild(int idObj, int idx) const
{
  int idChild = -1;
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    idChild = a.getChild(idx);
  }
  return idChild;
}

int getChildCount(int idObj) const
{
  int cnt = 0;
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    cnt = a.getChildCount();
  }
  return cnt;
}

void getDim(int idObj, int& left, int& top, int& width, int& height) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    getDim_i(a, left, top, width, height);
  }
}

void getDim_i(ActorT const& a, int& left, int& top, int& width, int& height) const
{
  left = a.mDim.left;
  top = a.mDim.top;

  switch (a.mResType)
  {
  case ActorT::TYPES_LEVEL:
    {
      LevelT const& lvl = mRes.getLevel(a.mResId);
      width = lvl.mWidth;
      height = lvl.mHeight;
    }
    break;

  case ActorT::TYPES_COLBG:
    if (a.mDim.empty()) {
      width = height = 32;
    } else {
      width = a.mDim.width();
      height = a.mDim.height();
    }
    break;

  case ActorT::TYPES_SPRITE:
    {
      SpriteT const& spr = mRes.getSprite(a.mResId);
      width = spr.mTileset.mTileWidth;
      height = spr.mTileset.mTileHeight;
    }
    break;

  case ActorT::TYPES_TEXBG:
    if (a.mDim.empty()) {
      ImgT img = getTexBgImg_i(a);
      if (img.isValid()) {
        width = img.getWidth();
        height = img.getHeight();
      } else {
        width = height = 0;
      }
    } else {
      width = a.mDim.width();
      height = a.mDim.height();
    }
    break;

  case ActorT::TYPES_MAPBG:
    {
      MapT const& map = mRes.getMap(a.mResId);
      width = map.mWidth * map.mTileset.mTileWidth;
      height = map.mHeight * map.mTileset.mTileHeight;
    }
    break;

  default:
    width = a.mDim.width();
    height = a.mDim.height();
    break;
  }
}

int getFirstResLevelId() const
{
  if (!mRes.mLevelIdx.empty()) {
    return mRes.mLevelIdx[0];
  } else {
    return -1;
  }
}

int getMapId(int idObj) const
{
  int map = -1;
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    if (ActorT::TYPES_MAPBG == a.mResType) {
      map = a.mResId;
    }
  }
  return map;
}

std::string getName(int idObj) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    return a.mResName;
  } else {
    return "";
  }
}

int getNextResLevelId(int idLvl) const
{
  for (size_t i = 0; i < mRes.mLevelIdx.size(); i++) {
    if (mRes.mLevelIdx[i] == idLvl) {
      if (mRes.mLevelIdx.size() != i + 1) {
        return mRes.mLevelIdx[i + 1];
      }
    }
  }
  return -1;
}

int getParent(int idObj) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    return a.mParent;
  } else {
    return -1;
  }
}

void getPos(int idObj, float &x, float &y) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    x = a.mPosX;
    y = a.mPosY;
  }
}

void getRep(int idObj, bool &bRepX, bool &bRepY) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    bRepX = a.mRepX;
    bRepY = a.mRepY;
  }
}

template<class MapT>
int getResIdByName(const MapT &m, const char* name) const
{
  if (name) {
    typename MapT::const_iterator it = m.begin();
    for (; m.end() != it; ++it) {
      if (it->second.getName() == name) {
        return it->first;
      }
    }
  }
  return -1;
}

int getResLevelId(const char* name) const
{
  return getResIdByName(mRes.mLevel, name);
}

int getResMapId(const char* name) const
{
  return getResIdByName(mRes.mMap, name);
}

void getResMapSize(int idMap, int &cx, int &cy) const
{
  if (mRes.isMap(idMap)) {
    MapT const& map = mRes.getMap(idMap);
    cx = map.mWidth;
    cy = map.mHeight;
  }
}

int getResSoundId(const char* name) const
{
  return getResIdByName(mRes.mSnd, name);
}

int getResSpriteId(const char* name) const
{
  return getResIdByName(mRes.mSprite, name);
}

int getResTexId(const char* name) const
{
  return getResIdByName(mRes.mTex, name);
}

void getResTexSize(int idTex, int &w, int &h) const
{
  if (mRes.isTex(idTex)) {
    ImgT img = getImage(mRes.getTex(idTex).mFileName);
    if (img.isValid()) {
      w = img.getWidth();
      h = img.getHeight();
    }
  }
}

int getResTileByPos(int idMap, int x, int y) const
{
  if (mRes.isMap(idMap)) {
    MapT const& map = mRes.getMap(idMap);
    if (0 <= x && 0 <= y &&
        map.mWidth * map.mTileset.mTileWidth > x &&
        map.mHeight * map.mTileset.mTileHeight > y) {
      int idx = (x / map.mTileset.mTileWidth) + map.mWidth * (y / map.mTileset.mTileHeight);
      return map.mData[idx];
    }
  }
  return 0;
}

void getResTileSize(int idMap, int &w, int &h) const
{
  if (mRes.isMap(idMap)) {
    MapT const& map = mRes.getMap(idMap);
    w = map.mTileset.mTileWidth;
    h = map.mTileset.mTileHeight;
  }
}

float getRot(int idObj) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    return a.mRot;
  } else {
    return .0f;
  }
}

void getScale(int idObj, float &xscale, float &yscale) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    xscale = a.mXscale;
    yscale = a.mYscale;
  }
}

int getSpriteId(int idObj) const
{
  int spr = -1;
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    if (ActorT::TYPES_SPRITE == a.mResType) {
      spr = a.mResId;
    }
  }
  return spr;
}

int getSound(int idRes)
{
  int idSnd = -1;
  if (mRes.isSnd(idRes)) {
    SoundT const& s = mRes.getSnd(idRes);
    if (s.mStream) {
      std::stringstream ss;
      if (loadFile(s.mFileName, ss)) {
        idSnd = SndT::getSound(s.mFileName, s.mStream, ss.str()).s;
        if (s.mLoop && s.mStream && -1 != idSnd) {
          SndT((int)idSnd).setLoop(true);
        }
      }
    }
  }
  return idSnd;
}

int getTexId(int idObj) const
{
  int tex = -1;
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    if (ActorT::TYPES_TEXBG == a.mResType) {
      tex = a.mResId;
    } else if (ActorT::TYPES_MAPBG == a.mResType) {
      MapT const& map = mRes.getMap(a.mResId);
      tex = map.mTileset.mTextureId;
    }
  }
  return tex;
}

int getType(int idObj) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    return a.mResType;
  } else {
    return 0;
  }
}

bool getVisible(int idObj) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    return a.mVisible;
  } else {
    return false;
  }
}

bool isAnimPlaying(int idObj) const
{
  if (mActors.isUsed(idObj)) {
    ActorT const& a = mActors[idObj];
    return a.mSpriteAnimator;
  } else {
    return false;
  }
}

void killAllChild(int idObj)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (a.mChild.empty()) {
    return;
  }

  for (int i = (int)a.mChild.size() - 1; i >= 0; i--) {
    int idChild = a.mChild[i];
    mActors[idChild].free();
  }

  mDirty = true;
}

void killObj(int idObj)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  mActors[idObj].free();
  mDirty = true;

  if (mRoot == idObj) {
    mRoot = -1;
  }
}

void pauseAnim(int idObj)
{
  if (mActors.isUsed(idObj)) {
    mActors[idObj].pauseAnim();
  }
}

void playAnim(int idObj)
{
  if (mActors.isUsed(idObj)) {
    mDirty = mActors[idObj].playAnim();
  }
}

void playPackage(const char* name)
{
  if (name) {
    mCallStack.push_back(name);         // Assume new package is at the same folder.
  }
}

int playSound(int idRes)
{
  int idSnd = -1;
  if (mRes.isSnd(idRes)) {
    SoundT const& s = mRes.getSnd(idRes);
    if (SndT::isExist(s.mFileName)) {
      std::string dummy;
      idSnd = SndT::getSound(s.mFileName, s.mStream, dummy).s;
    } else {
      std::stringstream ss;
      if (loadFile(s.mFileName, ss)) {
        idSnd = SndT::getSound(s.mFileName, s.mStream, ss.str()).s;
      }
    }
    if (-1 != idSnd) {
      if (s.mLoop && s.mStream) {
        SndT((int)idSnd).setLoop(true);
      }
      SndT((int)idSnd).play();
      if (!s.mStream) {
        idSnd = -1;
      }
    }
  }
  return idSnd;
}

int pickObj(int type, float x, float y, int idRes, int idResXor) const
{
  int hit = -1;
  if (-1 != mRoot) {
    ActorT const& lvl = mActors[mRoot];
    for (int i = lvl.getChildCount() - 1; 0 <= i; --i) {
      ActorT const& a = mActors[lvl.getChild(i)];
      hit = pickObj_i(a, type, x, y, idRes, idResXor);
      if (-1 != hit) {
        break;
      }
    }
  }
  return hit;
}

int pickObj_i(ActorT const& a, int type, float x, float y, int idRes, int idResXor) const
{
  if (0 < idRes && a.mResId != idRes) {
    return -1;
  }

  if (0 < idResXor && a.mResId == idResXor) {
    return -1;
  }

  sw2::IntRect rc;

  if (ActorT::TYPES_COLBG == a.mResType) {
    rc = sw2::IntRect(0, 0, a.mDim.width(), a.mDim.height());
  } else if (ActorT::TYPES_TEXBG == a.mResType) {
    int l, t, w, h;
    getDim_i(a, l, t, w, h);
    rc = sw2::IntRect(0, 0, w, h);
  } else if (ActorT::TYPES_SPRITE == a.mResType) {
    SpriteT const& spr = mRes.getSprite(a.mResId);
    rc = sw2::IntRect(0, 0, spr.mTileset.mTileWidth, spr.mTileset.mTileHeight);
    rc.offset(spr.mOffsetX, spr.mOffsetY);
  } else if (ActorT::TYPES_MAPBG == a.mResType) {
    MapT const& map = mRes.getMap(a.mResId);
    rc = sw2::IntRect(0, 0, map.mWidth * map.mTileset.mTileWidth, map.mHeight * map.mTileset.mTileHeight);
  } else if (ActorT::TYPES_DUMMY != a.mResType) {
    return -1;
  }

  if (ActorT::TYPES_DUMMY != a.mResType) {
    float ax, ay;
    a.getPos(ax, ay);
    rc.offset((int)ax, (int)ay);
    sw2::IntPoint pt((int)x, (int)y);
    if (!rc.ptInRect(pt)) {
      return -1;
    }
  }

  for (int i = a.getChildCount() - 1; 0 <= i; --i) {
    ActorT const& b = mActors[a.getChild(i)];
    int hit = pickObj_i(b, type, x, y, idRes, idResXor);
    if (-1 != hit) {
      return hit;
    }
  }

  if (ActorT::TYPES_DUMMY == a.mResType) {
    return -1;
  } else {
    return -1 != type && type != a.mResType ? -1 : a.mId;
  }
}

void setAnchor(int idObj, float newx, float newy)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (a.mAnchorX == newx && a.mAnchorY == newy) {
    return;
  }

  a.mAnchorX = newx;
  a.mAnchorY = newy;

  mDirty = true;
}

void setAntiAlias(bool bAntiAlias)
{
  mAntiAlias = bAntiAlias;
}

void setBgColor(int idObj, unsigned int newclr)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (a.mBgColor == newclr) {
    return;
  }

  a.mBgColor = newclr;

  mDirty = true;
}

void setDim(int idObj, int newx, int newy, int neww, int newh)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];

  sw2::IntRect rc(newx, newy, newx + neww, newy + newh);
  if (a.mDim == rc) {
    return;
  }

  a.mDim = rc;

  mDirty = true;
}

void setMapId(int idObj, int idMap)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (ActorT::TYPES_MAPBG != a.mResType || !mRes.isMap(idMap)) {
    return;
  }

  if (idMap == a.mResId) {
    return;
  }

  a.mResId = idMap;

  mDirty = true;
}

void setName(int idObj, const char* name)
{
  if (name) {
    if (!mActors.isUsed(idObj)) {
      return;
    }
    ActorT& a = mActors[idObj];
    a.mResName = name;
  }
}

void setPos(int idObj, float newx, float newy)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (a.mPosX == newx && a.mPosY == newy) {
    return;
  }

  a.mPosX = newx;
  a.mPosY = newy;

  mDirty = true;
}

void setRep(int idObj, bool bRepX, bool bRepY)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (a.mRepX == bRepX && a.mRepY == bRepY) {
    return;
  }

  a.mRepX = bRepX;
  a.mRepY = bRepY;

  mDirty = true;
}

void setRot(int idObj, float newRot)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  newRot = fmod(newRot , 360.0f);

  ActorT& a = mActors[idObj];
  if (a.mRot == newRot) {
    return;
  }

  a.mRot = newRot;

  mDirty = true;
}

void setScale(int idObj, float newxScale, float newyScale)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (a.mXscale == newxScale && a.mYscale == newyScale) {
    return;
  }

  a.mXscale = newxScale;
  a.mYscale = newyScale;

  mDirty = true;
}

void setScript(int idObj, const char* script)
{
  if (script) {
    if (!mActors.isUsed(idObj)) {
      return;
    }
    ActorT& a = mActors[idObj];
    a.setScript(script);
  }
}

void setSpriteId(int idObj, int idSpr)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (a.setSprite(idSpr)) {
    mDirty = true;
  }
}

void setTexId(int idObj, int idTex)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (ActorT::TYPES_TEXBG != a.mResType || !mRes.isTex(idTex)) {
    return;
  }

  if (idTex == a.mResId) {
    return;
  }

  a.mResId = idTex;
  a.mImg = getImage(mRes.getTex(idTex).mFileName); // Update image cache.

  mDirty = true;
}

void setVisible(int idObj, bool bVisible)
{
  if (!mActors.isUsed(idObj)) {
    return;
  }

  ActorT& a = mActors[idObj];
  if (a.mVisible != bVisible) {
    a.mVisible = bVisible;
    mDirty = true;
  }
}

void stopAnim(int idObj)
{
  if (mActors.isUsed(idObj)) {
    mActors[idObj].stopAnim();
  }
}

// end of api.h
