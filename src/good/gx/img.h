
//
// img.h
// Load image file(bmp|jpg|png)
//
// Copyright (c) 2010 Waync Cheng.
// All Rights Reserved.
//
// 2010/08/14 Waync created.
//

#pragma once

#ifdef GOOD_SUPPORT_GDIPLUS
# include <gdiplus.h>
# pragma comment(lib, "Gdiplus.lib")
#endif

#ifdef GOOD_SUPPORT_STB_IMG
#define STBI_NO_LINEAR
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_ASSERT(x)
#define STBIR_ASSERT(x)
#define STBIW_ASSERT(x)
#include "stb_image.h"
#endif

namespace good {

namespace gx {

class GxImage
{
public:

  int w, h;
  int bpp;
  char* dat;

  GxImage() : w(0), h(0), bpp(0), dat(0)
  {
  }

  ~GxImage()
  {
    release();
  }

  bool create(int aw, int ah, int abpp)
  {
    char *pdat = new char[aw * ah * abpp];
    if (0 == pdat) {
      return false;
    }

#ifdef GOOD_WTL_PLAYER
    memset(pdat, 0xcd, aw * ah * abpp);
#endif

    release();

    w = aw;
    h = ah;
    bpp = abpp;
    dat = pdat;

    return true;
  }

  void release()
  {
    if (dat) {
      delete [] dat;
      dat = 0;
    }
  }

  bool isValid() const
  {
    return 0 != dat && 0 != w && 0 != h;
  }

  bool expand(int w2, int h2)
  {
    if (0 == dat || 4 != bpp) {
      return false;
    }

    if (0 >= w2 || 10000 < w2 || 0 >= h2 || 10000 < h2) {
      return false;
    }

    if (w == w2 && h == h2) {
      return true;
    }

    char* dat2 = new char[w2 * h2 * bpp];
    if (0 == dat2) {
      return false;
    }

    for (int i = 0; i < h && i < h2; i++) {
      int sz = w2 > w ? w * bpp : w2 * bpp;
      memcpy(dat2 + w2 * bpp * i, dat + w * bpp * i, sz);
    }

    delete [] dat;

    dat = dat2;

    w = w2;
    h = h2;

    return true;
  }

  bool resize(int w2, int h2)
  {
    //
    // For more infomation.
    // http://tech-algorithm.com/articles/bilinear-image-scaling/
    //

    if (0 == dat || 4 != bpp) {
      return false;
    }

    if (0 >= w2 || 10000 < w2 || 0 >= h2 || 10000 < h2) {
      return false;
    }

    if (w == w2 && h == h2) {
      return true;
    }

    char *newdat = new char[w2 * h2 * 4];
    if (0 == newdat) {
      return false;
    }

    float x_ratio = ((float)(w - 1)) / w2;
    float y_ratio = ((float)(h - 1)) / h2;

    int const *p = (int const*)dat;
    int *p2 = (int*)newdat;

    int offset = 0;
    for (int i = 0; i < h2; i++) {
      for (int j = 0; j < w2; j++) {

        int x = (int)(x_ratio * j);
        int y = (int)(y_ratio * i);
        float x_diff = (x_ratio * j) - x;
        float y_diff = (y_ratio * i) - y;

        int index = y * w + x;
        int a = p[index];
        int b = p[index + 1];
        int c = p[index + w];
        int d = p[index + w + 1];

        float blue = (a & 0xff) * (1 - x_diff) * (1 - y_diff) +
                     (b & 0xff) * x_diff * (1 - y_diff) +
                     (c & 0xff) * y_diff * (1 - x_diff) +
                     (d & 0xff) * x_diff * y_diff;
        float green = ((a >> 8) & 0xff) * (1 - x_diff) * (1 - y_diff) +
                      ((b >> 8) & 0xff) * x_diff * (1 - y_diff) +
                      ((c >> 8) & 0xff) * y_diff * (1 - x_diff) +
                      ((d >> 8) & 0xff) * x_diff * y_diff;
        float red = ((a >> 16) & 0xff) * (1 - x_diff) * (1 - y_diff) +
                    ((b >> 16) & 0xff) * x_diff * (1 - y_diff) +
                    ((c >> 16) & 0xff) * y_diff * (1 - x_diff) +
                    ((d >> 16) & 0xff) * x_diff * y_diff;
        float alpha = ((a >> 24) & 0xff) * (1 - x_diff) * (1 - y_diff) +
                    ((b >> 24) & 0xff) * x_diff * (1 - y_diff) +
                    ((c >> 24) & 0xff) * y_diff * (1 - x_diff) +
                    ((d >> 24) & 0xff) * x_diff * y_diff;

        p2[offset++] = ((((int)alpha) << 24) & 0xff000000) |
                       ((((int)red) << 16) & 0xff0000) |
                       ((((int)green) << 8) & 0xff00) |
                       ((int)blue);
      }
    }

    delete [] dat;
    dat = newdat;
    w = w2;
    h = h2;

    return true;
  }

