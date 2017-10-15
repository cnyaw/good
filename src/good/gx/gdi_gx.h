
//
// gdi_gx.h
// WIN32 gdi graphics impl.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/12/07 Waync created.
//

#pragma once

#include "img.h"

namespace good {

namespace gx {

class GdiImage : public Image<GdiImage>
{
public:

  HBITMAP mBmp;
  bool mHasAlphaChannel;

  GdiImage() : mBmp(0), mHasAlphaChannel(false)
  {
  }

  bool isValid() const
  {
    return 0 != mBmp;
  }

  bool hasAlphaChannel() const
  {
    return mHasAlphaChannel;
  }

  int getWidth() const
  {
    BITMAP bm = {0};
    ::GetObject(mBmp, sizeof(BITMAP), &bm);
    return bm.bmWidth;
  }

  int getHeight() const
  {
    BITMAP bm = {0};
    ::GetObject(mBmp, sizeof(BITMAP), &bm);
    return bm.bmHeight;
  }

  static bool existImage(std::string const& name);

  static GdiImage getImage(std::string const& name);
};

class GdiImageResource
{
  std::map<std::string, GdiImage> mImg;

  GdiImageResource()
  {
  }

public:

  static GdiImageResource& inst()
  {
    static GdiImageResource i;
    return i;
  }

  ~GdiImageResource()
  {
    clear();
  }

  void clear()
  {
    for (std::map<std::string, GdiImage>::iterator it = mImg.begin();
         mImg.end() != it;
         ++it) {
      ::DeleteObject(it->second.mBmp);
    }
    mImg.clear();
  }

  bool existImage(std::string const& name)
  {
    return mImg.end() != mImg.find(name);
  }

  GdiImage getImage(std::string const& name)
  {
    std::map<std::string, GdiImage>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return it->second;
    }

    GxImage img;
    if (img.load(name.c_str())) {
      GdiImage gimg;
      gimg.mBmp = CreateBitmap(img.w, img.h, 1, 32, img.dat);
      gimg.mHasAlphaChannel = 4 == img.bpp;
      mImg[name] = gimg;
      img.release();
      return gimg;
    } else {
      SW2_TRACE_ERROR("load image resource %s failed", name.c_str());
      mImg[name] = GdiImage();
      return GdiImage();
    }
  }
};

bool GdiImage::existImage(std::string const& name)
{
  return GdiImageResource::inst().existImage(name);
}

GdiImage GdiImage::getImage(std::string const& name)
{
  return GdiImageResource::inst().getImage(name);
}

class GdiGraphics : public Graphics<GdiImage>
{
public:

  HDC mDc;
  HDC mMemDc;

  GdiGraphics() : mDc(0), mMemDc(0)
  {
  }

  GdiGraphics(HDC dc) : mDc(dc), mMemDc(0)
  {
  }

  ~GdiGraphics()
  {
    if (mMemDc) {
      ::DeleteDC(mMemDc);
    }
  }

  bool drawImage(int x, int y, GdiImage const& img, int srcx, int srcy, int srcw, int srch, unsigned int color = 0xffffffff, float rot = .0f, float xscale = 1.0f, float yscale = 1.0f)
  {
    if (NULL == mDc || NULL == img.mBmp) {
      return false;
    }

    if (NULL == mMemDc) {
      mMemDc = ::CreateCompatibleDC(mDc);
    }

    if (NULL == mMemDc) {
      return false;
    }

    HBITMAP hPrevBmp = (HBITMAP)::SelectObject(mMemDc, img.mBmp);

    if (img.hasAlphaChannel()) {
      BLENDFUNCTION bf;
      bf.BlendOp = AC_SRC_OVER;
      bf.BlendFlags = 0;
      bf.SourceConstantAlpha = 255;
      bf.AlphaFormat = AC_SRC_ALPHA;
      AlphaBlend(mDc, x, y, srcw, srch, mMemDc, srcx, srcy, srcw, srch, bf);
    } else {
      BitBlt(mDc, x, y, srcw, srch, mMemDc, srcx, srcy, SRCCOPY);
    }

    ::SelectObject(mMemDc, hPrevBmp);

    return true;
  }
};

} // namespace gx

} // namespace good

// end of gdi_gx.h
