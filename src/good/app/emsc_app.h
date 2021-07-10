//
// emsc_app.h
// EMSC application player.
//
// Copyright (c) 2021 Waync Cheng.
// All Rights Reserved.
//
// 2021/7/7 Waync created.
//

#pragma once

#include <algorithm>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <cctype>
#include <math.h>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include <emscripten.h>

#define GOOD_SUPPORT_ONLY_ONE_TEXTURE
#define GOOD_SUPPORT_NO_LOGO
#include "rt/rt.h"

#define GOOD_SUPPORT_STB_IMG
#define GOOD_SUPPORT_EMSC_IMG
#include "gx/opengl_gx.h"
#include "gx/imgp_gx.h"
#include "snd/openal_snd.h"

namespace good {

namespace rt {

class EmscFileSystem : public sw2::ArchiveFileSystem
{
public:
  mutable std::string pkgName;

  EmscFileSystem()
  {
  }

  virtual bool isFileExist(std::string const& name) const
  {
    return isGoodArchive(name);
  }

  virtual bool loadFile(std::string const& name, std::ostream& outs, std::string const& password) const
  {
    if (!isGoodArchive(name)) {
      return false;
    }
    pkgName = name;
    char buff[128];
    sprintf(buff, "loadPkg('%s', 'cLoadPkg')", name.c_str());
    emscripten_run_script(buff);
    return true;
  }
};

template<class AppT>
class EmscApplication : public Application<AppT, gx::GLImage, snd::ALSound, gx::Imgp>
{
public:

  typedef Application<AppT, gx::GLImage, snd::ALSound, gx::Imgp> BaseT;
  EmscFileSystem fs;
  gx::GLGraphics gx;
  SDL_Surface* mScreen;
  int app_keys;

  bool doInit(std::string const& name)
  {
    if (0 > SDL_Init(SDL_INIT_VIDEO)) {
      SW2_TRACE_ERROR("init SDL failed");
      return false;
    }

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    mScreen = SDL_SetVideoMode(BaseT::mRes.mWidth, BaseT::mRes.mHeight, 16, SDL_OPENGL);
    if (!mScreen) {
      SW2_TRACE_ERROR("set video mode(%dx%d) failed", BaseT::mRes.mWidth, BaseT::mRes.mHeight);
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

    BaseT::trace("package loaded %s %dx%d", name.c_str(), BaseT::mRes.mWidth, BaseT::mRes.mHeight);
    emscripten_set_canvas_size(BaseT::mRes.mWidth, BaseT::mRes.mHeight);

    BaseT::mAr->addFileSystem(&fs);
    app_keys = 0;

    return true;
  }

  void doUninit()
  {
    gx::GLImageResource::inst().clear();
    SDL_Quit();
  }

  void doTrace(const char *s)
  {
    printf("%s\n", s);
  }

  void step()
  {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      // NOP.
    }

    Uint32 keys = app_keys;

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

    if (BaseT::trigger(keys, ptMouse)) {
      BaseT::renderAll();
    }

    if (BaseT::mExit) {
      ::exit(0);
    }
  }
};

} // namespace rt

} // namespace good

// end of emsc_app.h