  bool convert32()
  {
    switch (bpp)
    {
    case 3:
      {
        char *newdat = new char[w * h * 4];
        if (0 == newdat) {
          return false;
        }

        for (int i = 0; i < h; i++) {
          for (int j = 0; j < w; j++) {
            int ij = j + i * w;
            int i3 = ij * 3;
            int i4 = ij * 4;
            newdat[i4 + 0] = dat[i3 + 0];
            newdat[i4 + 1] = dat[i3 + 1];
            newdat[i4 + 2] = dat[i3 + 2];
            newdat[i4 + 3] = (char)0xff;
          }
        }

        delete [] dat;
        dat = newdat;
        bpp = 4;
      }
      return true;

    case 4:
      return true;
    }

    return false;
  }

  void flip()
  {
    if (0 == dat) {
      return;
    }

    unsigned int *pdat = (unsigned int*)dat;
    for (int i = 0; i < w; i++) {
      for (int j = 0; j < h / 2; j++) {
        unsigned int tmp = pdat[i + w * j];
        pdat[i + w * j] = pdat[i + w * (h - j - 1)];
        pdat[i + w * (h - j - 1)] = tmp;
      }
    }
  }

  void draw(int x, int y, GxImage const &img)
  {
    draw(x, y, img, 0, 0, img.w, img.h);
  }

  void draw(int x, int y, GxImage const &img, int sx, int sy, int sw, int sh)
  {
    if (0 == dat || 0 == img.dat || img.bpp != bpp || 4 != bpp) {
      return;
    }

    if (img.w < sw) {
      sw = img.w;
    }

    if (img.h < sh) {
      sh = img.h;
    }

    if (img.w < sx + sw) {
      sw = img.w - sx;
    }

    if (img.h < sy + sh) {
      sh = img.h - sy;
    }

    if (w < x + sw) {
      sw = w - x;
    }

    if (h < y + sh) {
      sh = h - y;
    }

    int len1 = w * bpp, len2 = img.w * bpp, line = sw * bpp;
    char *p1 = dat + (y * w + x) * bpp, *p2 = img.dat + (sy * img.w + sx) * bpp;
    for (int i = 0; i < sh; i++, p1 += len1, p2 += len2) {
      memcpy(p1, p2, line);
    }
  }

  bool load(std::string const& name)
  {
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

    return loadFromStream(s);
  }

  bool loadFromStream(std::string const& stream)
  {
#ifdef GOOD_SUPPORT_GDIPLUS
    if (Win32GdiplusLoadImage(stream)) {
      return true;
    }
#endif
#ifdef GOOD_SUPPORT_ANDROID_IMG
    if (AndroidLoadImage(stream)) {
      return true;
    }
#endif
#ifdef GOOD_SUPPORT_STB_IMG
  int n;
  dat = (char*)stbi_load_from_memory((const stbi_uc*)stream.data(), (int)stream.size(), &w, &h, &n, 4);
  if (isValid()) {
    bpp = 4;
    return true;
  }
#endif
    return false;
  }

  bool loadFromChar(int size, int ch, bool bAntiAlias)
  {
#ifdef GOOD_SUPPORT_GDIPLUS
    if (Win32LoadImageFromChar(size, ch, bAntiAlias)) {
      return true;
    }
#endif
#ifdef GOOD_SUPPORT_ANDROID_IMG
    if (AndroidLoadImageFromChar(size, ch, bAntiAlias)) {
      return true;
    }
#endif
#ifdef GOOD_SUPPORT_EMSC_IMG
    if (EmscLoadImageFromChar(size, ch, bAntiAlias, this)) {
      return true;
    }
#endif
    return false;
  }

#ifdef WIN32
  void blt(HDC hdc, int x, int y) const
  {
    if (0 == dat) {
      return;
    }
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biClrImportant = BI_RGB;
    bmi.bmiHeader.biXPelsPerMeter = bmi.bmiHeader.biYPelsPerMeter = 1;
    SetDIBitsToDevice(hdc, x, y, w, h, 0, 0, 0, h, dat, &bmi, DIB_RGB_COLORS);
  }
#endif

#ifdef GOOD_SUPPORT_GDIPLUS
  bool Win32GdiplusLoadImage(std::string const& stream)
  {
    //
    // Create IStream from memory stream.
    //

    HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, stream.length());
    if (NULL == hMem) {
      return false;
    }

    ::CopyMemory(::GlobalLock(hMem), stream.data(), stream.length());
    ::GlobalUnlock(hMem);

