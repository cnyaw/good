
//
// opengl_gx.h
// OpenGL graphics impl.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/05/19 Waync created.
//

#pragma once

#include "imgm.h"
#include "mat4.h"

#include "white_pixel.h"

namespace good {

namespace gx {

template<class ImgMgr>
class GL_Surface : public ImageSurface
{
public:
  enum {
    GL_PACK_TEX_WIDTH = 1024,
    GL_PACK_TEX_HEIGHT = 1024
  };

  typedef ImageSurface BaseT;

  GLuint tex;

  GL_Surface() : BaseT(), tex(0)
  {
  }

  bool init()
  {
    if (!BaseT::init(GL_PACK_TEX_WIDTH, GL_PACK_TEX_HEIGHT)) {
      return false;
    }

    glGenTextures(1, &tex);

    return true;
  }

  void free()
  {
    BaseT::free();

    if (tex) {
      glDeleteTextures(1, &tex);
      tex = 0;
    }
  }

  void update(sw2::IntRect const &rc, GxImage const &aimg)
  {
    //
    // Blt the img to the pack texture.
    //

    BaseT::update(rc, aimg);

    //
    // Transfer pack texture data to GL.
    //

    restoreSur();
  }

  void restoreSur()
  {
    ImgMgr::inst().lastTex = 0;

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      GL_PACK_TEX_WIDTH,
      GL_PACK_TEX_HEIGHT,
      0,
#ifdef _android_
      GL_RGBA,
#else
      GL_BGRA_EXT,
#endif
      GL_UNSIGNED_BYTE,
      img.dat);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#ifdef _android_
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#endif
  }
};

template<class ImgMgr>
class GLImageRect : public ImageRect<GL_Surface<ImgMgr> >
{
};

class GLImageResource : public ImageManager<GLImageResource, GL_Surface<GLImageResource>, GLImageRect<GLImageResource> >
{
public:

  GL_Surface<GLImageResource> const* lastTex;

  static GLImageResource& inst()
  {
    static GLImageResource i;
    return i;
  }

  void restoreSur()
  {
    std::vector<GL_Surface<GLImageResource>*>::iterator it = mSur.begin();
    for (; it != mSur.end(); ++it) {
      (*it)->restoreSur();
    }
  }
};

class GLImage : public Image<GLImage>
{
public:

  GLImageRect<GLImageResource> const* mSur;

  GLImage() : mSur(0)
  {
  }

  GLImage(GLImageRect<GLImageResource> const* sur) : mSur(sur)
  {
  }

  bool isValid() const
  {
    return 0 != mSur && 0 != mSur->tex && 0 != mSur->tex->tex && 0 != mSur->tex->img.dat;
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
    return GLImageResource::inst().existImage(name);
  }

  static GLImage getImage(std::string const& name)
  {
    return GLImage(GLImageResource::inst().getImage(name));
  }

  static GLImage getImage(std::string const& name, std::string const& stream)
  {
    return GLImage(GLImageResource::inst().getImage(name, stream));
  }

  static GLImage getImage(std::string const& name, int size, int ch, bool bAntiAlias)
  {
    return GLImage(GLImageResource::inst().getImage(name, size, ch, bAntiAlias));
  }
};

class GLGraphics : public Graphics<GLImage>
{
public:
  enum {
    NUM_VERT_PER_OBJ = 4,
    MAX_OBJ = 256
  };

  int SCREEN_W, SCREEN_H;

  int nDrawCalls, nLastDrawCalls;
  float xAnchor, yAnchor;

  int obj_index;
  std::vector<float> vert, texcoord;
  std::vector<unsigned char> colors;
  std::vector<unsigned short> indexs;

  GLGraphics(int w = 320, int h = 480) : SCREEN_W(w), SCREEN_H(h), xAnchor(.0f), yAnchor(.0f), obj_index(0)
  {
    vert.resize(3 * NUM_VERT_PER_OBJ * MAX_OBJ); // xyz.
    texcoord.resize(2 * NUM_VERT_PER_OBJ * MAX_OBJ); // uv.
    colors.resize(4 * NUM_VERT_PER_OBJ * MAX_OBJ); // rgba.
    indexs.resize(6 * MAX_OBJ);
    for (int i = 0, j = 0; i < 6 * MAX_OBJ; i += 6, j += 4) {
      indexs[i + 0] = 0 + j;
      indexs[i + 1] = 1 + j;
      indexs[i + 2] = 2 + j;
      indexs[i + 3] = 2 + j;
      indexs[i + 4] = 3 + j;
      indexs[i + 5] = 0 + j;
    }
  }

