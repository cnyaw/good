//
// 2017/6/3 Waync created.
//

#include <algorithm>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <cctype>
#include <math.h>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"

#include <emscripten.h>

#define GOOD_SUPPORT_STGE
#define GOOD_SUPPORT_NO_LOGO
#include "rt/rt.h"

#define GOOD_SUPPORT_PNG
#include "gx/sdl_gx.h"
#include "gx/imgp_gx.h"
#include "snd/openal_snd.h"

class EmccApplication : public good::rt::Application<EmccApplication, good::gx::SDLImage, good::snd::ALSound, good::gx::Imgp>
{
  EmccApplication() : gx(mScreen)
  {
  }

public:

  good::gx::SDLGraphics gx;
  SDL_Surface* mScreen;

  static EmccApplication& getInst()
  {
    static EmccApplication i;
    return i;
  }

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

    trace("package loaded %s %dx%d", name.c_str(), mRes.mWidth, mRes.mHeight);
    emscripten_set_canvas_size(mRes.mWidth, mRes.mHeight);
    return true;
  }

  void doUninit()
  {
    good::gx::SDLImageResource::inst().clear();
    SDL_Quit();
  }

  void doTrace(const char *s)
  {
    printf("%s\n", s);
  }

  void step()
  {
    Uint32 keys = 0;
    poll(keys);

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

    if (trigger(keys, ptMouse)) {
      renderAll(gx);
    }

    if (mExit) {
      ::exit(0);
    }
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

EmccApplication &app = EmccApplication::getInst();

void trigger()
{
  app.step();
}

void GoodTraceTool(int level, const char* format, va_list args)
{
  char buf[1024];
  vsnprintf(buf, sizeof(buf), format, args);
  app.trace(buf);
}

int main(int argc, char* argv[])
{
  SW2_TRACE_FUNC(GoodTraceTool);
  if (2 == argc) {
    char buff[128];
    sprintf(buff, "loadPkg('%s', 'cRunPkg')", argv[1]);
    emscripten_run_script(buff);
  } else {
    app.trace("no good package assigned");
  }
  return 0;
}

extern "C" int EMSCRIPTEN_KEEPALIVE cRunPkg(void *pBuff, int size)
{
  printf("init good, data len=%d\n", size);
  std::string stream((const char*)pBuff, size);
  std::istringstream ss(stream);
  if (!app.init(ss)) {
    return 1;
  }
  printf("init good done\n");
  emscripten_set_main_loop(trigger, 0, 1);
  app.uninit();
  return 0;
}