    IStream *s = NULL;
    if (FAILED(::CreateStreamOnHGlobal(hMem, TRUE, &s))) {
      GlobalFree(hMem);
      return false;
    }

    //
    // Init Gdi+.
    //

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    bool ret = false;

    //
    // Open image.
    //

    {                                   // This block make sure bmp destroyed before GDI+ uninit.
      Gdiplus::Bitmap bmp(s);
      if (Gdiplus::Ok == bmp.GetLastStatus()) {

        Gdiplus::Rect rc(0, 0, bmp.GetWidth(), bmp.GetHeight());

        //
        // Copy image pixel data.
        //

        Gdiplus::BitmapData BmpData;
        if (Gdiplus::Ok == bmp.LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &BmpData)) {

          w = bmp.GetWidth();
          h = bmp.GetHeight();
          bpp = 4;

          dat = new char[w * h * bpp];
          memcpy(dat, BmpData.Scan0, w * h * bpp);

          bmp.UnlockBits(&BmpData);

          ret = true;
        }
      }
    }

    //
    // Uninit Gdi+.
    //

    Gdiplus::GdiplusShutdown(gdiplusToken);

    //
    // Release IStream.
    //

    s->Release();

    return ret;
  }

  bool Win32LoadImageFromChar(int size, int ch, bool bAntiAlias)
  {
    HDC dc = GetDC(0);
    HDC memdc = CreateCompatibleDC(dc);

    LOGFONT lf;
    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    lf.lfHeight = -size;
    HFONT hFont = CreateFontIndirect(&lf);
    hFont = (HFONT)SelectObject(memdc, hFont);

    wchar_t buff[2] = {(wchar_t)ch, 0};

    RECT rc = {0};
    DrawTextW(memdc, buff, 1, &rc, DT_LEFT|DT_TOP|DT_SINGLELINE|DT_CALCRECT);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth  = rc.right;
    bmi.bmiHeader.biHeight = -rc.bottom;
    bmi.bmiHeader.biPlanes   = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    unsigned int* pPixBmp = 0;
    HBITMAP membmp = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, (void**)&pPixBmp, NULL, 0);
    if (membmp) {
      membmp = (HBITMAP)SelectObject(memdc, membmp);
      FillRect(memdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
      SetTextColor(memdc, RGB(255,255,255));
      SetBkMode(memdc, TRANSPARENT);
      DrawTextW(memdc, buff, 1, &rc, DT_LEFT|DT_TOP|DT_SINGLELINE);
      w = rc.right;
      h = rc.bottom;
      bpp = 4;
      dat = new char[w * h * bpp];
      for (int i = 0; i < w * h; i++) {
        unsigned char clr = 0 == pPixBmp[i] ? 0 : 0xff;
        memset(dat + 4 * i, clr, 4);
      }

      DeleteObject(SelectObject(memdc, membmp));
    }

    DeleteObject(SelectObject(memdc, hFont));
    DeleteDC(memdc);
    ReleaseDC(0, dc);
    return 0 != dat;
  }
#endif // GOOD_SUPPORT_GDIPLUS

#ifdef GOOD_SUPPORT_ANDROID_IMG
  bool AndroidFromIntArray(jintArray iarr)
  {
    if (!iarr) {
      return false;
    }

    jint *iarrbody = tmpEnv->GetIntArrayElements(iarr, 0);
    w = iarrbody[0];
    h = iarrbody[1];
    bpp = 4;
    dat = new char[w * h * bpp];
    memcpy(dat, iarrbody + 2, sizeof(int) * w * h);

    return true;
  }

  bool AndroidLoadImage(std::string const& stream)
  {
    if (!tmpEnv) {
      return false;
    }

    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "imgGetImage", "([B)[I");
    if (!mid) {
      return false;
    }

    jbyteArray barr = tmpEnv->NewByteArray(stream.size());
    tmpEnv->SetByteArrayRegion(barr, 0, stream.size(), (const jbyte*)stream.c_str());

    jintArray iarr = (jintArray)tmpEnv->CallStaticObjectMethod(cls, mid, barr);
    return AndroidFromIntArray(iarr);
  }

  bool AndroidLoadImageFromChar(int size, int ch, bool bAntiAlias)
  {
    if (!tmpEnv) {
      return false;
    }

    jclass cls = tmpEnv->FindClass(JNI_ACTIVITY);
    jmethodID mid = tmpEnv->GetStaticMethodID(cls, "imgGetCharImage", "(IIZ)[I");
    if (!mid) {
      return false;
    }

    jintArray iarr = (jintArray)tmpEnv->CallStaticObjectMethod(cls, mid, size, ch, bAntiAlias);
    return AndroidFromIntArray(iarr);
  }
#endif // GOOD_SUPPORT_ANDROID_IMG
};

} // namespace gx

} // namespace good

// end of img.h
