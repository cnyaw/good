
//
// rt.h
// Good runtime engine.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/03/01 Waync created.
//

#pragma once

//
// Lua.
//

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

//
// smallworld.
//

#include "swArchive.h"

//
// Following configs that can be define before good.h/rt.h.
//
// #define GOOD_SUPPORT_NO_LOGO
// Is skip display good logo at app start.
//
// #define GOOD_MAX_STGE_OBJ_MGR
// Number of STGE object manager.
//
// #define GOOD_SUPPORT_SDL
// Define in editor player uses SDL renderer, default is OpenGL.
//
// #define GOOD_SUPPORT_STGE
// Is enable STGE feature.
//
// #define GOOD_SUPPORT_GDIPLUS
// Is enable GDI+ image decoder, windows only.
//
// #define GOOD_SUPPORT_PNG
// Is enable PNG  decoder.
//
// #define GOOD_SUPPORT_JPG
// Is enable JPG decoder.
//
// #define GOOD_SUPPORT_ANDROID_IMG
// Is enable android system image decoder.
//
// #define GOOD_RENDER_WITH_DIRTY_FLAG
// Render screen only when dirty flag is set.
//

//
// STGE.
//

#ifdef GOOD_SUPPORT_STGE
# include "../../../../stge/stge/stge.h"
# include "../../../../stge/stge/par_yard.h"
#endif

//
// good.
//

#include "../good.h"

enum KEYS
{
  GOOD_KEYS_UP          = 1,
  GOOD_KEYS_DOWN        = 1 << 1,
  GOOD_KEYS_LEFT        = 1 << 2,
  GOOD_KEYS_RIGHT       = 1 << 3,
  GOOD_KEYS_RETURN      = 1 << 4,
  GOOD_KEYS_BTN_A       = 1 << 5,
  GOOD_KEYS_BTN_B       = 1 << 6,
  GOOD_KEYS_LBUTTON     = 1 << 7,
  GOOD_KEYS_RBUTTON     = 1 << 8,
  GOOD_KEYS_ESCAPE      = 1 << 9,
  GOOD_KEYS_ANY         = 0xffffffff
};

#define GOOD_DEFAULT_TEXT_SIZE 16
#define GOOD_DEFAULT_TEXT_OFFSET 10
#define GOOD_MIN_TEXT_SIZE 10
#define GOOD_MAX_TEXT_SIZE 512

#define GOOD_MAX_SCRIPT_NAME_LEN 64
#define GOOD_MAX_PARAM_NAME_LEN 64

#ifndef GOOD_MAX_STGE_OBJ_MGR
# define GOOD_MAX_STGE_OBJ_MGR 4
#endif

#include "actor.h"
#ifdef GOOD_SUPPORT_STGE
# include "particle.h"
#endif
#include "script.h"
#include "app.h"

// end of rt.h
