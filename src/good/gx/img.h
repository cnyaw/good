
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

#ifndef GOOD_SUPPORT_ANDROID_IMG
extern "C" {
# ifdef GOOD_SUPPORT_PNG
#   include "png.h"
# endif
# ifdef GOOD_SUPPORT_JPG
#   include "jpeglib.h"                 // Chang RGB_RED/G/B order in jmorecfg.h to BGR.
# endif
}
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

    int *pdat = (int*)dat;
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
    return Win32GdiplusLoadImage(stream);
#else
    if (
#ifdef GOOD_SUPPORT_ANDROID_IMG
        AndroidLoadImage(stream) ||
#else
#ifdef GOOD_SUPPORT_PNG
        loadPNG(stream) ||
#endif
#ifdef GOOD_SUPPORT_JPG
        loadJPEG(stream) ||
#endif
#endif // GOOD_SUPPORT_ANDROID_IMG
        loadBMP(stream)) {
      return true;
    } else {
      return false;
    }
#endif
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
    return false;
  }

#ifdef WIN32
  void blt(HDC hdc, int x, int y)
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

    wchar_t buff[2] = {ch, 0};

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
#else
  unsigned int rgb(unsigned char R, unsigned char G, unsigned char B)
  {
#if defined(DEVKIT_PSP) || defined(_android_)
    return B | (G << 8) | (R << 16) | (0xff << 24);
#else
    return R | (G << 8) | (B << 16) | (0xff << 24);
#endif
  }

  bool loadBMP(std::string const& stream)
  {
#pragma pack(1)
    struct BMP_HEADER {

      //
      // File Header.
      //

      short Id;
      int FileSize;
      int Reserved;
      int Offset;

      //
      // Info Header.
      //

      int HeaderSize;
      int Width;
      int Height;
      short Planes;
      short BitsPerPixel;
      int Compression;
      int BitmapDataSize;
      int HRes;
      int VRes;
      int UsedColors;
      int ImportantColors;

      //
      // Palette.
      //

      int Palette[1];
    };
#pragma pack()

    BMP_HEADER const *hdr = (BMP_HEADER const*)stream.data();

    if (((char*)&hdr->Id)[0] != 'B' || ((char*)&hdr->Id)[1] != 'M') {
      return false;
    }

    const int bits = hdr->BitsPerPixel, dx = hdr->Width;
    const int dx_bytes = (1 == bits) ?
                          (dx / 8 + ((dx % 8) ? 1 : 0)) :
                          ((4 == bits) ?
                            (dx / 2 + ((dx % 2) ? 1 : 0)) :
                            (dx * bits / 8));
    const int align = (4 - dx_bytes % 4) % 4;

    unsigned char *ptrs = (unsigned char*)stream.data() + hdr->Offset;

    //
    // Decompress buffer (not support)
    //

    if (hdr->Compression) {
      return false;
    }

    //
    // Read pixel data.
    //

    release();

    w = hdr->Width;
    h = abs(hdr->Height);
    bpp = 4;

    unsigned int *pdat = new unsigned int[w * h];
    dat = (char*)pdat;

    switch (bits) {
      case 1: {                         // Monochrome.
        int i = 0;
        for (int y = h - 1; y >= 0; y--) {
          unsigned char mask = 0x80, val = 0;
          for (int x = 0; x < w; x++) {
            if (0x80 == mask) {
              val = *(ptrs++);
            }
            unsigned char *col = (unsigned char*)(hdr->Palette + ((val & mask) ? 1 : 0));
            pdat[i++] = rgb(col[0], col[1], col[2]);
            mask = 1 == mask ? 0x80 : mask >> 1;
        } ptrs+=align; }
      } break;

      case 4: {                         // 16 colors.
        int i = 0;
        for (int y = h - 1; y >= 0; y--) {
          unsigned char mask = 0xf0, val = 0;
          for (int x = 0; x < w; x++) {
            if (0xf0 == mask) {
              val = *(ptrs++);
            }
            const unsigned char color = (unsigned char)((mask < 16) ? (val & mask) : ((val&mask) >> 4));
            unsigned char *col = (unsigned char*)(hdr->Palette + color);
            pdat[i++] = rgb(col[0], col[1], col[2]);
            mask = mask == 0xf0 ? 0x0f : 0xf0;
        } ptrs+=align; }
      } break;

      case 8: {                         //  256 colors.
        int i = 0;
        for (int y = h - 1; y >= 0; y--) {
          for (int x = 0; x < w; x++) {
            const unsigned char *col = (unsigned char*)(hdr->Palette + *(ptrs++));
            pdat[i++] = rgb(col[0], col[1], col[2]);
          } ptrs+=align;
        }
      } break;

      case 16: {                        // 16 bits colors.
        int i = 0;
        for (int y = h - 1; y >= 0; y--) {
          for (int x = 0; x < w; x++) {
            const unsigned char c1 = *(ptrs++), c2 = *(ptrs++);
            const unsigned short col = (unsigned short)(c1 | (c2 << 8));
            pdat[i++] = rgb(col & 0x1F, (col >> 5) & 0x1f, (col >> 10) & 0x1f);
          } ptrs+=align;
        }
      } break;

      case 24: {                        // 24 bits colors.
        int i = 0;
        for (int y = h - 1; y >= 0; y--) {
          for (int x = 0; x < w; x++) {
            pdat[i++] = rgb(ptrs[0], ptrs[1], ptrs[2]);
            ptrs += 3;
          } ptrs+=align;
        }
      } break;

      case 32: {                        // 32 bits colors.
        int i = 0;
        for (int y = h - 1; y >= 0; y--) {
          for (int x = 0; x < w; x++) {
            pdat[i++] = rgb(ptrs[0], ptrs[1], ptrs[2]);
            ptrs += 4;
          } ptrs+=align;
        }
      } break;
    }

    if (0 < h) {
      flip();
    }

    return true;
  }

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
#else
#ifdef GOOD_SUPPORT_JPG
  struct myjpg_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
  };

  static void myjpg_error_exit(j_common_ptr cinfo)
  {
    myjpg_error_mgr *myerr = (myjpg_error_mgr*)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
  }

  struct myjpg_source_mgr
  {
    struct jpeg_source_mgr pub;
    const char *dat;
    int len;
    int ptr;
  };

  void myjpg_init_jpeg_src(j_decompress_ptr cinfo, std::string const& stream)
  {
    myjpg_source_mgr *src = (myjpg_source_mgr*)cinfo->src;
    if (0 == src) {
      src = new myjpg_source_mgr;
      src->dat = stream.data();
      src->len = (int)stream.length();
      src->ptr = 0;
      src->pub.init_source = myjpg_init_source;
      src->pub.fill_input_buffer = myjpg_fill_input_buffer;
      src->pub.skip_input_data = myjpg_skip_input_data;
      src->pub.term_source = myjpg_term_source;
      src->pub.resync_to_restart = jpeg_resync_to_restart;
      cinfo->src = (struct jpeg_source_mgr*)src;
    }
  }

  static void myjpg_init_source(j_decompress_ptr cinfo)
  {
    myjpg_source_mgr *src = (myjpg_source_mgr*)cinfo->src;
    src->ptr = 0;
    src->pub.bytes_in_buffer = src->len;
    src->pub.next_input_byte = (JOCTET*)src->dat;
  }

  static boolean myjpg_fill_input_buffer(j_decompress_ptr cinfo)
  {
    myjpg_source_mgr *src = (myjpg_source_mgr*)cinfo->src;
    src->pub.bytes_in_buffer = src->len - src->ptr;
    src->pub.next_input_byte = (JOCTET*)src->dat + src->ptr;
    return TRUE;
  }

  static void myjpg_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
  {
    myjpg_source_mgr *src = (myjpg_source_mgr*)cinfo->src;
    src->ptr += num_bytes;
    src->pub.bytes_in_buffer = src->len - src->ptr;
    src->pub.next_input_byte = (JOCTET*)src->dat + src->ptr;
  }

  static void myjpg_term_source(j_decompress_ptr cinfo)
  {
    //
    // NOP.
    //
  }

  bool loadJPEG(std::string const& stream)
  {
    jpeg_decompress_struct cinfo;

    myjpg_error_mgr err;

    cinfo.err = jpeg_std_error(&err.pub);
    err.pub.error_exit = myjpg_error_exit;

    if (setjmp(err.setjmp_buffer)) {
      jpeg_destroy_decompress(&cinfo);
      return false;
    }

    jpeg_create_decompress(&cinfo);

    myjpg_init_jpeg_src(&cinfo, stream);

    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);

    release();

    w = cinfo.output_width;
    h = cinfo.output_height;
    bpp = cinfo.output_components;
    dat = new char[w * h * bpp];

    int row_stride = cinfo.output_width * cinfo.output_components;

    char *p = dat;
    while (cinfo.output_scanline < cinfo.output_height) {
      (void)jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&p, 1);
      p += row_stride;
    }

    (void)jpeg_finish_decompress(&cinfo);

    return true;
  }
