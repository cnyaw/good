/*
  gl_code.cpp
  good player Android platforma implementation.

  Copyright (c) 2014 Waync Cheng.
  All Rights Reserved.

  2014/7/6 Waync created
 */

#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define JNI_ACTIVITY "weilican/good/goodJniActivity"

#define  LOG_TAG    "libgood"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include <algorithm>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <cctype>

#define glOrtho glOrthof                // Fixed GLES.

JNIEnv *tmpEnv;

#define GOOD_SUPPORT_STGE
#include "rt/rt.h"
#include "app/android_app.h"

class GoodApp : public good::rt::AndroidApplication<GoodApp>
{
  GoodApp();
public:
  bool bVStickMode;
  int touchState;                       // -1: none, 0: init, 1: stick mode.
  int touchX, touchY;
  unsigned int touchTime;

  static GoodApp& getInst()
  {
    static GoodApp i;
    return i;
  }

  bool doInit(std::string const& name)
  {
    touchState = -1;
    return true;
  }

  void doTrigger(int keys, int mousex, int mousey)
  {
    translateTouchEvent(keys, mousex, mousey);
    good::rt::AndroidApplication<GoodApp>::doTrigger(keys, mousex, mousey);
  }

  void translateTouchEvent(int &keys, int x, int y)
  {
    if (!bVStickMode) {
      return;
    }

    const int DELTAX = width / 10;
    const int DELTAY = height / 10;

    if (-1 == touchState && (keys & GOOD_KEYS_LBUTTON) && !isKeyDown(GOOD_KEYS_LBUTTON)) {
      touchState = 0;
      touchX = x;
      touchY = y;
      touchTime = sw2::Util::getTickCount() + 100;
    } else {
      if (0 == touchState) {
        if (touchX != x || touchY != y) {
          if (sw2::Util::getTickCount() > touchTime) {
            touchState = 1;
          } else {
            touchState = -1;
          }
        }
      } else if (1 == touchState) {
        if (keys & GOOD_KEYS_LBUTTON) {
          if (x > touchX + DELTAX) {
            keys |= GOOD_KEYS_RIGHT;
          } else if (x < touchX - DELTAX) {
            keys |= GOOD_KEYS_LEFT;
          }
          if (y > touchY + DELTAY) {
            keys |= GOOD_KEYS_DOWN;
          } else if (y < touchY - DELTAY) {
            keys |= GOOD_KEYS_UP;
          }
        } else {
          touchState = -1;
        }
      }
    }
  }
};

void GoodTraceTool(const char* format, va_list args)
{
  char buf[2048];
  vsnprintf(buf, sizeof(buf), format, args);
  GoodApp::getInst().trace(buf);
}

GoodApp::GoodApp() : bVStickMode(true)
{
  SW2_TRACE_FUNC(GoodTraceTool);        // Override do trace func.
}

GoodApp& g = GoodApp::getInst();

#include "app/android_lib.h"

extern "C" {

JNIEXPORT void JNICALL Java_weilican_good_goodAppActivity_setVStickMode(JNIEnv * env, jobject obj, jboolean bEnable)
{
  g.bVStickMode = (bool)(JNI_TRUE == bEnable);
}

} // extern "C"
