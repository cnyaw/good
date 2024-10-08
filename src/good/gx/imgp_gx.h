
//
// imgp_gx.h
// imgp graphics impl, only support 32-bit.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/05/19 Waync created.
//

#pragma once

#include "imgm.h"

namespace good {

namespace gx {

//
// Imgp implementation.
//

class Imgp : public GxImage
{
public:

  //
  // Convert.
  //

  unsigned int rgba(unsigned int color) const
  {
#if defined(DEVKIT_PSP) || defined(_android_) || defined(__EMSCRIPTEN__)
    unsigned char b = (color & 0xff);
    unsigned char g = ((color >> 8) & 0xff);
    unsigned char r = ((color >> 16) & 0xff);
    unsigned char a = ((color >> 24) & 0xff);
    return r | (g << 8) | (b << 16) | (a << 24);
#else
    return color;
#endif
  }

  //
  // Fill.
  //

  Imgp& blend(const Imgp& img, unsigned int color, int x, int y)
  {
    return blend(img, color, x, y, img.w, img.h, 0, 0);
  }

  Imgp& blend(const Imgp& img, unsigned int color, int x, int y, int w, int h, int srcx, int srcy)
  {
    float alpha = ((color >> 24) & 0xff) / (float)0xff;

    if (0 == dat || 0 == img.dat || .0f > alpha) {
      return *this;
    }

    if (0 > x) {
      w += x;
      srcx -= x;
      x = 0;
    }

    if (0 > y) {
      h += y;
      srcy -= y;
      y = 0;
    }

    if (1.0f <= alpha) {
      return draw(img, x, y, w, h, srcx, srcy);
    }

    float alpha1 = 1.0f - alpha;
    for (int i = x, ii = srcx; i < x + w && i < this->w && ii < img.w; i ++, ii++) {
      for (int j = y, jj = srcy; j < y + h && j < this->h && jj < img.h; j ++, jj++) {
        unsigned char* ps = (unsigned char*)((unsigned int*)img.dat + ii + img.w * jj);
        if (0 == ps[3]) {               // 0==alpha.
          continue;
        }
        unsigned char B = ps[0];
        unsigned char G = ps[1];
        unsigned char R = ps[2];
        unsigned char* pd = (unsigned char*)((unsigned int*)dat + i + this->w * j);
        unsigned char b = pd[0];
        unsigned char g = pd[1];
        unsigned char r = pd[2];
        pd[0] = (unsigned char)(b * alpha1 + B * alpha);
        pd[1] = (unsigned char)(g * alpha1 + G * alpha);
        pd[2] = (unsigned char)(r * alpha1 + R * alpha);
        *((unsigned int*)pd) &= color;
      }
    }
    return *this;
  }

  Imgp& fill(unsigned int color)
  {
    if (0 == dat) {
      return *this;
    }

    color = rgba(color);

    unsigned int *p = (unsigned int*)dat;
    if (0 != color) {
      for (int i = w * h - 1; 0 <= i; i--) {
        p[i] = color;
      }
    } else {
      memset(dat, 0, sizeof(unsigned int) * w * h);
    }

    return *this;
  }

  Imgp& fill(unsigned int color, int x, int y, int w, int h)
  {
    if (0 == dat) {
      return *this;
    }

    if (0 > x) {
      w += x;
      x = 0;
    }

    if (0 > y) {
      h += y;
      y = 0;
    }

    if (x + w > this->w) {
      w = this->w - x;
    }

    if (y + h > this->h) {
      h = this->h - y;
    }

    color = rgba(color);

    unsigned int *p = (unsigned int*)dat;
    for (int i = x; i < x + w; i++) {
      for (int j = y; j < y + h; j++) {
        p[i + this->w * j] = color;
      }
    }

    return *this;
  }

  Imgp& fill(const Imgp& img)
  {
    return fill(img, 0, 0, w, h);
  }

