
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
// #define GOOD_SUPPORT_IMGP_GX
// Use ImgpGraphics gx.
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
// #define GOOD_SUPPORT_EMSC_IMG
// Is enable HTML5 image decoder.
//
// #define GOOD_SUPPORT_STB_IMG
// Is enable stb image decoder.
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

enum GOOD_CREATE_OBJ
{
  GOOD_CREATE_OBJ_RES_ID = 0,
  GOOD_CREATE_OBJ_ANY_ID,
  GOOD_CREATE_OBJ_EXCLUDE_RES_ID
};

enum GOOD_DRAW_TEXT_FONT
{
  GOOD_DRAW_TEXT_SYSTEM_FONT = 0,
  GOOD_DRAW_TEXT_FIXED_FONT             // Fixed 5x8 font.
};

#define GOOD_DEFAULT_TEXT_SIZE 16
#define GOOD_DEFAULT_TEXT_OFFSET 10
#define GOOD_MIN_TEXT_SIZE 10
#define GOOD_MAX_TEXT_SIZE 512

#define GOOD_MAX_PARAM_NAME_LEN 16

#ifndef GOOD_MAX_STGE_OBJ_MGR
# define GOOD_MAX_STGE_OBJ_MGR 4
#endif

#include "keys.h"
#include "actor.h"
#ifdef GOOD_SUPPORT_STGE
# include "particle.h"
#endif
#include "script.h"
#include "app.h"

// end of rt.h
