
//
// iPhone_app.h
// iPhone application player.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/07/24 Waync created.
//

#pragma once

#define GOOD_SUPPORT_STB_IMG
#define STB_IMAGE_IMPLEMENTATION

#include "gx/opengl_gx.h"
#include "gx/imgp_gx.h"
#include "snd/openal_snd.h"

namespace good {

namespace rt {

class IPhoneApplication : public Application<IPhoneApplication, gx::GLImage, snd::ALSound>
{
  enum {
    TIME_HANDLE_VIRTUAL_MOVE = 8,
    DELTAX = 10,
    DELTAY = 10
  };

  int mElapsedHitTime;

  sw2::IntPoint mCurPt, mHitPt;
  int mKeys;

  IPhoneApplication()
  {
    mElapsedHitTime = 0;
    mKeys = 0;
  }

public:

  static IPhoneApplication& getInst()
  {
    static IPhoneApplication i;
    return i;
  }

  bool doInit(std::string const& name)
  {
    return true;
  }

  void doUninit()
  {
    gx::GLImage::clear();
  }

  void doTrigger()
  {
    if (0 != (mKeys & GOOD_KEYS_LBUTTON)) {

      mElapsedHitTime++;

      mKeys &= ~(GOOD_KEYS_UP | GOOD_KEYS_DOWN | GOOD_KEYS_LEFT | GOOD_KEYS_RIGHT);

      if (mCurPt.x > mHitPt.x + DELTAX) {
        mKeys |= GOOD_KEYS_RIGHT;
      } else if (mCurPt.x < mHitPt.x - DELTAX) {
        mKeys |= GOOD_KEYS_LEFT;
      }

      if (mCurPt.y > mHitPt.y + DELTAY) {
        mKeys |= GOOD_KEYS_DOWN;
      } else if (mCurPt.y < mHitPt.y - DELTAY) {
        mKeys |= GOOD_KEYS_UP;
      }
    }

    trigger(mKeys, mCurPt);

    if (mExit) {
      exit(0);
    }
  }

  void doRender(gx::GLGraphics& gx)
  {
    render(gx);

    if (0 != (mKeys & GOOD_KEYS_LBUTTON) &&
        TIME_HANDLE_VIRTUAL_MOVE <= mElapsedHitTime) {
      gx.fillSolidColor(
           mHitPt.x - 5,
           mHitPt.y - 5,
           10,
           10,
           0x80ff0000,
           .0f,
           1.0f,
           1.0f);
      //gx.fillSolidColor(mCurPt.x - 5, mCurPt.y - 5, 10, 10, 0x80ffff00);
    }
  }

  //
  // Handle input, virtual pad, state: 0=begin, 1=move, 2=end.
  //

  void handleTouchs(std::vector<sw2::IntPoint> const& inputs, int state)
  {
    if (inputs.empty()) {
      return;
    }

    sw2::IntPoint const& pt = inputs[0];

    switch (state)
    {
    case 0:                             // Touch begin.
      mKeys |= GOOD_KEYS_LBUTTON;
      mElapsedHitTime = 0;
      mCurPt = mHitPt = pt;
      break;

    case 1:                             // Touch move.
      mCurPt = pt;
      break;

    case 2:                             // Touch end.
      mKeys = 0;
      mElapsedHitTime = 0;
      break;
    }
  }
};

} // namespace rt

} // namespace good

// end of iPhone_app.h
