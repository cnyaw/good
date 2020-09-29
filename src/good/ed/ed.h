
//
// ed.h
// Good engine editor.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/11/29 Waync created.
//

#pragma once

//
// General.
//

#include "../good.h"

#include "util.h"
#include "undo.h"

//
// Commands.
//

namespace good {
namespace ed {

enum
{
  // Level.
  GOOD_LEVELED_CMD_ADDOBJ,
  GOOD_LEVELED_CMD_REMOVEOBJ,
  GOOD_LEVELED_CMD_MOVEOBJ,
  GOOD_LEVELED_CMD_ZORDER,
  GOOD_LEVELED_CMD_SETBGCOLOR,
  GOOD_LEVELED_CMD_SETDIM,
  GOOD_LEVELED_CMD_SETPROP,
  GOOD_LEVELED_CMD_CHANGEPARENT,
  GOOD_LEVELED_CMD_SETSCRIPT,
  GOOD_LEVELED_CMD_SETSIZE,
  GOOD_LEVELED_CMD_SETLEVELPOS,
  GOOD_LEVELED_CMD_SETNAME,
  // Map.
  GOOD_MAPED_CMD_DRAW,
  GOOD_MAPED_CMD_DRAW_PATTERN,
  GOOD_MAPED_CMD_FILL,
  // Sprite.
  GOOD_SPRITEED_CMD_INSERT,
  GOOD_SPRITEED_CMD_REMOVE,
  GOOD_SPRITEED_CMD_SETTIME
};

}
} // namespace good::ed

//
// good.
//

#include "snd.h"
#include "tex.h"
#include "ts.h"
#include "map.h"
#include "spr.h"
#include "obj.h"
#include "lvl.h"
#include "grp.h"

#include "prj.h"

// end of ed.h
