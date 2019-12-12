
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

#include "img.h"

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
#if defined(DEVKIT_PSP) || defined(_android_)
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

  Imgp& blend(const Imgp& img, float alpha/*0..1*/, int x, int y)
  {
    return blend(img, alpha, x, y, img.w, img.h, 0, 0);
  }

  Imgp& blend(const Imgp& img, float alpha/*0..1*/, int x, int y, int w, int h, int srcx, int srcy)
  {
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

    if (1.0f < alpha) {
      return draw(img, x, y, w, h, srcx, srcy);
    }

    float alpha1 = 1.0f - alpha;
    for (int i = x, ii = srcx; i < x + w && i < this->w; i ++, ii++) {
      for (int j = y, jj = srcy; j < y + h && j < this->h; j ++, jj++) {
        unsigned char* pd = (unsigned char*)((unsigned int*)dat + i + this->w * j);
        unsigned char b = pd[0];
        unsigned char g = pd[1];
        unsigned char r = pd[2];
        unsigned char* ps = (unsigned char*)((unsigned int*)img.dat + ii + img.w * jj);
        unsigned char B = ps[0];
        unsigned char G = ps[1];
        unsigned char R = ps[2];
        pd[0] = (unsigned char)(b * alpha1 + B * alpha);
        pd[1] = (unsigned char)(g * alpha1 + G * alpha);
        pd[2] = (unsigned char)(r * alpha1 + R * alpha);
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

    color = rgba(color);

    unsigned int *p = (unsigned int*)dat;
    for (int i = x; i < x + w && i < this->w; i++) {
      for (int j = y; j < y + h && j < this->h; j++) {
        p[i + this->w * j] = color;
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

    for (int i = x, ii = srcx; i < x + w && i < this->w; i ++, ii++) {
      for (int j = y, jj = srcy; j < y + h && j < this->h; j ++, jj++) {
        p[i + this->w * j] = p2[ii + img.w * jj] & color;
      }
    }

    return *this;
  }

  Imgp& drawTrans(const Imgp& img, unsigned int keycolor)
  {
    return drawTrans(img, keycolor, 0, 0, img.w, img.h, 0, 0);
  }

  Imgp& drawTrans(const Imgp& img, unsigned int keycolor, int x, int y)
  {
    return drawTrans(img, keycolor, x, y, img.w, img.h, 0, 0);
  }

  Imgp& drawTrans(const Imgp& img, unsigned int keycolor, int x, int y, int w, int h)
  {
    return drawTrans(img, keycolor, x, y, w, h, 0, 0);
  }

  Imgp& drawTrans(const Imgp& img, unsigned int keycolor, int x, int y, int w, int h, int srcx, int srcy)
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

    keycolor = rgba(keycolor);

    unsigned int *p = (unsigned int*)dat;
    for (int i = x, ii = srcx; i < x + w && i < this->w; i ++, ii++) {
      for (int j = y, jj = srcy; j < y + h && j < this->h; j ++, jj++) {
        unsigned int c = p[ii + img.w * jj];
        if (keycolor != c) {
          p[i + this->w * j] = c;
        }
      }
    }

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
      (void)*p++;                       // Blue.
      *p++ = 0;                         // Green.
      *p++ = 0;                         // Red.
      (void)*p++;                       // Alpha.
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
      (void)*p++;                       // Green.
      *p++ = 0;                         // Red.
      (void)*p++;                       // Alpha.
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
      (void)*p++;                       // Red.
      (void)*p++;                       // Alpha.
    }

    return *this;
  }

  //
  // Text.
  //

  const unsigned char* getChar5_8() const;

  int calcTextWidth(const char* str, int len, int size = 2) const
  {
    if (-1 == len) {
      len = (int)strlen(str);
    }

    int x = 0;
    for (int i = 0; i < len; i++) {
      int c = str[i] - ' ';
      const unsigned char* p = getChar5_8();
      while (c --) {
        while (0xff != *p++) {
        }
      }
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
      int c = str[i] - ' ';
      const unsigned char* p = getChar5_8();
      while (c --) {
        while (0xff != *p++) {
        }
      }
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

const unsigned char* Imgp::getChar5_8() const
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
  return char5_8;
}

class ImgpImageResource
{
  std::map<std::string, Imgp*> mImg;

  ImgpImageResource()
  {
  }

public:

  static ImgpImageResource& inst()
  {
    static ImgpImageResource i;
    return i;
  }

  ~ImgpImageResource()
  {
    clear();
  }

  void clear()
  {
    for (std::map<std::string, Imgp*>::iterator it = mImg.begin();
         mImg.end() != it;
         ++it) {
      delete it->second;
    }

    mImg.clear();
  }

  bool existImage(std::string const& name)
  {
    return mImg.end() != mImg.find(name);
  }

  Imgp* getImage(std::string const& name)
  {
    std::map<std::string, Imgp*>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return it->second;
    }

    std::ifstream ifs(name.c_str(), std::ios::binary);
    if (!ifs) {
      return 0;
    }

    ifs.seekg(0, std::ios_base::end);
    int lenstream = (int)ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    std::string s;
    s.resize(lenstream);

    ifs.read((char*)s.data(), lenstream);

    return getImage(name, s);
  }

  Imgp* getImage(std::string const& name, std::string const& stream)
  {
    std::map<std::string, Imgp*>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return it->second;
    }

    Imgp* sur = new Imgp;
    if (0 == sur) {
      return 0;
    }

    if (!sur->loadFromStream(stream)) {
      SW2_TRACE_ERROR("load image resource stream %s failed", name.c_str());
      mImg[name] = 0;
      return 0;
    }

    mImg[name] = sur;

    return sur;
  }

  Imgp* getImage(std::string const& name, GxImage &img)
  {
    std::map<std::string, Imgp*>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return it->second;
    }

    Imgp* sur = new Imgp;
    if (0 == sur) {
      return 0;
    }

    if (!sur->create(img.w, img.h, img.bpp)) {
      SW2_TRACE_ERROR("load gx image %s failed", name.c_str());
      return 0;
    }

    ((GxImage*)sur)->draw(0, 0, img);

    mImg[name] = sur;

    return sur;
  }

  Imgp* getImage(std::string const& name, int size, int ch, bool bAntiAlias)
  {
    std::map<std::string, Imgp*>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return it->second;
    }

    GxImage img;
    if (!img.loadFromChar(size, ch, bAntiAlias)) {
      SW2_TRACE_ERROR("create char img %s failed", name.c_str());
      mImg[name] = 0;
      return false;
    }

    return getImage(name, img);
  }
};

