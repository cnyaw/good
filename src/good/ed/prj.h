
//
// prj.h
// Editor, project manager.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/11/29 Waync created.
//

#pragma once

namespace good {

namespace ed {

template<class PrjT>
class Traits
{
public:
  typedef Sound<PrjT> SoundT;
  typedef Texture<PrjT> TextureT;
  typedef Map<PrjT> MapT;
  typedef Sprite<PrjT> SpriteT;
  typedef Object<PrjT> ObjectT;
  typedef Level<PrjT> LevelT;
};

template<class ImgT>
class Project
{
private:

  Project() : mModified(false)
  {
  }

public:

  enum { UNDO_LEVEL = INT_MAX };

  typedef Traits<Project<ImgT> > TraitsT;
  typedef good::Resource<TraitsT> ResT;
  typedef typename TraitsT::SoundT SoundT;
  typedef typename TraitsT::TextureT TextureT;
  typedef typename TraitsT::MapT MapT;
  typedef typename TraitsT::SpriteT SpriteT;
  typedef typename TraitsT::ObjectT ObjectT;
  typedef typename TraitsT::LevelT LevelT;

  ResT mRes;
  mutable bool mModified;               // Is mRes modified?

  //
  // Singleton.
  //

  static Project& inst()
  {
    static Project i;
    return i;
  }

  //
  // Modify property.
  //

  bool setName(std::string const& name)
  {
    if (name == mRes.mName) {
      return false;
    }

    mRes.mName = name;

    mModified = true;

    return true;
  }

  bool setWindowSettings(int w, int h)
  {
    if (w == mRes.mWidth && h == mRes.mHeight) {
      return false;
    }

    mRes.mWidth = w;
    mRes.mHeight = h;

    mModified = true;

    return true;
  }

  //
  // Get res opt.
  //

#define IMPL_GET_RESOURCE_ITEM(ResName, ResType) \
  ResType& get ## ResName(int id) \
  { \
    return mRes.get ## ResName(id); \
  } \
  ResType const& get ## ResName(int id) const \
  { \
    return mRes.get ## ResName(id); \
  }

#define IMPL_GET_STRING_RESOURCE_ITEM(ResName) \
  std::string get ## ResName(int id) const \
  { \
    return mRes.get ## ResName(id); \
  }

  IMPL_GET_RESOURCE_ITEM(Snd, SoundT)
  IMPL_GET_RESOURCE_ITEM(Tex, TextureT)
  IMPL_GET_RESOURCE_ITEM(Map, MapT)
  IMPL_GET_RESOURCE_ITEM(Sprite, SpriteT)
  IMPL_GET_RESOURCE_ITEM(Level, LevelT)

  IMPL_GET_STRING_RESOURCE_ITEM(Script)
  IMPL_GET_STRING_RESOURCE_ITEM(StgeScript)
  IMPL_GET_STRING_RESOURCE_ITEM(Dep)

#undef IMPL_GET_RESOURCE_ITEM
#undef IMPL_GET_STRING_RESOURCE_ITEM

  //
  // Load override.
  //

  bool load(std::string const& name)
  {
    std::ifstream ifs(name.c_str());
    if (!ifs) {
      return false;
    }

    if (!load(ifs)) {
      return false;
    }

    mRes.mFileName = name;

    return true;
  }

  bool load(std::istream& ins)
  {
    ResT res;
    if (!res.load(ins)) {
      return false;
    }

    closeAll();

    mRes = res;

    return true;
  }

  //
  // Release all resource.
  //

  void closeAll()
  {
    removeAllResource(mRes.mSnd, mRes.mSndIdx);
    removeAllResource(mRes.mTex, mRes.mTexIdx);
    removeAllResource(mRes.mMap, mRes.mMapIdx);
    removeAllResource(mRes.mSprite, mRes.mSpriteIdx);
    removeAllResource(mRes.mLevel, mRes.mLevelIdx);
    removeAllResource(mRes.mScript, mRes.mScriptIdx);
    removeAllResource(mRes.mStgeScript, mRes.mStgeScriptIdx);
    removeAllResource(mRes.mDep, mRes.mDepIdx);

    mModified = false;

    mRes.mName.clear();
    mRes.mFileName.clear();

    mRes.mId.clear();

    mRes.mWidth = 640;
    mRes.mHeight = 480;
    mRes.mFps = GOOD_DEFAULT_TICK_PER_SECOND;
  }

