
//
// actor.h
// Runtime, animatable object.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/03/01 Waync created
//

#pragma once

namespace good {

namespace rt {

template<class AppT, class ImgT>
class Actor
{
public:
  enum TYPE
  {
    TYPES_LEVEL = 1,
    TYPES_COLBG,
    TYPES_TEXBG,
    TYPES_MAPBG,
    TYPES_SPRITE,
    TYPES_DUMMY
  };

  int mId;                              // Actor ID.

  int mParent;
  std::vector<int> mChild;

  int mResType, mResId;
  std::string mResName;

  int mCurrFrame, mCurrFrameTime;       // Support sprite.

  bool mSpriteAnimator, mScriptAnimator;

  bool mVisible;
  float mPosX, mPosY;                   // Relative to parent(not include level(top level)).
  std::string mScript;

  sw2::IntRect mDim;
  unsigned int mBgColor;                // 32-bit color, ARGB.

  bool mRepX, mRepY;                    // Valid for tex & map bg.

  float mRot;
  float mXscale, mYscale;
  float mAnchorX, mAnchorY;

  bool mOwnerDraw;

  mutable ImgT mImg;                    // Cache for image.

  //
  // New/del.
  //

  void create(int id, int resType, int resId)
  {
    mId = id;

    mParent = -1;
    mChild.clear();

    mResType = resType;
    mResId = resId;

    mVisible = true;
    mPosX = mPosY = .0f;

    mScript[0] = '\0';

    mCurrFrame = 0;
    mCurrFrameTime = 0;

    mDim = sw2::IntRect(0,0,0,0);
    mBgColor = 0xffffffff;

    mRepX = mRepY = false;

    mRot = .0f;
    mXscale = mYscale = 1.0f;
    mAnchorX = mAnchorY = .0f;

    mOwnerDraw = false;

    mSpriteAnimator = true;
    mScriptAnimator = false;

    mImg = ImgT();

    AppT const& app = AppT::getInst();

    if (TYPES_SPRITE == resType && app.mRes.isSprite(resId)) {
      mResId = -1;                      // Force apply.
      if (setSpriteId(resId)) {
        playAnim();
      }
    }
  }

  void free()
  {
    OnDestroy();

    AppT& app = AppT::getInst();

    for (size_t i = 0; i < mChild.size(); ++i) {
      typename AppT::ActorT& a = app.mActors[mChild[i]];
      a.mParent = -1;                   // Disconnect the chain to avoid re-entry.
      a.free();
    }

    mChild.clear();

    if (-1 != mParent) {
      typename AppT::ActorT& p = app.mActors[mParent];
      p.removeChild(mId);
      mParent = -1;
    }

    if (app.mLua) {
      lua_getglobal(app.mLua, GOOD_RT_OBJ);
      lua_pushinteger(app.mLua, mId);
      lua_pushnil(app.mLua);
      lua_rawset(app.mLua, -3);
      lua_pop(app.mLua, 1);             // GOOD_RT_OBJ.
    }

    app.mActors.free(mId);
  }

  void init(Object const& o)
  {
    mVisible = o.mVisible;

    mPosX = (float)o.mPosX;
    mPosY = (float)o.mPosY;
    mRepX = o.mRepX;
    mRepY = o.mRepY;
    mRot = o.mRot;
    mXscale = o.mScaleX;
    mYscale = o.mScaleY;
    mAnchorX = o.mAnchorX;
    mAnchorY = o.mAnchorY;

    mDim = o.mDim;
    mBgColor = good::converBgColor(o.mBgColor);

    mResName = o.getName();

    if (!o.mScript.empty()) {
      setScript(o.mScript.c_str());
    }
  }

  //
  // Property.
  //

  void getPos(float& x, float& y, bool world = true) const
  {
    x = mPosX;
    y = mPosY;

    if (!world) {
      return;
    }

    AppT const& app = AppT::getInst();

    int p = mParent;

    while (app.mActors.isUsed(p)) {
      typename AppT::ActorT const& a = app.mActors[p];
      if (TYPES_LEVEL == a.mResType) {
        break;
      }
      x += a.mPosX;
      y += a.mPosY;
      p = a.mParent;
    }
  }