  void init()
  {
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glAlphaFunc(GL_GREATER, 0.1f);
    glEnable(GL_ALPHA_TEST);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &*vert.begin());

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, &*texcoord.begin());

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, &*colors.begin());

    glClearColor(136/255.0f, 133/255.0f, 135/255.0f, 1);

    GLImageResource::inst().lastTex = 0;
    nDrawCalls = nLastDrawCalls = 0;
  }

  void resize(int width, int height)
  {
    if (height == 0) {
      height = 1;
    }

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, SCREEN_W, SCREEN_H, 0, 0, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void beginDraw(int width, int height)
  {
    nLastDrawCalls = nDrawCalls;
    nDrawCalls = 0;
    imgWhitePixel = 0;
  }

  void endDraw()
  {
    flush();
  }

  void flush()
  {
    if (obj_index) {
      glDrawElements(GL_TRIANGLES, 6 * obj_index, GL_UNSIGNED_SHORT, &*indexs.begin());
      nDrawCalls += 1;
      obj_index = 0;
    }
  }

  void setAnchor(float x, float y)
  {
    xAnchor = x;
    yAnchor = y;
  }

  void applyObjVert(const Mat4 &mat) const
  {
    static const GLfloat square[] = {
       -0.5f, -0.5f, 0,
       0.5f, -0.5f, 0,
       0.5f, 0.5f, 0,
       -0.5f, 0.5f, 0
    };

    mat.transform(NUM_VERT_PER_OBJ, square, ((float*)&*vert.begin()) + 3 * NUM_VERT_PER_OBJ * obj_index);
  }

  void applyObjTransform(int x, int y, int srcw, int srch, float xscale, float yscale, float rot) const
  {
    Mat4 mat;
    mat.setIdentity();

    float srcwscale = srcw * xscale;
    float srchscale = srch * yscale;
    float fabsw = fabs(srcwscale);
    float fabsh = fabs(srchscale);

    float ax = -(xAnchor - .5f) * fabsw;
    float ay = -(yAnchor - .5f) * fabsh;
    float asx = xAnchor * srcw * (1 - fabs(xscale));
    float asy = yAnchor * srch * (1 - fabs(yscale));

    mat.postTranslate(x + fabsw * .5f - ax + asx, y + fabsh * .5f - ay + asy, 0);

    if (.0f != rot) {
      mat.postRotate(rot, 0, 0, 1);
    }

    mat.postTranslate(ax, ay, .0f);
    mat.postScale(srcwscale, srchscale, 1);

    applyObjVert(mat);
  }

  void applyObjColor(unsigned int color) const
  {
    unsigned char b = (color & 0xff);
    unsigned char g = ((color >> 8) & 0xff);
    unsigned char r = ((color >> 16) & 0xff);
    unsigned char a = ((color >> 24) & 0xff);
    unsigned char *pColors = ((unsigned char*)&*colors.begin()) + 4 * NUM_VERT_PER_OBJ * obj_index;
    for (int i = 0; i < NUM_VERT_PER_OBJ; i++, pColors += 4) {
      pColors[0] = r;
      pColors[1] = g;
      pColors[2] = b;
      pColors[3] = a;
    }
  }

  bool drawImage(int x, int y, GLImage const& img, int srcx, int srcy, int srcw, int srch, unsigned int color, float rot, float xscale, float yscale)
  {
    if (!img.isValid()) {
      return false;
    }

    {
      int imgw = img.getWidth(), imgh = img.getHeight();

      srcw = (std::min)(srcw, imgw), srch = (std::min)(srch, imgh);

      if (srcx + srcw > imgw) {
        srcw = imgw - srcx;
        if (0 >= srcw) {
          return false;
        }
      }

      if (srcy + srch > imgh) {
        srch = imgh - srcy;
        if (0 >= srch) {
          return false;
        }
      }
    }

    if (img.mSur->tex != GLImageResource::inst().lastTex || MAX_OBJ == obj_index) {
      flush();
    }

    applyObjTransform(x, y, srcw, srch, xscale, yscale, rot);

    float imgw = GL_Surface<GLImageResource>::GL_PACK_TEX_WIDTH;
    float imgh = GL_Surface<GLImageResource>::GL_PACK_TEX_HEIGHT;

    srcx += img.mSur->left;
    srcy += img.mSur->top;

    float x0 = (.5f + srcx) / (float)imgw, y0 = (.5f + srcy) / (float)imgh; // Apply half pixel correction to avoid texture edge color problem.
    float x1 = (-.5f + srcx + srcw) / (float)imgw, y1 = (-.5f + srcy + srch) / (float)imgh;

    float *pTexCoord = ((float*)&*texcoord.begin()) + 2 * NUM_VERT_PER_OBJ * obj_index;
    pTexCoord[0] = x0;
    pTexCoord[1] = y0;
    pTexCoord[2] = x1;
    pTexCoord[3] = y0;
    pTexCoord[4] = x1;
    pTexCoord[5] = y1;
    pTexCoord[6] = x0;
    pTexCoord[7] = y1;

    applyObjColor(color);

    obj_index += 1;

    if (img.mSur->tex != GLImageResource::inst().lastTex) {
      glBindTexture(GL_TEXTURE_2D, img.mSur->tex->tex);
      GLImageResource::inst().lastTex = img.mSur->tex;
    }

    return true;
  }

  GLImage imgWhitePixel;

  bool fillSolidColor(int x, int y, int w, int h, unsigned int color, float rot, float xscale, float yscale)
  {
    if (!imgWhitePixel.isValid()) {
      std::string s((const char*)WHITE_PIXEL_MOD, sizeof(WHITE_PIXEL_MOD));
      imgWhitePixel = GLImage::getImage("fillSolidColor_whitepixel", s);
    }

    if (!imgWhitePixel.isValid()) {
      return false;
    }

    if (imgWhitePixel.mSur->tex != GLImageResource::inst().lastTex || MAX_OBJ == obj_index) {
      flush();
    }

    applyObjTransform(x, y, w, h, xscale, yscale, rot);

    float imgw = GL_Surface<GLImageResource>::GL_PACK_TEX_WIDTH;
    float imgh = GL_Surface<GLImageResource>::GL_PACK_TEX_HEIGHT;

    int srcx = imgWhitePixel.mSur->left;
    int srcy = imgWhitePixel.mSur->top;

    float x0 = (.5f + srcx) / (float)imgw, y0 = (.5f + srcy) / (float)imgh; // Apply half pixel correction to avoid texture edge color problem.
    float x1 = (-.5f + srcx + 1) / (float)imgw, y1 = (-.5f + srcy + 1) / (float)imgh;

    float *pTexCoord = ((float*)&*texcoord.begin()) + 2 * NUM_VERT_PER_OBJ * obj_index;
    pTexCoord[0] = x0;
    pTexCoord[1] = y0;
    pTexCoord[2] = x1;
    pTexCoord[3] = y0;
    pTexCoord[4] = x1;
    pTexCoord[5] = y1;
    pTexCoord[6] = x0;
    pTexCoord[7] = y1;

    applyObjColor(color);

    obj_index += 1;

    if (imgWhitePixel.mSur->tex != GLImageResource::inst().lastTex) {
      glBindTexture(GL_TEXTURE_2D, imgWhitePixel.mSur->tex->tex);
      GLImageResource::inst().lastTex = imgWhitePixel.mSur->tex;
    }

    return true;
  }

  void restoreSur()
  {
    GLImageResource::inst().restoreSur();
  }

  bool drawTex(int x, int y, GL_Surface<GLImageResource> *pSur, int w, int h, unsigned int color)
  {
    if (pSur != GLImageResource::inst().lastTex || MAX_OBJ == obj_index) {
      flush();
    }

    Mat4 mat;
    mat.setIdentity();
    mat.postTranslate(x + w / 2.0f, y + h / 2.0f, 0);
    mat.postScale((float)w, (float)h, 1);
    applyObjVert(mat);

    float *pTexCoord = ((float*)&*texcoord.begin()) + 2 * NUM_VERT_PER_OBJ * obj_index;
    pTexCoord[0] = .0f;
    pTexCoord[1] = .0f;
    pTexCoord[2] = 1.0f;
    pTexCoord[3] = .0f;
    pTexCoord[4] = 1.0f;
    pTexCoord[5] = 1.0f;
    pTexCoord[6] = .0f;
    pTexCoord[7] = 1.0f;

    applyObjColor(color);

    obj_index += 1;

    if (pSur != GLImageResource::inst().lastTex) {
      glBindTexture(GL_TEXTURE_2D, pSur->tex);
      GLImageResource::inst().lastTex = pSur;
    }

    return true;
  }
};

} // namespace gx

} // namespace good

// end of opengl_gx.h