class ImgpImage : public good::gx::Image<ImgpImage>
{
public:

  Imgp* mSur;
  bool mHasKeyColor;
  unsigned int mKeyColor;

  ImgpImage() : mSur(0), mHasKeyColor(false), mKeyColor(0)
  {
  }

  ImgpImage(Imgp* sur) : mSur(sur), mHasKeyColor(false), mKeyColor(0)
  {
  }

  bool isValid() const
  {
    return 0 != mSur && 0 != mSur->dat;
  }

  int getWidth() const
  {
    return mSur->w;
  }

  int getHeight() const
  {
    return mSur->h;
  }

  bool hasKeyColor() const
  {
    return mHasKeyColor;
  }

  unsigned int getKeyColor() const
  {
    return mKeyColor;
  }

  void setKeyColor(unsigned int kcolor)
  {
    mKeyColor = kcolor;
    mHasKeyColor = true;
  }

  static bool existImage(std::string const& name)
  {
    return ImgpImageResource::inst().existImage(name);
  }

  static ImgpImage getImage(std::string const& name)
  {
    return ImgpImage(ImgpImageResource::inst().getImage(name));
  }

  static ImgpImage getImage(std::string const& name, std::string const& stream)
  {
    return ImgpImage(ImgpImageResource::inst().getImage(name, stream));
  }

  static ImgpImage getImage(std::string const& name, int size, int ch, bool bAntiAlias)
  {
     return ImgpImage(ImgpImageResource::inst().getImage(name, size, ch, bAntiAlias));
  }

  static ImgpImage getImage(std::string const& name, GxImage &img)
  {
    return ImgpImage(ImgpImageResource::inst().getImage(name, img));
  }

  void draw(int x, int y, const Imgp &c, int sx, int sy, int sw, int sh)
  {
    if (isValid()) {
      mSur->draw(c, x, y, sx, sy, sw, sh);
    }
  }

  void drawToCanvas(int x, int y, Imgp &c, int sx, int sy, int sw, int sh) const
  {
    if (isValid()) {
      c.draw(*mSur, x, y, sx, sy, sw, sh);
    }
  }
};

class ImgpGraphics : public good::gx::Graphics<ImgpImage>
{
public:

  Imgp& mSur;

  ImgpGraphics(Imgp& sur) : mSur(sur)
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

  bool drawImage(int x, int y, ImgpImage const& img, int srcx, int srcy, int srcw, int srch, unsigned int color, float rot, float xscale, float yscale)
  {
    if (img.hasKeyColor()) {
      mSur.drawTrans(*img.mSur, img.getKeyColor(), x, y, srcw, srch, srcx, srcy);
    } else {
      float alpha = ((color >> 24) & 0xff) / (float)0xff;
      mSur.blend(*img.mSur, alpha, x, y, srcw, srch, srcx, srcy);
    }

    return true;
  }

  bool fillSolidColor(int left, int top, int width, int height, unsigned int color, float rot, float xscale, float yscale)
  {
    mSur.fill(color, left, top, width, height);
    return true;
  }
};

} // namespace gx

} // namespace good

// end of imgp_gx.h
