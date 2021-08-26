
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
  // [Module] Good
  //

  //
  // [API]
  // "name":"AddChild",
  // "desc":"Add an object as child object to an object.",
  // "proto":"void AddChild(idParent, idChild[, index])",
  // "param":[{
  //   "name":"idParent",
  //   "desc":"Parent object ID. -1 as level object ID."
  // },{
  //   "name":"idChild",
  //   "desc":"Child object ID."
  // },{
  //   "name":"index(optional)",
  //   "desc":"Insert position."
  // }],
  // "ret":"n/a",
  // "ex":"function AcGenSmokeObj(param)<br/>
  //   local o = Good.GenObj(-1, 1, \'AnimSmokeObj\')<br/>
  //   Good.SetAnchor(o, .5, .5)<br/>
  //   local x,y = Good.GetPos(param._id, 1)<br/>
  //   Good.SetPos(o, x, y)<br/>
  //   Good.AddChild(-1, o)                  -- Make topmost.<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"CallPackage",
  // "desc":"Call a good package.",
  // "proto":"void CallPackage(name)",
  // "param":[{
  //   "name":"name",
  //   "desc":"Package name."
  // }],
  // "ret":"n/a",
  // "ex":"Level.OnStep = function(param)<br/>
  //   if (Input.IsKeyPressed(Input.LBUTTON)) then<br/>
  //     Good.CallPackage(\'breakout.txt\')<br/>
  //   end<br/>
  // end"
  // [/API]
  //

  static int CallPackage(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    AppT::getInst().callPackage(name);

    return 0;
  }

  //
  // [API]
  // "name":"Exit",
  // "desc":"Exit and terminate game application.",
  // "proto":"void Exit()",
  // "ret":"n/a",
  // "ex":"if (Input.IsKeyPushed(Input.ESCAPE)) then<br/>
  //   Good.Exit()<br/>
  // end"
  // [/API]
  //

  static int Exit(lua_State* L)
  {
    AppT::getInst().exit();

    return 0;
  }

  //
  // [API]
  // "name":"FindChild",
  // "desc":"Find object with a name.",
  // "proto":"int FindChild(idObj, name[, recursive])",
  // "param":[{
  //    "name":"idObj",
  //    "desc":"The object ID the find child."
  //  },{
  //    "name":"name",
  //    "desc":"Name of child object."
  //  },{
  //    "name":"recursive(optional)",
  //    "desc":"1 to recursive search child objects. Default is 0."
  //  }],
  // "ret":"Return an object ID if found else return -1. Note name of objects may duplicate therefore the function returns first found object ID.",
  // "ex":"local btn_quit = Good.FindChild(menu_obj, \'quit game\')"
  // [/API]
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
  // [API]
  // "name":"FireUserIntEvent",
  // "desc":"Fire a user defined integer event to application.",
  // "proto":"void FireUserIntEvent(i)",
  // "param":[{
  //   "name":"i",
  //   "desc":"A number to notify to game application."
  // }],
  // "ret":"n/a",
  // "ex":"local CMD_SEL_COIN_EATER = 6<br/>
  // local id = param._id<br/>
  // Good.FireUserIntEvent(CMD_SEL_COIN_EATER + 256 * Good.GetTexId(id))"
  // [/API]
  //

  static int FireUserIntEvent(lua_State* L)
  {
    int i = luaL_checkint(L, 1);

    AppT::getInst().doUserIntEvent(i);

    return 0;
  }

  //
  // [API]
  // "name":"GenDummy",
  // "desc":"Create a new dummy object.",
  // "proto":"int GenDummy(idParent[, script])",
  // "param":[{
  //   "name":"idParent",
  //   "desc":"Parent object ID. -1 as level object ID."
  // },{
  //   "name":"script(optional)",
  //   "desc":"Script handler."
  // }],
  // "ret":"Return a new created object ID else return -1.",
  // "ex":"function UpdateStatistics()<br/>
  //   local msg_dummy = Good.FindChild(menu_obj, \'msg dummy\')<br/>
  //   Good.KillObj(Good.GetChild(msg_dummy, 0)) -- First child is the dummy of msgs.<br/>
  //   local dummy = Good.GenDummy(msg_dummy)<br/>
  //   GenKillsInfo(dummy)<br/>
  //   GenStatsInfo(dummy)<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"GenObj",
  // "desc":"Create a new object by a resource ID.",
  // "proto":"int GenObj(idParent, idRes[, script])",
  // "param":[{
  //   "name":"idParent",
  //   "desc":"Parent object ID. -1 as level object ID."
  // },{
  //   "name":"idRes",
  //   "desc":"Resource ID. -1 to create a color block object."
  // },{
  //   "name":"script(optional)",
  //   "desc":"Script handler."
  // }],
  // "ret":"Return a new created object ID else return -1.",
  // "ex":"function AcGenSmokeObj(param)<br/>
  //   local o = Good.GenObj(-1, 1, \'AnimSmokeObj\')<br/>
  //   Good.SetAnchor(o, .5, .5)<br/>
  //   local x,y = Good.GetPos(param._id, 1)<br/>
  //   Good.SetPos(o, x, y)<br/>
  //   Good.AddChild(-1, o)                  -- Make topmost.<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"GenObjEx",
  // "desc":"Create a new object by an extern package resource.",
  // "proto":"int GenObjEx(pkg, idParent, idRes[, script])",
  // "param":[{
  //   "name":"pkg",
  //   "desc":"Package name."
  // },{
  //   "name":"idParent",
  //   "desc":"Parent object ID. -1 as level object ID."
  // },{
  //   "name":"idRes",
  //   "desc":"Resource ID. -1 to create a color block object."
  // },{
  //   "name":"script(optional)",
  //   "desc":"Script handler."
  // }],
  // "ret":"Return a new created object ID else return -1.",
  // "ex":"local b = Good.GenObjEx(\'test4.good\', -1, 45)<br/>
  // Good.SetPos(b, 200,200)"
  // [/API]
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
  // [API]
  // "name":"GenTextObj",
  // "desc":"Create a new text object by a text string.",
  // "proto":"int GenTextObj(idParent, text[, size, script])",
  // "param":[{
  //   "name":"idParent",
  //   "desc":"Parent object ID. -1 as level object ID."
  // },{
  //   "name":"text",
  //   "desc":"A text string."
  // },{
  //   "name":"size(optional)",
  //   "desc":"String char size. Default is 16."
  // },{
  //   "name":"script(optional)",
  //   "desc":"Script handler."
  // }],
  // "ret":"Return a new created text object ID else return -1. Note the new created object is a dummy object, and with each char as a child objects.",
  // "ex":"local s = string.format(\'%d\', combat_power)<br/>
  // local sobj = Good.GenTextObj(bg, s, CITY_LABLE_TEXT_SIZE)<br/>
  // local w = GetTextObjWidth(sobj)<br/>
  // Good.SetPos(sobj, (CITY_LABLE_W - w)/2, (CITY_LABLE_H - CITY_LABLE_TEXT_SIZE)/2)"
  // [/API]
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
  // [API]
  // "name":"GetAnchor",
  // "desc":"Get anchor of an object.",
  // "proto":"number,number GetAnchor(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return horzontal and verticle anchor of the object else return 0,0. Value of anchor if between 0..1. The anchor of top-left corner of an object is [0,0].",
  // "ex":"local ax,ay = Good.GetAnchor(o)"
  // [/API]
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
  // [API]
  // "name":"GetBgColor",
  // "desc":"Get color of an object.",
  // "proto":"number GetBgColor(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return color of the object in ARGB format else return 0.",
  // "ex":"local clr = Good.GetBgColor(a)<br/>
  // Good.SetBgColor(b, clr)"
  // [/API]
  //

  static int GetBgColor(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushnumber(L, AppT::getInst().getBgColor(idObj));

    return 1;
  }

  //
  // [API]
  // "name":"GetChild",
  // "desc":"Get child object of an object.",
  // "proto":"int GetChild(idObj, index)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"index",
  //   "desc":"Index of child object."
  // }],
  // "ret":"Return child object ID of the object else return -1.",
  // "ex":"local bg = Good.GetChild(o, 0)<br/>
  // Good.GetParam(bg).new_clr = new_clr<br/>
  // Good.SetScript(bg, \'AnimSetCityColor\')"
  // [/API]
  //

  static int GetChild(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    int idx = luaL_checkint(L, 2);

    lua_pushinteger(L, AppT::getInst().getChild(idObj, idx));

    return 1;
  }

  //
  // [API]
  // "name":"GetChildCount",
  // "desc":"Get child count of an object.",
  // "proto":"int GetChildCount(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return number of child of the object else return 0.",
  // "ex":"local c = Good.GetChildCount(dummy_group_id)<br/>
  // for i = 0, c - 1 do<br/>
  //   local o = Good.GetChild(dummy_group_id, i)<br/>
  //   Good.SetAnchor(o, 0.5, 0.5)<br/>
  // end"
  // [/API]
  //

  static int GetChildCount(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getChildCount(idObj));

    return 1;
  }

  //
  // [API]
  // "name":"GetDim",
  // "desc":"Get dimension of an object.",
  // "proto":"int,int,int,int GetDim(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return dimension of the object else return 0,0,0,0. Object dimension consists of top-left corner coordinate, width and height. According to the type of object, the definition of object dimension is different.",
  // "ex":"function PtInObj(mx, my, o)<br/>
  //   local l,t,w,h = Good.GetDim(o)<br/>
  //   local x, y = Good.GetPos(o)<br/>
  //   return PtInRect(mx, my, x, y, x + w, y + h)<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"GetLevelId",
  // "desc":"Get current level object ID.",
  // "proto":"int GetLevelId()",
  // "ret":"Return current level object ID.",
  // "ex":"local lvl_param = Good.GetParam(Good.GetLevelId())"
  // [/API]
  //

  static int GetLevelId(lua_State* L)
  {
    lua_pushinteger(L, AppT::getInst().mRoot);

    return 1;
  }

  //
  // [API]
  // "name":"GetMapId",
  // "desc":"Get map resource ID of an object.",
  // "proto":"int GetMapId(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return map resource ID of the object else return -1.",
  // "ex":"local tilex, tiley = Resource.GetTileSize(Good.GetMapId(map))"
  // [/API]
  //

  static int GetMapId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getMapId(idObj));

    return 1;
  }

  //
  // [API]
  // "name":"GetName",
  // "desc":"Get name of an object.",
  // "proto":"string GetName(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return name of the object else return empty string.",
  // "ex":"function GetCityId(o)</br>
  //   return tonumber(Good.GetName(o))</br>
  // end"
  // [/API]
  //

  static int GetName(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushstring(L, AppT::getInst().getName(idObj).c_str());

    return 1;
  }

  //
  // [API]
  // "name":"GetParam",
  // "desc":"Get param of an object.",
  // "proto":"table GetParam(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return param of the object else return nil.",
  // "ex":"local pp = Good.GetParam(param.player)<br/>
  // if (-1 ~= pp.movefx) then<br/>
  //   Stge.KillTask(pp.movefx, OM_FX)<br/>
  // end<br/>
  // if (-1 ~= pp.fire) then<br/>
  //   Stge.KillTask(pp.fire, OM_MYBULLET)<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"GetParent",
  // "desc":"Get parent object ID of an object.",
  // "proto":"int GetParent(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return parent object ID of the object else return -1.",
  // "ex":"local dummy = Good.GetParent(param._id)<br/>
  // Good.KillObj(dummy)"
  // [/API]
  //

  static int GetParent(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getParent(idObj));

    return 1;
  }

  //
  // [API]
  // "name":"GetPos",
  // "desc":"Get position of an object.",
  // "proto":"number,number GetPos(idObj[, world])",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"world(optional)",
  //   "desc":"1 for world position else 0 for local position. Default is 0."
  // }],
  // "ret":"Return x and y position of the object else return 0,0.",
  // "ex":"local x,y = Good.GetPos(id)<br/>
  // x = x + param.dirx<br/>
  // y = y + param.diry<br/>
  // Good.SetPos(id, x, y)"
  // [/API]
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
  // [API]
  // "name":"GetRep",
  // "desc":"Get repeat mode of an object.",
  // "proto":"bool,bool GetRep(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return horzontal and verticle repeat mode of the object else return false,fales.",
  // "ex":"local xrep,yrep = Good.GetRep(o)"
  // [/API]
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
  // [API]
  // "name":"GetRot",
  // "desc":"Get rotation of an object.",
  // "proto":"number GetRot(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return rotation value in degree of the object else return 0.",
  // "ex":"local rot = Good.GetRot(o)"
  // [/API]
  //

  static int GetRot(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushnumber(L, AppT::getInst().getRot(idObj));

    return 1;
  }

  //
  // [API]
  // "name":"GetScale",
  // "desc":"Get scale of an object.",
  // "proto":"number,number GetScale(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return horzontal and verticle scale factor of the object else return 0,0.",
  // "ex":"local sx,sh = Good.GetScale(o)"
  // [/API]
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
  // [API]
  // "name":"GetSpriteId",
  // "desc":"Get sprite resource ID of an object.",
  // "proto":"int GetSpriteId(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return sprite resource ID of the object else return -1.",
  // "ex":"local dir = Good.GetSpriteId(id)"
  // [/API]
  //

  static int GetSpriteId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getSpriteId(idObj));

    return 1;
  }

  //
  // [API]
  // "name":"GetTexId",
  // "desc":"Get texture resource ID of an object.",
  // "proto":"int GetTexId(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return texture resource ID of the object else return -1.",
  // "ex":"local o = Good.GetTexId(Good.PickObj(x, y, Good.TEXBG))<br/>
  // if (texCow == o) then<br/>
  //   Sound.PlaySound(sndCow)<br/>
  // end"
  // [/API]
  //

  static int GetTexId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getTexId(idObj));

    return 1;
  }

  //
  // [API]
  // "name":"GetType",
  // "desc":"Get object type of specified object.",
  // "proto":"int GetType(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return type of the object else return -1.",
  // "ex":"if (Good.SPRITE == Good.GetType(o)) then<br/>
  //   Good.PlayAnim(o)<br/>
  // end"
  // [/API]
  //

  static int GetType(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getType(idObj));

    return 1;
  }

  //
  // [API]
  // "name":"GetVisible",
  // "desc":"Get visible property of an object.",
  // "proto":"int GetVisible(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return 1 if is visible else return 0.",
  // "ex":"if (0 == Good.GetVisible(btnGo)) then<br/>
  //   return<br/>
  // end"
  // [/API]
  //

  static int GetVisible(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getVisible(idObj) ? 1 : 0);

    return 1;
  }

  //
  // [API]
  // "name":"GetWindowSize",
  // "desc":"Get size of application window.",
  // "proto":"int,int GetWindowSize()",
  // "ret":"Return width and height in pixel of application window.",
  // "ex":"local SCREEN_W, SCREEN_H = Good.GetWindowSize()"
  // [/API]
  //

  static int GetWindowSize(lua_State* L)
  {
    AppT const& app = AppT::getInst();

    lua_pushinteger(L, app.mRes.mWidth);
    lua_pushinteger(L, app.mRes.mHeight);

    return 2;
  }

  //
  // [API]
  // "name":"IsAnimPlaying",
  // "desc":"Get sprite animation playing property of an object.",
  // "proto":"bool IsAnimPlaying(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"Return true if animation is playing else return false.",
  // "ex":"local anim = Good.IsAnimPlaying(o)"
  // [/API]
  //

  static int IsAnimPlaying(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    lua_pushboolean(L, AppT::getInst().isAnimPlaying(idObj));

    return 1;
  }

  //
  // [API]
  // "name":"KillAllChild",
  // "desc":"Destroy all child objects of an object.",
  // "proto":"void KillAllChild(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"n/a",
  // "ex":"Good.KillAllChild(o)"
  // [/API]
  //

  static int KillAllChild(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    AppT::getInst().killAllChild(idObj);

    return 0;
  }

  //
  // [API]
  // "name":"KillObj",
  // "desc":"Destroy an object.",
  // "proto":"void KillObj(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"n/a",
  // "ex":"if (nil ~= coin_obj) then<br/>  Good.KillObj(coin_obj)<br/>  coin_obj = nil<br/>end"
  // [/API]
  //

  static int KillObj(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    AppT::getInst().killObj(idObj);

    return 0;
  }

  //
  // [API]
  // "name":"OpenUrl",
  // "desc":"Open a URL.",
  // "proto":"void OpenUrl(url)",
  // "param":[{
  //   "name":"url",
  //   "desc":"The URL to open."
  // }],
  // "ret":"n/a",
  // "ex":"Good.OpenUrl(\'good-ed.blogspot.com\')"
  // [/API]
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
  // [API]
  // "name":"PauseAnim",
  // "desc":"Pause sprite animtion of an object.",
  // "proto":"void PauseAnim(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"n/a",
  // "ex":"Good.PauseAnim(Stge.GetParticleBind(id, iMgr))"
  // [/API]
  //

  static int PauseAnim(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    AppT::getInst().pauseAnim(idObj);

    return 0;
  }

  //
  // [API]
  // "name":"PickObj",
  // "desc":"Pick object by specified position.",
  // "proto":"int PickObj(x, y[, type, idRes, idResXor])",
  // "param":[{
  //   "name":"x",
  //   "desc":"X coordinate to pick."
  // },{
  //   "name":"y",
  //   "desc":"Y coordinate to pick."
  // },{
  //   "name":"type(optional)",
  //   "desc":"Object type filter."
  // },{
  //   "name":"idRes(optional)",
  //   "desc":"Resource ID filter."
  // },{
  //   "name":"idResXor(optional)",
  //   "desc":"Exclusive resource ID filter."
  // }],
  // "ret":"Return picked object ID else return -1.",
  // "ex":"local hit = Good.PickObj(x, y, Good.SPRITE, grass)<br/>
  // if (0 >= hit) then<br/>
  //   gameIsOver(param, x, y)<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"PlayAnim",
  // "desc":"Start playing sprite animtion of an object.",
  // "proto":"void PlayAnim(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"n/a",
  // "ex":"Good.PlayAnim(Stge.GetParticleBind(id, iMgr))"
  // [/API]
  //

  static int PlayAnim(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);

    AppT::getInst().playAnim(idObj);

    return 0;
  }

  //
  // [API]
  // "name":"PlayPackage",
  // "desc":"Play a good package.",
  // "proto":"void PlayPackage(name)",
  // "param":[{
  //   "name":"name",
  //   "desc":"Package name."
  // }],
  // "ret":"n/a",
  // "ex":"Good.PlayPackage(\'menu.good\')"
  // [/API]
  //

  static int PlayPackage(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    AppT::getInst().playPackage(name);

    return 0;
  }

  //
  // [API]
  // "name":"SetAnchor",
  // "desc":"Set anchor of an object.",
  // "proto":"void SetAnchor(idObj, xAnchor, yAnchor)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"xAnchor",
  //   "desc":"Horizontal anchor value, between 0..1."
  // },{
  //   "name":"yAnchor",
  //   "desc":"Vertical anchor value, between 0..1."
  // }],
  // "ret":"n/a",
  // "ex":"Good.SetAnchor(o, 0.5, 0.5)"
  // [/API]
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
  // [API]
  // "name":"SetBgColor",
  // "desc":"Set color of an object.",
  // "proto":"void SetBgColor(idObj, color)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"color",
  //   "desc":"New ARGB color."
  // }],
  // "ret":"n/a",
  // "ex":"Good.SetBgColor(curr_lvl_id, LerpARgb(fade_from, fade_to, t))"
  // [/API]
  //

  static int SetBgColor(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    unsigned int newclr = (unsigned int)luaL_checknumber(L, 2);

    AppT::getInst().setBgColor(idObj, newclr);

    return 0;
  }

  //
  // [API]
  // "name":"SetDim",
  // "desc":"Set dimension of an object.",
  // "proto":"void SetDim(idObj, left, top, width, height)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"left",
  //   "desc":"Left param of dimension."
  // },{
  //   "name":"top",
  //   "desc":"Top param of dimension."
  // },{
  //   "name":"width",
  //   "desc":"Width param of dimension."
  // },{
  //   "name":"height",
  //   "desc":"Height param of dimension."
  // }],
  // "ret":"n/a",
  // "ex":"local function RandDice(o)<br/>
  //   local n = math.random(1, 6)<br/>
  //   Good.SetDim(o, DICE_SZ * (n - 1), 0, DICE_SZ, DICE_SZ)<br/>
  //   return n<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"SetMapId",
  // "desc":"Set map resource ID of an object.",
  // "proto":"void SetMapId(idObj, idMap)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"idMap",
  //   "desc":"Map resource ID."
  // }],
  // "ret":"n/a",
  // "ex":"Good.SetMapId(o, Resource.GetMapId(\'map1\'))"
  // [/API]
  //

  static int SetMapId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    int idMap = luaL_checkint(L, 2);

    AppT::getInst().setMapId(idObj, idMap);

    return 0;
  }

  //
  // [API]
  // "name":"SetName",
  // "desc":"Set name of an object.",
  // "proto":"void SetName(idObj, name)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"name",
  //   "desc":"New object name to set"
  // }],
  // "ret":"n/a",
  // "ex":"Good.SetName(o, tostring(id))"
  // [/API]
  //

  static int SetName(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    const char* name = luaL_checkstring(L, 2);

    AppT::getInst().setName(idObj, name);

    return 0;
  }

  //
  // [API]
  // "name":"SetPos",
  // "desc":"Set position of an object.",
  // "proto":"void SetPos(idObj, x, y)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"x",
  //   "desc":"New x coordinate."
  // },{
  //   "name":"y",
  //   "desc":"New y coordinate."
  // }],
  // "ret":"n/a",
  // "ex":"function vscroll(id, spd)<br/>
  //   local x,y = Good.GetPos(id)<br/>
  //   y = y + spd<br/>
  //   Good.SetPos(id, x, y)<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"SetRep",
  // "desc":"Set repeat mode of an object.",
  // "proto":"void SetRep(idObj, xRep, yRep)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"xRep",
  //   "desc":"New horzontal repeat mode."
  // },{
  //   "name":"yRep",
  //   "desc":"New vertical repeat mode."
  // }],
  // "ret":"n/a",
  // "ex":"function SetBkg(id)<br/>
  //   Good.SetBgColor(id, BG_COLOR)<br/>
  //   local o = GenTexObj(-1, 76, 0, 0)<br/>
  //   Good.SetRep(o, 1, 1)<br/>
  //   Good.AddChild(id, o, 0)<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"SetRot",
  // "desc":"Set rotation of an object.",
  // "proto":"void SetRot(idObj, rot)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"rot",
  //   "desc":"New rotation value, in degree."
  // }],
  // "ret":"n/a",
  // "ex":"Good.SetRot(o, 90)"
  // [/API]
  //

  static int SetRot(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    lua_Number newRot = luaL_checknumber(L, 2);

    AppT::getInst().setRot(idObj, (float)newRot);

    return 0;
  }

  //
  // [API]
  // "name":"SetScale",
  // "desc":"Set scale of an object.",
  // "proto":"void SetScale(idObj, xScale, yScale)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"xScale",
  //   "desc":"New horzontal scale factor."
  // },{
  //   "name":"yScale",
  //   "desc":"New vertical scale factor."
  // }],
  // "ret":"n/a",
  // "ex":"Good.SetScale(o, 1.5, -1.5)"
  // [/API]
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
  // [API]
  // "name":"SetScript",
  // "desc":"Set script of an object.",
  // "proto":"void SetScript(idObj, script)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"script",
  //   "desc":"New script handler name. Empty string to disable script handler."
  // }],
  // "ret":"n/a",
  // "ex":"function CaveMazeScrollTo(param, dir)<br/>
  //   param.tx, param.ty = GetMazeDirOffset(dir)<br/>
  //   Good.SetScript(param._id, \'AnimCaveMazeScroll\')<br/>
  // end"
  // [/API]
  //

  static int SetScript(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    const char* script = luaL_checkstring(L, 2);

    AppT::getInst().setScript(idObj, script);

    return 0;
  }

  //
  // [API]
  // "name":"SetSpriteId",
  // "desc":"Set sprite resource ID of an object.",
  // "proto":"void SetSpriteId(idObj, idSpr)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"idSpr",
  //   "desc":"Sprite resource ID."
  // }],
  // "ret":"n/a",
  // "ex":"if (Input.IsKeyPushed(Input.LEFT)) then<br/>
  //   Good.SetSpriteId(id, head_left)<br/>
  // elseif (Input.IsKeyPushed(Input.RIGHT)) then<br/>
  //   Good.SetSpriteId(id, head_right)<br/>
  // end"
  // [/API]
  //

  static int SetSpriteId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    int idSpr = luaL_checkint(L, 2);

    AppT::getInst().setSpriteId(idObj, idSpr);

    return 0;
  }

  //
  // [API]
  // "name":"SetTexId",
  // "desc":"Set texture respurce ID of an object.",
  // "proto":"void SetTexId(idObj, idTex)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"idTex",
  //   "desc":"Texture resource ID."
  // }],
  // "ret":"n/a",
  // "ex":"Good.SetTexId(weapon_obj, weapon_tex_id[curr_weapon_index])"
  // [/API]
  //

  static int SetTexId(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    int idTex = luaL_checkint(L, 2);

    AppT::getInst().setTexId(idObj, idTex);

    return 0;
  }

  //
  // [API]
  // "name":"SetVisible",
  // "desc":"Set visible property of an object.",
  // "proto":"void SetVisible(idObj, bVisible)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"bVisible",
  //   "desc":"1 for visible else 0 for invisible."
  // }],
  // "ret":"n/a",
  // "ex":"Good.SetVisible(o, 1)"
  // [/API]
  //

  static int SetVisible(lua_State* L)
  {
    int idObj = luaL_checkint(L, 1);
    bool bVisible = 0 != luaL_checkint(L, 2);

    AppT::getInst().setVisible(idObj, bVisible);

    return 0;
  }

  //
  // [API]
  // "name":"StopAnim",
  // "desc":"Stop playing sprite animation of an object.",
  // "proto":"void StopAnim(idObj)",
  // "param":[{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // }],
  // "ret":"n/a",
  // "ex":"Good.StopAnim(o)"
  // [/API]
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

  //
  // [API]
  // "name":"LEVEL",
  // "desc":"Object type constant, level object."
  // [/API]
  //
  // [API]
  // "name":"COLBG",
  // "desc":"Object type constant, color block object."
  // [/API]
  //
  // [API]
  // "name":"TEXBG",
  // "desc":"Object type constant, texture block object."
  // [/API]
  //
  // [API]
  // "name":"MAPBG",
  // "desc":"Object type constant, map object."
  // [/API]
  //
  // [API]
  // "name":"SPRITE",
  // "desc":"Object type constant, sprite object."
  // [/API]
  //
  // [API]
  // "name":"DUMMY",
  // "desc":"Object type constant, dummy object."
  // [/API]
  //

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
      {"SetAnchor", &SetAnchor},
      {"SetBgColor", &SetBgColor},
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
  // [Module] Input
  //

  //
  // [API]
  // "name":"GetMousePos",
  // "desc":"Get current mouse pointer position.",
  // "proto":"number,number GetMousePos()",
  // "ret":"Return current x and y position of mouse pointer.",
  // "ex":"local x, y = Input.GetMousePos()<br/>
  // local o = Good.GetTexId(Good.PickObj(x, y, Good.TEXBG))"
  // [/API]
  //

  static int GetMousePos(lua_State* L)
  {
    AppT& app = AppT::getInst();

    lua_pushnumber(L, app.mMousePos.x);
    lua_pushnumber(L, app.mMousePos.y);

    return 2;
  }

  //
  // [API]
  // "name":"IsKeyDown",
  // "desc":"Check is specified key down.",
  // "proto":"bool IsKeyDown(flag)",
  // "param":[{
  //   "name":"flag",
  //   "desc":"Flag of keys."
  // }],
  // "ret":"Return true if specified key is down else return false.",
  // "ex":"if (Input.IsKeyDown(Input.UP)) then<br/>
  //   if (0 < y) then<br/>
  //     y = y - MOV_SPD_Y<br/>
  //   end<br/>
  // elseif (Input.IsKeyDown(Input.DOWN)) then<br/>
  //   if (h - SINGLE_SCR_H > y) then<br/>
  //     y = y + MOV_SPD_Y<br/>
  //   end<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"IsKeyPressed",
  // "desc":"Check is specified key pressed.",
  // "proto":"bool IsKeyPressed(flag)",
  // "param":[{
  //   "name":"flag",
  //   "desc":"Flag of keys."
  // }],
  // "ret":"Return true if specified key is pressed else return false. A key pressed is true only when the key is at the timing of key up.",
  // "ex":"Title.OnStep = function(param)<br/>
  //   if (Input.IsKeyPressed(Input.LBUTTON)) then<br/>
  //     Good.GenObj(-1, map_lvl_id)<br/>
  //   end<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"IsKeyPushed",
  // "desc":"Check is specified key pushed.",
  // "proto":"bool IsKeyPushed(flag)",
  // "param":[{
  //   "name":"flag",
  //   "desc":"Flag of keys."
  // }],
  // "ret":"Return true if specified key is pushed else return false. A key pushed is true only when the key is at the timing of key down.",
  // "ex":"if (Input.IsKeyPushed(Input.ESCAPE)) then<br/>
  //   CloseOptionDialog()<br/>
  //   return<br/>
  // end"
  // [/API]
  //

  static int IsKeyPushed(lua_State* L)
  {
    int code = luaL_checkint(L, 1);

    AppT& app = AppT::getInst();
    app.mHandledKeys |= code;

    lua_pushboolean(L, app.isKeyPushed(code));

    return 1;
  }

  //
  // [API]
  // "name":"UP",
  // "desc":"Input flag constant, up arrow key."
  // [/API]
  //
  // [API]
  // "name":"DOWN",
  // "desc":"Input flag constant, down arrow key."
  // [/API]
  //
  // [API]
  // "name":"LEFT",
  // "desc":"Input flag constant, left arrow key."
  // [/API]
  //
  // [API]
  // "name":"RIGHT",
  // "desc":"Input flag constant, right arrow key."
  // [/API]
  //
  // [API]
  // "name":"RETURN",
  // "desc":"Input flag constant, enter/return key."
  // [/API]
  //
  // [API]
  // "name":"BTN_A",
  // "desc":"Input flag constant, button A key."
  // [/API]
  //
  // [API]
  // "name":"BTN_B",
  // "desc":"Input flag constant, button B key."
  // [/API]
  //
  // [API]
  // "name":"LBUTTON",
  // "desc":"Input flag constant, left mouse button key."
  // [/API]
  //
  // [API]
  // "name":"RBUTTON",
  // "desc":"Input flag constant, right mouse button key."
  // [/API]
  //
  // [API]
  // "name":"ESCAPE",
  // "desc":"Input flag constant, ESC key."
  // [/API]
  //
  // [API]
  // "name":"ANY",
  // "desc":"Input flag constant, any key."
  // [/API]
  //

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
  // [Module] Resource
  //

  //
  // [API]
  // "name":"GenTex",
  // "desc":"Create a texture at runtime.",
  // "proto":"int GenTex(idCanvas[, name])",
  // "param":[{
  //   "name":"idCanvas",
  //   "desc":"Source canvas ID."
  // },{
  //   "name":"name",
  //   "desc":"Resource texture name."
  // }],
  // "ret":"Return new texture resource ID else return -1",
  // "ex":"local function GenLotteryTex()<br/>
  //   if (nil == lottery_canvas) then<br/>
  //     lottery_canvas = Graphics.GenCanvas(LOTTERY_W, LOTTERY_H)<br/>
  //   end<br/>
  //   FillImage(lottery_canvas, 0, 0, cash_tex_id, LOTTERY_W, LOTTERY_H)<br/>
  //   lottery_tex_id = Resource.GenTex(lottery_canvas, \'lobbery_tex\')<br/>
  // end"
  // [/API]
  //

  static int GenResTex(lua_State* L)
  {
    int canvas = luaL_checkint(L, 1);

    const char* name = 0;
    if (2 <= lua_gettop(L)) {
      name = luaL_checkstring(L, 2);
    }

    lua_pushinteger(L, AppT::getInst().genResTex(canvas, name));

    return 1;
  }

  //
  // [API]
  // "name":"GetFirstLevelId",
  // "desc":"Get first level resource ID.",
  // "proto":"int GetFirstLevelId()",
  // "ret":"Return first level resource ID else return -1.",
  // "ex":"local idResLvl = Resource.GetFirstLevelId()"
  // [/API]
  //

  static int GetFirstLevelId(lua_State* L)
  {
    lua_pushinteger(L, AppT::getInst().getFirstResLevelId());

    return 1;
  }

  //
  // [API]
  // "name":"GetMapSize",
  // "desc":"Get size of a map resource ID.",
  // "proto":"int,int GetMapSize(idMap)",
  // "param":[{
  //   "name":"idMap",
  //   "desc":"Map resource ID."
  // }],
  // "ret":"Return width and height in tile of the map resource else return 0,0.",
  // "ex":"local cx, cy = Resource.GetMapSize(idResMap)"
  // [/API]
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
  // [API]
  // "name":"GetNextLevelId",
  // "desc":"Get next level resource ID.",
  // "proto":"int GetNextLevelId(idLvl)",
  // "param":[{
  //   "name":"idLvl",
  //   "desc":"Level resource ID."
  // }],
  // "ret":"Return next level resource ID else return -1.",
  // "ex":"local idResLvl = Resource.GetFirstLevelId()<br/>
  // idResLvl = Resource.GetNextLevelId(idResLvl)"
  // [/API]
  //

  static int GetNextLevelId(lua_State* L)
  {
    int idLvl = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getNextResLevelId(idLvl));

    return 1;
  }

  //
  // [API]
  // "name":"GetLevelId",
  // "desc":"Get level resource ID with a name.",
  // "proto":"int GetLevelId(name)",
  // "param":[{
  //   "name":"name",
  //   "desc":"Name of level resource."
  // }],
  // "ret":"Return level resource ID of the name else return -1.",
  // "ex":"local lvlTitle = Resource.GetLevelId(\'title\')"
  // [/API]
  //

  static int GetResLevelId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResLevelId(name));

    return 1;
  }

  //
  // [API]
  // "name":"GetMapId",
  // "desc":"Get map resource ID with a name.",
  // "proto":"int GetMapId(name)",
  // "param":[{
  //   "name":"name",
  //   "desc":"Name of map resource."
  // }],
  // "ret":"Return map resource ID of the name else return -1.",
  // "ex":"local mapForest = Resource.GetMapId(\'forest\')"
  // [/API]
  //

  static int GetResMapId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResMapId(name));

    return 1;
  }

  //
  // [API]
  // "name":"GetSoundId",
  // "desc":"Get sound resource ID with a name.",
  // "proto":"int GetSoundId(name)",
  // "param":[{
  //   "name":"name",
  //   "desc":"Name of sound resource."
  // }],
  // "ret":"Return sound resource ID of the name else return -1.",
  // "ex":"local sndDestroy = Resource.GetSoundId(\'destroy\')"
  // [/API]
  //

  static int GetResSoundId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResSoundId(name));

    return 1;
  }

  //
  // [API]
  // "name":"GetSpriteId",
  // "desc":"Get sprite resource ID with a name.",
  // "proto":"int GetSpriteId(name)",
  // "param":[{
  //   "name":"name",
  //   "desc":"Name of sprite resource."
  // }],
  // "ret":"Return sprite resource ID of the name else return -1.",
  // "ex":"local sprBullet = Resource.GetSpriteId(\'bullet\')"
  // [/API]
  //

  static int GetResSpriteId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResSpriteId(name));

    return 1;
  }

  //
  // [API]
  // "name":"GetTexId",
  // "desc":"Get texture resource ID with a name.",
  // "proto":"int GetTexId(name)",
  // "param":[{
  //   "name":"name",
  //   "desc":"Name of texture resource."
  // }],
  // "ret":"Return texture resource ID of the name else return -1.",
  // "ex":"local texMonkey = Resource.GetTexId(\'monkey\')"
  // [/API]
  //

  static int GetResTexId(lua_State* L)
  {
    const char* name = luaL_checkstring(L, 1);

    lua_pushinteger(L, AppT::getInst().getResTexId(name));

    return 1;
  }

  //
  // [API]
  // "name":"GetTexSize",
  // "desc":"Get size of texture resource ID.",
  // "proto":"int,int GetTexSize(idTex)",
  // "param":[{
  //   "name":"idTex",
  //   "desc":"Texture resource ID."
  // }],
  // "ret":"Return width and height in pixel of the texture resource else return 0,0.",
  // "ex":"local tw, th = Resource.GetTexSize(map_tex_id)<br/>
  // local canvas = Graphics.GenCanvas(tw, th)<br/>
  // Graphics.DrawImage(canvas, 0, 0, map_tex_id, 0, 0, tw, th)"
  // [/API]
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
  // [API]
  // "name":"GetTileByPos",
  // "desc":"Get tile index of a map resource by position.",
  // "proto":"int GetTileByPos(idMap, x, y)",
  // "param":[{
  //   "name":"idMap",
  //   "desc":"Map resource ID."
  // },{
  //   "name":"x",
  //   "desc":"X position relative to top-left corner."
  // },{
  //   "name":"y",
  //   "desc":"Y position relative to top-left corner."
  // }],
  // "ret":"Return tile index of position of the map resource else return 0.",
  // "ex":"local map = Good.GetMapId(map_obj_id)<br/>
  // local tile = Resource.GetTileByPos(map, x, y)"
  // [/API]
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
  // [API]
  // "name":"GetTileSize",
  // "desc":"Get tile size of a resource ID.",
  // "proto":"int,int GetTileSize(idRes)",
  // "param":[{
  //   "name":"idRes",
  //   "desc":"Map or sprite resource ID."
  // }],
  // "ret":"Return width and height in pixel of the resource else return 0,0.",
  // "ex":"local tilex, tiley = Resource.GetTileSize(Good.GetMapId(map))"
  // [/API]
  //

  static int GetTileSize(lua_State* L)
  {
    int idRes = luaL_checkint(L, 1);

    lua_Integer w = 0, h = 0;
    AppT::getInst().getResTileSize(idRes, w, h);

    lua_pushinteger(L, w);
    lua_pushinteger(L, h);

    return 2;
  }

  //
  // [API]
  // "name":"UpdateTex",
  // "desc":"Update a texture at runtime.",
  // "proto":"void UpdateTex(idTex, x, y, idCanvas, srcx, srcy, srcw, srch)",
  // "param":[{
  //   "name":"idTex",
  //   "desc":"Texture resource ID."
  // },{
  //   "name":"x",
  //   "desc":"X position relative to top-left corner."
  // },{
  //   "name":"y",
  //   "desc":"Y position relative to top-left corner."
  // },{
  //   "name":"idCanvas",
  //   "desc":"Source canvas ID."
  // },{
  //   "name":"srcx",
  //   "desc":"X position relative to top-left corner of source canvas."
  // },{
  //   "name":"srcy",
  //   "desc":"Y position relative to top-left corner of source canvas."
  // },{
  //   "name":"srcw",
  //   "desc":"Width in pixel of source canvas."
  // },{
  //   "name":"srch",
  //   "desc":"Height in pixel of source canvas."
  // }],
  // "ret":"n/a",
  // "ex":"local function ResetLotteryTex()<br/>
  //   if (nil == lottery_canvas) then<br/>
  //     lottery_canvas = Graphics.GenCanvas(LOTTERY_W, LOTTERY_H)<br/>
  //   end<br/>
  //   FillImage(lottery_canvas, 0, 0, cash_tex_id, LOTTERY_W, LOTTERY_H)<br/>
  //   Resource.UpdateTex(lottery_tex_id, 0, 0, lottery_canvas, 0, 0, LOTTERY_W, LOTTERY_H)<br/>
  // end"
  // [/API]
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
      {"GenTex", &GenResTex},
      {"GetFirstLevelId", &GetFirstLevelId},
      {"GetLevelId", &GetResLevelId},
      {"GetNextLevelId", &GetNextLevelId},
      {"GetMapId", &GetResMapId},
      {"GetMapSize", &GetMapSize},
      {"GetSoundId", &GetResSoundId},
      {"GetSpriteId", &GetResSpriteId},
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
  // [Module] Sound
  //

  //
  // [API]
  // "name":"GetMusicVolume",
  // "desc":"Get volume of music.",
  // "proto":"number GetMusicVolume()",
  // "ret":"Return volumn of music, between 0..1.",
  // "ex":"local vol = Sound.GetMusicVolume()"
  // [/API]
  //

  static int GetMusicVolume(lua_State* L)
  {
    lua_pushnumber(L, SndT::getMusicVolume());

    return 1;
  }

  //
  // [API]
  // "name":"GetSound",
  // "desc":"Create a sound object by sound resource ID.",
  // "proto":"int GetSound(idRes)",
  // "param":[{
  //   "name":"idRes",
  //   "desc":"Sound resource ID."
  // }],
  // "ret":"Return a sound object ID by the sound resource else return -1.",
  // "ex":"local s = Sound.GetSound(Resource.GetSoundId(\'hit\'))"
  // [/API]
  //

  static int GetSound(lua_State* L)
  {
    int idRes = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().getSound(idRes));

    return 1;
  }

  //
  // [API]
  // "name":"GetSoundVolume",
  // "desc":"Get volume of sound.",
  // "proto":"number GetSoundVolume()",
  // "ret":"Return volumn of sound, between 0..1.",
  // "ex":"local vol = Sound.GetSoundVolume()"
  // [/API]
  //

  static int GetSoundVolume(lua_State* L)
  {
    lua_pushnumber(L, SndT::getSoundVolume());

    return 1;
  }

  //
  // [API]
  // "name":"KillSound",
  // "desc":"Destroy a sound object.",
  // "proto":"void KillSound(idSnd)",
  // "param":[{
  //   "name":"idSnd",
  //   "desc":"Sound object ID."
  // }],
  // "ret":"n/a",
  // "ex":"if (-1 ~= bgm_id) then<br/>
  //   Sound.KillSound(bgm_id)<br/>
  //   bgm_id = -1<br/>
  // end"
  // [/API]
  //

  static int KillSound(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    SndT::releaseSound(SndT(idSnd));

    return 0;
  }

  //
  // [API]
  // "name":"IsLoop",
  // "desc":"Get loop property of a sound object.",
  // "proto":"bool IsLoop(idSnd)",
  // "param":[{
  //   "name":"idSnd",
  //   "desc":"Sound object ID."
  // }],
  // "ret":"Return true if the sound object is looping else return false.",
  // "ex":"local b = Sound.IsLoop(s)"
  // [/API]
  //

  static int IsLoop(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    lua_pushboolean(L, SndT(idSnd).isLoop());

    return 1;
  }

  //
  // [API]
  // "name":"IsPlaying",
  // "desc":"Get playing property of a sound object.",
  // "proto":"bool IsPlaying(idSnd)",
  // "param":[{
  //   "name":"idSnd",
  //   "desc":"Sound object ID."
  // }],
  // "ret":"Return true if the sound object is playing else return false.",
  // "ex":"local b = Sound.IsPlaying(s)"
  // [/API]
  //

  static int IsPlaying(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    lua_pushboolean(L, AppT::getInst().isSoundPlaying(idSnd));

    return 1;
  }

  //
  // [API]
  // "name":"Pause",
  // "desc":"Pause playing a sound object.",
  // "proto":"void Pause(idSnd)",
  // "param":[{
  //   "name":"idSnd",
  //   "desc":"Sound object ID."
  // }],
  // "ret":"n/a",
  // "ex":"Sound.Pause(s)"
  // [/API]
  //

  static int Pause(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    SndT(idSnd).pause();

    return 0;
  }

  //
  // [API]
  // "name":"PlaySound",
  // "desc":"Start playing sound by a sound resource ID.",
  // "proto":"int PlaySound(idRes)",
  // "param":[{
  //   "name":"idRes",
  //   "desc":"Sound resource ID."
  // }],
  // "ret":"Return a sound object ID by the sound resource else return -1.",
  // "ex":"function PlaySound(id)<br/>
  //   if (enableSnd) then<br/>
  //     return Sound.PlaySound(id)<br/>
  //   end<br/>
  //   return -1<br/>
  // end"
  // [/API]
  //

  static int PlaySound(lua_State* L)
  {
    int idRes = luaL_checkint(L, 1);

    lua_pushinteger(L, AppT::getInst().playSound(idRes));

    return 1;
  }

  //
  // [API]
  // "name":"Resume",
  // "desc":"Resume playing a sound object.",
  // "proto":"void Resume(idSnd)",
  // "param":[{
  //   "name":"idSnd",
  //   "desc":"Sound object ID."
  // }],
  // "ret":"n/a",
  // "ex":"Sound.Resume(s)"
  // [/API]
  //

  static int Resume(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    SndT(idSnd).play();

    return 0;
  }

  //
  // [API]
  // "name":"SetLoop",
  // "desc":"Set loop property of a sound object.",
  // "proto":"void SetLoop(idSnd, bLoop)",
  // "param":[{
  //   "name":"idSnd",
  //   "desc":"Sound object ID."
  // },{
  //   "name":"bLoop",
  //   "desc":"1 to enable loop play else 0 to disable loop play."
  // }],
  // "ret":"n/a",
  // "ex":"Sound.SetLoop(s, 1)"
  // [/API]
  //

  static int SetLoop(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    bool bLoop = 0 != luaL_checkint(L, 2);

    SndT(idSnd).setLoop(bLoop);

    return 0;
  }

  //
  // [API]
  // "name":"SetMusicVolume",
  // "desc":"Set volume of music.",
  // "proto":"void SetMusicVolume(volume)",
  // "param":[{
  //   "name":"volume",
  //   "desc":"New volumn value of music, between 0..1."
  // }],
  // "ret":"n/a",
  // "ex":"Sound.SetMusicVolume(1)"
  // [/API]
  //

  static int SetMusicVolume(lua_State* L)
  {
    lua_Number vol = luaL_checknumber(L, 1);
    SndT::setMusicVolume((float)vol);

    return 0;
  }

  //
  // [API]
  // "name":"SetSoundVolume",
  // "desc":"Set volume of sound.",
  // "proto":"void SetSoundVolume(volume)",
  // "param":[{
  //   "name":"volume",
  //   "desc":"New volumn value of sound, between 0..1."
  // }],
  // "ret":"n/a",
  // "ex":"Sound.SetSoundVolume(0.8)"
  // [/API]
  //

  static int SetSoundVolume(lua_State* L)
  {
    lua_Number vol = luaL_checknumber(L, 1);
    SndT::setSoundVolume((float)vol);

    return 0;
  }

  //
  // [API]
  // "name":"Stop",
  // "desc":"Stop playing a sound object.",
  // "proto":"void Stop(idSnd)",
  // "param":[{
  //   "name":"idSnd",
  //   "desc":"Sound object ID."
  // }],
  // "ret":"n/a",
  // "ex":"Sound.Stop(s)"
  // [/API]
  //

  static int Stop(lua_State* L)
  {
    int idSnd = luaL_checkint(L, 1);
    AppT::getInst().stopSound(idSnd);

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
      {"PlaySound", &PlaySound},
      {"Resume", &Resume},
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
  // [Module] Graphics
  //

  //
  // [API]
  // "name":"DrawImage",
  // "desc":"Draw a texture resource ID.",
  // "proto":"void DrawImage(idCanvas, x, y, texId, srcx, srcy, srcw, srch[, color, rot, scalex, scaley])",
  // "param":[{
  //   "name":"idCanvas",
  //   "desc":"Target canvas ID. -1 to draw to screen."
  // },{
  //   "name":"x",
  //   "desc":"X position relative to top-left corner."
  // },{
  //   "name":"y",
  //   "desc":"Y position relative to top-left corner."
  // },{
  //   "name":"texId",
  //   "desc":"Source texture resource ID."
  // },{
  //   "name":"srcx",
  //   "desc":"X position relative to top-left corner of source texture."
  // },{
  //   "name":"srcy",
  //   "desc":"Y position relative to top-left corner of source texture."
  // },{
  //   "name":"srcw",
  //   "desc":"Width in pixel of source texture."
  // },{
  //   "name":"srch",
  //   "desc":"Height in pixel of source texture."
  // },{
  //   "name":"color(optional)",
  //   "desc":"Blending color in ARGB format."
  // },{
  //   "name":"rot(optional)",
  //   "desc":"Rotation factor."
  // },{
  //   "name":"xscale(optional)",
  //   "desc":"Horizontal scale factor."
  // },{
  //   "name":"yscale(optional)",
  //   "desc":"Vertical scale factor."
  // }],
  // "ret":"n/a",
  // "ex":"local function FillImage(canvas, x, y, tex, w, h)<br/>
  //   Graphics.FillRect(canvas, x, y, w, h, COLOR_YELLOW)<br/>
  //   local cx, cy = Resource.GetTexSize(tex)<br/>
  //   for ay = 0, h, cy do<br/>
  //     for ax = 0, w, cx do<br/>
  //       Graphics.DrawImage(canvas, ax, ay, tex, 0, 0, cx, cy)<br/>
  //     end<br/>
  //   end<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"DrawText",
  // "desc":"Draw a text string.",
  // "proto":"void DrawText(idCanvas, x, y, text[, size, color])",
  // "param":[{
  //   "name":"idCanvas",
  //   "desc":"Target canvas ID. -1 to draw to screen."
  // },{
  //   "name":"x",
  //   "desc":"X position relative to top-left corner."
  // },{
  //   "name":"y",
  //   "desc":"Y position relative to top-left corner."
  // },{
  //   "name":"text",
  //   "desc":"A text string."
  // },{
  //   "name":"size(optional)",
  //   "desc":"Text size."
  // },{
  //   "name":"color(optional)",
  //   "desc":"Text color."
  // }],
  // "ret":"n/a",
  // "ex":"Graphics.DrawText(-1, x, y, \'hello world\', 20)"
  // [/API]
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
  // [API]
  // "name":"FillRect",
  // "desc":"Fill color to a rectange area.",
  // "proto":"void FillRect(idCanvas, x, y, w, h, color[, rot, scalex, scaley])",
  // "param":[{
  //   "name":"idCanvas",
  //   "desc":"Target canvas ID. -1 to draw to screen."
  // },{
  //   "name":"x",
  //   "desc":"X position relative to top-left corner."
  // },{
  //   "name":"y",
  //   "desc":"Y position relative to top-left corner."
  // },{
  //   "name":"w",
  //   "desc":"Rectange width."
  // },{
  //   "name":"h",
  //   "desc":"Rectange height."
  // },{
  //   "name":"color",
  //   "desc":"Fill color in ARGB format."
  // },{
  //   "name":"rot(optional)",
  //   "desc":"Rotation factor."
  // },{
  //   "name":"xscale(optional)",
  //   "desc":"Horizontal scale factor."
  // },{
  //   "name":"yscale(optional)",
  //   "desc":"Vertical scale factor."
  // }],
  // "ret":"n/a",
  // "ex":"Graphics.FillRect(idCanvas, j * TILE_W, i * TILE_H, TILE_W, TILE_H, clr)"
  // [/API]
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
  // [API]
  // "name":"GenCanvas",
  // "desc":"Create a new canvas.",
  // "proto":"int GenCanvas(w, h)",
  // "param":[{
  //   "name":"w",
  //   "desc":"Canvas width."
  // },{
  //   "name":"h",
  //   "desc":"Canvas height."
  // }],
  // "ret":"Return new canvas ID else return -1.",
  // "ex":"local canvas = Graphics.GenCanvas(W, H)"
  // [/API]
  //

  static int GenCanvas(lua_State* L)
  {
    int w = luaL_checkint(L, 1);
    int h = luaL_checkint(L, 2);

    lua_pushinteger(L, AppT::getInst().genCanvas(w, h));

    return 1;
  }

  //
  // [API]
  // "name":"GetAntiAlias",
  // "desc":"Get anti-alias property of text drawing.",
  // "proto":"int GetAntiAlias()",
  // "ret":"Return 1 if anti-alias is enable else return 0.",
  // "ex":"local aa = Graphics.GetAntiAlias()"
  // [/API]
  //

  static int GetAntiAlias(lua_State* L)
  {
    lua_pushinteger(L, AppT::getInst().getAntiAlias() ? 1 : 0);

    return 1;
  }

  //
  // [API]
  // "name":"GetFont",
  // "desc":"Get font property of text drawing.",
  // "proto":"int GetFont()",
  // "ret":"Return select font of text drawing.",
  // "ex":"local fon = Graphics.GetFont()"
  // [/API]
  //

  static int GetFont(lua_State *L)
  {
    lua_pushinteger(L, AppT::getInst().getFont());

    return 1;
  }

  //
  // [API]
  // "name":"GetPixel",
  // "desc":"Get pixel color of a canvas.",
  // "proto":"number GetPixel(idCanvas, x, y)",
  // "param":[{
  //   "name":"idCanvas",
  //   "desc":"The canvas ID."
  // },{
  //   "name":"x",
  //   "desc":"X position relative to top-left corner."
  // },{
  //   "name":"y",
  //   "desc":"Y position relative to top-left corner."
  // }],
  // "ret":"Return pixel in ARGB format of the canvas else return 0.",
  // "ex":"local pix = Graphics.GetPixel(idCanvas, x, y)"
  // [/API]
  //

  static int GetPixel(lua_State *L)
  {
    int canvas = luaL_checkint(L, 1);
    int x = luaL_checkint(L, 2);
    int y = luaL_checkint(L, 3);

    lua_pushnumber(L, AppT::getInst().getPixel(canvas, x, y));

    return 1;
  }

  //
  // [API]
  // "name":"KillCanvas",
  // "desc":"Destroy a canvas.",
  // "proto":"void KillCanvas(idCanvas)",
  // "param":[{
  //   "name":"idCanvas",
  //   "desc":"The canvas ID."
  // }],
  // "ret":"n/a",
  // "ex":"Graphics.KillCanvas(idCanvas)"
  // [/API]
  //

  static int KillCanvas(lua_State* L)
  {
    int id = luaL_checkint(L, 1);

    AppT::getInst().killCanvas(id);

    return 0;
  }

  //
  // [API]
  // "name":"SetAnchor",
  // "desc":"Set anchor property of drawing context.",
  // "proto":"void SetAnchor(xAnchor, yAnchor)",
  // "param":[{
  //   "name":"xAnchor",
  //   "desc":"Horizontal anchor value, between 0..1."
  // },{
  //   "name":"yAnchor",
  //   "desc":"Vertical anchor value, between 0..1."
  // }],
  // "ret":"n/a",
  // "ex":"Graphics.SetAnchor(0.5, 0.5)"
  // [/API]
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
  // [API]
  // "name":"SetAntiAlias",
  // "desc":"Set anti-alias property of text drawing.",
  // "proto":"void SetAntiAlias(bAntiAlias)",
  // "param":[{
  //   "name":"bAntiAlias",
  //   "desc":"0 to disable anti-alias else 1 to enable anti-alias."
  // }],
  // "ret":"n/a",
  // "ex":"Graphics.SetAntiAlias(0)"
  // [/API]
  //

  static int SetAntiAlias(lua_State* L)
  {
    bool bAntiAlias = 0 != luaL_checkint(L, 1);

    AppT::getInst().setAntiAlias(bAntiAlias);

    return 0;
  }

  //
  // [API]
  // "name":"SetFont",
  // "desc":"Set font property of text drawing.",
  // "proto":"void SetFont(font)",
  // "param":[{
  //   "name":"font",
  //   "desc":"Select font ID. 0 for system font, 1 for fixed 5x8 font."
  // }],
  // "ret":"n/a",
  // "ex":"Graphics.SetFont(Graphics.FIXED_FONT)"
  // [/API]
  //

  static int SetFont(lua_State* L)
  {
    int font = luaL_checkint(L, 1);

    AppT::getInst().setFont(font);

    return 0;
  }

  //
  // [API]
  // "name":"SYSTEM_FONT",
  // "desc":"Select font constant, system font."
  // [/API]
  //
  // [API]
  // "name":"FIXED_FONT",
  // "desc":"Select font constant, fixed 5x8 font."
  // [/API]
  //

  static bool installGraphicsModuleApi(lua_State* L)
  {
    static RegConstType GraphicsConst[] = {
      {"SYSTEM_FONT", GOOD_DRAW_TEXT_SYSTEM_FONT},
      {"FIXED_FONT", GOOD_DRAW_TEXT_FIXED_FONT},
      { 0 }
    };

    static RegApiType GraphicsApi[] = {
      {"DrawImage", &DrawImage},
      {"DrawText", &DrawText},
      {"FillRect", &FillRect},
      {"GenCanvas", &GenCanvas},
      {"GetAntiAlias", &GetAntiAlias},
      {"GetFont", &GetFont},
      {"GetPixel", &GetPixel},
      {"KillCanvas", &KillCanvas},
      {"SetAnchor", &GraphicsSetAnchor},
      {"SetAntiAlias", &SetAntiAlias},
      {"SetFont", &SetFont},
      { 0 }
    };

    lua_newtable(L);

    regCONST(L, GraphicsConst);
    regAPI(L, GraphicsApi);

    lua_setglobal(L, "Graphics");

    return true;
  }

  //
  // [Module] Stge
  //

#ifdef GOOD_SUPPORT_STGE

  //
  // [API]
  // "name":"BindParticle",
  // "desc":"Bind a particle to an object.",
  // "proto":"void BindParticle(idParticle, idObj[, iObjMgr])",
  // "param":[{
  //   "name":"idParticle",
  //   "desc":"The particle ID."
  // },{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"Game.OnNewParticle = function(param, particle)<br/>
  //   local o = GenColorObj(-1, 2, 2, 0xffffffff)<br/>
  //   Stge.BindParticle(particle, o)<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"BindTask",
  // "desc":"Bind a particle task object to an object.",
  // "proto":"void BindTask(idTask, idObj[, iObjMgr])",
  // "param":[{
  //   "name":"idTask",
  //   "desc":"The task ID."
  // },{
  //   "name":"idObj",
  //   "desc":"The object ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"if (Input.IsKeyDown(Input.BTN_A + Input.LBUTTON)) then<br/>
  //   if (-1 == param.fire) then<br/>
  //     param.fire = Stge.RunScript(\'weapon_1\', 0, 0, OM_MYBULLET)<br/>
  //     Stge.BindTask(param.fire, param.dummy, OM_MYBULLET)<br/>
  //   end<br/>
  // elseif (-1 ~= param.fire) then<br/>
  //   Stge.KillTask(param.fire, OM_MYBULLET)<br/>
  //   param.fire = -1<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"GetParticleDirection",
  // "desc":"Get direction property of an particle.",
  // "proto":"number GetParticleDirection(idParticle[, iObjMgr])",
  // "param":[{
  //   "name":"idParticle",
  //   "desc":"The particle ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return direction of the particle in degree else return 0.",
  // "ex":"local dir = Stge.GetParticleDirection(p)"
  // [/API]
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
  // [API]
  // "name":"GetFirstParticle",
  // "desc":"Get first particle.",
  // "proto":"int GetFirstParticle([iObjMgr])",
  // "param":[{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return first particle else return -1.",
  // "ex":"local id = Stge.GetFirstParticle(iMgr)"
  // [/API]
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
  // [API]
  // "name":"GetFirstTask",
  // "desc":"Get first particle task.",
  // "proto":"int GetFirstTask([iObjMgr])",
  // "param":[{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return first particle task else return -1.",
  // "ex":"local id = Stge.GetFirstTask(iMgr)"
  // [/API]
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
  // [API]
  // "name":"GetNextParticle",
  // "desc":"Get next particle.",
  // "proto":"int GetNextParticle(idCur[, iObjMgr])",
  // "param":[{
  //   "name":"idCur",
  //   "desc":"Current particle ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return next particle else return -1.",
  // "ex":"id = Stge.GetNextParticle(id)"
  // [/API]
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
  // [API]
  // "name":"GetNextTask",
  // "desc":"Get next particle task.",
  // "proto":"int GetNextTask(idCur[, iObjMgr])",
  // "param":[{
  //   "name":"idCur",
  //   "desc":"Current task ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return next particle task else return -1.",
  // "ex":"id = Stge.GetNextTask(id)"
  // [/API]
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
  // [API]
  // "name":"GetParticleBind",
  // "desc":"Get binded object ID of a particle.",
  // "proto":"int GetParticleBind(idParticle[, iObjMgr])",
  // "param":[{
  //   "name":"idParticle",
  //   "desc":"The particle ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return binded object ID of the particle else return -1.",
  // "ex":"Game.OnKillParticle = function(param, particle)<br/>
  //   Good.KillObj(Stge.GetParticleBind(particle))<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"GetParticleCount",
  // "desc":"Get number of particles.",
  // "proto":"int GetParticleCount([iObjMgr])",
  // "param":[{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return number of the particles else return 0.",
  // "ex":"local c = Stge.GetParticleCount()"
  // [/API]
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
  // [API]
  // "name":"GetParticlePos",
  // "desc":"Get position of a particle.",
  // "proto":"number,number GetParticlePos(idParticle[, iObjMgr])",
  // "param":[{
  //   "name":"idParticle",
  //   "desc":"The particle ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return x and y position of the particles else return 0,0.",
  // "ex":"local x, y = Stge.GetParticlePos(p)"
  // [/API]
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
  // [API]
  // "name":"GetPlayer",
  // "desc":"Get bind object ID of player.",
  // "proto":"int GetPlayer()",
  // "ret":"Return object ID the player else return -1.",
  // "ex":"local p = Stge.GetPlayer()"
  // [/API]
  //

  static int GetPlayer(lua_State* L)
  {
    lua_pushinteger(L, AppT::getInst().mGoodPlayer);

    return 1;
  }

  //
  // [API]
  // "name":"GetTaskBind",
  // "desc":"Get binded object ID of a particle task.",
  // "proto":"int GetTaskBind(idTask[, iObjMgr])",
  // "param":[{
  //   "name":"idTask",
  //   "desc":"The task ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return binded object ID of the particle task else return -1.",
  // "ex":"local o = Stge.GetTaskBind(idTask)"
  // [/API]
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
  // [API]
  // "name":"GetTaskCount",
  // "desc":"Get number of particle tasks.",
  // "proto":"int GetTaskCount([iObjMgr])",
  // "param":[{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return number of the particle tasks else return 0.",
  // "ex":"local c = Stge.GetTaskCount()"
  // [/API]
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
  // [API]
  // "name":"GetUserData",
  // "desc":"Get user defined data of a particle.",
  // "proto":"number GetUserData(idParticle, index[, iObjMgr])",
  // "param":[{
  //   "name":"idParticle",
  //   "desc":"The particle ID."
  // },{
  //   "name":"index",
  //   "desc":"Index of the user data, between 0..3."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return user defined data number of the particle else return 0.",
  // "ex":"local color = Stge.GetUserData(p, 0)"
  // [/API]
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
  // [API]
  // "name":"IsActive",
  // "desc":"Get active property of a particle manager.",
  // "proto":"bool IsActive([iObjMgr])",
  // "param":[{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return true if particle manager is active else return false.",
  // "ex":"local b = Stge.IsActive()"
  // [/API]
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
  // [API]
  // "name":"KillAllParticle",
  // "desc":"Kill all particles.",
  // "proto":"void KillAllParticle([iObjMgr])",
  // "param":[{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"Stge.KillAllParticle(iMgr)"
  // [/API]
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
  // [API]
  // "name":"KillAllTask",
  // "desc":"Kill all particle tasks.",
  // "proto":"void KillAllTask([iObjMgr])",
  // "param":[{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"Stge.KillAllTask()"
  // [/API]
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
  // [API]
  // "name":"KillParticle",
  // "desc":"Kill a particle.",
  // "proto":"void KillParticle(idParticle[, iObjMgr])",
  // "param":[{
  //   "name":"idParticle",
  //   "desc":"The particle ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"local p = Stge.GetFirstParticle(OM_OBJECT)<br/>
  // while (-1 ~= p) do<br/>
  //   local nextp = Stge.GetNextParticle(p, OM_OBJECT)<br/>
  //   if (BULLETS == Stge.GetUserData(p, 0)) then<br/>
  //     Stge.KillParticle(p, OM_OBJECT)<br/>
  //   end<br/>
  //   p = nextp<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"KillTask",
  // "desc":"Kill a particle task.",
  // "proto":"void KillTask(idTask[, iObjMgr])",
  // "param":[{
  //   "name":"idTask",
  //   "desc":"The task ID."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"if (Input.IsKeyDown(Input.BTN_A + Input.LBUTTON)) then<br/>
  //   if (-1 == param.fire) then<br/>
  //     param.fire = Stge.RunScript(\'weapon_1\', 0, 0, OM_MYBULLET)<br/>
  //     Stge.BindTask(param.fire, param.dummy, OM_MYBULLET)<br/>
  //   end<br/>
  // elseif (-1 ~= param.fire) then<br/>
  //   Stge.KillTask(param.fire, OM_MYBULLET)<br/>
  //   param.fire = -1<br/>
  // end"
  // [/API]
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
  // [API]
  // "name":"RunScript",
  // "desc":"Start to run a particle script.",
  // "proto":"int RunScript(script[, x, y, iObjMgr])",
  // "param":[{
  //   "name":"script",
  //   "desc":"STGE script name."
  // },{
  //   "name":"x(optional)",
  //   "desc":"X position of the script task."
  // },{
  //   "name":"y(optional)",
  //   "desc":"Y position of the script task."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"Return a particle task ID else return -1.",
  // "ex":"Stge.RunScript(\'fx_boss_destroy\', ox, oy, OM_FX)"
  // [/API]
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
  // [API]
  // "name":"SetActive",
  // "desc":"Set active property of a particle.",
  // "proto":"void SetActive(bActive[, iObjMgr])",
  // "param":[{
  //   "name":"bActive",
  //   "desc":"0 to disable particle manager else 1 to enable particle manager."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"Stge.SetActive(1)"
  // [/API]
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
  // [API]
  // "name":"SetParticleDirection",
  // "desc":"Set direction of a particle.",
  // "proto":"void SetParticleDirection(idParticle, dir[, iObjMgr])",
  // "param":[{
  //   "name":"idTask",
  //   "desc":"The task ID."
  // },{
  //   "name":"dir",
  //   "desc":"Direction of the particle, in degree."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"Stge.SetParticleDirection(p, 90)"
  // [/API]
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
  // [API]
  // "name":"SetParticlePos",
  // "desc":"Set position of a particle.",
  // "proto":"void SetParticlePos(idParticle, x, y[, iObjMgr])",
  // "param":[{
  //   "name":"idParticle",
  //   "desc":"The particle ID."
  // },{
  //   "name":"x",
  //   "desc":"X position of the particle."
  // },{
  //   "name":"y",
  //   "desc":"Y position of the particle."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"Stge.SetParticlePos(p, 10, 10)"
  // [/API]
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
  // [API]
  // "name":"SetPlayer",
  // "desc":"Bind player object ID.",
  // "proto":"void SetPlayer(idPlayer)",
  // "param":[{
  //   "name":"idPlayer",
  //   "desc":"The object ID as player."
  // }],
  // "ret":"n/a",
  // "ex":"Stge.SetPlayer(idPlayer)"
  // [/API]
  //

  static int SetPlayer(lua_State* L)
  {
    int idPlayer = luaL_checkint(L, 1);

    AppT::getInst().setPlayer(idPlayer);

    return 0;
  }

  //
  // [API]
  // "name":"SetTaskDirection",
  // "desc":"Set direction of a particle task.",
  // "proto":"void SetTaskDirection(idTask, dir[, iObjMgr])",
  // "param":[{
  //   "name":"idTask",
  //   "desc":"The task ID."
  // },{
  //   "name":"dir",
  //   "desc":"Direction of the task, in degree."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"Stge.SetTaskDirection(task, 180)"
  // [/API]
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
  // [API]
  // "name":"SetUserData",
  // "desc":"Set user defined data to a particle.",
  // "proto":"void SetUserData(idParticle, index, data[, iObjMgr])",
  // "param":[{
  //   "name":"idParticle",
  //   "desc":"The particle ID."
  // },{
  //   "name":"index",
  //   "desc":"Index of the user data, between 0..3."
  // },{
  //   "name":"data",
  //   "desc":"User defined data value."
  // },{
  //   "name":"iObjMgr(optional)",
  //   "desc":"Index of the particle manager, between 0..3, default is 0."
  // }],
  // "ret":"n/a",
  // "ex":"Stge.SetUserData(p, 1, hp)"
  // [/API]
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
