
//
// sdl_app.h
// SDL test runtime.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/08/17 Waync created.
//

#pragma once

#include "SDL.h"

#ifdef WIN32
# define GOOD_SUPPORT_GDIPLUS
#else
# define GOOD_SUPPORT_STB_IMG
#endif

#include "../gx/sdl_gx.h"
#include "../gx/imgp_gx.h"
#include "../snd/audiere_snd.h"

namespace good {

namespace rt {

#define TICK (int)(1000 / GOOD_DEFAULT_TICK_PER_SECOND)

class SDLApplication : public Application<SDLApplication, gx::SDLImage, snd::AudiereSound, gx::Imgp>
{
  SDLApplication() : gx(mScreen)
  {
  }
public:

  //
  // Display.
  //

  gx::SDLGraphics gx;
  SDL_Surface* mScreen;

  static SDLApplication& getInst()
  {
    static SDLApplication inst;
    return inst;
  }

  void go(std::string const& name)
  {
    SDL_Init(SDL_INIT_EVERYTHING);

    if (init(name)) {
      mainLoop();
    }

    uninit();
  }

  void mainLoop()
  {
    //
    // Main loop.
    //

    Uint32 nextTime = SDL_GetTicks() + TICK;
    Uint32 keys = 0;

    while (true) {

      //
      // Poll event.
      //

      if (poll(keys)) {
        break;
      }

      sw2::IntPoint ptMouse;
      Uint8 btnMouse = SDL_GetMouseState(&ptMouse.x, &ptMouse.y);

      if (btnMouse & SDL_BUTTON_LMASK) {
        keys |= GOOD_KEYS_LBUTTON;
      } else {
        keys &= ~GOOD_KEYS_LBUTTON;
      }

      if (btnMouse & SDL_BUTTON_RMASK) {
        keys |= GOOD_KEYS_RBUTTON;
      } else {
        keys &= ~GOOD_KEYS_RBUTTON;
      }

      //
      // Update.
      //

      if (trigger(keys, ptMouse)) {
        renderAll();
        SDL_Flip(mScreen);
      }

      //
      // FPS control.
      //

      SDL_Delay(timeLeft(nextTime));
      nextTime += TICK;
    }
  }

  //
  // Init/uninit.
  //

  bool doInit(std::string const& name)
  {
    if (0 > SDL_Init(SDL_INIT_VIDEO)) {
      SW2_TRACE_ERROR("init SDL failed");
      return false;
    }

    int flag = SDL_DOUBLEBUF | SDL_HWSURFACE;
    if (mRes.mFullScreen) {
      flag |= SDL_FULLSCREEN;
    }

    mScreen = SDL_SetVideoMode(mRes.mWidth, mRes.mHeight, mRes.mColorBits, flag);
    if (!mScreen) {
      SW2_TRACE_ERROR("set video mode(%dx%dx%d) failed", mRes.mWidth, mRes.mHeight, mRes.mColorBits);
      SDL_Quit();
      return false;
    }

    if (!mRes.mName.empty()) {
      SDL_WM_SetCaption(mRes.mName.c_str(), NULL);
    }

    gx.mSur = mScreen;

    return true;
  }

  void doUninit()
  {
    gx::SDLImageResource::inst().clear();
    snd::AudiereSoundResource::inst().free();
    SDL_Quit();
  }

  bool poll(Uint32& keys) const
  {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch(event.type)
      {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_UP: keys |= GOOD_KEYS_UP; break;
        case SDLK_DOWN: keys |= GOOD_KEYS_DOWN; break;
        case SDLK_LEFT: keys |= GOOD_KEYS_LEFT; break;
        case SDLK_RIGHT: keys |= GOOD_KEYS_RIGHT; break;
        case SDLK_ESCAPE: keys |= GOOD_KEYS_ESCAPE; break;
        case SDLK_RETURN: keys |= GOOD_KEYS_RETURN; break;
        case SDLK_z: keys |= GOOD_KEYS_BTN_A; break;
        case SDLK_x: keys |= GOOD_KEYS_BTN_B; break;
        }
        return false;

      case SDL_KEYUP:
        switch (event.key.keysym.sym)
        {
        case SDLK_UP: keys &= ~GOOD_KEYS_UP; break;
        case SDLK_DOWN: keys &= ~GOOD_KEYS_DOWN; break;
        case SDLK_LEFT: keys &= ~GOOD_KEYS_LEFT; break;
        case SDLK_RIGHT: keys &= ~GOOD_KEYS_RIGHT; break;
        case SDLK_ESCAPE: keys &= ~GOOD_KEYS_ESCAPE; break;
        case SDLK_RETURN: keys &= ~GOOD_KEYS_RETURN; break;
        case SDLK_z: keys &= ~GOOD_KEYS_BTN_A; break;
        case SDLK_x: keys &= ~GOOD_KEYS_BTN_B; break;
        }
        return false;

      case SDL_QUIT:
        return true;
      }
    }

    return false;
  }

  Uint32 timeLeft(Uint32 nextTime) const
  {
    Uint32 now = SDL_GetTicks();
    if(nextTime <= now) {
      return 0;
    } else {
      if ((int)(nextTime - now) > 3 * TICK) {
        nextTime = now + TICK;
      }
      return nextTime - now;
    }
  }
};

} // namespace rt

} // namespace good

// end of sdl_app.h