  Imgp& fill(const Imgp& img, int x, int y, int w, int h)
  {
    if (0 == dat || 0 == img.dat) {
      return *this;
    }

    int cx = img.w, cy = img.h;
    int nx = x, ny = y;

    if (0 < nx) {
      nx = (nx % cx) - cx;
    } else if (0 > nx) {
      nx = - (-nx % cx);
    }

    if (0 < ny) {
      ny = (ny % cy) - cy;
    } else if (0 > ny) {
      ny = - (-ny % cy);
    }

    for (int ay = ny; -cy <= ay && ay < h; ay += cy) {
      for (int ax = nx; -cx <= ax && ax < w; ax += cx) {
        draw(img, ax, ay);
      }
    }

    return *this;
  }

  Imgp& draw(const Imgp& img)
  {
    return draw(img, 0, 0, img.w, img.h, 0, 0);
  }

  Imgp& draw(const Imgp& img, int x, int y)
  {
    return draw(img, x, y, img.w, img.h, 0, 0);
  }

  Imgp& draw(const Imgp& img, int x, int y, int w, int h)
  {
    return draw(img, x, y, w, h, 0, 0);
  }

  Imgp& draw(const Imgp& img, int x, int y, int w, int h, int srcx, int srcy, unsigned int color = 0xffffffff)
  {
    if (0 == dat || 0 == img.dat) {
      return *this;
    }

    if (0 > x) {
      w += x;
      srcx -= x;
      x = 0;
    }

    if (0 > y) {
      h += y;
      srcy -= y;
      y = 0;
    }

    unsigned int *p = (unsigned int*)dat;
    unsigned int *p2 = (unsigned int*)img.dat;

    for (int i = x, ii = srcx; i < x + w && i < this->w && ii < img.w; i ++, ii++) {
      for (int j = y, jj = srcy; j < y + h && j < this->h && jj < img.h; j ++, jj++) {
        int c = p2[ii + img.w * jj];
        if ((c >> 24) & 0xff) {
          p[i + this->w * j] = c & color;
        }
      }
    }

    return *this;
  }

  Imgp& rect(unsigned int color, int x, int y, int w, int h)
  {
    fill(color, x, y, w, 1);
    fill(color, x, y, 1, h);
    fill(color, x, y + h - 1, w, 1);
    fill(color, x + w - 1, y, 1, h);
    return *this;
  }

  //
  // Image processing.
  //

  Imgp& blueScale()
  {
    if (0 == dat) {
      return *this;
    }

    unsigned char* p = (unsigned char*)dat;
    for (int i = 0; i < w * h; i++) {
      p++;                              // Blue.
      *p++ = 0;                         // Green.
      *p++ = 0;                         // Red.
      p++;                              // Alpha.
    }

    return *this;
  }

  Imgp& flip()
  {
    GxImage::flip();

    return *this;
  }

  Imgp& grayScale()
  {
    if (0 == dat) {
      return *this;
    }

    unsigned int* p = (unsigned int*)dat;
    for (int i = 0; i < w * h; i++) {
      unsigned char* pc = (unsigned char*)p ++;
      unsigned char b = pc[0];
      unsigned char g = pc[1];
      unsigned char r = pc[2];
      pc[0] = pc[1] = pc[2] = (r + g + b) / 3;
    }

    return *this;
  }

  Imgp& greenScale()
  {
    if (0 == dat) {
      return *this;
    }

    unsigned char* p = (unsigned char*)dat;
    for (int i = 0; i < w * h; i++) {
      *p++ = 0;                         // Blue.
      p++;                              // Green.
      *p++ = 0;                         // Red.
      p++;                              // Alpha.
    }

    return *this;
  }

  Imgp& invert()
  {
    if (0 == dat) {
      return *this;
    }

    unsigned int *p = (unsigned int*)dat;
    for (int i = 0; i < w * h; i++) {
      p[i] ^= 0x00ffffff;
    }

    return *this;
  }

  Imgp& invert(int x, int y, int w, int h)
  {
    if (0 == dat) {
      return *this;
    }

    if (0 > x) {
      w += x;
      x = 0;
    }

    if (0 > y) {
      h += y;
      y = 0;
    }

    if (x + w > this->w) {
      w = this->w - x;
    }

    if (y + h > this->h) {
      h = this->h - y;
    }

    unsigned int *p = (unsigned int*)dat;
    for (int i = x; i < x + w; i++) {
      for (int j = y; j < y + h; j++) {
        p[i + this->w * j] ^= 0x00ffffff;
      }
    }

    return *this;
  }

