//
// android_lib.h
// Android JNI lib implementation.
//
// Copyright (c) 2016 Waync Cheng.
// All Rights Reserved.
//
// 2016/8/31 Waync created.
//

#pragma once

namespace good {

namespace rt {

//
// NOTE: the global variable g is the Application instance.
//

class AndroidAssetFileSystem : public sw2::ArchiveFileSystem
{
public:
  AAssetManager* assmgr;

  AndroidAssetFileSystem() : assmgr(0)
  {
  }

  virtual bool isFileExist(std::string const& name) const
  {
    if (0 == assmgr) {
      return false;
    }

    AAsset* asset = AAssetManager_open(assmgr, name.c_str(), AASSET_MODE_UNKNOWN);
    if (0 == asset) {
      return false;
    }

    AAsset_close(asset);

    return true;
  }

  virtual bool loadFile(std::string const& name, std::ostream& outs, std::string const& password) const
  {
    if (0 == assmgr) {
      return false;
    }

    AAsset* asset = AAssetManager_open(assmgr, name.c_str(), AASSET_MODE_UNKNOWN);
    if (0 == asset) {
      return false;
    }

    long size = AAsset_getLength(asset);
    char* buffer = (char*)malloc(sizeof(char) * size);
    AAsset_read(asset, buffer, size);
    AAsset_close(asset);

    outs.write(buffer, size);
    free(buffer);

    return true;
  }
};

} // rt

} // good

extern "C" {

JNIEXPORT void JNICALL Java_weilican_good_goodJniActivity_setcwd(JNIEnv * env, jobject obj, jstring path)
{
  if (!path) {
    return;
  }
  const char* strChars = env->GetStringUTFChars(path, (jboolean *)0);
  chdir(strChars);
}

JNIEXPORT void JNICALL Java_weilican_good_goodJniActivity_appDestroy(JNIEnv *env, jobject obj)
{
  g.onAppDestroy();
}

JNIEXPORT void JNICALL Java_weilican_good_goodJniActivity_appPause(JNIEnv *env, jobject obj)
{
  g.onAppPause();
}

JNIEXPORT void JNICALL Java_weilican_good_goodJniLib_create(JNIEnv * env, jobject obj, jstring str, jobject assetManager)
{
  tmpEnv = env;

  if (!str) {
    return;
  }

  g.onAppCreate();

  const char* strChars = env->GetStringUTFChars(str, (jboolean *)0);
  if ('/' == strChars[0]) {
    g.uninit();
    if (g.init(strChars)) {
      g.trace("init good succ: %d, %d", g.mRes.mWidth, g.mRes.mHeight);
    } else {
      g.trace("init good from file %s fail", strChars);
    }
  } else {
    AAssetManager* assmgr = AAssetManager_fromJava(env, assetManager);
    if (NULL == assmgr) {
      g.trace("asset mgr is null");
    }

    g.uninit();

    static good::rt::AndroidAssetFileSystem fs;
    fs.assmgr = assmgr;

    std::stringstream ss;
    if (!fs.loadFile(strChars, ss, "")) {
      g.trace("_ASSET_NOT_FOUND_ %s", strChars);
      return;
    }

    if (g.initFromStream(ss.str())) {
      g.trace("init good succ: %d, %d", g.mRes.mWidth, g.mRes.mHeight);
      g.mAr->addArchiveFileSystem(&fs);
    } else {
      g.trace("init good from asset %s fail", strChars);
    }
  }
}

JNIEXPORT void JNICALL Java_weilican_good_goodJniLib_init(JNIEnv *env, jobject obj, jint width, jint height)
{
  tmpEnv = env;

  g.width = width;
  g.height = height;
  g.gx.init();
  g.gx.resize(width, height);
  g.gx.restoreSur();
  g.mDirty = true;                      // Force redraw.
}

JNIEXPORT jboolean JNICALL Java_weilican_good_goodJniLib_step(JNIEnv *env, jobject obj, jint keys, jfloat mousex, jfloat mousey)
{
  tmpEnv = env;

  int x = (int)(mousex * g.mRes.mWidth / (float)g.width);
  int y = (int)(mousey * g.mRes.mHeight / (float)g.height);

  g.doTrigger((int)keys, x, y);
#ifdef GOOD_RENDER_WITH_DIRTY_FLAG
  if (g.mDirty) {
    g.renderAll();
  }
#else
  g.renderAll();
#endif

  return (jboolean)g.mExit;
}

JNIEXPORT jbyteArray JNICALL Java_weilican_good_goodJniLib_getSound(JNIEnv *env, jobject obj, jint idRes)
{
  tmpEnv = env;

  return good::snd::AndroidSound::getSoundStream(idRes);
}

} // extern "C"

// end of android_lib.h
