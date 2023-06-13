
//
// app.h
// Runtime app base.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/03/01 Waync created.
//

#pragma once

#ifndef GOOD_SUPPORT_NO_LOGO
#include "../logo/logo.c"
#endif

namespace good {

namespace rt {

template<class T, class ImgT, class SndT, class CanvasT>
class Application
{
public:

  typedef good::Resource<> ResT;
  typedef ResT::SoundT SoundT;
  typedef ResT::TextureT TextureT;
  typedef ResT::MapT MapT;
  typedef ResT::SpriteT SpriteT;
  typedef ResT::ObjectT ObjectT;
  typedef ResT::LevelT LevelT;

  typedef Actor<T, ImgT> ActorT;

  Application() : mRoot(-1), mStartLevel(-1), mAntiAlias(true), mSelFont(GOOD_DRAW_TEXT_SYSTEM_FONT)
  {
  }

  //
  // Flag to exit current running package.
  //

  bool mExit;

  //
  // Key state.
  //

  sw2::KeyStates mKeys;
  int mHandledKeys;
  sw2::IntPoint mMousePos;

  //
  // Lua state.
  //

  lua_State *mLua;

  //
  // Res.
  //

  ResT mRes;                            // Good resource.

  std::map<std::string, std::map<int, int> > mExternalResMap; // <PkgName, <ExtResId, TmpResId>>, External package res id mapping.

  //
  // Actor.
  //

  sw2::ObjectPool<ActorT, 32, true> mActors;

  int mRoot;
  int mStartLevel;                      // Starting level id, otherwise use first level.
  bool mCreateRoot;                     // Later create level object.

  //
  // Share dirty flag, modify by script.
  //

  mutable bool mDirty;
  mutable bool mTexDirty;

  //
  // Flag for Graphics API.
  //

  mutable bool mRenderState;

  //
  // File system.
  //

  sw2::Archive *mAr;
  std::map<std::string, int> mFileSys;  // Loaded file system name.
  std::map<std::string, std::string> mPkgPrjMap; // <PkgName, PrjName>, proj name of package mapping.

  //
  // Call stack.
  //

  std::vector<std::string> mPkgCallStack; // Package call stack.
  std::string mNextPlayPkg;             // Next package to play.

  //
  // Canvas.
  //

  sw2::ObjectPool<CanvasT, 8> mCanvas;

  //
  // Overridable.
  //

#include "override.h"

  //
  // STGE.
  //

#ifdef GOOD_SUPPORT_STGE
#include "stge.h"
#endif

  //
  // Render.
  //

#include "render.h"

  //
  // Script API.
  //

#include "api.h"

  //
  // Init.
  //

  std::string decidePrjName(const std::string &name)
  {
    if (isGoodArchive(name)) {
      std::map<std::string, std::string>::const_iterator it = mPkgPrjMap.find(name);
      if (mPkgPrjMap.end() != it) {
        return it->second;
      } else if (mAr->isFileExist(GOOD_PACKAGE_ENTRY)) {
        std::stringstream ss;
        if (!loadFile(GOOD_PACKAGE_ENTRY, ss)) {
          trace("get entry point failed");
          return "";
        } else {
          std::string PrjName = ss.str();
          mPkgPrjMap[name] = PrjName;
          return PrjName;
        }
      } else {
        std::string PrjName = name.substr(0, name.find_last_of('.') + 1) + "txt";
        mPkgPrjMap[name] = PrjName;
        return PrjName;
      }
    }
    return name;
  }

  bool init(std::string const& pathname)
  {
    if (!allocAr()) {
      return false;
    }

#ifndef GOOD_SUPPORT_NO_LOGO
    if (addAndPlayLogoFileSystem(pathname)) {
      return true;
    }
#endif

    std::string ResName(pathname);
    std::replace(ResName.begin(), ResName.end(), '\\', '/');

    std::string path = ResName.substr(0, ResName.find_last_of('/') + 1);
    if (path.empty()) {
      path = "./";
    }

    do
    {
      if (!addPathFileSystem(path)) {
        break;
      }
      std::string name = ResName.substr(ResName.find_last_of('/') + 1);
      if (isGoodArchive(name) && !name.empty() && mFileSys.end() == mFileSys.find(name)) {
        std::stringstream ss;
        if (!loadFile(name, ss) || !addStreamFileSystem(name, ss)) {
          break;
        }
      }
      std::string prjname = decidePrjName(name);
      return init_i(prjname);
    } while (0);

    uninit_i();
    return false;
  }

