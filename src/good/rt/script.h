
//
// script.h
// Runtime, script API.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/03/11 Waync created.
//

#pragma once

namespace good {

namespace rt {

template<class AppT, class SndT>
class LuaScript
{
public:

  struct RegApiType
  {
    char const *name;
    int (*api)(lua_State*);
  };

  struct RegConstType
  {
    char const *name;
    unsigned int value;
  };

  static void regAPI(lua_State* L, RegApiType const *apis)
  {
    for (int i = 0; apis[i].name; ++i) {
      lua_pushlstring(L, apis[i].name, ::strlen(apis[i].name));
      lua_pushcfunction(L, apis[i].api);
      lua_rawset(L, -3);
    }
  }

  static void regCONST(lua_State* L, RegConstType const *consts)
  {
    for (int i = 0; consts[i].name; ++i) {
      lua_pushlstring(L, consts[i].name, ::strlen(consts[i].name));
      lua_pushinteger(L, consts[i].value);
      lua_rawset(L, -3);
    }
  }

  static bool regAPI(lua_State* L)
  {
    if (0 == L) {
      return false;
    }

    static bool (*RegApi[])(lua_State*) = {
      &installGoodModuleApi,
      &installInputModuleApi,
      &installResourceModuleApi,
      &installSoundModuleApi,
      &installGraphicsModuleApi,
#ifdef GOOD_SUPPORT_STGE
      &installStgeModuleApi,
#endif
      0
    };

    for (int i = 0; 0 != RegApi[i]; i++) {
      if (!RegApi[i](L)) {
        return false;
      }
    }

    return true;
  }

  //
  // Good API.
  //

  //
  // [in] idParent, idChild[, index] [out]
  //

  static int AddChild(lua_State* L)
  {
    int idParent = luaL_checkint(L, 1);
    int idChild = luaL_checkint(L, 2);

    int index = -1;
    if (3 <= lua_gettop(L)) {
      index = luaL_checkint(L, 3);
    }

    AppT::getInst().addChild(idParent, idChild, index);

    return 0;
  }

  //
  // [in] name [out]
  //

  static int CallPackage(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    AppT::getInst().callPackage(name);

    return 0;
  }

  //
  // [in] idObj [out] id.
  //

  static int CloneObj(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().cloneObj(idObj));

    return 1;
  }

  //
  // [in] [out]
  //

  static int Exit(lua_State* L)
  {
    AppT::getInst().exit();

    return 0;
  }

  //
  // [in] idObj, childName[, recursive] [out] id.
  //

  static int FindChild(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    const char* name = luaL_checkstring(L, 2);

    int recursive = 0;
    if (3 <= lua_gettop(L)) {
      recursive = luaL_checkint(L, 3);
    }

    lua_pushinteger(L, AppT::getInst().findChild(idObj, name, 0 != recursive));

    return 1;
  }

  //
  // [in] int [out]
  //

  static int FireUserIntEvent(lua_State* L)
  {
    int i = luaL_checkint(L, 1);

    AppT::getInst().doUserIntEvent(i);

    return 0;
  }

  //
  // [in] idParent[, script] [out] id.
  //

  static int GenDummy(lua_State* L)
  {
    int idParent = luaL_checkint(L, 1);

    const char* script = 0;
    if (2 <= lua_gettop(L)) {
      script = luaL_checkstring(L, 2);
    }

    lua_pushinteger(L, AppT::getInst().genDummy(idParent, script));

    return 1;
  }

  //
  // [in] idParent, idRes[, script] [out] id.
  //

  static int GenObj(lua_State* L)
  {
    int idParent = luaL_checkint(L, 1);
    int idRes = luaL_checkint(L, 2);

    const char* script = 0;
    if (3 <= lua_gettop(L)) {
      script = luaL_checkstring(L, 3);
    }

    lua_pushinteger(L, AppT::getInst().genObj(idParent, idRes, script));

    return 1;
  }

  //
  // [in] pkg, idParent, idRes[, script] [out] id.
  //

  static int GenObjEx(lua_State* L)
  {
    const char* pkg = luaL_checkstring(L, 1);
    int idParent = luaL_checkint(L, 2);
    int idRes = luaL_checkint(L, 3);

    const char* script = 0;
    if (4 <= lua_gettop(L)) {
      script = luaL_checkstring(L, 4);
    }

    lua_pushinteger(L, AppT::getInst().genObjEx(pkg, idParent, idRes, script));

    return 1;
  }

  //
  // [in] idParent, text[, size, script] [out] id
  //

  static int GenTextObj(lua_State* L)
  {
    int idParent = luaL_checkint(L, 1);
    const char* utf8text = luaL_checkstring(L, 2);

    int size = GOOD_DEFAULT_TEXT_SIZE;
    if (3 <= lua_gettop(L)) {
      size = luaL_checkint(L, 3);
    }

    const char* script = 0;
    if (4 <= lua_gettop(L)) {
      script = luaL_checkstring(L, 4);
    }

    lua_pushinteger(L, AppT::getInst().genTextObj(idParent, utf8text, size, script));

    return 1;
  }

  //
  // [in] idObj [out] anchorx, anchory
  //

  static int GetAnchor(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    float ax = .0f, ay = .0f;
    AppT::getInst().getAnchor(idObj, ax, ay);

    lua_pushnumber(L, ax);
    lua_pushnumber(L, ay);

    return 2;
  }

  //
  // [in] idObj [out] bg color.
  //

  static int GetBgColor(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushnumber(L, AppT::getInst().getBgColor(idObj));

    return 1;
  }

  //
  // [in] idObj, idx [out] child id.
  //

  static int GetChild(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    int idx = luaL_checkint(L, 2);

    lua_pushinteger(L, AppT::getInst().getChild(idObj, idx));

    return 1;
  }

