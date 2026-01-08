//
// emsc_lib.h
// EMSC application common.
//
// Copyright (c) 2021 Waync Cheng.
// All Rights Reserved.
//
// 2021/7/7 Waync created.
//

#pragma once

void NotifyImageManagerSurfaceReset()
{
  app.handleImageManagerSurfaceReset();
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
  return app.addStreamFileSystem(app.fs.pkgName, stream);
}

int EMSCRIPTEN_KEEPALIVE cRunPkg(void *pBuff, int size)
{
  printf("init good, data len=%d\n", size);
  std::string stream((const char*)pBuff, size);
  if (!app.initFromStream(stream)) {
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