  bool initFromStream(std::istream& stream)
  {
    if (!allocAr()) {
      return false;
    }

    do
    {
      if (!addStreamFileSystem("", stream)) {
        break;
      }
      std::stringstream ss;
      if (!loadFile(GOOD_PACKAGE_ENTRY, ss)) {
        trace("entry point not found");
        break;
      }
      std::string prjname = ss.str();
#ifndef GOOD_SUPPORT_NO_LOGO
      if (addAndPlayLogoFileSystem(prjname)) {
        return true;
      }
#endif
      return init_i(prjname);
    } while (0);

    uninit_i();
    return false;
  }

  bool init_i(std::string const& prjname)
  {
    do
    {
      std::stringstream ss;
      if (!loadFile(prjname, ss)) {
        trace("load file archive [%s] failed", prjname.c_str());
        break;
      }

      if (!mRes.load(ss)) {
        trace("load resource [%s] failed", prjname.c_str());
        break;
      }

      if (!initDepex() || !initScript()) {
        break;
      }

#ifdef GOOD_SUPPORT_STGE
      initStge();
#endif

      mRes.mFileName = prjname;
      mExternalResMap.clear();

      if (!static_cast<T*>(this)->doInit(prjname)) {
        break;
      }

      mExit = false;
      mKeys.reset();

      mCreateRoot = true;               // Later create level object.

      static_cast<T*>(this)->onPackageChanged(); // Notify package has change.
      mDirty = true;
      mTexDirty = false;

      return true;

    } while (0);

    uninit_i();
    return false;
  }

  bool initDepex()
  {
    for (size_t i = 0; i < mRes.mDepIdx.size(); i++) {

      std::string depname = mRes.mDep[mRes.mDepIdx[i]];
      sw2::Util::toLowerString(depname);

      //
      // Skip non package files and non search path.
      //

      if (!isGoodArchive(depname) && '/' != *depname.rbegin()) {
        continue;
      }

      //
      // Add new file system.
      //

      if (!addPathFileSystem(depname)) {
        trace("add depex file system '%s' failed", depname.c_str());
        return false;
      }
    }

    return true;
  }

  bool initScript()
  {
    mLua = lua_open();

    if (0 == mLua) {
      trace("open lua state failed");
      return false;
    }

    luaL_openlibs(mLua);

    if (!LuaScript<T, SndT>::regAPI(mLua)) {
      uninitScript();
      return false;
    }

    for (size_t i = 0; i < mRes.mScriptIdx.size(); ++i) {
      std::string name = mRes.mScript[mRes.mScriptIdx[i]];
      (void)loadLuaScript(name);
    }

    return true;
  }

  void uninitScript()
  {
    if (mLua) {
      lua_close(mLua);
      mLua = 0;
    }
  }

  void uninit()
  {
    uninit_i();

    if (mAr) {
      sw2::Archive::free(mAr);
      mAr = 0;
    }

    mFileSys.clear();
    mPkgPrjMap.clear();
    mExternalResMap.clear();
  }

  void uninit_i()
  {
    static_cast<T*>(this)->doUninit();

    uninitScript();
#ifdef GOOD_SUPPORT_STGE
    uninitStge();
#endif

    mActors.clear();

    for (int i = mCanvas.first(); -1 != i; i = mCanvas.next(i)) {
      mCanvas[i].release();
    }
    mCanvas.clear();

    mRoot = -1;
  }

  void trace(const char* format, ...)
  {
    char buf[1024];

    va_list va;
    va_start(va, format);
    vsnprintf(buf, sizeof(buf), format, va);
    va_end(va);

    static_cast<T*>(this)->doTrace(buf);

    mDirty = true;                      // Force redraw.
  }

  void playPackage_i(std::string const& name)
  {
    uninit_i();
    init(name);
  }

  void exitPackage()
  {
    if (!mPkgCallStack.empty()) {
      mNextPlayPkg = mPkgCallStack.back();
      mPkgCallStack.erase(mPkgCallStack.begin() + (mPkgCallStack.size() - 1));
    } else {
      mExit = true;
    }
  }

