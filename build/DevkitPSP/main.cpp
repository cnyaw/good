/*
    main.cpp
    good game player DevkitPSP.

    Copyright (c) 2010 waync cheng
    All Rights Reserved.

    2010/08/05 waync created
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspiofilemgr.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <sstream>
#include <iterator>

#include <pspgu.h>
#include <pspgum.h>
#include <pspctrl.h>

#include <algorithm>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

PSP_MODULE_INFO("Good devkitPSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

static unsigned int __attribute__((aligned(16))) list[262144];

int exitCallback(int arg1, int arg2, void *common)
{
  sceKernelExitGame();
  return 0;
}

int callbackThread(SceSize args, void *argp)
{
  int cbid;

  cbid = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
  sceKernelRegisterExitCallback(cbid);

  sceKernelSleepThreadCB();

  return 0;
}

int setupCallbacks(void)
{
  int thid = 0;

  thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, 0);
  if(thid >= 0) {
    sceKernelStartThread(thid, 0, 0);
  }

  return thid;
}

#define GOOD_SUPPORT_STGE
#define GOOD_SUPPORT_STB_IMG
#define STB_IMAGE_IMPLEMENTATION
#define GOOD_SUPPORT_NO_LOGO
#include "rt/rt.h"
#include "DevkitPSP_app.h"

#define printf pspDebugScreenPrintf

bool getPrjName(std::string& prjname)
{
  //
  // Find all project files in this folder.(.txt, .good, .zip).
  //

  char cwd[MAXPATHLEN];
  strcpy(cwd, "ms0:/PSP/GAME/good/");

  int dfd = sceIoDopen(cwd);
  if (0 > dfd) {
    return false;
  }

  std::vector<std::string> names;
  std::string name;

  SceIoDirent dir;
  memset(&dir, 0, sizeof(dir));

  while (0 < sceIoDread(dfd, &dir)) {
    if (0 == (dir.d_stat.st_attr & FIO_SO_IFDIR)) { // A file.
      name = dir.d_name;
      std::transform(name.begin(), name.end(), name.begin(), tolower);

      if (name.npos != name.find(".txt") ||
      name.npos != name.find(".good") ||
      name.npos != name.find(".zip"))
      names.push_back(dir.d_name);
    }
  }
  sceIoDclose(dfd);

  if (names.empty()) {
    return false;
  }

  std::sort(names.begin(), names.end());

  //
  // List project files.
  //

  static int sel = 0;

  pspDebugScreenInit();
  pspDebugScreenClear();

  printf("cwd: %s\n", cwd);

  for (size_t i = 0; i < names.size(); i++) {
    printf("  %s\n", names[i].c_str());
  }

  pspDebugScreenSetXY(0, 1 + sel);
  printf(">");

  //
  // Handle input.
  //

  SceCtrlData oldPad = {0};
  while (true) {

    SceCtrlData pad = {0};
    sceCtrlReadBufferPositive(&pad, 1);

    if (oldPad.Buttons != pad.Buttons) {

      if (pad.Buttons & PSP_CTRL_CIRCLE) {
        prjname = names[sel];
        return true;
      } else if (pad.Buttons & PSP_CTRL_LTRIGGER) {
        return false;
      } else if (pad.Buttons & PSP_CTRL_UP) {

        pspDebugScreenSetXY(0, 1 + sel);
        printf(" ");

        if (0 == sel) {
          sel = (int)names.size() - 1;
        } else {
          sel -= 1;
        }

        pspDebugScreenSetXY(0, 1 + sel);
        printf(">");

      } else if (pad.Buttons & PSP_CTRL_DOWN) {

        pspDebugScreenSetXY(0, 1 + sel);
        printf(" ");

        if ((int)names.size() - 1 == sel) {
          sel = 0;
        } else {
          sel += 1;
        }

        pspDebugScreenSetXY(0, 1 + sel);
        printf(">");
      }

      oldPad = pad;
    }

    sceDisplayWaitVblankStart();
  }

  return false;
}

int main(int argc, char* argv[])
{
  setupCallbacks();

  sceCtrlSetSamplingCycle(0);
  sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

  good::rt::DevkitPspApplication& g = good::rt::DevkitPspApplication::getInst();
  g.gx.init();

  std::string prj;
  while (getPrjName(prj)) {
    g.loopGame(prj);
  }

  g.gx.uninit();

  sceKernelExitGame();
  return 0;
}

//
// Workaround for fixing link error.
//

extern "C" {
  int _isatty(int file) {
    (void)file;                         // Avoid warning.
    return 1;
  }
}
