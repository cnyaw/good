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

#include <emscripten.h>
#include <emscripten/html5.h>

#define GOOD_SUPPORT_STB_IMG
#define GOOD_SUPPORT_EMSC_IMG
#include "gx/gx.h"

#include "swUtil.h"
#include "../gx/imgp_gx.h"

#define GOOD_SUPPORT_ONLY_ONE_TEXTURE
#define GOOD_SUPPORT_NO_LOGO
#include "rt/rt.h"

#include "sdl_app.h"

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
class EmscApplication : public SDLApplicationBase<AppT>
{
public:

  typedef SDLApplicationBase<AppT> BaseT;
  EmscFileSystem fs;

  bool doInit(std::string const& name)
  {
    if (!BaseT::doInit(name)) {
      SW2_TRACE_ERROR("BaseT::init failed");
      return false;
    }

    BaseT::trace("package loaded %s %dx%d", name.c_str(), BaseT::mRes.mWidth, BaseT::mRes.mHeight);
    emscripten_set_canvas_element_size("canvas", BaseT::mRes.mWidth, BaseT::mRes.mHeight);

    BaseT::mAr->addArchiveFileSystem(&fs);

    return true;
  }

  void doTrace(const char *s)
  {
    printf("%s\n", s);
  }
};

} // namespace rt

} // namespace good

// end of emsc_app.h
