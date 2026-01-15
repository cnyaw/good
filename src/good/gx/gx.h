
//
// gx.h
// Graphics.
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

class Image
{
public:
  bool isValid() const;
  int getWidth() const;
  int getHeight() const;
};

template<class ImgT>
class Graphics
{
public:

  void beginDraw(int width, int height);
  void endDraw();

  bool drawImage(int x, int y, ImgT const& img, int srcx, int srcy, int srcw, int srch, unsigned int color, float rot, float xscale, float yscale);

  bool drawImage(int x, int y, ImgT const& img, unsigned int color, float rot, float xscale, float yscale)
  {
    return drawImage(
             x,
             y,
             img,
             0,
             0,
             img.getWidth(),
             img.getHeight(),
             color,
             rot,
             xscale,
             yscale);
  }

  bool fillSolidColor(int left, int top, int width, int height, unsigned int color, float rot, float xscale, float yscale);
};

template<class CanvasT>
class CanvasGraphics
{
public:
  CanvasT &mSur;
  CanvasGraphics(CanvasT &sur) : mSur(sur)
  {
  }

  template<class ImgT>
  bool drawImage(int x, int y, const ImgT &img, int srcx, int srcy, int srcw, int srch, unsigned int color = 0xffffffff, float rot = .0f, float xscale = 1.0f, float yscale = 1.0f)
  {
    img.drawToCanvas(x, y, mSur, srcx, srcy, srcw, srch);
    return true;
  }
};

} // namespace gx

} // namespace good

// end of gx.h