#endif // GOOD_SUPPORT_JPG

#ifdef GOOD_SUPPORT_PNG
  static void png_read_from_mem(png_structp png_ptr, png_bytep data, png_size_t length)
  {
    char const* p = (char const*)png_get_io_ptr(png_ptr);
    memcpy (data, p, length);
    p += length;
    png_set_read_fn(png_ptr, (png_voidp*)p, png_read_from_mem);
  }

  bool loadPNG(std::string const& stream)
  {
    char const* p = stream.c_str();

    if (!png_check_sig((png_bytep)p, 8)) {
      return false;
    }

    png_structp png_ptr;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (0 == png_ptr) {
      return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (0 == info_ptr) {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      return false;
    }

    png_set_read_fn(png_ptr, (png_voidp*)p, png_read_from_mem);

    png_read_info(png_ptr, info_ptr);

    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);

    if (PNG_COLOR_TYPE_PALETTE == color_type) {
      png_set_palette_to_rgb(png_ptr);
    }

    if (PNG_COLOR_TYPE_GRAY == color_type && 8 > bit_depth) {

      //
      // Convert 1-2-4 bits grayscale images to 8 bits grayscale.
      //

      png_set_gray_to_rgb(png_ptr);
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
      png_set_tRNS_to_alpha(png_ptr);
    }

    if (16 == bit_depth) {
      png_set_strip_16(png_ptr);
    } else if (8 > bit_depth) {
      png_set_packing(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    png_get_IHDR(
      png_ptr,
      info_ptr,
      &width,
      &height,
      &bit_depth,
      &color_type,
      NULL,
      NULL,
      NULL);

    release();

    w = width;
    h = height;

    switch (color_type)
    {
    case PNG_COLOR_TYPE_GRAY:
      bpp = 1;
      break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
      bpp = 2;
      break;

    case PNG_COLOR_TYPE_RGB:
      bpp = 3;
      break;

    case PNG_COLOR_TYPE_RGB_ALPHA:
      bpp = 4;
      break;
    }

    dat = new char[width * height * bpp];

    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);

    for (int i = 0; i < (int)height; ++i) {
      row_pointers[i] = (png_bytep)(dat + (i * width * bpp));
    }

    png_read_image(png_ptr, row_pointers);

    png_read_end(png_ptr, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    free(row_pointers);

    return true;
  }
#endif // GOOD_SUPPORT_PNG
#endif // GOOD_SUPPORT_ANDROID_IMG
#endif // GOOD_SUPPORT_GDIPLUS
};

} // namespace gx

} // namespace good

// end of img.h