  //
  // Snd opt.
  //

  int addSnd(std::string const& fileName)
  {
    //
    // Return id; -1 failed.
    //

    if (fileName.empty()) {
      return -1;
    }

    int id = mRes.mId.alloc();
    assert(-1 != id);

    SoundT snd;
    snd.mId = id;
    snd.mFileName = fileName;
    snd.mLoop = false;
    snd.mStream = false;

    mRes.mSndIdx.push_back(id);
    mRes.mSnd[id] = snd;

    mModified = true;

    return id;
  }

  bool removeSnd(int id)
  {
    return removeResource(mRes.mSnd, mRes.mSndIdx, id);
  }

  //
  // Tex opt.
  //

  int addTex(std::string const& fileName)
  {
    //
    // Return id; -1 failed.
    //

    ImgT img = ImgT::getImage(fileName);
    if (!img.isValid()) {
      SW2_TRACE_ERROR("Add texture failed, fail to get image %s", fileName.c_str());
      return -1;
    }

    int id = mRes.mId.alloc();
    assert(-1 != id);

    TextureT t;
    t.mFileName = fileName;
    t.mId = id;

    mRes.mTexIdx.push_back(id);
    mRes.mTex[id] = t;

    mModified = true;

    return id;
  }

  bool isTexUsed(int id, std::string &lvlName, std::string &oName) const
  {
    if (isTexUsed_i(id, mRes.mMap, oName) || isTexUsed_i(id, mRes.mSprite, oName)) {
      return true;
    }
    typename std::map<int, LevelT>::const_iterator it = mRes.mLevel.begin();
    for (; mRes.mLevel.end() != it; ++it) {
      const LevelT &lvl = it->second;
      if (isTexUsed_i(id, lvl, lvlName, oName)) {
        return true;
      }
    }
    return false;
  }

  template<class ResMapT>
  bool isTexUsed_i(int id, const ResMapT &map, std::string &oName) const
  {
    for (typename ResMapT::const_iterator it = map.begin(); map.end() != it; ++it) {
      if (it->second.mTileset.mTextureId == id) {
        oName = it->second.getName();
        return true;
      }
    }
    return false;
  }

  bool isTexUsed_i(int id, const LevelT &lvl, std::string &lvlName, std::string &oName) const
  {
    typename std::map<int, ObjectT>::const_iterator it = lvl.mObj.begin();
    for (; lvl.mObj.end() != it; ++it) {
      if (it->second.mTextureId == id) {
        lvlName = lvl.getName();
        oName = it->second.getName();
        return true;
      }
    }
    return false;
  }

  bool removeTex(int id)
  {
    return removeResource(mRes.mTex, mRes.mTexIdx, id);
  }

  //
  // Map opt.
  //

  int addMap(std::string const& name, int width, int height, int tileWidth, int tileHeight, int texture)
  {
    //
    // Return id; -1 failed.
    //

    ImgT img = ImgT::getImage(getTex(texture).mFileName);
    if (!img.isValid()) {
      SW2_TRACE_ERROR("Add map failed, invalid texture id %d", texture);
      return -1;
    }

    MapT m;
    m.mName = name;
    m.mTileset.mTextureId = texture;
    m.mWidth = width;
    m.mHeight = height;
    m.mTileset.mTileWidth = tileWidth;
    m.mTileset.mTileHeight = tileHeight;

    m.mTileset.mCxTile = img.getWidth() / tileWidth;
    m.mTileset.mCyTile = img.getHeight() / tileHeight;

    if (0 == m.mTileset.mCxTile || 0 == m.mTileset.mCyTile) {
      SW2_TRACE_ERROR("Add map failed, texture is too small to create tile");
      return -1;
    }

    int id = mRes.mId.alloc();
    assert(-1 != id);

    m.mId = id;

    mRes.mMap[m.mId] = m;
    mRes.mMapIdx.push_back(m.mId);

    MapT& m2 = mRes.mMap[m.mId];
    m2.mData.resize(m2.mWidth * m2.mHeight);
    m2.mData.assign(m2.mWidth * m2.mHeight, 0); // Set empty.

    mModified = true;

    return id;
  }