  int getType() const
  {
    return mResType;
  }

  bool setMapId(int idMap)
  {
    AppT& app = AppT::getInst();

    if (!app.mRes.isMap(idMap)) {
      return false;
    }

    if (idMap == mResId) {
      return false;
    }

    mResType = TYPES_MAPBG;
    mResId = idMap;
    mImg = ImgT();

    return true;
  }

  bool setTexId(int idTex)
  {
    AppT& app = AppT::getInst();

    if (!app.mRes.isTex(idTex)) {
      return false;
    }

    if (idTex == mResId) {
      return false;
    }

    mResType = TYPES_TEXBG;
    mResId = idTex;
    mImg = ImgT();

    return true;
  }

  //
  // Relationship.
  //

  int getChildCount() const
  {
    return (int)mChild.size();
  }

  int getChild(int idx) const
  {
    if (0 > idx || (int)mChild.size() <= idx) {
      return -1;
    } else {
      return mChild[idx];
    }
  }

  void addChild(int idChild, int index = -1)
  {
    AppT& app = AppT::getInst();

    if (!app.mActors.isUsed(idChild)) {
      return;
    }

    if (0 > index || (int)mChild.size() <= index) {
      mChild.push_back(idChild);
    } else {
      mChild.insert(mChild.begin() + index, idChild);
    }

    app.mActors[idChild].mParent = mId;
  }

  void removeChild(int idChild)
  {
    for (std::vector<int>::iterator it = mChild.begin(); mChild.end() != it; ++it) {
      if (idChild == *it) {
        AppT::getInst().mActors[*it].mParent = -1;
        mChild.erase(it);
        break;
      }
    }
  }

  //
  // Animator.
  //

  void setScript(const char* script)
  {
    if (0 == script) {
      script = "";
    }

    if (mScript == script) {            // Same script.
      return;
    }

    mScript = script;

    mScriptAnimator = false;
    mOwnerDraw = false;

    if ('\0' == script[0]) {            // Empty script.
      return;
    }

    AppT& app = AppT::getInst();
    assert(app.mLua);

    lua_getglobal(app.mLua, script);

    if (LUA_TTABLE == lua_type(app.mLua, -1)) {

      //
      // Check for OnStep handler.
      //

      lua_getfield(app.mLua, -1, "OnStep");
      mScriptAnimator = LUA_TFUNCTION == lua_type(app.mLua, -1);
      lua_remove(app.mLua, -1);

      //
      // Check for OnDraw handler.
      //

      lua_getfield(app.mLua, -1, "OnDraw");
      mOwnerDraw = LUA_TFUNCTION == lua_type(app.mLua, -1);
      lua_remove(app.mLua, -1);

    } else {
      app.trace("Set script failed, script '%s' not found!\n", script);
    }

    lua_remove(app.mLua, -1);
  }

  bool setSpriteId(int idSprite)
  {
    AppT& app = AppT::getInst();

    if (!app.mRes.isSprite(idSprite)) {
      return false;
    }

    if (idSprite == mResId) {
      return false;
    }

    typename AppT::SpriteT const& spr = app.mRes.getSprite(idSprite);

    if ((int)spr.mFrame.size() <= mCurrFrame) {
      mCurrFrame = 0;
    }

    mResType = TYPES_SPRITE;
    mCurrFrameTime = spr.mTime[mCurrFrame];
    mResId = idSprite;
    mImg = ImgT();

    return true;
  }

  //
  // Animation.
  //

  bool animateSprite()
  {
    if (TYPES_SPRITE != mResType) {
      return false;
    }

    if (0 < mCurrFrameTime && 0 < --mCurrFrameTime) {
      return false;
    }

    typename AppT::SpriteT const& spr = AppT::getInst().mRes.getSprite(mResId);

    if (!spr.mLoop && mCurrFrame + 1 >= (int)spr.mFrame.size()) {
      mSpriteAnimator = false;
      return true;
    }

    if (++mCurrFrame >= (int)spr.mFrame.size()) {
      mCurrFrame = 0;
    }

    mCurrFrameTime = spr.mTime[mCurrFrame];

    return true;
  }