  Imgp& mirror()
  {
    if (0 == dat) {
      return *this;
    }

    unsigned int *p = (unsigned int*)dat;
    for (int i = 0; i < w / 2; i++) {
      for (int j = 0; j < h; j++) {
        unsigned int tmp = p[i + w * j];
        p[i + w * j] = p[w - i - 1 + w * j];
        p[w - i - 1 + w * j] = tmp;
      }
    }

    return *this;
  }

  Imgp& redScale()
  {
    if (0 == dat) {
      return *this;
    }

    unsigned char* p = (unsigned char*)dat;
    for (int i = 0; i < w * h; i++) {
      *p++ = 0;                         // Blue.
      *p++ = 0;                         // Green.
      p++;                              // Red.
      p++;                              // Alpha.
    }

    return *this;
  }

  bool findDiffPix_i(const Imgp &img, int x, int y) const
  {
    int i = x + y * w;
    return ((const int*)dat)[i] != ((const int*)img.dat)[i];
  }

  bool findDiffBound(const Imgp &img, int &x, int &y, int &w, int &h) const
  {
    if (img.w != this->w || img.h != this->h || img.bpp != bpp) {
      return false;
    }
    bool dp = false;
    int maxy = y + h - 1, maxx = x + w - 1, minx = x, miny = y;
    for (int y0 = y + h - 1; y0 >= y; y0--) {
      for (int x0 = x + w - 1; x0 >= x; x0--) {
        dp = findDiffPix_i(img, x0, y0);
        if (dp) {
          maxy = y0;
          break;
        }
      }
      if (dp) {
        break;
      }
    }
    if (!dp) {
      return false;
    }
    for (int x0 = x + w - 1; x0 >= x; x0--) {
      for (int y0 = maxy; y0 >= y; y0--) {
        dp = findDiffPix_i(img, x0, y0);
        if (dp) {
          maxx = x0;
          break;
        }
      }
      if (dp) {
        break;
      }
    }
    for (int x0 = x; x0 < maxx; x0++) {
      for (int y0 = maxy; y0 >= y; y0--) {
        dp = findDiffPix_i(img, x0, y0);
        if (dp) {
          minx = x0;
          break;
        }
      }
      if (dp) {
        break;
      }
    }
    for (int y0 = y; y0 <= maxy; y0++) {
      for (int x0 = x; x0 <= maxx; x0++) {
        dp = findDiffPix_i(img, x0, y0);
        if (dp) {
          miny = y0;
          break;
        }
      }
      if (dp) {
        break;
      }
    }
    x = minx;
    y = miny;
    w = (std::min)(w, maxx - minx + 1);
    h = (std::min)(h, maxy - miny + 1);
    return true;
  }

  template<class RectT>
  void findDiffBounds(int depth, std::vector<RectT> &bb, const Imgp &img, int x, int y, int w, int h) const
  {
    if (0 == depth) {
      bb.push_back(RectT(x, y, w, h));
      return;
    }

    const int w_1 = (int)(w/2.0f), w_2 = w - w_1;
    const int h_1 = (int)(h/2.0f), h_2 = h - h_1;

    int xl = x, yl = y, wl = w_1, hl = h;
    if (!findDiffBound(img, xl, yl, wl, hl)) {
      return;
    }
    int xr = x + w_1, yr = y, wr = w_2, hr = h;
    if (!findDiffBound(img, xr, yr, wr, hr)) {
      return;
    }

    int xt = x, yt = y, wt = w, ht = h_1;
    if (!findDiffBound(img, xt, yt, wt, ht)) {
      return;
    }
    int xb = x, yb = y + h_1, wb = w, hb = h_2;
    if (!findDiffBound(img, xb, yb, wb, hb)) {
      return;
    }

    const int a1 = wl * hl + wr * hr;
    const int a2 = wt * ht + wb * hb;
    if (a1 <= a2) {
      findDiffBounds(depth - 1, bb, img, xl, yl, wl, hl);
      findDiffBounds(depth - 1, bb, img, xr, yr, wr, hr);
    } else {
      findDiffBounds(depth - 1, bb, img, xt, yt, wt, ht);
      findDiffBounds(depth - 1, bb, img, xb, yb, wb, hb);
    }
  }