  bool isMapUsed(int id, std::string &lvlName, std::string &oName) const
  {
    typename std::map<int, LevelT>::const_iterator it = mRes.mLevel.begin();
    for (; mRes.mLevel.end() != it; ++it) {
      const LevelT &lvl = it->second;
      if (isMapUsed_i(id, lvl, lvlName, oName)) {
        return true;
      }
    }
    return false;
  }

  bool isMapUsed_i(int id, const LevelT &lvl, std::string &lvlName, std::string &oName) const
  {
    typename std::map<int, ObjectT>::const_iterator it = lvl.mObj.begin();
    for (; lvl.mObj.end() != it; ++it) {
      if (it->second.mMapId == id) {
        lvlName = lvl.getName();
        oName = it->second.getName();
        return true;
      }
    }
    return false;
  }

  bool removeMap(int id)
  {
    return removeResource(mRes.mMap, mRes.mMapIdx, id);
  }

  //
  // Sprite opt.
  //

  int addSprite(std::string const& name, int width, int height, int texture)
  {
    //
    // Return id; -1 failed.
    //

    ImgT img = ImgT::getImage(getTex(texture).mFileName);
    if (!img.isValid()) {
      SW2_TRACE_ERROR("Add sprite failed, invalid texture id %d", texture);
      return -1;
    }

    SpriteT spr;
    spr.mName = name;
    spr.mTileset.mTextureId = texture;
    spr.mTileset.mTileWidth = width;
    spr.mTileset.mTileHeight = height;
    spr.mLoop = true;
    spr.mOffsetX = spr.mOffsetY = 0;

    spr.mTileset.mCxTile = img.getWidth() / width;
    spr.mTileset.mCyTile = img.getHeight() / height;

    if (0 == spr.mTileset.mCxTile || 0 == spr.mTileset.mCyTile) {
      SW2_TRACE_ERROR("Add sprite failed, texture is too small to create tile");
      return -1;
    }

    int id = mRes.mId.alloc();
    assert(-1 != id);

    spr.mId = id;

    mRes.mSprite[spr.mId] = spr;
    mRes.mSpriteIdx.push_back(spr.mId);

    mModified = true;

    return id;
  }

  bool isSpriteUsed(int id, std::string &lvlName, std::string &oName) const
  {
    typename std::map<int, LevelT>::const_iterator it = mRes.mLevel.begin();
    for (; mRes.mLevel.end() != it; ++it) {
      const LevelT &lvl = it->second;
      if (isSpriteUsed_i(id, lvl, lvlName, oName)) {
        return true;
      }
    }
    return false;
  }

  bool isSpriteUsed_i(int id, const LevelT &lvl, std::string &lvlName, std::string &oName) const
  {
    typename std::map<int, ObjectT>::const_iterator it = lvl.mObj.begin();
    for (; lvl.mObj.end() != it; ++it) {
      if (it->second.mSpriteId == id) {
        lvlName = lvl.getName();
        oName = it->second.getName();
        return true;
      }
    }
    return false;
  }

  bool removeSprite(int id)
  {
    return removeResource(mRes.mSprite, mRes.mSpriteIdx, id);
  }

  //
  // Level opt.
  //

