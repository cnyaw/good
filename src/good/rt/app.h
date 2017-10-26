
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

template<class T, class ImgT, class SndT>
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

  Application() : mStartLevel(-1), mAntiAlias(true)
  {
  }

  //
  // Flag to exit current running package.
  //

  bool mExit;

  //
  // Key state.
  //

  int mKeys, mPrevKeys, mHandledKeys;
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

  bool mActorsGrowed;                   // If pool growed we need to skip iterate animate of remaining child actors to avoid potential crash.
  sw2::ObjectPool<ActorT, 128, true> mActors;

  int mRoot;
  int mStartLevel;                      // Starting level id, otherwise use first level.
  bool mCreateRoot;                     // Later create level object.

  //
  // Share dirty flag, modify by script.
  //

  mutable bool mDirty;

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

  std::vector<std::string> mPackageStack; // Real package stack.
  std::vector<std::string> mCallStack;  // Next package to play.

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

  bool init(std::string const& pathname, std::string const& txtname = "")
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
      if (!addFileSystem(path)) {
        break;
      }
      std::string name = ResName.substr(ResName.find_last_of('/') + 1);
      if (isGoodArchive(name) && !name.empty() && mFileSys.end() == mFileSys.find(name)) {
        std::stringstream ss;
        if (!loadFile(name, ss) || !addFileSystem(name, ss)) {
          break;
        }
      }
      std::string prjname;
      if (!txtname.empty()) {
        prjname = txtname;
      } else {
        prjname = decidePrjName(name);
      }
      return init_i(prjname);
    } while (0);

    uninit_i();
    return false;
  }

  bool init(std::istream& stream)
  {
    if (!allocAr()) {
      return false;
    }

    do
    {
      if (!addFileSystem("", stream)) {
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
      mKeys = mPrevKeys = 0;

      mCreateRoot = true;               // Later create level object.

      static_cast<T*>(this)->onPackageChanged(); // Notify package has change.
      mDirty = true;

      return true;

    } while (0);

    uninit_i();
    return false;
  }

  bool initDepex()
  {
    for (size_t i = 0; i < mRes.mDepIdx.size(); i++) {

      std::string depname = mRes.mDep[mRes.mDepIdx[i]];
      std::for_each(depname.begin(), depname.end(), ::tolower);

      //
      // Skip non package files and non search path.
      //

      if (!isGoodArchive(depname) && '/' != *depname.rbegin()) {
        continue;
      }

      //
      // Add new file system.
      //

      if (!addFileSystem(depname)) {
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
      std::stringstream ss;
      if (!loadFile(name, ss)) {
        trace("Script '%s' not found.\n", name.c_str());
        continue;
      }
      std::string sdat = ss.str();
      int s = luaL_loadbuffer(mLua, sdat.data(), sdat.length(), name.c_str());
      if (0 == s) {
        s = lua_pcall(mLua, 0, LUA_MULTRET, 0);
      }
      if (0 != s) {
        trace("[%s] %s\n", name.c_str(), lua_tostring(mLua, -1));
      }
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

    mRoot = -1;
  }

  void trace(const char* format, ...)
  {
    char buf[1024];

    va_list va;
    va_start(va, format);
    vsnprintf(buf, sizeof(buf), format, va);
    va_end(va);

    return static_cast<T*>(this)->doTrace(buf);
  }

  bool openUrl(std::string const& url) const
  {
    return static_cast<T const*>(this)->doOpenUrl(url);
  }

  void playPackage_i(std::string const& name)
  {
    uninit_i();
    init(name);
  }

  void exitPackage()
  {
    if (!mPackageStack.empty()) {
      mCallStack.push_back(mPackageStack.back());
      mPackageStack.erase(mPackageStack.begin() + (mPackageStack.size() - 1));
    } else {
      mExit = true;
    }
  }

  //
  // Trigger logic.
  //

  bool trigger(int keys, sw2::IntPoint const& mousePos)
  {
    mPrevKeys = mKeys;
    mKeys = keys;
    mHandledKeys = 0;

    mMousePos = mousePos;

    mRenderState = false;
    mActorsGrowed = false;

    //
    // Later create level object.
    //

    if (mCreateRoot) {
      mCreateRoot = false;
      if (!mRes.mLevelIdx.empty()) {
        if (GOOD_LOGO_NAME != mRes.mName && -1 != mStartLevel) {
          mRoot = createLevel(mStartLevel);
          mStartLevel = -1;
        } else {
          mRoot = createLevel(mRes.mLevelIdx[0]);
        }
      } else {
        mRoot = -1;
      }
    }

    //
    // Update call stack.
    //

    if (!mCallStack.empty()) {
      std::string name(mCallStack.front());
      mCallStack.erase(mCallStack.begin());
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

      mDirty = mActors[mRoot].animate() || mDirty;
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
    int capacity = mActors.capacity();
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

    if (!mActorsGrowed) {
      mActorsGrowed = capacity != mActors.capacity();
    }

    return newid;
  }

  int createObj(int idItem, LevelT const& lvl, ObjectT const& o, bool bValidId, char const *pPkgName, ResT *pRes)
  {
    mActors[idItem].init(o);

    for (size_t i = 0; i < o.mObjIdx.size(); ++i) {
      int idObj = o.mObjIdx[i];
      if (-1 == createObj(idItem, lvl, idObj, bValidId, pPkgName, pRes)) {
        mActors[idItem].free();         // Free obj; ref may invalid if pool grow up.
        return -1;
      }
    }

    mActors[idItem].OnCreate();

    return idItem;
  }

  int createObj(int idParent, LevelT const& lvl, int resId, bool bValidId, char const *pPkgName, ResT *pRes)
  {
    int idItem = -1;

    if (bValidId) {
      idItem = allocActor(resId);
      if (resId != idItem) {
        return -1;
      }
    } else {
      idItem = allocActor();
    }

    ObjectT const& o = lvl.getObj(resId);
    ActorT &a = mActors[idItem];

    switch (o.mType)
    {
    case ObjectT::TYPE_SPRITE:
      if (pRes) {
        a.create(idItem, ActorT::TYPES_SPRITE, dupRes(pPkgName, *pRes, o.mSpriteId));
      } else {
        a.create(idItem, ActorT::TYPES_SPRITE, o.mSpriteId);
      }
      break;

    case ObjectT::TYPE_COLBG:
      a.create(idItem, ActorT::TYPES_COLBG, -1);
      break;

    case ObjectT::TYPE_TEXBG:
      if (pRes) {
        a.create(idItem, ActorT::TYPES_TEXBG, dupRes(pPkgName, *pRes, o.mTextureId));
      } else {
        a.create(idItem, ActorT::TYPES_TEXBG, o.mTextureId);
      }
      break;

    case ObjectT::TYPE_MAPBG:
      if (pRes) {
        a.create(idItem, ActorT::TYPES_MAPBG, dupRes(pPkgName, *pRes, o.mMapId));
      } else {
        a.create(idItem, ActorT::TYPES_MAPBG, o.mMapId);
      }
      break;

    case ObjectT::TYPE_DUMMY:
      a.create(idItem, ActorT::TYPES_DUMMY, -1);
      break;
    }

    if (-1 == createObj(idItem, lvl, o, bValidId, pPkgName, pRes)) {
      return -1;
    }

    mActors[idParent].addChild(idItem); // idParent.addChild(idItem); ref may invalid if pool grow up.

    return idItem;
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

    if (-1 == createObj(idLvl, lvl, lvl, true, 0, 0)) {
      mRoot = -1;
      return -1;
    }

    mDirty = true;

    return idLvl;
  }

  //
  // Helper.
  //

  bool isKeyDown(int key) const
  {
    return 0 != (mKeys & key);
  }

  bool isKeyPressed(int key) const
  {
    return !(mKeys & key) && (mPrevKeys & key);
  }

  bool isKeyPushed(int keyCode) const
  {
    return (mKeys & keyCode) && !(mPrevKeys & keyCode);
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

    if (!mAr->addFileSystem(ss)) {
      trace("add file system '%s' failed", name);
      return false;
    }

    mFileSys[name] = 1;

    mPackageStack.push_back(prjname);
    mCallStack.push_back("logo.txt");

    return true;
  }
#endif

  bool addFileSystem(std::string const& name)
  {
    if (name.empty() || mFileSys.end() != mFileSys.find(name)) {
      return true;
    }

    if (!mAr->addFileSystem(name)) {
      trace("add file system '%s' failed", name.c_str());
      return false;
    }

    mFileSys[name] = 1;

    return true;
  }

  bool addFileSystem(std::string const& ssname, std::istream& stream)
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

    if (!mAr->addFileSystem(stream)) {
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
};

} // namespace rt

} // namespace good

// end of app.h
