
//
// sdl_gx.h
// SDL graphics impl.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/01/17 Waync created.
//

#pragma once

#include "img.h"

namespace good {

namespace gx {

class SDLImageResource
{
  std::map<std::string, SDL_Surface*> mImg;

  SDLImageResource()
  {
  }

public:

  static SDLImageResource& inst()
  {
    static SDLImageResource i;
    return i;
  }

  ~SDLImageResource()
  {
    clear();
  }

  void clear()
  {
    for (std::map<std::string, SDL_Surface*>::iterator it = mImg.begin();
         mImg.end() != it;
         ++it) {
      SDL_FreeSurface(it->second);
    }

    mImg.clear();
  }

  bool existImage(std::string const& name)
  {
    return mImg.end() != mImg.find(name);
  }

  SDL_Surface* getImage(std::string const& name)
  {
    std::map<std::string, SDL_Surface*>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return it->second;
    }

    std::ifstream ifs(name.c_str(), std::ios::binary);
    if (!ifs) {
      mImg[name] = 0;
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

  static SDL_Surface* createSurface(const GxImage &img)
  {
    SDL_Surface* sur = SDL_CreateRGBSurfaceFrom(
                         (void*)img.dat,
                         img.w,
                         img.h,
                         img.bpp * 8,
                         4 * ((img.w * img.bpp * 8 + 31) / 32), // Pitch.
                         0xff0000,
                         0xff00,
                         0xff,
                         0xff000000);
    return sur;
  }

  SDL_Surface* getImage(std::string const& name, std::string const& stream)
  {
    std::map<std::string, SDL_Surface*>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return it->second;
    }

    GxImage img;
    if (!img.loadFromStream(stream)) {
      SW2_TRACE_ERROR("load image resource %s failed", name.c_str());
      mImg[name] = 0;
      return 0;
    }

    return getImage(name, img);
  }

  SDL_Surface* getImage(std::string const& name, GxImage &img)
  {
    std::map<std::string, SDL_Surface*>::const_iterator it = mImg.find(name);
    if (mImg.end() != it) {
      return it->second;
    }

    img.convert32();

    SDL_Surface* sur = createSurface(img);
    if (sur) {
      SDL_Surface* sur2 = SDL_DisplayFormatAlpha(sur);
      SDL_FreeSurface(sur);
      mImg[name] = sur2;
      return sur2;
    } else {
      SW2_TRACE_ERROR("create image surface %s failed", name.c_str());
      mImg[name] = 0;
      return 0;
    }
  }
};

class SDLImage : public Image<SDLImage>
{
public:

  SDL_Surface* mSur;

  SDLImage() : mSur(0)
  {
  }

  SDLImage(SDL_Surface* sur) : mSur(sur)
  {
  }

  bool isValid() const
  {
    return 0 != mSur;
  }

  int getWidth() const
  {
    return mSur->w;
  }

  int getHeight() const
  {
    return mSur->h;
  }

  static bool existImage(std::string const& name)
  {
    return SDLImageResource::inst().existImage(name);
  }

  static SDLImage getImage(std::string const& name)
  {
    return SDLImage(SDLImageResource::inst().getImage(name));
  }

  static SDLImage getImage(std::string const& name, std::string const& stream)
  {
    return SDLImage(SDLImageResource::inst().getImage(name, stream));
  }

  static SDLImage getImage(std::string const& name, int size, int ch, bool bAntiAlias)
  {
     return SDLImage();
  }

  static SDLImage getImage(std::string const& name, GxImage &img)
  {
    return SDLImage(SDLImageResource::inst().getImage(name, img));
  }

  template<class CanvasT>
  void draw(int x, int y, const CanvasT &c, int sx, int sy, int sw, int sh)
  {
    if (!isValid()) {
      return;
    }

    SDL_Surface* sur = SDLImageResource::createSurface(c);
    if (sur) {
      SDL_Surface* sur2 = SDL_DisplayFormatAlpha(sur);
      SDL_FreeSurface(sur);

      SDL_Rect rcSrc, rcDst;
      rcSrc.x = sx, rcSrc.y = sy, rcSrc.w = sw, rcSrc.h = sh;
      rcDst.x = x, rcDst.y = y, rcDst.w = sw, rcDst.h = sh;
      SDL_BlitSurface(sur2, &rcSrc, mSur, &rcDst);

      SDL_FreeSurface(sur2);
    }
  }

  template<class CanvasT>
  void drawToCanvas(int x, int y, CanvasT &c, int sx, int sy, int sw, int sh) const
  {
    if (!isValid()) {
      return;
    }

    SDL_LockSurface(mSur);

    for (int i = 0; i < sh; i++) {
      unsigned int *pC = (unsigned int*)c.dat + (y + i) * c.w + x;
      unsigned char *pImg = (unsigned char*)mSur->pixels + (sy + i) * mSur->pitch + sx;
      memcpy(pC, pImg, sw * mSur->format->BytesPerPixel);
    }

    SDL_UnlockSurface(mSur);
  }
};

class SDLGraphics : public Graphics<SDLImage>
{
public:

  SDL_Surface* mSur;

  SDLGraphics(SDL_Surface* sur) : mSur(sur)
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

  void upadte()
  {
    SDL_UpdateRect(mSur, 0, 0, 0, 0);
  }

  void setAnchor(float x, float y)
  {
  }

  bool drawImage(int x, int y, SDLImage const& img, int srcx, int srcy, int srcw, int srch, unsigned int color, float rot, float xscale, float yscale)
  {
    SDL_Rect rcSrc, rcDst;
    rcSrc.x = srcx, rcSrc.y = srcy, rcSrc.w = srcw, rcSrc.h = srch;
    rcDst.x = x, rcDst.y = y, rcDst.w = srcw, rcDst.h = srch;
    return 0 == SDL_BlitSurface(img.mSur, &rcSrc, mSur, &rcDst);
  }

  bool fillSolidColor(int left, int top, int width, int height, unsigned int color, float rot, float xscale, float yscale)
  {
    SDL_Rect rc;
    rc.x = left;
    rc.y = top;
    rc.w = width;
    rc.h = height;
    return 0 == SDL_FillRect(mSur, &rc, SDL_MapRGB(mSur->format, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff));
  }
};

} // namespace gx

} // namespace good

// end of sdl_gx.h