  int addLevel(std::string const& name, int width, int height)
  {
    int id = mRes.mId.alloc();
    assert(-1 != id);

    LevelT lvl;
    lvl.mType = -1;
    lvl.mId = id;
    lvl.mName = name;
    lvl.mWidth = width;
    lvl.mHeight = height;
    lvl.mBgColor = 0xffffffff;
    lvl.mVisible = true;
    lvl.mSpriteId = lvl.mMapId = lvl.mTextureId = -1;
    lvl.mPosX = lvl.mPosY = 0;
    lvl.mRepX = lvl.mRepY = false;
    lvl.mRot = .0f;
    lvl.mScaleX = lvl.mScaleY = 1.0f;
    lvl.mAnchorX = lvl.mAnchorY = .0f;

    mRes.mLevel[lvl.mId] = lvl;
    mRes.mLevelIdx.push_back(lvl.mId);

    mModified = true;

    return id;
  }

  int addLevelObj(int idLvl, int x, int y)
  {
    LevelT& lvl = getLevel(idLvl);
    int idObj = lvl.addObj(idLvl, lvl.mAddSpr, lvl.mAddMap, lvl.mAddTex, x, y);
    if (-1 == idObj) {
      return -1;
    }

    if (lvl.isAddColorTool()) {
      lvl.getObj(idObj).setBgColor(lvl.mAddCol); // Set default color.
    }

    if (lvl.isAddTexTool()) {
      ImgT img = ImgT::getImage(getTex(lvl.mAddTex).mFileName);
      lvl.getObj(idObj).setDim(0, 0, img.getWidth(), img.getHeight()); // Set default dim.
    }

    return idObj;
  }

  void getObjDim(ObjectT const& obj, sw2::IntRect &rc)
  {
    switch (obj.mType)
    {
    case ObjectT::TYPE_MAPBG:
      {
        //
        // Background is a tileset.
        //

        MapT const& map = getMap(obj.mMapId);
        rc = sw2::IntRect(0, 0, map.mWidth * map.mTileset.mTileWidth, map.mHeight * map.mTileset.mTileHeight);
      }
      break;

    case ObjectT::TYPE_TEXBG:
      {
        //
        // Background is an image.
        //

        assert(!obj.mDim.empty());
        rc = sw2::IntRect(0, 0, obj.mDim.width(), obj.mDim.height());
      }
      break;

    case ObjectT::TYPE_COLBG:
      {
        //
        // Background is a solid color block.
        //

        if (obj.mDim.empty()) {
          rc = sw2::IntRect(0, 0, 32, 32);
        } else {
          rc = sw2::IntRect(0, 0, obj.mDim.width(), obj.mDim.height());
        }
      }
      break;

    case ObjectT::TYPE_SPRITE:
      {
        SpriteT const& spr = getSprite(obj.mSpriteId);
        rc = sw2::IntRect(0, 0, spr.mTileset.mTileWidth, spr.mTileset.mTileHeight);
      }
      break;

    default:                            // Unknown type.
      return;
    }

    rc.offset(obj.mPosX, obj.mPosY);
  }

  bool isLevelObjUsed(const std::vector<int> &v, std::string &lvlName, std::string &oName) const
  {
    for (size_t i = 0; i < v.size(); i++) {
      if (isLevelObjUsed(v[i], lvlName, oName)) {
        return true;
      }
    }
    return false;
  }

  bool isLevelObjUsed(int id, std::string &lvlName, std::string &oName) const
  {
    typename std::map<int, LevelT>::const_iterator it = mRes.mLevel.begin();
    for (; mRes.mLevel.end() != it; ++it) {
      const LevelT &lvl = it->second;
      if (isLevelObjUsed_i(id, lvl, lvlName, oName)) {
        return true;
      }
    }
    return false;
  }

  bool isLevelObjUsed_i(int id, const LevelT &lvl, std::string &lvlName, std::string &oName) const
  {
    typename std::map<int, ObjectT>::const_iterator it = lvl.mObj.begin();
    for (; lvl.mObj.end() != it; ++it) {
      if (it->second.getLevelObjId() == id) {
        lvlName = lvl.getName();
        oName = it->second.getName();
        return true;
      }
    }
    return false;
  }

