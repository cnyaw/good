
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

#ifdef __EMSCRIPTEN__
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "../snd/openal_snd.h"
#define SndT snd::ALSound
#else
#include "SDL.h"
#include "SDL_opengl.h"
#include "../snd/audiere_snd.h"
#define SndT snd::AudiereSound
#endif

#ifdef WIN32
# define GOOD_SUPPORT_GDIPLUS_IMG
#else
# define GOOD_SUPPORT_STB_IMG
#endif

#include "../gx/opengl_gx.h"
#include "../gx/imgp_gx.h"

namespace good {

namespace rt {

#define TICK (int)(1000 / GOOD_DEFAULT_TICK_PER_SECOND)

template<class AppT>
class SDLApplicationBase : public Application<AppT, gx::GLImage, SndT, gx::Imgp>
{
public:
  typedef Application<AppT, gx::GLImage, SndT, gx::Imgp> BaseT;

  gx::GLGraphics gx;
  SDL_Surface* mScreen;

  bool step()
  {
    //
    // Poll event.
    //

    Uint32 keys = 0;
    if (poll(keys)) {
      return false;
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

    if (BaseT::trigger(keys, ptMouse)) {
      BaseT::renderAll();
      SDL_GL_SwapBuffers();
    }

    return true;
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

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);

    int flag = SDL_OPENGL | SDL_DOUBLEBUF;
    mScreen = SDL_SetVideoMode(BaseT::mRes.mWidth, BaseT::mRes.mHeight, 16, flag);
    if (!mScreen) {
      SW2_TRACE_ERROR("set video mode(%dx%dx%d) failed", BaseT::mRes.mWidth, BaseT::mRes.mHeight, 16);
      SDL_Quit();
      return false;
    }

    if (!BaseT::mRes.mName.empty()) {
      SDL_WM_SetCaption(BaseT::mRes.mName.c_str(), NULL);
    }

    gx.init();
    gx.SCREEN_W = BaseT::mRes.mWidth;
    gx.SCREEN_H = BaseT::mRes.mHeight;
    gx.resize(BaseT::mRes.mWidth, BaseT::mRes.mHeight);

    return true;
  }

  void doUninit()
  {
    gx::GLImageResource::inst().clear();
#ifndef __EMSCRIPTEN__
    snd::AudiereSoundResource::inst().free();
#endif
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
};

class SDLApplication : public SDLApplicationBase<SDLApplication>
{
  SDLApplication()
  {
  }
public:
  static SDLApplication& getInst()
  {
    static SDLApplication inst;
    return inst;
  }

  void go(std::string const& name)
  {
    if (init(name)) {
      mainLoop();
    }

    uninit();
  }

  void mainLoop()
  {
    sw2::FpsHelper fps;
    fps.start(mRes.mFps);

    while (step()) {
      fps.tick();
      fps.wait();
    }
  }
};

} // namespace rt

} // namespace good

// end of sdl_app.h