  //
  // [in] idObj, [out] number child.
  //

  static int GetChildCount(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getChildCount(idObj));

    return 1;
  }

  //
  // [in] idObj, [out] left, top, width, height.
  //

  static int GetDim(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    int left = 0, top = 0, width = 0, height = 0;
    AppT::getInst().getDim(idObj, left, top, width, height);

    lua_pushinteger(L, left);
    lua_pushinteger(L, top);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);

    return 4;
  }

  //
  // [in], [out] curr level id.
  //

  static int GetLevelId(lua_State* L)
  {
    lua_pushinteger(L, AppT::getInst().mRoot);

    return 1;
  }

  //
  // [in] idObj [out] map id.
  //

  static int GetMapId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getMapId(idObj));

    return 1;
  }

  //
  // [in] idObj [out] obj name.
  //

  static int GetName(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushstring(L, AppT::getInst().getName(idObj).c_str());

    return 1;
  }

  //
  // [in] idObj, [out] obj param.
  //

  static int GetParam(lua_State* L)
  {
    int id = luaL_checkint(L, 1);

    AppT const& app = AppT::getInst();

    if (!app.mActors.isUsed(id)) {
      lua_pushnil(L);
      return 1;
    }

    char buff[GOOD_MAX_PARAM_NAME_LEN];
    app.getScriptParamName(id, buff);
    lua_getglobal(L, buff);

    return 1;
  }

  //
  // [in] idObj, [out] parent id.
  //

  static int GetParent(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getParent(idObj));

    return 1;
  }

  //
  // [in] idObj[, world], [out] x, y.
  //

  static int GetPos(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    bool world = false;
    if (2 <= lua_gettop(L)) {
      world = 0 != luaL_checkint(L, 2);
    }

    float x = .0f, y = .0f;
    AppT::getInst().getPos(idObj, x, y, world);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    return 2;
  }

  //
  // [in] idObj, [out] repX, repY.
  //

  static int GetRep(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    bool bRepX = false, bRepY = false;
    AppT::getInst().getRep(idObj, bRepX, bRepY);

    lua_pushboolean(L, bRepX);
    lua_pushboolean(L, bRepY);

    return 2;
  }

  //
  // [in] idObj, [out] rot.
  //

  static int GetRot(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushnumber(L, AppT::getInst().getRot(idObj));

    return 1;
  }

  //
  // [in] idObj, [out] xscale, yscale.
  //

  static int GetScale(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    float xscale = 1.0f, yscale = 1.0f;
    AppT::getInst().getScale(idObj, xscale, yscale);

    lua_pushnumber(L, xscale);
    lua_pushnumber(L, yscale);

    return 2;
  }

  //
  // [in] idObj, [out] sprite resource id.
  //

  static int GetSpriteId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getSpriteId(idObj));

    return 1;
  }

  //
  // [in] idObj, [out] texture resource id.
  //

  static int GetTexId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getTexId(idObj));

    return 1;
  }

  //
  // [in] idObj, [out] type.
  //

  static int GetType(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getType(idObj));

    return 1;
  }

  //
  // [in] idObj, [out] isVisible.
  //

  static int GetVisible(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getVisible(idObj) ? 1 : 0);

    return 1;
  }

  //
  // [in] [out] w, h.
  //

  static int GetWindowSize(lua_State* L)
  {
    AppT const& app = AppT::getInst();

    lua_pushinteger(L, app.mRes.mWidth);
    lua_pushinteger(L, app.mRes.mHeight);

    return 2;
  }

  //
  // [in] idObj, [out] IsAnimPlaying
  //

  static int IsAnimPlaying(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushboolean(L, AppT::getInst().isAnimPlaying(idObj));

    return 1;
  }

  //
  // [in] idObj, [out]
  //

  static int KillAllChild(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    AppT::getInst().killAllChild(idObj);

    return 0;
  }

  //
  // [in] idObj, [out]
  //

  static int KillObj(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    AppT::getInst().killObj(idObj);

    return 0;
  }

  //
  // [in] url, [out]
  //

  static int OpenUrl(lua_State* L)
  {
    const char* url = luaL_checkstring(L, 1);
    if (url) {
      (void)AppT::getInst().openUrl(url);
    }

    return 0;
  }

  //
  // [in] idObj [out]
  //

  static int PauseAnim(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    AppT::getInst().pauseAnim(idObj);

    return 0;
  }

  //
  // [in] x,y[, type, idRes, idResXor] [out] hit obj id.
  //

  static int PickObj(lua_State* L)
  {
    lua_Number x = luaL_checknumber(L, 1);
    lua_Number y = luaL_checknumber(L, 2);

    int type = -1;
    if (3 <= lua_gettop(L)) {
      type = luaL_checkint(L, 3);
    }

    int idRes = -1;
    if (4 <= lua_gettop(L)) {
      idRes = luaL_checkint(L, 4);
    }

    int idResXor = -1;
    if (5 <= lua_gettop(L)) {
      idResXor = luaL_checkint(L, 5);
    }

    lua_pushinteger(L, AppT::getInst().pickObj(type, (float)x, (float)y, idRes, idResXor));

    return 1;
  }

  //
  // [in] idObj [out]
  //

  static int PlayAnim(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    AppT::getInst().playAnim(idObj);

    return 0;
  }

  //
  // [in] name [out]
  //

  static int PlayPackage(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    AppT::getInst().playPackage(name);

    return 0;
  }

  //
  // [in] idObj, bgColor, [out]
  //

  static int SetBgColor(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    unsigned int newclr = (unsigned int)luaL_checknumber(L, 2);

    AppT::getInst().setBgColor(idObj, newclr);

    return 0;
  }

  //
  // [in] idObj, xAnchor, yAnchor, [out]
  //

  static int SetAnchor(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    lua_Number newx = luaL_checknumber(L, 2);
    lua_Number newy = luaL_checknumber(L, 3);

    AppT::getInst().setAnchor(idObj, (float)newx, (float)newy);

    return 0;
  }

  //
  // [in] idObj, left, top, width, height, [out]
  //

  static int SetDim(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    int newx = luaL_checkint(L, 2);
    int newy = luaL_checkint(L, 3);
    int neww = luaL_checkint(L, 4);
    int newh = luaL_checkint(L, 5);

    AppT::getInst().setDim(idObj, newx, newy, neww, newh);

    return 0;
  }

  //
  // [in] idObj, idMap [out]
  //

  static int SetMapId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    int idMap = luaL_checkint(L, 2);

    AppT::getInst().setMapId(idObj, idMap);

    return 0;
  }

  //
  // [in] idObj, name [out]
  //

  static int SetName(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    const char* name = luaL_checkstring(L, 2);

    AppT::getInst().setName(idObj, name);

    return 0;
  }

  //
  // [in] idObj, x, y, [out]
  //

  static int SetPos(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    lua_Number newx = luaL_checknumber(L, 2);
    lua_Number newy = luaL_checknumber(L, 3);

    AppT::getInst().setPos(idObj, (float)newx, (float)newy);

    return 0;
  }

  //
  // [in] idObj, RepX, RepY, [out]
  //

  static int SetRep(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    bool bRepX = 0 != luaL_checkint(L, 2);
    bool bRepY = 0 != luaL_checkint(L, 3);

    AppT::getInst().setRep(idObj, bRepX, bRepY);

    return 0;
  }

  //
  // [in] idObj, rot, [out]
  //

  static int SetRot(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    lua_Number newRot = luaL_checknumber(L, 2);

    AppT::getInst().setRot(idObj, (float)newRot);

    return 0;
  }

  //
  // [in] idObj, xscale, yscale, [out]
  //

  static int SetScale(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    lua_Number newxScale = luaL_checknumber(L, 2);
    lua_Number newyScale = luaL_checknumber(L, 3);

    AppT::getInst().setScale(idObj, (float)newxScale, (float)newyScale);

    return 0;
  }

  //
  // [in] idObj, script [out]
  //

  static int SetScript(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    const char* script = luaL_checkstring(L, 2);

    AppT::getInst().setScript(idObj, script);

    return 0;
  }

  //
  // [in] idObj, idSpr [out]
  //

  static int SetSpriteId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    int idSpr = luaL_checkint(L, 2);

    AppT::getInst().setSpriteId(idObj, idSpr);

    return 0;
  }

  //
  // [in] idObj, idTex [out]
  //

  static int SetTexId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    int idTex = luaL_checkint(L, 2);

    AppT::getInst().setTexId(idObj, idTex);

    return 0;
  }

  //
  // [in] idObj, bVisible [out]
  //

  static int SetVisible(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    bool bVisible = 0 != luaL_checkint(L, 2);

    AppT::getInst().setVisible(idObj, bVisible);

    return 0;
  }

  //
  // [in] idObj [out]
  //

  static int StopAnim(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    AppT::getInst().stopAnim(idObj);

    return 0;
  }

  //
  // [in] str, [out]
  //

  static int OutputDebugString(lua_State* L)
  {
    const char* str = luaL_checkstring(L, 1);
    if (str) {
      AppT::getInst().trace(str);
    }

    return 0;
  }

  static bool installGoodModuleApi(lua_State* L)
  {
    static RegConstType GoodConst[] = {
      {"LEVEL", AppT::ActorT::TYPES_LEVEL},
      {"COLBG", AppT::ActorT::TYPES_COLBG},
      {"TEXBG", AppT::ActorT::TYPES_TEXBG},
      {"MAPBG", AppT::ActorT::TYPES_MAPBG},
      {"SPRITE", AppT::ActorT::TYPES_SPRITE},
      {"DUMMY", AppT::ActorT::TYPES_DUMMY},
      { 0 }
    };

    static RegApiType GoodApi[] = {
      {"AddChild", &AddChild},
      {"CallPackage", &CallPackage},
      {"CloneObj", &CloneObj},
      {"Exit", &Exit},
      {"FindChild", &FindChild},
      {"FireUserIntEvent", &FireUserIntEvent},
      {"GetAnchor", &GetAnchor},
      {"GenDummy", &GenDummy},
      {"GenObj", &GenObj},
      {"GenObjEx", &GenObjEx},
      {"GenTextObj", &GenTextObj},
      {"GetBgColor", &GetBgColor},
      {"GetChild", &GetChild},
      {"GetChildCount", &GetChildCount},
      {"GetDim", &GetDim},
      {"GetLevelId", &GetLevelId},
      {"GetMapId", &GetMapId},
      {"GetName", &GetName},
      {"GetParam", &GetParam},
      {"GetParent", &GetParent},
      {"GetPos", &GetPos},
      {"GetRep", &GetRep},
      {"GetRot", &GetRot},
      {"GetScale", &GetScale},
      {"GetSpriteId", &GetSpriteId},
      {"GetTexId", &GetTexId},
      {"GetType", &GetType},
      {"GetVisible", &GetVisible},
      {"GetWindowSize", &GetWindowSize},
      {"IsAnimPlaying", &IsAnimPlaying},
      {"KillAllChild", &KillAllChild},
      {"KillObj", &KillObj},
      {"OpenUrl", &OpenUrl},
      {"OutputDebugString", &OutputDebugString},
      {"PauseAnim", &PauseAnim},
      {"PickObj", &PickObj},
      {"PlayAnim", &PlayAnim},
      {"PlayPackage", &PlayPackage},
      {"SetBgColor", &SetBgColor},
      {"SetAnchor", &SetAnchor},
      {"SetDim", &SetDim},
      {"SetMapId", &SetMapId},
      {"SetName", &SetName},
      {"SetPos", &SetPos},
      {"SetRep", &SetRep},
      {"SetRot", &SetRot},
      {"SetScale", &SetScale},
      {"SetScript", &SetScript},
      {"SetSpriteId", &SetSpriteId},
      {"SetTexId", &SetTexId},
      {"SetVisible", &SetVisible},
      {"StopAnim", &StopAnim},
      { 0 }
    };

    lua_newtable(L);

    regCONST(L, GoodConst);
    regAPI(L, GoodApi);

    lua_setglobal(L, "Good");

    char p[] = "Good.Trace=function(...) Good.OutputDebugString(string.format(...)) end print=Good.Trace";
    int s = luaL_loadbuffer(L, p, strlen(p), "script_good.h");
    if (0 != s || 0 != lua_pcall(L, 0, LUA_MULTRET, 0)) {
      SW2_TRACE_ERROR("Install Good.Trace failed, %s\n", lua_tostring(L, -1));
    }

    return true;
  }

  //
  // Input API.
  //

  //
  // [in]  [out] x, y.
  //

  static int GetMousePos(lua_State* L)
  {
    AppT& app = AppT::getInst();

    lua_pushnumber(L, app.mMousePos.x);
    lua_pushnumber(L, app.mMousePos.y);

    return 2;
  }

  //
  // [in] code, [out] bool.
  //

  static int IsKeyDown(lua_State* L)
  {
    int code = luaL_checkint(L, 1);

    AppT& app = AppT::getInst();
    app.mHandledKeys |= code;

    lua_pushboolean(L, app.isKeyDown(code));

    return 1;
  }

  //
  // [in] code, [out] bool.
  //

  static int IsKeyPressed(lua_State* L)
  {
    int code = luaL_checkint(L, 1);

    AppT& app = AppT::getInst();
    app.mHandledKeys |= code;

    lua_pushboolean(L, app.isKeyPressed(code));

    return 1;
  }

  //
  // [in] code, [out] bool.
  //

  static int IsKeyPushed(lua_State* L)
  {
    int code = luaL_checkint(L, 1);

    AppT& app = AppT::getInst();
    app.mHandledKeys |= code;

    lua_pushboolean(L, app.isKeyPushed(code));

    return 1;
  }

  static bool installInputModuleApi(lua_State* L)
  {
    static RegConstType InputConst[] = {
      {"UP", (unsigned int)GOOD_KEYS_UP},
      {"DOWN", (unsigned int)GOOD_KEYS_DOWN},
      {"LEFT", (unsigned int)GOOD_KEYS_LEFT},
      {"RIGHT", (unsigned int)GOOD_KEYS_RIGHT},
      {"RETURN", (unsigned int)GOOD_KEYS_RETURN},
      {"BTN_A", (unsigned int)GOOD_KEYS_BTN_A},
      {"BTN_B", (unsigned int)GOOD_KEYS_BTN_B},
      {"LBUTTON", (unsigned int)GOOD_KEYS_LBUTTON},
      {"RBUTTON", (unsigned int)GOOD_KEYS_RBUTTON},
      {"ESCAPE", (unsigned int)GOOD_KEYS_ESCAPE},
      {"ANY", (unsigned int)GOOD_KEYS_ANY},
      { 0 }
    };

    static RegApiType InputApi[] = {
      {"GetMousePos", &GetMousePos},
      {"IsKeyDown", &IsKeyDown},
      {"IsKeyPressed", &IsKeyPressed},
      {"IsKeyPushed", &IsKeyPushed},
      { 0 }
    };

    lua_newtable(L);

    regCONST(L, InputConst);
    regAPI(L, InputApi);

    lua_setglobal(L, "Input");

    return true;
  }

  //
  // Resource API.
  //

  //
  // [in] [out] level id.
  //

  static int GetFirstLevelId(lua_State* L)
  {
    lua_pushinteger(L, AppT::getInst().getFirstResLevelId());

    return 1;
  }

  //
  // [in] idMap [out] tile cx, cy.
  //

  static int GetMapSize(lua_State* L)
  {
    int idMap = luaL_checkint(L, 1);

    lua_Integer cx = 0, cy = 0;
    AppT::getInst().getResMapSize(idMap, cx, cy);

    lua_pushinteger(L, cx);
    lua_pushinteger(L, cy);

    return 2;
  }

  //
  // [in] idLvl [out] level id.
  //

  static int GetNextLevelId(lua_State* L)
  {
    int idLvl = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getNextResLevelId(idLvl));

    return 1;
  }

  //
  // [in] name [out] level id.
  //

  static int GetResLevelId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResLevelId(name));

    return 1;
  }

  //
  // [in] name [out] map id.
  //

  static int GetResMapId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResMapId(name));

    return 1;
  }

  //
  // [in] name [out] sound id.
  //

  static int GetResSoundId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResSoundId(name));

    return 1;
  }

  //
  // [in] name [out] sprite id.
  //

  static int GetResSpriteId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResSpriteId(name));

    return 1;
  }

  //
  // [in] idSprite [out] sprite w h
  //

  static int GetResSpriteSize(lua_State* L)
  {
    int idSprite = luaL_checkint(L, 1);

    lua_Integer w = 0, h = 0;
    AppT::getInst().getResSpriteSize(idSprite, w, h);

    lua_pushinteger(L, w);
    lua_pushinteger(L, h);

    return 2;
  }

  //
  // [in] name [out] texture id.
  //

  static int GetResTexId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResTexId(name));

    return 1;
  }

  //
  // [in] idTex [out] w, h.
  //

  static int GetTexSize(lua_State* L)
  {
    int idTex = luaL_checkint(L, 1);

    lua_Integer w = 0, h = 0;
    AppT::getInst().getResTexSize(idTex, w, h);

    lua_pushinteger(L, w);
    lua_pushinteger(L, h);

    return 2;
  }

  //
  // [in] idMap, x, y [out] tile value.
  //

  static int GetTileByPos(lua_State* L)
  {
    int idMap = luaL_checkint(L, 1);
    int x = luaL_checkint(L, 2);
    int y = luaL_checkint(L, 3);

    lua_pushinteger(L, AppT::getInst().getResTileByPos(idMap, x, y));

    return 1;
  }

  //
  // [in] idMap [out] tile w, h.
  //

  static int GetTileSize(lua_State* L)
  {
    int idMap = luaL_checkint(L, 1);

    lua_Integer w = 0, h = 0;
    AppT::getInst().getResTileSize(idMap, w, h);

    lua_pushinteger(L, w);
    lua_pushinteger(L, h);

    return 2;
  }

  //
  // [in] idTex, x, y, idCanvas, sx, sy, sw, sh [out]
  //

  static int UpdateResTex(lua_State* L)
  {
    int idTex = luaL_checkint(L, 1);
    int x = luaL_checkint(L, 2);
    int y = luaL_checkint(L, 3);
    int idCanvas = luaL_checkint(L, 4);
    int sx = luaL_checkint(L, 5);
    int sy = luaL_checkint(L, 6);
    int sw  = luaL_checkint(L, 7);
    int sh = luaL_checkint(L, 8);

    AppT::getInst().updateResTex(idTex, x, y, idCanvas, sx, sy, sw, sh);

    return 0;
  }

  static bool installResourceModuleApi(lua_State* L)
  {
    static RegApiType ResourceApi[] = {
      {"GetFirstLevelId", &GetFirstLevelId},
      {"GetLevelId", &GetResLevelId},
      {"GetNextLevelId", &GetNextLevelId},
      {"GetMapId", &GetResMapId},
      {"GetMapSize", &GetMapSize},
      {"GetSoundId", &GetResSoundId},
      {"GetSpriteId", &GetResSpriteId},
      {"GetSpriteSize", &GetResSpriteSize},
      {"GetTexId", &GetResTexId},
      {"GetTexSize", &GetTexSize},
      {"GetTileByPos", &GetTileByPos},
      {"GetTileSize", &GetTileSize},
      {"UpdateTex", &UpdateResTex},
      { 0 }
    };

    lua_newtable(L);

    regAPI(L, ResourceApi);

    lua_setglobal(L, "Resource");

    return true;
  }

  //
  // Sound API.
  //

  //
  // [in] ,[out] volume.
  //

  static int GetMusicVolume(lua_State* L)
  {
    lua_pushnumber(L, SndT::getMusicVolume());

    return 1;
  }

  //
  // [in] idRes [out] idSnd.
  //

  static int GetSound(lua_State* L)
  {
    int idRes = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getSound(idRes));

    return 1;
  }

  //
  // [in] ,[out] volume.
  //

  static int GetSoundVolume(lua_State* L)
  {
    lua_pushnumber(L, SndT::getSoundVolume());

    return 1;
  }

  //
  // [in] idSnd [out]
  //

  static int KillSound(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    SndT::releaseSound(SndT(idSnd));

    return 0;
  }

  //
  // [in] idSnd [out] isLoop.
  //

  static int IsLoop(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    lua_pushboolean(L, SndT(idSnd).isLoop());

    return 1;
  }

  //
  // [in] idSnd [out] isPlaying.
  //

  static int IsPlaying(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    lua_pushboolean(L, SndT(idSnd).isPlaying());

    return 1;
  }

  //
  // [in] idSnd [out]
  //

  static int Pause(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    SndT(idSnd).pause();

    return 0;
  }

  //
  // [in] idSnd [out]
  //

  static int Play(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    SndT(idSnd).play();

    return 0;
  }

  //
  // [in] idRes [out] idSnd.
  //

  static int PlaySound(lua_State* L)
  {
    int idRes = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().playSound(idRes));

    return 1;
  }

  //
  // [in] idSnd, bLoop [out]
  //

  static int SetLoop(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    bool bLoop = 0 != luaL_checkint(L, 2);

    SndT(idSnd).setLoop(bLoop);

    return 0;
  }

  //
  // [in] volume [out]
  //

  static int SetMusicVolume(lua_State* L)
  {
    lua_Number vol = luaL_checknumber(L, 1);
    SndT::setMusicVolume((float)vol);

    return 0;
  }

  //
  // [in] volume [out]
  //

  static int SetSoundVolume(lua_State* L)
  {
    lua_Number vol = luaL_checknumber(L, 1);
    SndT::setSoundVolume((float)vol);

    return 0;
  }

  //
  // [in] idSnd [out]
  //

  static int Stop(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    SndT(idSnd).stop();

    return 0;
  }

  static bool installSoundModuleApi(lua_State* L)
  {
    static RegApiType SoundApi[] = {
      {"GetMusicVolume", &GetMusicVolume},
      {"GetSound", &GetSound},
      {"GetSoundVolume", &GetSoundVolume},
      {"IsLoop", &IsLoop},
      {"IsPlaying", &IsPlaying},
      {"KillSound", &KillSound},
      {"Pause", &Pause},
      {"Play", &Play},
      {"PlaySound", &PlaySound},
      {"SetLoop", &SetLoop},
      {"SetMusicVolume", &SetMusicVolume},
      {"SetSoundVolume", &SetSoundVolume},
      {"Stop", &Stop},
      { 0 }
    };

    lua_newtable(L);

    regAPI(L, SoundApi);

    lua_setglobal(L, "Sound");

    return true;
  }

  //
  // Graphics API.
  //

  //
  // [in] canvas, x, y, texId, srcx, srcy, srcw, srch[, color, rot, scalex, scaley] [out]
  //

  static int DrawImage(lua_State* L)
  {
    int canvas = luaL_checkint(L, 1);

    AppT& app = AppT::getInst();
    if (!app.mCanvas.isUsed(canvas) && !app.mRenderState) {
      return 0;
    }

    int x = luaL_checkint(L, 2);
    int y = luaL_checkint(L, 3);
    int texId = luaL_checkint(L, 4);
    int srcx = luaL_checkint(L, 5);
    int srcy = luaL_checkint(L, 6);
    int srcw = luaL_checkint(L, 7);
    int srch = luaL_checkint(L, 8);

    if (!app.mRes.isTex(texId)) {
      return 0;
    }

    unsigned int color = 0xffffffff;
    if (9 <= lua_gettop(L)) {
      color = (unsigned int)luaL_checknumber(L, 9);
    }

    lua_Number rot = 0;
    if (10 <= lua_gettop(L)) {
      rot = luaL_checknumber(L, 10);
    }

    lua_Number scalex = 1;
    if (11 <= lua_gettop(L)) {
      scalex = luaL_checknumber(L, 11);
    }

    lua_Number scaley = 1;
    if (12 <= lua_gettop(L)) {
      scaley = luaL_checknumber(L, 12);
    }

    app.drawImage(canvas, x, y, texId, srcx, srcy, srcw, srch, color, (float)rot, (float)scalex, (float)scaley);

    return 0;
  }

  //
  // [in] canvas, x, y, text[, size, color] [out]
  //

  static int DrawText(lua_State* L)
  {
    int canvas = luaL_checkint(L, 1);

    AppT& app = AppT::getInst();
    if (!app.mCanvas.isUsed(canvas) && !app.mRenderState) {
      return 0;
    }

    int x = luaL_checkint(L, 2);
    int y = luaL_checkint(L, 3);
    const char* utf8text = luaL_checkstring(L, 4);

    int size = GOOD_DEFAULT_TEXT_SIZE;
    if (5 <= lua_gettop(L)) {
      size = luaL_checkint(L, 5);
    }

    unsigned int color = 0xffffffff;
    if (6 <= lua_gettop(L)) {
      color = (unsigned int)luaL_checknumber(L, 6);
    }

    app.drawText(canvas, x, y, utf8text, size, color);

    return 0;
  }

  //
  // [in] canvas, x, y, w, h, color[, rot, scalex, scaley] [out]
  //

  static int FillRect(lua_State* L)
  {
    int canvas = luaL_checkint(L, 1);

    AppT& app = AppT::getInst();
    if (!app.mCanvas.isUsed(canvas) && !app.mRenderState) {
      return 0;
    }

    int x = luaL_checkint(L, 2);
    int y = luaL_checkint(L, 3);
    int w = luaL_checkint(L, 4);
    int h = luaL_checkint(L, 5);
    unsigned int color = (unsigned int)luaL_checknumber(L, 6);

    lua_Number rot = 0;
    if (7 <= lua_gettop(L)) {
      rot = luaL_checknumber(L, 7);
    }

    lua_Number scalex = 1;
    if (8 <= lua_gettop(L)) {
      scalex = luaL_checknumber(L, 8);
    }

    lua_Number scaley = 1;
    if (9 <= lua_gettop(L)) {
      scaley = luaL_checknumber(L, 9);
    }

    app.fillRect(canvas, x, y, w, h, color, (float)rot, (float)scalex, (float)scaley);

    return 0;
  }

  //
  // [in] w, h [out] id
  //

  static int GenCanvas(lua_State* L)
  {
    int w = luaL_checkint(L, 1);
    int h = luaL_checkint(L, 2);

    lua_pushinteger(L, AppT::getInst().genCanvas(w, h));

    return 1;
  }

  //
  // [in] [out] bAntiAlias
  //

  static int GetAntiAlias(lua_State* L)
  {
    lua_pushinteger(L, AppT::getInst().mAntiAlias ? 1 : 0);

    return 1;
  }

  //
  // [in] id [out]
  //

  static int KillCanvas(lua_State* L)
  {
    int id = luaL_checkint(L, 1);

    AppT::getInst().killCanvas(id);

    return 0;
  }

  //
  // [in] xAnchor, yAnchor [out]
  //

  static int GraphicsSetAnchor(lua_State* L)
  {
    AppT& app = AppT::getInst();
    if (!app.mRenderState) {
      return 0;
    }

    float xAnchor = (float)luaL_checknumber(L, 1);
    float yAnchor = (float)luaL_checknumber(L, 2);

    app.gx.setAnchor(xAnchor, yAnchor);

    return 0;
  }

  //
  // [in] bAntiAlias [out]
  //

  static int SetAntiAlias(lua_State* L)
  {
    bool bAntiAlias = 0 != luaL_checkint(L, 1);

    AppT::getInst().setAntiAlias(bAntiAlias);

    return 0;
  }

  static bool installGraphicsModuleApi(lua_State* L)
  {
    static RegApiType GraphicsApi[] = {
      {"DrawImage", &DrawImage},
      {"DrawText", &DrawText},
      {"FillRect", &FillRect},
      {"GenCanvas", &GenCanvas},
      {"GetAntiAlias", &GetAntiAlias},
      {"KillCanvas", &KillCanvas},
      {"SetAnchor", &GraphicsSetAnchor},
      {"SetAntiAlias", &SetAntiAlias},
      { 0 }
    };

    lua_newtable(L);

    regAPI(L, GraphicsApi);

    lua_setglobal(L, "Graphics");

    return true;
  }

  //
  // STGE API.
  //