  bool removeLevel(int id)
  {
    return removeResource(mRes.mLevel, mRes.mLevelIdx, id);
  }

  bool removeLevelObj(int idLvl, const std::vector<int> &ids)
  {
    return getLevel(idLvl).removeObj(ids);
  }

  //
  // Script opt.
  //

  int addScript(std::string const& name)
  {
    if (name.empty()) {
      return -1;
    }

    if (isPathExist_i(mRes.mScript, name)) {
      return -1;
    }

    int id = mRes.mId.alloc();
    assert(-1 != id);

    mRes.mScriptIdx.push_back(id);
    mRes.mScript[id] = name;

    mModified = true;

    return id;
  }

  bool isPathExist_i(const std::map<int, std::string> &m, const std::string &n) const
  {
    for (std::map<int, std::string>::const_iterator it = m.begin(); m.end() != it; ++it) {
      if (it->second == n) {
        SW2_TRACE_ERROR("'%s' is already added.", n.c_str());
        return true;
      }
    }
    return false;
  }

  bool removeScript(int id)
  {
    return removeResource(mRes.mScript, mRes.mScriptIdx, id);
  }

  //
  // STGE script opt.
  //

  int addStgeScript(std::string const& name)
  {
    if (name.empty()) {
      return -1;
    }

    if (isPathExist_i(mRes.mStgeScript, name)) {
      return -1;
    }

    int id = mRes.mId.alloc();
    assert(-1 != id);

    mRes.mStgeScriptIdx.push_back(id);
    mRes.mStgeScript[id] = name;

    mModified = true;

    return id;
  }

  bool removeStgeScript(int id)
  {
    return removeResource(mRes.mStgeScript, mRes.mStgeScriptIdx, id);
  }

  //
  // Depenedecy opt.
  //

  int addDep(std::string const& name)
  {
    if (name.empty()) {
      return -1;
    }

    if (isPathExist_i(mRes.mDep, name)) {
      return -1;
    }

    int id = mRes.mId.alloc();
    assert(-1 != id);

    mRes.mDepIdx.push_back(id);
    mRes.mDep[id] = name;

    mModified = true;

    return id;
  }

  bool removeDep(int id)
  {
    return removeResource(mRes.mDep, mRes.mDepIdx, id);
  }

  //
  // Change order.
  //

  template<class V, class M>
  bool insertAfter(V& v, M const& m, int id, int idCur)
  {
    //
    // Same type?
    //

    if (-1 != idCur && m.end() == m.find(idCur)) {
      return false;
    }

    //
    // No change?
    //

    if (-1 == idCur && id == v[0]) {
      return false;
    }

    //
    // Remove prev.
    //

    for (size_t i = 0; i < v.size(); ++i) {
      if (v[i] == id) {
        if (0 < i && v[i - 1] == idCur) { // No change?
          return false;
        }
        v.erase(v.begin() + i);
        break;
      }
    }

    //
    // Insert.
    //

    if (-1 != idCur) {
      for (size_t i = 0; i < v.size(); ++i) {
        if (v[i] == idCur) {
          v.insert(v.begin() + i + 1, id);
          break;
        }
      }
    } else {
      v.insert(v.begin(), id);
    }

    mModified = true;

    return true;
  }

