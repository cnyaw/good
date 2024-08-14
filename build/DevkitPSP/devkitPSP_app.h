
//
// devkitPSP_app.h
// DevkitPSP application player.
//
// Copyright (c) 20010 Waync Cheng.
// All Rights Reserved.
//
// 2010/08/04 Waync created.
//

#pragma once

#include "devkitpsp_gx.h"
#include "gx/imgp_gx.h"
#include "snd/openal_snd.h"

namespace good {

namespace rt {

class DevkitPspApplication : public Application<DevkitPspApplication, gx::DevkitPSPImage, snd::ALSound, gx::Imgp>
{
  DevkitPspApplication()
  {
  }

public:

  gx::DevkitPSPGraphics gx;

  static DevkitPspApplication& getInst()
  {
    static DevkitPspApplication i;
    return i;
  }

  void loopGame(std::string const& prj)
  {
    if (init(prj)) {
      while (!mExit) {
        doTrigger();
        doRender();
      }
    }

    uninit();
  }

  bool doInit(std::string const& name)
  {
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

    return true;
  }

  void doUninit()
  {
    gx::DevkitPSPImage::clear();
  }

  void doTrigger()
  {
    //
    // Translate inputs.
    //

    SceCtrlData pad = {0};
    sceCtrlReadBufferPositive(&pad, 1);

    int keys = 0;

    if (pad.Buttons & PSP_CTRL_CIRCLE) {
      keys |= GOOD_KEYS_BTN_A;
    }

    if (pad.Buttons & PSP_CTRL_CROSS) {
      keys |= GOOD_KEYS_BTN_B;
    }

    if (pad.Buttons & PSP_CTRL_START) {
      keys |= GOOD_KEYS_RETURN;
    }

    if (pad.Buttons & PSP_CTRL_LTRIGGER) {
      keys |= GOOD_KEYS_ESCAPE;
    }

    if (pad.Buttons & PSP_CTRL_UP) {
      keys |= GOOD_KEYS_UP;
    } else if (pad.Buttons & PSP_CTRL_DOWN) {
      keys |= GOOD_KEYS_DOWN;
    }

    if (pad.Buttons & PSP_CTRL_LEFT) {
      keys |= GOOD_KEYS_LEFT;
    } else if (pad.Buttons & PSP_CTRL_RIGHT) {
      keys |= GOOD_KEYS_RIGHT;
    }

    //
    // Trigger game.
    //

    trigger(keys, sw2::IntPoint());
  }

  void doRender()
  {
    if (mDirty) {
      renderAll();
    }
  }
};

} // namespace rt

} // namespace good

// end of devkitPSP_app.h
