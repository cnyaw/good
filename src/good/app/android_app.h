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

#define GOOD_SUPPORT_ANDROID_IMG
#include "../gx/opengl_gx.h"
#include "../gx/imgp_gx.h"
#include "../snd/android_snd.h"

namespace good {

namespace rt {

template<class AppT>
class AndroidApplication : public Application<AppT, gx::GLImage, snd::AndroidSound, gx::Imgp>
{
public:

  gx::GLGraphics gx;
  int width;
  int height;

  void doUninit()
  {
    gx::GLImageResource::inst().clear();

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

  bool doOpenUrl(std::string const& url) const
  {
    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "doOpenUrl", "(Ljava/lang/String;)V");
    if (mid) {
      jstring jstr = tmpEnv->NewStringUTF(url.c_str());
      tmpEnv->CallStaticVoidMethod(cls, mid, jstr);
    }
    return true;
  }
};

} // namespace rt

} // namespace good

// end of android_app.h