  //
  // Text.
  //

  const unsigned char* getChar5_8(char ch) const
  {
    static const unsigned char char5_8[] = // font 5x8, bitmap
    {
      0x00, 0x00, 0xff,                   // space
      0xf4, 0xff,                         // !
      0xc0, 0x00, 0xc0, 0xff,             // "
      0x48, 0xfc, 0x48, 0xfc, 0x48, 0xff, // #
      0x24, 0x54, 0xd6, 0x48, 0xff,       // $
      0x24, 0x08, 0x10, 0x24, 0xff,       // %
      0x58, 0xa4, 0x54, 0x08, 0x14, 0xff, // &
      0x40, 0x80, 0xff,                   // '
      0x7c, 0x82, 0xff,                   // (
      0x82, 0x7c, 0xff,                   // )
      0x28, 0x10, 0x28, 0xff,             // *
      0x10, 0x38, 0x10, 0xff,             // +
      0x01, 0x02, 0xff,                   // ,
      0x10, 0x10, 0x10, 0xff,             // -
      0x02, 0xff,                         // .
      0x04, 0x08, 0x10, 0x20, 0xff,       // /
      0x78, 0x94, 0xa4, 0x78,    0xff,    // 0
      0x44, 0xfc, 0x04, 0xff,             // 1
      0x44, 0x8c, 0x94, 0x64, 0xff,       // 2
      0x04, 0xa4, 0xa4, 0x58, 0xff,       // 3
      0x78, 0x08, 0xfc, 0x08, 0xff,       // 4
      0xe4, 0xa4, 0xa4, 0x98, 0xff,       // 5
      0x78, 0xa4, 0xa4, 0x18, 0xff,       // 6
      0x80, 0x9c, 0xa0, 0xc0, 0xff,       // 7
      0x58, 0xa4, 0xa4, 0x58, 0xff,       // 8
      0x60, 0x94, 0x94, 0x78, 0xff,       // 9
      0x28, 0xff,                         // :
      0x02, 0x24, 0xff,                   //;
      0x10, 0x28, 0x44, 0xff,             // <
      0x28, 0x28, 0x28, 0xff,             // =
      0x44, 0x28, 0x10, 0xff,             // >
      0x40, 0x80, 0x94, 0x60, 0xff,       // ?
      0x78, 0x84, 0xb4, 0x74, 0xff,       // @
      0x7c, 0x90, 0x90, 0x7c, 0xff,       // A
      0xfc, 0xa4, 0xa4, 0x58, 0xff,       // B
      0x78, 0x84, 0x84, 0x48, 0xff,       // C
      0xfc, 0x84, 0x84, 0x78, 0xff,       // D
      0xfc, 0xa4, 0xa4, 0x84, 0xff,       // E
      0xfc, 0xa0, 0xa0, 0x80, 0xff,       // F
      0x78, 0x84, 0x94, 0x58, 0xff,       // G
      0xfc, 0x20, 0x20, 0xfc, 0xff,       // H
      0x84, 0xfc, 0x84, 0xff,             // I
      0x08, 0x04, 0x84, 0xf8, 0x80, 0xff, // J
      0xfc, 0x30, 0x48, 0x84, 0xff,       // K
      0xfc, 0x04, 0x04, 0x04, 0xff,       // L
      0xfc, 0x40, 0x30, 0x40, 0xfc, 0xff, // M
      0xfc, 0x40, 0x20, 0xfc, 0xff,       // N
      0x78, 0x84, 0x84, 0x78, 0xff,       // O
      0xfc, 0x90, 0x90, 0x60, 0xff,       // P
      0x78, 0x84, 0x94, 0x88, 0x74, 0xff, // Q
      0xfc, 0x90, 0x98, 0x64, 0xff,       // R
      0x44, 0xa4, 0xa4, 0x98, 0xff,       // S
      0x80, 0x80, 0xfc, 0x80, 0x80, 0xff, // T
      0xf8, 0x04, 0x04, 0xf8, 0xff,       // U
      0xf0, 0x08, 0x04, 0x08, 0xf0, 0xff, // V
      0xfc, 0x08, 0x30, 0x08, 0xfc, 0xff, // W
      0x84, 0x48, 0x30, 0x48, 0x84, 0xff, // X
      0x80, 0x40, 0x3c, 0x40, 0x80, 0xff, // Y
      0x8c, 0x94, 0xa4, 0xc4, 0xff,       // Z
      0xfe, 0x82, 0xff,                   // [
      0x20, 0x10, 0x08, 0x04, 0xff,       // \.
      0x82, 0xfe, 0xff,                   // ]
      0x40, 0x80, 0x40, 0xff,             // ^
      0x02, 0x02, 0x02, 0x02, 0xff,       // _
      0x80, 0x40, 0xff,                   // `
      0x18, 0x24, 0x28, 0x1c, 0xff,       // a
      0xfc, 0x24, 0x24, 0x18, 0xff,       // b
      0x18, 0x24, 0x24, 0x10, 0xff,       // c
      0x18, 0x24, 0x24, 0xfc, 0xff,       // d
      0x18, 0x24, 0x2c, 0x14, 0xff,       // e
      0x10, 0xfc, 0x90, 0x50, 0xff,       // f
      0x18, 0x25, 0x25, 0x3e, 0xff,       // g
      0xfc, 0x20, 0x20, 0x1c, 0xff,       // h
      0x5c, 0xff,                         // i
      0x01, 0x5f, 0xff,                   // j
      0xfc, 0x10, 0x28, 0x04, 0xff,       // k
      0x80, 0xfc, 0xff,                   // l
      0x3c, 0x20, 0x18, 0x20, 0x3c, 0xff, // m
      0x3c, 0x20, 0x20, 0x1c, 0xff,       // n
      0x18, 0x24, 0x24, 0x18, 0xff,       // o
      0x1f, 0x24, 0x24, 0x18, 0xff,       // p
      0x18, 0x24, 0x24, 0x1f, 0xff,       // q
      0x3c, 0x20, 0x20, 0x10, 0xff,       // r
      0x14, 0x34, 0x2c, 0x28, 0xff,       // s
      0x20, 0x78, 0x24, 0x24, 0xff,       // t
      0x38, 0x04, 0x04, 0x3c, 0xff,       // u
      0x38, 0x04, 0x04, 0x38, 0xff,       // v
      0x3c, 0x04, 0x18, 0x04, 0x3c, 0xff, // w
      0x24, 0x18, 0x24, 0xff,             // x
      0x38, 0x05, 0x05, 0x3e, 0xff,       // y
      0x24, 0x2c, 0x34, 0x24, 0xff,       // z
      0x10, 0x6c, 0x82, 0xff,             // {
      0xfe, 0xff,                         // |
      0x82, 0x6c, 0x10, 0xff,             // }
      0x40, 0x80, 0x40, 0x80, 0xff,       // ~
    };
    int c = ch - ' ';
    const unsigned char* p = char5_8;
    while (c --) {
      while (0xff != *p++) {
      }
    }
    return p;
  }