#ifdef GOOD_SUPPORT_STGE

  //
  // [in] idParticle, idObj[, iObjMgr] [out]
  //

  static int BindParticle(lua_State* L)
  {
    int idParticle = luaL_checkint(L, 1);
    int idObj = luaL_checkint(L, 2);

    int iObjMgr = 0;
    if (3 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 3);
    }

    AppT::getInst().bindParticle(idParticle, idObj, iObjMgr);

    return 0;
  }

  //
  // [in] idTask, idObj[, iObjMgr] [out]
  //

  static int BindTask(lua_State* L)
  {
    int idTask = luaL_checkint(L, 1);
    int idObj = luaL_checkint(L, 2);

    int iObjMgr = 0;
    if (3 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 3);
    }

    AppT::getInst().bindTask(idTask, idObj, iObjMgr);

    return 0;
  }

  //
  // [in] idParticle[, iObjMgr] [out] dir.
  //

  static int GetParticleDirection(lua_State* L)
  {
    int idParticle = luaL_checkint(L, 1);

    int iObjMgr = 0;
    if (2 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 2);
    }

    lua_pushnumber(L, AppT::getInst().getParticleDirection(idParticle, iObjMgr));

    return 1;
  }

  //
  // [in] [iObjMgr] [out] id.
  //

  static int GetFirstParticle(lua_State* L)
  {
    int iObjMgr = 0;
    if (1 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 1);
    }

    lua_pushinteger(L, AppT::getInst().getFirstParticle(iObjMgr));

    return 1;
  }

  //
  // [in] [iObjMgr] [out] id.
  //

  static int GetFirstTask(lua_State* L)
  {
    int iObjMgr = 0;
    if (1 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 1);
    }

    lua_pushinteger(L, AppT::getInst().getFirstTask(iObjMgr));

    return 1;
  }

  //
  // [in] idCur[, iObjMgr] [out] id.
  //

  static int GetNextParticle(lua_State* L)
  {
    int idCur = luaL_checkint(L, 1);

    int iObjMgr = 0;
    if (2 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 2);
    }

    lua_pushinteger(L, AppT::getInst().getNextParticle(idCur, iObjMgr));

    return 1;
  }

  //
  // [in] idCur[, iObjMgr] [out] id.
  //

  static int GetNextTask(lua_State* L)
  {
    int idCur = luaL_checkint(L, 1);

    int iObjMgr = 0;
    if (2 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 2);
    }

    lua_pushinteger(L, AppT::getInst().getNextTask(idCur, iObjMgr));

    return 1;
  }

  //
  // [in] idParticle[, iObjMgr] [out] idObj.
  //

  static int GetParticleBind(lua_State* L)
  {
    int idParticle = luaL_checkint(L, 1);

    int iObjMgr = 0;
    if (2 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 2);
    }

    lua_pushinteger(L, AppT::getInst().getParticleBind(idParticle, iObjMgr));

    return 1;
  }

  //
  // [in] [iObjMgr] [out] count.
  //

  static int GetParticleCount(lua_State* L)
  {
    int iObjMgr = 0;
    if (1 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 1);
    }

    lua_pushinteger(L, AppT::getInst().getParticleCount(iObjMgr));

    return 1;
  }

  //
  // [in] idParticle[, iObjMgr] [out] x, y.
  //

  static int GetParticlePos(lua_State* L)
  {
    int idParticle = luaL_checkint(L, 1);

    int iObjMgr = 0;
    if (2 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 2);
    }

    float x = .0f, y = .0f;
    AppT::getInst().getParticlePos(idParticle, x, y, iObjMgr);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    return 2;
  }

  //
  // [in] [out] idPlayer.
  //

  static int GetPlayer(lua_State* L)
  {
    lua_pushinteger(L, AppT::getInst().mGoodPlayer);

    return 1;
  }

  //
  // [in] idTask[, iObjMgr] [out] idObj.
  //

  static int GetTaskBind(lua_State* L)
  {
    int idTask = luaL_checkint(L, 1);

    int iObjMgr = 0;
    if (2 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 2);
    }

    lua_pushinteger(L, AppT::getInst().getTaskBind(idTask, iObjMgr));

    return 1;
  }

  //
  // [in] [iObjMgr] [out] count.
  //

  static int GetTaskCount(lua_State* L)
  {
    int iObjMgr = 0;
    if (1 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 1);
    }

    lua_pushinteger(L, AppT::getInst().getTaskCount(iObjMgr));

    return 1;
  }

  //
  // [in] idParticle, index[, iObjMgr] [out] data.
  //

  static int GetUserData(lua_State* L)
  {
    int idParticle = luaL_checkint(L, 1);
    int index = luaL_checkint(L, 2);

    int iObjMgr = 0;
    if (3 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 3);
    }

    lua_pushnumber(L, AppT::getInst().getUserData(idParticle, index, iObjMgr));

    return 1;
  }

  //
  // [in] [iObjMgr] [out]
  //

  static int IsActive(lua_State* L)
  {
    int iObjMgr = 0;
    if (1 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 1);
    }

    lua_pushboolean(L, AppT::getInst().isStgeActive(iObjMgr));

    return 1;
  }

  //
  // [in] [iObjMgr] [out]
  //

  static int KillAllParticle(lua_State* L)
  {
    int iObjMgr = 0;
    if (1 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 1);
    }

    AppT::getInst().killAllParticle(iObjMgr);

    return 0;
  }

  //
  // [in] [iObjMgr] [out]
  //

  static int KillAllTask(lua_State* L)
  {
    int iObjMgr = 0;
    if (1 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 1);
    }

    AppT::getInst().killAllTask(iObjMgr);

    return 0;
  }

  //
  // [in] idParticle[, iObjMgr] [out]
  //

  static int KillParticle(lua_State* L)
  {
    int idParticle = luaL_checkint(L, 1);

    int iObjMgr = 0;
    if (2 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 2);
    }

    AppT::getInst().killParticle(idParticle, iObjMgr);

    return 0;
  }

  //
  // [in] idTask[, iObjMgr] [out]
  //

  static int KillTask(lua_State* L)
  {
    int idTask = luaL_checkint(L, 1);

    int iObjMgr = 0;
    if (2 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 2);
    }

    AppT::getInst().killTask(idTask, iObjMgr);

    return 0;
  }

  //
  // [in] script[, x, y, iObjMgr] [out]
  //

  static int RunScript(lua_State* L)
  {
    const char* script = luaL_checkstring(L, 1);

    float x = .0f, y = .0f;

    if (2 <= lua_gettop(L)) {
      x = (float)luaL_checknumber(L, 2);
    }

    if (3 <= lua_gettop(L)) {
      y = (float)luaL_checknumber(L, 3);
    }

    int iObjMgr = 0;
    if (4 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 4);
    }

    lua_pushinteger(L, AppT::getInst().runStgeScript(script, 2 <= lua_gettop(L) ? &x : 0, 3 <= lua_gettop(L) ? &y : 0, iObjMgr));

    return 1;
  }

  //
  // [in] bActive[, iObjMgr] [out]
  //

  static int SetActive(lua_State* L)
  {
    bool bActive = 0 != luaL_checkint(L, 1);

    int iObjMgr = 0;
    if (2 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 2);
    }

    AppT::getInst().setStgeActive(bActive, iObjMgr);

    return 0;
  }

  //
  // [in] idParticle, dir[, iObjMgr] [out]
  //

  static int SetParticleDirection(lua_State* L)
  {
    int idParticle = luaL_checkint(L, 1);
    lua_Number newdir = luaL_checknumber(L, 2);

    int iObjMgr = 0;
    if (3 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 3);
    }

    AppT::getInst().setParticleDirection(idParticle, (float)newdir, iObjMgr);

    return 0;
  }

  //
  // [in] idParticle, x, y[, iObjMgr] [out]
  //

  static int SetParticlePos(lua_State* L)
  {
    int idParticle = luaL_checkint(L, 1);
    lua_Number newx = luaL_checknumber(L, 2);
    lua_Number newy = luaL_checknumber(L, 3);

    int iObjMgr = 0;
    if (4 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 4);
    }

    AppT::getInst().setParticlePos(idParticle, (float)newx, (float)newy, iObjMgr);

    return 0;
  }

  //
  // [in] idPlayer [out]
  //

  static int SetPlayer(lua_State* L)
  {
    int idPlayer = luaL_checkint(L, 1);

    AppT::getInst().setPlayer(idPlayer);

    return 0;
  }

  //
  // [in] idTask, dir[, iObjMgr] [out]
  //

  static int SetTaskDirection(lua_State* L)
  {
    int idTask = luaL_checkint(L, 1);
    lua_Number dir = luaL_checknumber(L, 2);

    int iObjMgr = 0;
    if (3 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 3);
    }

    AppT::getInst().setTaskDirection(idTask, (float)dir, iObjMgr);

    return 0;
  }

  //
  // [in] idParticle, index, data[, iObjMgr] [out]
  //

  static int SetUserData(lua_State* L)
  {
    int idParticle = luaL_checkint(L, 1);
    int index = luaL_checkint(L, 2);
    lua_Number data = luaL_checknumber(L, 3);

    int iObjMgr = 0;
    if (4 <= lua_gettop(L)) {
      iObjMgr = luaL_checkint(L, 4);
    }

    AppT::getInst().setUserData(idParticle, index, (float)data, iObjMgr);

    return 0;
  }

  static bool installStgeModuleApi(lua_State* L)
  {
    static RegApiType StgeApi[] = {
      {"BindParticle", &BindParticle},
      {"BindTask", &BindTask},
      {"GetDirection", &GetParticleDirection},
      {"GetFirstParticle", &GetFirstParticle},
      {"GetFirstTask", &GetFirstTask},
      {"GetNextParticle", &GetNextParticle},
      {"GetNextTask", &GetNextTask},
      {"GetParticleBind", &GetParticleBind},
      {"GetParticleCount", &GetParticleCount},
      {"GetPlayer", &GetPlayer},
      {"GetPos", &GetParticlePos},
      {"GetTaskBind", &GetTaskBind},
      {"GetTaskCount", &GetTaskCount},
      {"GetUserData", &GetUserData},
      {"IsActive", &IsActive},
      {"KillAllParticle", &KillAllParticle},
      {"KillAllTask", &KillAllTask},
      {"KillParticle", &KillParticle},
      {"KillTask", &KillTask},
      {"RunScript", &RunScript},
      {"SetActive", &SetActive},
      {"SetDirection", &SetParticleDirection},
      {"SetPlayer", &SetPlayer},
      {"SetPos", &SetParticlePos},
      {"SetTaskDirection", &SetTaskDirection},
      {"SetUserData", &SetUserData},
      { 0 }
    };

    lua_newtable(L);

    regAPI(L, StgeApi);

    lua_setglobal(L, "Stge");

    return true;
  }
#endif // GOOD_SUPPORT_STGE

};

} // namespace rt

} // namespace good

// end of script.h
