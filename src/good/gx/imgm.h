
//
// imgm.h
// Common image manager.
//
// Copyright (c) 2015 Waync Cheng.
// All Rights Reserved.
//
// 2015/9/18 Waync created.
//

#pragma once

#include "gx.h"
#include "rcpack.h"

#ifdef GOOD_SUPPORT_ONLY_ONE_TEXTURE
void NotifyImageManagerSurfaceChanged();
#endif

namespace good {

namespace gx {

class ImageSurface
{
public:

  RectPackNode *root;
  GxImage img;

  ImageSurface() : root(0)
  {
  }

  ~ImageSurface()
  {
    free();
  }

  bool init(int TEX_WIDTH, int TEX_HEIGHT)
  {
    reset(TEX_WIDTH, TEX_HEIGHT);
    if (0 == root) {
      return false;
    }

    if (!img.create(TEX_WIDTH, TEX_HEIGHT, 4)) {
      return false;
    }

    return true;
  }

  void free()
  {
    if (root) {
      root->free();
      root = 0;
    }
  }

  void reset(int TEX_WIDTH, int TEX_HEIGHT)
  {
    free();
    root = new RectPackNode(0, 0, TEX_WIDTH, TEX_HEIGHT);
  }

  bool add(sw2::IntRect &r)
  {
    return root->add(r);
  }

  void draw(sw2::IntRect const &rc, GxImage const &aimg)
  {
    img.draw(rc.left, rc.top, aimg);
  }

  int size() const
  {
    if (root) {
      return root->size(root);
    } else {
      return 0;
    }
  }
};

class ImageRect
{
public:

  ImageRect() : sur(0), left(0), top(0), w(0), h(0)
  {
  }

  void *sur;
  int left, top, w, h;
};

template<class T, class SurT>
class ImageManager
{
protected:
  std::vector<SurT*> mSur;
  std::map<std::string, ImageRect> mImg;

  ImageManager()
  {
  }

public:

  ~ImageManager()
  {
    clear();
  }

  int GetTextureCount() const
  {
    return (int)mSur.size();
  }

  SurT* GetTex(int i) const
  {
    return mSur[i];
  }

  void clear()
  {
    mImg.clear();

    typename std::vector<SurT*>::iterator it = mSur.begin();
    for (; it != mSur.end(); ++it) {
      delete *it;
    }

    mSur.clear();
  }

  void UpdateSurface(SurT *psur, sw2::IntRect const &rc, GxImage const &img, ImageRect &sur)
  {
    psur->draw(rc, img);
    sur.left = rc.left;
    sur.top = rc.top;
    sur.w = img.w;
    sur.h = img.h;
    sur.sur = (void*)psur;
  }

  bool LoadSurface(GxImage &img, ImageRect& sur)
  {
    //
    // Add the image to existing texture pack.
    //

    img.convert32();                    // Make sure img is 32-bits.

    sw2::IntRect rc(0, 0, img.w, img.h);

    typename std::vector<SurT*>::iterator it;
    for (it = mSur.begin(); it != mSur.end(); ++it) {
      SurT *psur = *it;
      if (psur->add(rc)) {
        UpdateSurface(psur, rc, img, sur);
        return true;
      }
    }

#ifdef GOOD_SUPPORT_ONLY_ONE_TEXTURE
    if (!mSur.empty()) {
      SurT *psur = mSur[0];
      if (psur) {
        mImg.clear();
        psur->reset(psur->img.w, psur->img.h);
        memset(psur->img.dat, 0, psur->img.w * psur->img.h * psur->img.bpp);
        NotifyImageManagerSurfaceChanged();
        if (psur->add(rc)) {
          UpdateSurface(psur, rc, img, sur);
          return true;
        }
      }
      return false;
    }
    // There is no packed texture created yet, fall through to create one.
#endif

    //
    // Add the image to new texture pack.
    //

    SurT *psur = new SurT;
    if (0 == psur) {
      return false;
    }

    if (!psur->init()) {
      delete psur;
      return false;
    }

    mSur.push_back(psur);

    if (psur->add(rc)) {
      UpdateSurface(psur, rc, img, sur);
      return true;
    }

    return false;
  }

  bool existImage(std::string const& name)
  {
    return mImg.end() != mImg.find(name);
  }

  ImageRect const* getImage(std::string const& name)
  {
    typename std::map<std::string, ImageRect>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return &it->second;
    }

    std::string s;
    if (!sw2::Util::loadFileContent(name.c_str(), s)) {
      SW2_TRACE_ERROR("open image resource %s failed", name.c_str());
      mImg[name] = ImageRect();
      return 0;
    }

    return getImage_i(name, s);
  }

  ImageRect const* getImage_i(std::string const& name, std::string const& stream)
  {
    GxImage img;
    if (!img.loadFromStream(stream)) {
      SW2_TRACE_ERROR("load image stream %s failed", name.c_str());
      mImg[name] = ImageRect();
      return 0;
    }
    return getImage_i(name, img);
  }

  ImageRect const* getImage(std::string const& name, std::string const& stream)
  {
    typename std::map<std::string, ImageRect>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return &it->second;
    }
    return getImage_i(name, stream);
  }

  ImageRect const* getImage_i(std::string const& name, GxImage &img)
  {
    ImageRect sur;
    if (((T*)this)->LoadSurface(img, sur)) {
      mImg[name] = sur;
      return &mImg[name];
    } else {
      SW2_TRACE_ERROR("load image %s failed", name.c_str());
      mImg[name] = sur;
      return 0;
    }
  }

  ImageRect const* getImage(std::string const& name, GxImage &img)
  {
    typename std::map<std::string, ImageRect>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return &it->second;
    }
    return getImage_i(name, img);
  }
};

} // namespace gx

} // namespace good

// end of imgm.h
