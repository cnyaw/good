
//
// good.h
// Good editor, common header.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/01/06 Waync created.
//

#pragma once

#define CONFIG_FILE_NAME "Config"

#define EDITOR_SNAP_SCALE 8
#define WINDOW_MENU_POSITION 4
#define BKGND_TILE_SIZE 32

enum APP_GOOD_MESSAGE
{
  WM_GOOD_FIRST = WM_APP + 8,

  //
  // Undo/redo.
  //

  WM_GOOD_CANUNDO,                      // Return 0 if can't undo.
  WM_GOOD_UNDO,
  WM_GOOD_CANREDO,                      // Return 0 if can't redo.
  WM_GOOD_REDO,

  //
  // Update.
  //

  WM_GOOD_UPDATE,                       // Update view notify.
  WM_GOOD_SETCURSEL,                    // Resource tree selection change.

  //
  // Save file(source).
  //

  WM_GOOD_CANSAVE,                      // Return 0 if can't save.
  WM_GOOD_SAVE,                         // Save file.

  WM_GOOD_LAST
};

#define MSG_WM_GOOD(func) \
  if (WM_GOOD_FIRST <= uMsg && WM_GOOD_LAST > uMsg) \
  { \
    SetMsgHandled(TRUE); \
    lResult = (int)func(uMsg, wParam, lParam); \
    if(IsMsgHandled()) \
      return TRUE; \
  }

//
// Let MainFrame handle tooltip notification.
//

#define FORWARD_TOOLTIP_GETDISPINFO() \
  if (WM_NOTIFY == uMsg && \
      (TTN_GETDISPINFOA == ((LPNMHDR)lParam)->code || \
       TTN_GETDISPINFOW == ((LPNMHDR)lParam)->code)) { \
    FORWARD_NOTIFICATIONS() \
  }

enum APP_GOOD_RESOURCE_TYPE
{
  GOOD_RESOURCE_PROJECT,
  GOOD_RESOURCE_TEXTURE = 12,
  GOOD_RESOURCE_MAP,
  GOOD_RESOURCE_SPRITE,
  GOOD_RESOURCE_LEVEL,
  GOOD_RESOURCE_LEVEL_OBJECT,
  GOOD_RESOURCE_SCRIPT,
  GOOD_RESOURCE_AUDIO,
  GOOD_RESOURCE_PARTICLE,
  GOOD_RESOURCE_DEPENDENCY
};

//
// Include files.
//

#include "../good/ed/ed.h"

#define GOOD_SUPPORT_STGE
#include "../good/rt/rt.h"

#include "resource.h"
#include "resource1.h"

#include "../good/app/wtl_player.h"
#include "../good/gx/gdi_gx.h"

typedef good::gx::ImgpImage ImgT;
typedef good::gx::GdiGraphics GxT;
typedef good::ed::Project<ImgT> PrjT;

#include "Util.h"
#include "UtilWnd.h"

#include "Property.h"
#include "PropWnd.h"

#include "DlgAbout.h"
#include "DlgNewProj.h"
#include "DlgMapOption.h"
#include "DlgPicker.h"
#include "DlgAddNewItem.h"
#include "DlgSelTexDim.h"
#include "DlgSnapCustom.h"
#include "DlgGotoView.h"
#include "DlgLevelOption.h"

#include "Explorer.h"

#include "EditTex.h"
#include "EditMap.h"
#include "EditSprite.h"
#include "EditLevel.h"
#include "EditError.h"
#include "EditStgeScript.h"

// end of good.h