  //
  // Trigger logic.
  //

  bool trigger(int keys, sw2::IntPoint const& mousePos)
  {
    mKeys.update(keys);
    mHandledKeys = 0;

    mMousePos = mousePos;

    mRenderState = false;

    //
    // Later create level object.
    //

    if (mCreateRoot) {
      mCreateRoot = false;
      if (!mRes.mLevelIdx.empty()) {
        if (GOOD_LOGO_NAME != mRes.mName && -1 != mStartLevel) {
          mRoot = genObj(-1, mStartLevel, 0);
          mStartLevel = -1;
        } else {
          mRoot = genObj(-1, mRes.mLevelIdx[0], 0);
        }
      } else {
        mRoot = -1;
      }
    }

    //
    // Play next package.
    //

    if (!mNextPlayPkg.empty()) {
      std::string name(mNextPlayPkg);
      mNextPlayPkg.clear();
      playPackage_i(name);
    }

    //
    // Trigger objects.
    //

    if (mActors.isUsed(mRoot)) {

      //
      // Update STGE.
      //

#ifdef GOOD_SUPPORT_STGE
      if (!triggerStge()) {
        return false;
      }
#endif

      //
      // Update game.
      //

      mDirty = ActorT::animate(mRoot) || mDirty;
    }

    //
    // Update active package stack.
    //

    if (isKeyPressed(GOOD_KEYS_ESCAPE) && 0 == (mHandledKeys & GOOD_KEYS_ESCAPE)) {
      exitPackage();
    }

    static_cast<T*>(this)->onTrigger();

    return mDirty;
  }

  //
  // Create actor.
  //

  int allocActor(int id = -1)
  {
    int newid = -1;
    if (-1 != id) {
      newid = mActors.alloc(id);
    } else {
      newid = mActors.alloc();
    }

    if (-1 == newid) {
      return -1;
    }

    if (mLua) {
      lua_newtable(mLua);
      lua_pushliteral(mLua, "_id");
      lua_pushinteger(mLua, newid);
      lua_rawset(mLua, -3);
      char buff[GOOD_MAX_PARAM_NAME_LEN];
      getScriptParamName(newid, buff);
      lua_setglobal(mLua, buff);
    }

    return newid;
  }

  int allocActorId_i(int idType, int resId, ResT *pRes)
  {
    int idItem = -1;
    switch (idType)
    {
    case GOOD_CREATE_OBJ_RES_ID:
      idItem = allocActor(resId);
      if (resId != idItem) {
        return -1;
      }
      break;
    case GOOD_CREATE_OBJ_ANY_ID:
      idItem = allocActor();
      break;
    case GOOD_CREATE_OBJ_EXCLUDE_RES_ID:
      idItem = allocActorIdExcludeResId_i(pRes);
      break;
    }
    return idItem;
  }

  int allocActorIdExcludeResId_i(ResT *pRes)
  {
    const sw2::ObjectPool<int,32,true> &pool = pRes ? pRes->mId : mRes.mId;
    for (int i = mActors.firstFree(); -1 != i; i = mActors.nextFree(i)) {
      if (!pool.isUsed(i)) {
        return allocActor(i);
      }
    }
    return allocActor();
  }

  int createObj_i(int idItem, LevelT const& lvl, ObjectT const& o, int idType, char const *pPkgName, ResT *pRes)
  {
    if (ObjectT::TYPE_LVLOBJ != o.mType) {
      mActors[idItem].init(o);
      if (ObjectT::TYPE_TEXT == o.mType) {
        createTextObj_i(idItem, o, pRes);
      }
    }

    for (size_t i = 0; i < o.mObjIdx.size(); ++i) {
      int idObj = o.mObjIdx[i];
      if (-1 == createChildObj_i(idItem, lvl, idObj, -1, idType, pPkgName, pRes)) {
        mActors[idItem].free();         // Free obj; ref may invalid if pool grow up.
        return -1;
      }
    }

    return idItem;
  }