  bool insertAfter(int id, int idCur)
  {
    //
    // Insert id after idCur(-1 means insert 1st).
    //

    if (id == idCur) {
      return false;
    }

    if (mRes.isSnd(id)) {
      return insertAfter(mRes.mSndIdx, mRes.mSnd, id, idCur);
    } else if (mRes.isTex(id)) {
      return insertAfter(mRes.mTexIdx, mRes.mTex, id, idCur);
    } else if (mRes.isMap(id)) {
      return insertAfter(mRes.mMapIdx, mRes.mMap, id, idCur);
    } else if (mRes.isSprite(id)) {
      return insertAfter(mRes.mSpriteIdx, mRes.mSprite, id, idCur);
    } else if (mRes.isLevel(id)) {
      return insertAfter(mRes.mLevelIdx, mRes.mLevel, id, idCur);
    } else if (mRes.isScript(id)) {
      return insertAfter(mRes.mScriptIdx, mRes.mScript, id, idCur);
    } else if (mRes.isStgeScript(id)) {
      return insertAfter(mRes.mStgeScriptIdx, mRes.mStgeScript, id, idCur);
    } else if (mRes.isDep(id)) {
      return insertAfter(mRes.mDepIdx, mRes.mDep, id, idCur);
    }

    return false;
  }

  //
  // Store all.
  //

  bool canStore() const
  {
    //
    // Is modified?
    //

    return mModified;
  }

  bool store(std::string const& name) const
  {
    std::ofstream ofs(name.c_str());
    if (ofs) {
      return store(ofs);
    }

    return false;
  }

  bool store(std::ostream& outs) const
  {
    sw2::Ini ini;
    sw2::Ini& secPrj = ini["good"];     // Insert.

    //
    // Property.
    //

    secPrj["version"] = GOOD_VERSION;

    if (!mRes.mName.empty()) {
      secPrj["name"] = mRes.mName;
    }

    //
    // Settings.
    //

    if (640 != mRes.mWidth || 480 != mRes.mHeight) {
      std::vector<int> v;
      v.push_back(mRes.mWidth);
      v.push_back(mRes.mHeight);
      secPrj["window"] = intVecToStr(v);
    }

    if (GOOD_DEFAULT_TICK_PER_SECOND != mRes.mFps) {
      secPrj["fps"] = mRes.mFps;
    }

    //
    // Scripts.
    //

    if (!storeStringTableResource(mRes.mScript, mRes.mScriptIdx, "scripts", ini)) {
      return false;
    }

    //
    // STGE scripts.
    //

    if (!storeStringTableResource(mRes.mStgeScript, mRes.mStgeScriptIdx, "stges", ini)) {
      return false;
    }

    //
    // Dependencies.
    //

    if (!storeStringTableResource(mRes.mDep, mRes.mDepIdx, "deps", ini)) {
      return false;
    }

    //
    // Sounds.
    //

    if (!storeResources(mRes.mSnd, mRes.mSndIdx, "snds", ini["good"], ini)) {
      return false;
    }

    //
    // Textures.
    //

    if (!storeResources(mRes.mTex, mRes.mTexIdx, "texs", ini["good"], ini)) {
      return false;
    }

    //
    // Maps.
    //

    if (!storeResources(mRes.mMap, mRes.mMapIdx, "maps", ini["good"], ini)) {
      return false;
    }

    //
    // Sprites.
    //

    if (!storeResources(mRes.mSprite, mRes.mSpriteIdx, "sprites", ini["good"], ini)) {
      return false;
    }

    //
    // Levels.
    //

    if (!storeResources(mRes.mLevel, mRes.mLevelIdx, "levels", ini["good"], ini)) {
      return false;
    }

    if (!ini.store(outs)) {
      return false;
    }

    mModified = false;

    return true;
  }

  //
  // Helper.
  //

  template<class T, class V>
  void removeAllResource(T& t, V& v)
  {
    for (size_t i = 0; i < v.size(); ++i) {
      t[v[i]].clear();
    }

    t.clear();
    v.clear();
  }

  template<class T, class V>
  bool removeResource(T& t, V& v, int id)
  {
    typename T::iterator it = t.find(id);
    if (t.end() == it) {
      return false;
    }

    it->second.clear();
    t.erase(it);

    for (size_t i = 0; i < v.size(); ++i) {
      if (id == v[i]) {
        v.erase(v.begin() + i);
        break;
      }
    }

    mRes.mId.free(id);

    mModified = true;

    return true;
  }

  template<class T>
  bool rename(T& t, std::string const& name)
  {
    if (name == t.getName() || name == t.mName) {
      return false;
    }

    t.mName = name;

    mModified = true;

    return true;
  }

