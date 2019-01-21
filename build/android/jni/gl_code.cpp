/*
  gl_code.cpp
  good player Android platforma implementation.

  Copyright (c) 2014 Waync Cheng.
  All Rights Reserved.

  2014/7/6 Waync created
 */

#define GOOD_SUPPORT_STGE

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

void GoodTraceTool(int level, const char* format, va_list args)
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
