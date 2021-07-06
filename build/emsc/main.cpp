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
#include "SDL/SDL_opengl.h"

#include <emscripten.h>

#define GOOD_SUPPORT_ONLY_ONE_TEXTURE
#define GOOD_SUPPORT_STGE
#define GOOD_SUPPORT_NO_LOGO
#include "rt/rt.h"

#define GOOD_SUPPORT_STB_IMG
#define GOOD_SUPPORT_EMSC_IMG
#include "gx/opengl_gx.h"
#include "gx/imgp_gx.h"
#include "snd/openal_snd.h"

class EmscFileSystem : public sw2::ArchiveFileSystem
{
public:
  mutable std::string pkgName;

  EmscFileSystem()
  {
  }

  virtual bool isFileExist(std::string const& name) const
  {
    return good::isGoodArchive(name);
  }

  virtual bool loadFile(std::string const& name, std::ostream& outs, std::string const& password) const
  {
    if (!good::isGoodArchive(name)) {
      return false;
    }
    pkgName = name;
    char buff[128];
    sprintf(buff, "loadPkg('%s', 'cLoadPkg')", name.c_str());
    emscripten_run_script(buff);
    return false;                       // FIXME: workaround now, return fail but addFileSystem in cLoadPkg.
  }
};

EmscFileSystem fs;

class EmccApplication : public good::rt::Application<EmccApplication, good::gx::GLImage, good::snd::ALSound, good::gx::Imgp>
{
  EmccApplication()
  {
  }

public:

  good::gx::GLGraphics gx;
  SDL_Surface* mScreen;
  int app_keys;

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

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    mScreen = SDL_SetVideoMode(mRes.mWidth, mRes.mHeight, 16, SDL_OPENGL);
    if (!mScreen) {
      SW2_TRACE_ERROR("set video mode(%dx%d) failed", mRes.mWidth, mRes.mHeight);
      SDL_Quit();
      return false;
    }

    if (!mRes.mName.empty()) {
      SDL_WM_SetCaption(mRes.mName.c_str(), NULL);
    }

    gx.init();
    gx.SCREEN_W = mRes.mWidth;
    gx.SCREEN_H = mRes.mHeight;
    gx.resize(mRes.mWidth, mRes.mHeight);

    trace("package loaded %s %dx%d", name.c_str(), mRes.mWidth, mRes.mHeight);
    emscripten_set_canvas_size(mRes.mWidth, mRes.mHeight);

    mAr->addFileSystem(&fs);
    app_keys = 0;

    return true;
  }

  void doUninit()
  {
    good::gx::GLImageResource::inst().clear();
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

    if (trigger(keys, ptMouse)) {
      renderAll();
    }

    if (mExit) {
      ::exit(0);
    }
  }
};

EmccApplication &app = EmccApplication::getInst();

void NotifyImageManagerSurfaceChanged()
{
  app.handleImageManagerSurfaceChanged();
}

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

extern "C" {

int EMSCRIPTEN_KEEPALIVE cLoadImageFromChar(int w, int h, void *pBuff, int size)
{
  return good::gx::GxImage::cLoadImageFromChar(w, h, pBuff, size);
}

int EMSCRIPTEN_KEEPALIVE cLoadPkg(void *pBuff, int size)
{
  std::string stream((const char*)pBuff, size);
  std::istringstream ss(stream);
  return app.addFileSystem(fs.pkgName, ss);
}

int EMSCRIPTEN_KEEPALIVE cSetKeyStates(int keys)
{
  app.app_keys = keys;
  return 1;
}

int EMSCRIPTEN_KEEPALIVE cRunPkg(void *pBuff, int size)
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

} // extern "C"

int main(int argc, char* argv[])
{
  srand(time(0));
  SW2_TRACE_FUNC(GoodTraceTool);
  if (2 == argc) {
    char buff[128];
    sprintf(buff, "loadPkg('%s', 'cRunPkg')", argv[1]);
    emscripten_run_script(buff);
    emscripten_exit_with_live_runtime();
  } else {
    app.trace("no good package assigned");
  }
  return 0;
}
