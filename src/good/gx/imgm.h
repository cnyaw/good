
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

#include "img.h"
#include "rcpack.h"

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
    root = new RectPackNode(0, 0, TEX_WIDTH, TEX_HEIGHT);
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

  bool add(sw2::IntRect &r)
  {
    return root->add(r);
  }

  void update(sw2::IntRect const &rc, GxImage const &aimg)
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

template<class SurT>
class ImageRect
{
public:

  ImageRect() : tex(0), left(0), top(0), w(0), h(0)
  {
  }

  SurT *tex;
  int left, top, w, h;
};

template<class T, class SurT, class RectT>
class ImageManager
{
protected:
  std::vector<SurT*> mSur;
  std::map<std::string, RectT> mImg;

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

  void UpdateSurface(SurT *psur, sw2::IntRect const &rc, GxImage const &img, RectT &sur)
  {
    psur->update(rc, img);
    sur.left = rc.left;
    sur.top = rc.top;
    sur.w = img.w;
    sur.h = img.h;
    sur.tex = psur;
  }

  bool LoadSurface(GxImage &img, RectT& sur)
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

  RectT const* getImage(std::string const& name)
  {
    typename std::map<std::string, RectT>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return &it->second;
    }

    std::string s;
    if (!loadFileBinaryContent(name.c_str(), s)) {
      SW2_TRACE_ERROR("open image resource %s failed", name.c_str());
      mImg[name] = RectT();
      return 0;
    }

    return getImage_i(name, s);
  }

  RectT const* getImage_i(std::string const& name, std::string const& stream)
  {
    GxImage img;
    if (!img.loadFromStream(stream)) {
      SW2_TRACE_ERROR("load image stream %s failed", name.c_str());
      mImg[name] = RectT();
      return 0;
    }
    return getImage_i(name, img);
  }

  RectT const* getImage(std::string const& name, std::string const& stream)
  {
    typename std::map<std::string, RectT>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return &it->second;
    }
    return getImage_i(name, stream);
  }

  RectT const* getImage_i(std::string const& name, GxImage &img)
  {
    RectT sur;
    if (((T*)this)->LoadSurface(img, sur)) {
      mImg[name] = sur;
      return &mImg[name];
    } else {
      SW2_TRACE_ERROR("load image %s failed", name.c_str());
      mImg[name] = sur;
      return 0;
    }
  }

  RectT const* getImage(std::string const& name, GxImage &img)
  {
    typename std::map<std::string, RectT>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return &it->second;
    }
    return getImage_i(name, img);
  }
};

} // namespace gx

} // namespace good

// end of imgm.h
