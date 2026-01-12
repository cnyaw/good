
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

void NotifyImageManagerSurfaceUpdate();
void NotifyImageManagerSurfaceReset();

namespace good {

namespace gx {

bool findBound(const GxImage &img, int &x, int &y, int &w, int &h);

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
    memset(img.dat, 0, img.w * img.h * img.bpp);
  }

  bool add(sw2::IntRect &r)
  {
    return root->add(r);
  }

  void draw(sw2::IntRect const &rc, GxImage const &aimg, int ox, int oy)
  {
    img.draw(rc.left, rc.top, aimg, ox, oy, rc.width(), rc.height());
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
  void *sur;
  int left, top, w, h, ox, oy, ow, oh;

  ImageRect() : sur(0), left(0), top(0), w(0), h(0)
  {
  }

  bool calcBound(int &offsetx, int &offsety, int &srcx, int &srcy, int &srcw, int &srch) const
  {
    sw2::IntRect rcSrc(srcx, srcy, srcx + srcw, srcy + srch);
    sw2::IntRect rcTex(ox, oy, ox + w, oy + h);
    sw2::IntRect rcInt;
    if (!rcSrc.intersect(rcTex, rcInt)) {
      return false;
    }
    srcx = rcInt.left - ox;
    srcy = rcInt.top - oy;
    srcw = rcInt.width();
    srch = rcInt.height();
    offsetx = rcInt.left <= ox ? ox : 0;
    offsety = rcInt.top <= oy ? oy : 0;
    return true;
  }
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

  void UpdateSurface(SurT *psur, sw2::IntRect const &rc, GxImage const &img, ImageRect &sur, int ox, int oy)
  {
    psur->draw(rc, img, ox, oy);
    sur.left = rc.left;
    sur.top = rc.top;
    sur.w = rc.width();
    sur.h = rc.height();
    sur.ox = ox;
    sur.oy = oy;
    sur.ow = img.w;
    sur.oh = img.h;
    sur.sur = (void*)psur;
  }

  bool LoadSurface(GxImage &img, ImageRect& sur)
  {
    //
    // Add the image to existing texture pack.
    //

    img.convert32();                    // Make sure img is 32-bits.

    int x = 0, y = 0, w = img.w, h = img.h;
    findBound(img, x, y, w, h);

    sw2::IntRect rc(0, 0, w, h);

    typename std::vector<SurT*>::iterator it;
    for (it = mSur.begin(); it != mSur.end(); ++it) {
      SurT *psur = *it;
      if (psur->add(rc)) {
        UpdateSurface(psur, rc, img, sur, x, y);
        NotifyImageManagerSurfaceUpdate();
        return true;
      }
    }

    //
    // If there is no room for the img to pack, try to reset mSur.
    //

    if (!mSur.empty() && GOOD_SUPPORT_NUM_TEXTURES <= mSur.size()) {
      for (it = mSur.begin(); it != mSur.end(); ++it) {
        SurT *psur = *it;
        psur->reset(psur->img.w, psur->img.h);
      }
      mImg.clear();
      SurT *psur = mSur[0];
      if (psur->add(rc)) {
        UpdateSurface(psur, rc, img, sur, x, y);
        NotifyImageManagerSurfaceReset();
        return true;
      }
      NotifyImageManagerSurfaceReset();
      return false;
    }

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
      UpdateSurface(psur, rc, img, sur, x, y);
      NotifyImageManagerSurfaceUpdate();
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