  //
  // Create package.
  //

  void createPackageAddItems(std::map<std::string, int>& additems, std::string const& itemname) const
  {
    if (additems.end() == additems.find(itemname)) {
      additems[itemname] = 1;
    }
  }

  bool createPackageCheckDependency(std::map<std::string, int>& addedprj, std::string const& path, std::string const& prjname, std::map<std::string, int>& additems) const
  {
    if (addedprj.end() != addedprj.find(prjname)) {
      return true;
    }

    ResT res;
    if (!res.load(path + prjname)) {
      return false;
    }

    //
    // Add project.
    //

    createPackageAddItems(additems, prjname);

    addedprj[prjname] = 1;              // Save as added prj.

    //
    // Gather general resource items.
    //

    for (typename std::map<int, SoundT>::const_iterator it = res.mSnd.begin(); res.mSnd.end() != it; ++it) {
      createPackageAddItems(additems, it->second.mFileName);
    }

    for (typename std::map<int, TextureT>::const_iterator it = res.mTex.begin(); res.mTex.end() != it; ++it) {
      createPackageAddItems(additems, it->second.mFileName);
    }

    for (std::map<int, std::string>::const_iterator it = res.mStgeScript.begin(); res.mStgeScript.end() != it; ++it) {
      createPackageAddItems(additems, it->second);
    }

    for (std::map<int, std::string>::const_iterator it = res.mScript.begin(); res.mScript.end() != it; ++it) {
      createPackageAddItems(additems, it->second);
    }

    //
    // Check dependencies.
    //

    for (std::map<int, std::string>::const_iterator it = res.mDep.begin(); res.mDep.end() != it; ++it) {
      std::string tmp = it->second;
      sw2::Util::toLowerString(tmp);
      if (isGoodArchive(tmp) || '/' == *tmp.rbegin()) { // Skip package files and search path.
        continue;
      }
      if (!createPackageCheckDependency(addedprj, path, it->second, additems)) {
        return false;
      }
    }

    return true;
  }

  bool createPackage(std::ostream& os, bool encrypt) const
  {
    std::map<std::string, int> additems;
    std::map<std::string, int> addedprj;

    //
    // Add project item.
    //

    std::string path(mRes.mFileName);
    sw2::Util::trim(path);
    std::replace(path.begin(), path.end(), '\\', '/');

    std::string prjname = path.substr(path.find_last_of('/') + 1);

    path = path.substr(0, path.find_last_of('/') + 1);

    if ('/' != path[path.length() - 1]) {
      path.push_back('/');
    }

    if (0 == path.compare(0, 2, "./")) { // Start with "./".
      path.erase(0, 2);
    }

    //
    // Add entry point.
    //

    std::ofstream ofs(GOOD_PACKAGE_ENTRY);
    if (!ofs) {
      return false;
    }

    ofs << prjname;
    ofs.close();

    additems[GOOD_PACKAGE_ENTRY] = 1;

    //
    // Gather items.
    //

    if (!createPackageCheckDependency(addedprj, path, prjname, additems)) {
      return false;
    }

    std::vector<std::string> resitems;

    for (std::map<std::string, int>::const_iterator it = additems.begin(); additems.end() != it; ++it) {
      std::string fname = path + it->first;
      FILE *fItem = fopen(fname.c_str(), "r");
      if (fItem) {
        resitems.push_back(it->first);
        fclose(fItem);
      } else {
        SW2_TRACE_WARNING("Resource item '%s' not found!", fname.c_str());
      }
    }

    //
    // Create zip archive.
    //

    std::stringstream ss;
    if (!sw2::Util::zipStream(path, ss, os, resitems, encrypt ? GOOD_PACKAGE_PASSWORD : "")) {
      return false;
    }

    remove(GOOD_PACKAGE_ENTRY);

    return true;
  }
};

} // namespace ed

} // namespace good

// end of prj.h
