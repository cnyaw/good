
//
// res.h
// Good resource manager.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/11/29 Waync created.
//

#pragma once

namespace good {

class Traits
{
public:
  typedef Sound SoundT;
  typedef Texture TextureT;
  typedef Map<Tileset> MapT;
  typedef Sprite<Tileset> SpriteT;
  typedef Object ObjectT;
  typedef Level<Object> LevelT;
};

template<class TraitsT = Traits>
class Resource
{
public:

  typedef typename TraitsT::SoundT SoundT;
  typedef typename TraitsT::TextureT TextureT;
  typedef typename TraitsT::MapT MapT;
  typedef typename TraitsT::SpriteT SpriteT;
  typedef typename TraitsT::ObjectT ObjectT;
  typedef typename TraitsT::LevelT LevelT;

  //
  // Project property.
  //

  std::string mName;
  std::string mFileName;

  sw2::ObjectPool<int,32,true> mId;     // ID manager pool.

  //
  // Settings.
  //

  int mWidth, mHeight;                  // Window size.
  int mFps;

  //
  // Sounds.
  //

  std::vector<int> mSndIdx;
  std::map<int, SoundT> mSnd;

  //
  // Textures.
  //

  std::vector<int> mTexIdx;
  std::map<int, TextureT> mTex;

  //
  // Maps.
  //

  std::vector<int> mMapIdx;
  std::map<int, MapT> mMap;

  //
  // Sprites.
  //

  std::vector<int> mSpriteIdx;
  std::map<int, SpriteT> mSprite;

  //
  // Level.
  //

  std::vector<int> mLevelIdx;
  std::map<int, LevelT> mLevel;

  //
  // Script.
  //

  std::vector<int> mScriptIdx;
  std::map<int, std::string> mScript;

  //
  // STGE script.
  //

  std::vector<int> mStgeScriptIdx;
  std::map<int, std::string> mStgeScript;

  //
  // Project dependencies.
  //

  std::vector<int> mDepIdx;
  std::map<int, std::string> mDep;

  //
  // Codegen macros. Ex: isSnd, getSnd.
  //

#define IMPL_GET_RESOURCE_ITEM(ResName, ResType) \
  bool is ## ResName(int id) const \
  { \
    return m ## ResName.end() != m ## ResName.find(id); \
  } \
  ResType& get ## ResName(int id) \
  { \
    return const_cast<ResType&>(static_cast<Resource const &>(*this).get ## ResName(id)); \
  } \
  ResType const& get ## ResName(int id) const \
  { \
    typename std::map<int, ResType>::const_iterator it = m ## ResName.find(id); \
    if (m ## ResName.end() == it) { \
      throw std::range_error("get" # ResName); \
    } else { \
      return it->second; \
    } \
  }

#define IMPL_GET_STRING_RESOURCE_ITEM(ResName) \
  bool is ## ResName(int id) const \
  { \
    return m ## ResName.end() != m ## ResName.find(id); \
  } \
  std::string get ## ResName(int id) const \
  { \
    std::map<int, std::string>::const_iterator it = m ## ResName.find(id); \
    if (m ## ResName.end() == it) { \
      throw std::range_error("get" # ResName); \
    } else { \
      return it->second; \
    } \
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
  // Load resource.
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

    mFileName = name;

    return true;
  }

  bool load(std::istream& ins)
  {
    //
    // File.
    //

    sw2::Ini ini;
    if (!ini.load(ins)) {
      SW2_TRACE_ERROR("bad ini file format");
      return false;
    }

    if (!ini.find("good")) {
      SW2_TRACE_ERROR("[good] section not found");
      return false;
    }

    sw2::Ini sec = ini["good"];

    //
    // Property.
    //

    if (GOOD_VERSION != sec["version"].value) {
      SW2_TRACE_ERROR("version mismatch");
      return false;
    }

    mName = sec["name"].value;

    mId.clear();

    //
    // Default settings.
    //

    mWidth = 640;
    mHeight = 480;
    mFps = GOOD_DEFAULT_TICK_PER_SECOND;

    std::vector<int> v;
    assignListFromString(sec["window"].value, v);

    if (1 <= v.size()) {
      mWidth = v[0];
    }

    if (2 <= v.size()) {
      mHeight = v[1];
    }

    if (sec.find("fps")) {
      mFps = sec["fps"];
    }

    return loadAllResources(sec, ini);
  }

  bool loadAllResources(sw2::Ini &sec, const sw2::Ini &ini)
  {
    //
    // Sounds.
    //

    if (!loadResources(mId, mSnd, mSndIdx, "snds", sec, ini)) {
      return false;
    }

    //
    // Textures.
    //

    if (!loadResources(mId, mTex, mTexIdx, "texs", sec, ini)) {
      return false;
    }

    //
    // Maps.
    //

    if (!loadResources(mId, mMap, mMapIdx, "maps", sec, ini)) {
      return false;
    }

    //
    // Sprites.
    //

    if (!loadResources(mId, mSprite, mSpriteIdx, "sprites", sec, ini)) {
      return false;
    }

    //
    // Levels.
    //

    if (!loadResources(mId, mLevel, mLevelIdx, "levels", sec, ini)) {
      return false;
    }

    //
    // Scripts.
    //

    if (!loadStringTableResources(mId, mScript, mScriptIdx, "scripts", ini)) {
      return false;
    }

    //
    // STGE scripts.
    //

    if (!loadStringTableResources(mId, mStgeScript, mStgeScriptIdx, "stges", ini)) {
      return false;
    }

    //
    // Dependencies.
    //

    if (!loadStringTableResources(mId, mDep, mDepIdx, "deps", ini)) {
      return false;
    }

    return true;
  }
};

} // namespace good

// end of res.h