  void createObjInit_i(int idItem, const ObjectT *po, char const *pPkgName, ResT *pRes)
  {
    ActorT &a = mActors[idItem];

    switch (po->mType)
    {
    case ObjectT::TYPE_SPRITE:
      if (pRes) {
        a.create(idItem, ActorT::TYPES_SPRITE, dupRes_i(pPkgName, *pRes, po->mSpriteId));
      } else {
        a.create(idItem, ActorT::TYPES_SPRITE, po->mSpriteId);
      }
      break;

    case ObjectT::TYPE_COLBG:
      a.create(idItem, ActorT::TYPES_COLBG, -1);
      break;

    case ObjectT::TYPE_TEXBG:
      if (pRes) {
        a.create(idItem, ActorT::TYPES_TEXBG, dupRes_i(pPkgName, *pRes, po->mTextureId));
      } else {
        a.create(idItem, ActorT::TYPES_TEXBG, po->mTextureId);
      }
      break;

    case ObjectT::TYPE_MAPBG:
      if (pRes) {
        a.create(idItem, ActorT::TYPES_MAPBG, dupRes_i(pPkgName, *pRes, po->mMapId));
      } else {
        a.create(idItem, ActorT::TYPES_MAPBG, po->mMapId);
      }
      break;

    case ObjectT::TYPE_DUMMY:
    case ObjectT::TYPE_TEXT:
      a.create(idItem, ActorT::TYPES_DUMMY, -1);
      break;
    }
  }

  int createChildObj_i(int idParent, LevelT const& lvl, int resId, int preferId, int idType, char const *pPkgName, ResT *pRes)
  {
    const ObjectT *po = getLevelObjRes_i(lvl, resId, pRes);
    if (0 == po) {
      return -1;
    }

    if (ObjectT::TYPE_LVLOBJ == po->mType) {
      int idItem = createChildObj_i(idParent, lvl, po->getLevelObjId(), resId, GOOD_CREATE_OBJ_EXCLUDE_RES_ID, pPkgName, pRes);
      if (-1 != idItem) {
        ActorT &a = mActors[idItem];
        a.mPosX = (float)po->mPosX;
        a.mPosY = (float)po->mPosY;
      }
      if (-1 == createObj_i(idItem, lvl, *po, idType, pPkgName, pRes)) {
        mActors[idItem].free();
        return -1;
      }
      return idItem;
    }

    int idItem = -1;
    if (-1 != preferId) {
      idItem = allocActorId_i(GOOD_CREATE_OBJ_RES_ID, preferId, pRes);
    } else {
      idItem = allocActorId_i(idType, resId, pRes);
    }
    if (-1 == idItem) {
      return -1;
    }

    createObjInit_i(idItem, po, pPkgName, pRes);

    if (-1 == createObj_i(idItem, lvl, *po, idType, pPkgName, pRes)) {
      return -1;
    }

    mActors[idParent].addChild(idItem); // idParent.addChild(idItem); ref may invalid if pool grow up.

    return idItem;
  }

  void createTextObj_i(int idItem, ObjectT const& o, ResT *pRes)
  {
    genTextObj_i(idItem, o.mText.c_str(), o.mTextSize, GOOD_CREATE_OBJ_EXCLUDE_RES_ID, pRes);
    unsigned int c = getBgColor(idItem);
    for (int i = 0; i < getChildCount(idItem); i++) {
      int id = getChild(idItem, i);
      setBgColor(id, c);
    }
  }

  int createLevel(int resId)
  {
    int idLvl = allocActor(resId);
    if (resId != idLvl) {
      return -1;
    }

    mRoot = idLvl;
    mActors[idLvl].create(idLvl, ActorT::TYPES_LEVEL, resId);

    LevelT const& lvl = mRes.getLevel(resId);

    if (-1 == createObj_i(idLvl, lvl, lvl, GOOD_CREATE_OBJ_RES_ID, 0, 0)) {
      mRoot = -1;
      return -1;
    }

    mDirty = true;

    return idLvl;
  }

