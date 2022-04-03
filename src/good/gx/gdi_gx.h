
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

class GdiGraphics : public Graphics<ImgpImage>
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

  bool drawImage(int x, int y, ImgpImage const& img, int srcx, int srcy, int srcw, int srch, unsigned int color = 0xffffffff, float rot = .0f, float xscale = 1.0f, float yscale = 1.0f)
  {
    if (!img.isValid()) {
      return false;
    }

    Imgp tmpImg;
    if (!tmpImg.create(srcw, srch, 4)) {
      return false;
    }

    ImgpGraphics(tmpImg).drawImage(0, 0, img, srcx, srcy, srcw, srch, color, rot, xscale, yscale);

    HDC memdc = CreateCompatibleDC(mDc);
    HBITMAP membmp = CreateCompatibleBitmap(mDc, tmpImg.w, tmpImg.h);
    membmp = (HBITMAP)SelectObject(memdc, membmp);

    tmpImg.blt(memdc, 0, 0);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;
    AlphaBlend(mDc, x, y, tmpImg.w, tmpImg.h, memdc, 0, 0, tmpImg.w, tmpImg.h, bf);

    DeleteObject(SelectObject(memdc, membmp));
    DeleteDC(memdc);

    return true;
  }
};

} // namespace gx

} // namespace good

// end of gdi_gx.h