  int calcTextWidth(const char* str, int len, int size = 2) const
  {
    if (-1 == len) {
      len = (int)strlen(str);
    }

    int x = 0;
    for (int i = 0; i < len; i++) {
      char ch = str[i];
      if (' ' > ch || '~' < ch) {
        x += 5 * size + 2;
        continue;
      }
      const unsigned char* p = getChar5_8(ch);
      while (0xff != *p) {
        x += size, p++;
      }
      x += 2;
    }

    return x;
  }

  Imgp& drawText(const char* str, int len, int x, int y, unsigned int color, int size = 2)
  {
    if (0 == dat) {
      return *this;
    }

    if (-1 == len) {
      len = (int)strlen(str);
    }

    for (int i = 0; i < len; i++) {
      char ch = str[i];
      if (' ' > ch || '~' < ch) {
        fill(color, x, y, 5 * size, 8 * size); // Fill block to unknown char.
        x += 5 * size + 2;
        continue;
      }
      const unsigned char* p = getChar5_8(ch);
      while (0xff != *p) {
        unsigned char mask = 0x80;
        int yy = y;
        for (int j = 0; j < 8; j ++, yy += size, mask >>= 1) {
          if (*p & mask) {
            fill(color, x, yy, size, size);
          }
        }
        x += size, p++;
        if (w <= x) {
          break;
        }
      }
      x += 2;
      if (w <= x) {
        break;
      }
    }

    return *this;
  }
};

class ImgpImageSurface : public ImageSurface
{
public:
  enum {
    PACK_TEX_WIDTH = 1024,
    PACK_TEX_HEIGHT = 1024
  };
  bool init()
  {
    return ImageSurface::init(PACK_TEX_WIDTH, PACK_TEX_HEIGHT);
  }
};

class ImgpImageResource : public ImageManager<ImgpImageResource, ImgpImageSurface>
{
public:
  static ImgpImageResource& inst()
  {
    static ImgpImageResource i;
    return i;
  }
};

class ImgpImage : public Image<ImgpImage, ImgpImageResource>
{
public:

  const ImageRect *mSur;

  ImgpImage() : mSur(0)
  {
  }

  ImgpImage(const ImageRect *sur) : mSur(sur)
  {
  }

  bool isValid() const
  {
    return 0 != mSur && 0 != mSur->sur && 0 != ((ImgpImageSurface*)mSur->sur)->img.dat;
  }

  int getWidth() const
  {
    return mSur->w;
  }

  int getHeight() const
  {
    return mSur->h;
  }

  template<class CanvasT>
  void draw(int x, int y, const CanvasT &c, int sx, int sy, int sw, int sh)
  {
    if (isValid()) {
      ((ImgpImageSurface*)mSur->sur)->img.draw(mSur->left + x, mSur->top + y, c, sx, sy, sw, sh);
    }
  }

  template<class CanvasT>
  void drawToCanvas(int x, int y, CanvasT &c, int sx, int sy, int sw, int sh) const
  {
    if (isValid()) {
      c.draw((*(const CanvasT*)&(((ImgpImageSurface*)mSur->sur)->img)), x, y, sw, sh, mSur->left + sx, mSur->top + sy);
    }
  }

  unsigned int getPixel(int x, int y) const
  {
    if (isValid()) {
      return ((ImgpImageSurface*)mSur->sur)->img.getPixel(mSur->left + x, mSur->top + y);
    } else {
      return 0;
    }
  }
};

class ImgpGraphics : public Imgp, public Graphics<ImgpImage>
{
public:

  ImgpGraphics()
  {
  }

  void beginDraw(int width, int height)
  {
  }

  void endDraw()
  {
  }

  void restoreSur()
  {
  }

  void setAnchor(float x, float y)
  {
  }

  bool drawImage(int x, int y, ImgpImage const& img, int srcx, int srcy, int srcw, int srch, unsigned int color = 0xffffffff, float rot = .0f, float xscale = 1.0f, float yscale = 1.0f)
  {
    srcw = (std::min)(srcw, img.getWidth());
    srch = (std::min)(srch, img.getHeight());
    Imgp::blend(*((const Imgp*)&((ImgpImageSurface*)img.mSur->sur)->img), color, x, y, srcw, srch, img.mSur->left + srcx, img.mSur->top + srcy);
    return true;
  }

  bool fillSolidColor(int left, int top, int width, int height, unsigned int color = 0xffffffff, float rot = .0f, float xscale = 1.0f, float yscale = 1.0f)
  {
    Imgp::fill(color, left, top, width, height);
    return true;
  }
};

} // namespace gx

} // namespace good

// end of imgp_gx.h