  const ObjectT* getLevelObjRes_i(LevelT const& lvl, int resId, ResT *pRes) const
  {
    if (lvl.isObj(resId)) {
      return &(lvl.getObj(resId));
    }
    std::map<int, LevelT>::const_iterator it;
    if (pRes) {
      for (it = pRes->mLevel.begin(); pRes->mLevel.end() != it; ++it) {
        if (it->second.isObj(resId)) {
          return &(it->second.getObj(resId));
        }
      }
    } else {
      for (it = mRes.mLevel.begin(); mRes.mLevel.end() != it; ++it) {
        if (lvl.mId != it->second.mId && it->second.isObj(resId)) {
          return &(it->second.getObj(resId));
        }
      }
    }
    return 0;
  }

  //
  // Helper.
  //

  bool isKeyDown(int keys) const
  {
    return mKeys.isKeyDown(keys);
  }

  bool isKeyPressed(int keys) const
  {
    return mKeys.isKeyPressed(keys);
  }

  bool isKeyPushed(int keys) const
  {
    return mKeys.isKeyPushed(keys);
  }

  void getScriptParamName(int id, char *buff) const
  {
    sprintf(buff, "_rtobjp%d", id);
  }

#ifndef GOOD_SUPPORT_NO_LOGO
  bool addAndPlayLogoFileSystem(std::string const &prjname)
  {
    const char *name = "_rtGoodLogo";

    if (mFileSys.end() != mFileSys.find(name)) {
      return false;
    }

    std::stringstream ss;
    ss.write((const char*)GOOD_LOGO_MOD, sizeof(GOOD_LOGO_MOD));

    if (!mAr->addStreamFileSystem(ss)) {
      trace("add file system '%s' failed", name);
      return false;
    }

    mFileSys[name] = 1;

    mPkgCallStack.push_back(prjname);
    mNextPlayPkg = "logo.txt";

    return true;
  }
#endif

  bool addPathFileSystem(std::string const& name)
  {
    if (name.empty() || mFileSys.end() != mFileSys.find(name)) {
      return true;
    }

    if (!mAr->addPathFileSystem(name)) {
      trace("add file system '%s' failed", name.c_str());
      return false;
    }

    mFileSys[name] = 1;

    return true;
  }

  bool addStreamFileSystem(std::string const& ssname, std::istream& stream)
  {
    char name[256];
    if (ssname.empty()) {
      int curPos = (int)stream.tellg();
      stream.seekg(0, std::ios_base::end);
      int lenStream = (int)stream.tellg() - curPos;
      stream.seekg(curPos, std::ios_base::beg);
      unsigned int crc32 = 0;
      if (!sw2::Util::crc32(crc32, stream, lenStream)) {
        trace("calc stream file system crc32 failed!");
        return false;
      }
      stream.seekg(curPos, std::ios_base::beg);
      sprintf(name, "stream;%x;%x", lenStream, crc32);
    } else {
      strcpy(name, ssname.c_str());
    }

    if (mFileSys.end() != mFileSys.find(name)) {
      return true;
    }

    if (!mAr->addStreamFileSystem(stream)) {
      trace("add stream file system '%s' failed!", name);
      return false;
    }

    mFileSys[name] = 1;

    return true;
  }

  bool allocAr()
  {
    if (0 == mAr) {                     // Only init one time.
      mAr = sw2::Archive::alloc();
      if (0 == mAr) {
        trace("file system archive unavailable");
        return false;
      }
    }

    return true;
  }

  bool loadFile(std::string const& name, std::stringstream& ss) const
  {
    return mAr->loadFile(name, ss, GOOD_PACKAGE_PASSWORD);
  }

  bool loadLuaScript(const std::string &name)
  {
    std::stringstream ss;
    if (!loadFile(name, ss)) {
      trace("Script '%s' not found.\n", name.c_str());
      return false;
    }

    std::string sdat = ss.str();
    int s = luaL_loadbuffer(mLua, sdat.data(), sdat.length(), name.c_str());
    if (0 == s) {
      s = lua_pcall(mLua, 0, LUA_MULTRET, 0);
    }

    if (0 != s) {
      trace("[%s] %s\n", name.c_str(), lua_tostring(mLua, -1));
      return false;
    }

    return true;
  }

#ifdef GOOD_SUPPORT_ONLY_ONE_TEXTURE
  void handleImageManagerSurfaceChanged()
  {
    for (int i = mActors.first(); -1 != i; i = mActors.next(i)) {
      mActors[i].mImg = ImgT();
    }
  }
#endif
};

} // namespace rt

} // namespace good

// end of app.h
