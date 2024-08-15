//
// android_app.h
// Android application player.
//
// Copyright (c) 2016 Waync Cheng.
// All Rights Reserved.
//
// 2016/6/24 Waync created.
//

#pragma once

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

#define GOOD_SUPPORT_ANDROID_IMG
#include "gx/gx.h"

#include "swUtil.h"
#include "../gx/imgp_gx.h"

#include "rt/rt.h"

#include "../gx/opengl_gx.h"
#include "../snd/android_snd.h"

namespace good {

namespace rt {

template<class AppT>
class AndroidApplication : public Application<AppT, gx::GLImage, snd::AndroidSound>
{
public:

  gx::GLGraphics gx;
  int width;
  int height;

  void doUninit()
  {
    gx::GLImage::clear();

    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "sndStopAll", "()V");
    if (mid) {
      tmpEnv->CallStaticVoidMethod(cls, mid);
    }
  }

  void onPackageChanged()
  {
    AppT &app = AppT::getInst();
    app.gx.SCREEN_W = app.mRes.mWidth;
    app.gx.SCREEN_H = app.mRes.mHeight;
    app.gx.resize(width, height);
  }

  void doTrigger(int keys, int mousex, int mousey)
  {
    AppT &app = AppT::getInst();
    app.trigger(keys, sw2::IntPoint(mousex, mousey));
    if (app.mExit) {
      app.uninit();
    }
  }

  void doTrace(const char *s)
  {
    LOGI("%s\n", s);
  }

  void doUserIntEvent(int i) {
    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "doIntEvent", "(I)V");
    if (mid) {
      tmpEnv->CallStaticVoidMethod(cls, mid, i);
    }
  }
};

} // namespace rt

} // namespace good

// end of android_app.h