  bool animateScript()
  {
    return 0 < callScript("OnStep", 0);
  }

  bool playAnim()
  {
    if (TYPES_SPRITE == mResType) {

      typename AppT::SpriteT const& spr = AppT::getInst().mRes.getSprite(mResId);

      if (spr.mFrame.empty()) {
        return false;
      }

      if (mCurrFrame + 1 >= (int)spr.mFrame.size()) {
        mCurrFrame = 0;
      }

      mCurrFrameTime = spr.mTime[mCurrFrame];
    }

    mSpriteAnimator = true;

    return true;
  }

  bool pauseAnim()
  {
    mSpriteAnimator = false;

    return true;
  }

  bool stopAnim()
  {
    if (!pauseAnim()) {
      return false;
    }

    mCurrFrame = 0;

    return true;
  }

  static bool animate(int idObj)
  {
    //
    // Return true if anything changes.
    //

    AppT& app = AppT::getInst();

    if (!app.mActors.isUsed(idObj)) {
      return false;
    }

    int t = 0;

    typename AppT::ActorT &a = app.mActors[idObj];

    if (a.mVisible && a.mSpriteAnimator) {
      t += a.animateSprite();
      // Animate child may cause mActors grow or idObj is freed. To avoid access invalid obj, folowing code use mActors[idObj].
      for (size_t i = 0; i < app.mActors[idObj].mChild.size(); i++) {
        t += animate(app.mActors[idObj].mChild[i]);
        if (!app.mActors.isUsed(idObj)) {
          return 0 < t;
        }
      }
    }

    if (app.mActors[idObj].mScriptAnimator) {
      t += app.mActors[idObj].animateScript();
    }

    return 0 < t;
  }

  //
  // Script notify.
  //

  int callScript(char const* name, int nArg, ...) const
  {
    if ('\0' == mScript[0]) {
      return 0;
    }

    AppT& app = AppT::getInst();
    assert(app.mLua);

    lua_getglobal(app.mLua, mScript.c_str());
    if (LUA_TTABLE != lua_type(app.mLua, -1)) {
      return 0;
    }

    lua_getfield(app.mLua, -1, name);
    if (LUA_TFUNCTION != lua_type(app.mLua, -1)) {
      lua_pop(app.mLua, 2);             // Pop table(mScript) and func(name).
      return 0;
    }

    lua_remove(app.mLua, -2);           // Remove mScript.

    lua_getglobal(app.mLua, GOOD_RT_OBJ);
    lua_pushinteger(app.mLua, mId);
    lua_rawget(app.mLua, -2);
    lua_remove(app.mLua, -2);           // GOOD_RT_OBJ

    va_list list;
    va_start(list, nArg);

    for (int i = 0; i < nArg; i++) {
      lua_pushinteger(app.mLua, va_arg(list, int));
    }

    va_end(list);

    std::string script(mScript);        // In OnCreate may cause mScript invalid because of app.mActors grow. So make a copy to avoid crash.
    int s = lua_pcall(app.mLua, 1 + nArg, LUA_MULTRET, 0);
    if (0 != s) {
      app.trace("[%s.%s] %s\n", script.c_str(), name, lua_tostring(app.mLua, -1));
    }

    return 0;
  }

  void OnCreate()
  {
    callScript("OnCreate", 0);
  }

  void OnCreateRecursive()
  {
    AppT &app = AppT::getInst();
    for (size_t i = 0; i < mChild.size(); ++i) {
      typename AppT::ActorT& a = app.mActors[mChild[i]];
      a.OnCreateRecursive();
    }
    OnCreate();
  }

  void OnDestroy()
  {
    callScript("OnDestroy", 0);
  }

  void OnNewParticle(int idParticle, int iMgr)
  {
    callScript("OnNewParticle", 2, idParticle, iMgr);
  }

  void OnKillParticle(int idParticle, int iMgr)
  {
    callScript("OnKillParticle", 2, idParticle, iMgr);
  }

  void OnDraw() const
  {
    callScript("OnDraw", 0);
  }
};

} // namespace rt

} // namespace good

// end of actor.h
