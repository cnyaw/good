
//
// render.h
// Render objects, part of class Application<>.
//
// Copyright (c) 2013 Waync Cheng.
// All Rights Reserved.
//
// 2013/12/18 Waync created.
//

#pragma once

bool mAntiAlias;
int mSelFont;                           // GOOD_DRAW_TEXT_FONT.

#define BEGIN_TILE_FILL(XSTART, YSTART, LEFT, TOP, RIGHT, BOTTOM, W, H) \
  for (int ay = (YSTART) + (TOP); -(H) <= ay && ay < (BOTTOM); ay += (H)) { \
    for (int ax = (XSTART) + (LEFT); -(W) <= ax && ax < (RIGHT); ax += (W)) {\
      sw2::IntRect rc(0, 0, w, h); \
      rc.offset(ax, ay); \
      sw2::IntRect rcm; \
      if (!rc.intersect(rcv, rcm)) { \
        continue; \
      }

#define END_TILE_FILL() \
    } \
  }

void renderColorBg(ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, unsigned int color, float rot, float xscale, float yscale) const
{
  int l, t, w, h;
  getDim_i(a, l, t, w, h);

  sw2::IntRect rc(0, 0, w, h);
  rc.offset((int)cx, (int)cy);

  sw2::IntRect rcm;
  if (!rc.intersect(rcv, rcm)) {
    return;
  }

  T *pThis = (T*)this;
  pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
  pThis->gx.fillSolidColor(rcm.left - rcv.left, rcm.top - rcv.top, rcm.width(), rcm.height(), color, rot, xscale, yscale);
}

template<class GxT>
void renderMapBg_i(GxT &gx, const MapT &map, ImgT &img, float cx, float cy, sw2::IntRect const& rcv, unsigned int color = 0xffffffff, bool repX = false, bool repY = false) const
{
  int w = map.mWidth * map.mTileset.mTileWidth;
  int h = map.mHeight * map.mTileset.mTileHeight;

  int nx, ny, xbound, ybound;
  CalcDrawTileParam(repX, repY, (int)cx, (int)cy, w, h, rcv, nx, ny, xbound, ybound);

  BEGIN_TILE_FILL(nx, ny, rcv.left, rcv.top, xbound, ybound, w, h)
    CommonDrawMap(gx, map, img, (int)cx, (int)cy, rcv, rcm, rc, color);
  END_TILE_FILL()
}

void renderMapBg(ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, unsigned int color) const
{
  MapT const& map = mRes.getMap(a.mResId);

  ImgT img = getImage(a, mRes.getTex(map.mTileset.mTextureId).mFileName);
  if (!img.isValid()) {
    return;
  }

  T *pThis = (T*)this;
  pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
  renderMapBg_i(pThis->gx, map, img, cx, cy, rcv, color, a.mRepX, a.mRepY);
}

ImgT getTexBgImg_i(ActorT const& a) const
{
  if (-1 != a.mResId) {
    TextureT const& tex = mRes.getTex(a.mResId);
    return getImage(a, tex.mFileName);
  } else {
    return a.mImg;                      // This is a char map obj, use cache immediately.
  }
}

void calcRenderTexBgParam(ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, float xscale, float yscale, int w, int h, int &sw, int &sh, int &nx, int &ny, int &xbound, int &ybound) const
{
  sw = std::abs((int)(w * xscale));
  sh = std::abs((int)(h * yscale));

  CalcDrawTileParam(a.mRepX, a.mRepY, (int)cx, (int)cy, sw, sh, rcv, nx, ny, xbound, ybound);

  if (a.mRepX) {
    xbound += (int)(sw * a.mAnchorX);
  }

  if (a.mRepY) {
    ybound += (int)(sh * a.mAnchorY);
  }
}

void renderTexBg(ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, unsigned int color, float rot, float xscale, float yscale) const
{
  T *pThis = (T*)this;
  if (a.mDim.empty()) {
    ImgT img = getTexBgImg_i(a);
    if (!img.isValid()) {               // Pre validate img.
      return;
    }
    int w = img.getWidth();
    int h = img.getHeight();
    int sw, sh, nx, ny, xbound, ybound;
    calcRenderTexBgParam(a, cx, cy, rcv, xscale, yscale, w, h, sw, sh, nx, ny, xbound, ybound);
    BEGIN_TILE_FILL(nx, ny, rcv.left, rcv.top, xbound, ybound, sw, sh)
      pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
      pThis->gx.drawImage(ax - rcv.left, ay - rcv.top, img, a.mDim.left, a.mDim.top, w, h, color, rot, xscale, yscale);
    END_TILE_FILL()
  } else {
    int w = a.mDim.width();
    int h = a.mDim.height();
    int sw, sh, nx, ny, xbound, ybound;
    calcRenderTexBgParam(a, cx, cy, rcv, xscale, yscale, w, h, sw, sh, nx, ny, xbound, ybound);
    ImgT img;
    BEGIN_TILE_FILL(nx, ny, rcv.left, rcv.top, xbound, ybound, sw, sh)
      if (!img.isValid()) {
        img = getTexBgImg_i(a);
        if (!img.isValid()) {         // Later validate img.
          return;
        }
      }
      pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
      pThis->gx.drawImage(ax - rcv.left, ay - rcv.top, img, a.mDim.left, a.mDim.top, w, h, color, rot, xscale, yscale);
    END_TILE_FILL()
  }
}

void renderSprite(ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, unsigned int color, float rot, float xscale, float yscale) const
{
  int spriteId = a.mResId;

  SpriteT const& spr = mRes.getSprite(spriteId);
  if (spr.mFrame.empty()) {
    return;
  }

  sw2::IntRect rc;
  rc = sw2::IntRect(0, 0, spr.mTileset.mTileWidth, spr.mTileset.mTileHeight);
  rc.offset((int)cx + spr.mOffsetX, (int)cy + spr.mOffsetY);

  sw2::IntRect rcm;
  if (!rc.intersect(rcv, rcm)) {
    return;
  }

  ImgT img = getImage(a, mRes.getTex(spr.mTileset.mTextureId).mFileName);
  if (!img.isValid()) {
    return;
  }

  int srcx, srcy;
  CalcDrawSpriteTexOffset(spr, a.mCurrFrame, srcx, srcy);

  int offsetx = rcm.left - rc.left;
  int offsety = rcm.top - rc.top;

  T *pThis = (T*)this;
  pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
  pThis->gx.drawImage(rcm.left - rcv.left, rcm.top - rcv.top, img, srcx + offsetx, srcy + offsety, rcm.width(), rcm.height(), color, rot, xscale, yscale);
}

bool renderChilds(ActorT const& a, sw2::IntRect const& rcv, float rot, float xscale, float yscale) const
{
  float alpha = ((a.mBgColor >> 24) & 0xff) / (float)0xff;

  for (int i = 0; i < a.getChildCount(); ++i) {

    int id = a.getChild(i);

    ActorT const& child = mActors[id];
    if (!child.mVisible) {
      continue;
    }

    float arot = rot + child.mRot;
    float axscale = xscale * child.mXscale;
    float ayscale = yscale * child.mYscale;

    if (child.mOwnerDraw) {
      child.OnDraw();
      renderChilds(child, rcv, arot, axscale, ayscale);
      continue;
    }

    float cx, cy;
    child.getPos(cx, cy);

    float childAlpha = alpha * (((child.mBgColor >> 24) & 0xff) / (float)0xff);
    unsigned int childBgColor = (child.mBgColor & 0xffffff) | (((unsigned int)(childAlpha * 0xff) & 0xff) << 24);

    switch (child.getType())
    {
    case ActorT::TYPES_COLBG:           // Solid color background.
      renderColorBg(child, cx, cy, rcv, childBgColor, arot, axscale, ayscale);
      break;

    case ActorT::TYPES_MAPBG:           // Tilemap background.
      renderMapBg(child, cx, cy, rcv, childBgColor);
      break;

    case ActorT::TYPES_TEXBG:           // Texture background.
      renderTexBg(child, cx, cy, rcv, childBgColor, arot, axscale, ayscale);
      break;

    case ActorT::TYPES_SPRITE:          // Object instance.
      renderSprite(child, cx, cy, rcv, childBgColor, arot, axscale, ayscale);
      break;
    }

    renderChilds(child, rcv, arot, axscale, ayscale);
  }

  return true;
}

bool render() const
{
  if (!mActors.isUsed(mRoot)) {
    return false;
  }

  ActorT const& a = mActors[mRoot];
  if (ActorT::TYPES_LEVEL != a.mResType) {
    return false;
  }

  T *pThis = (T*)this;
  if (!a.mOwnerDraw) {
    pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
    pThis->gx.fillSolidColor(0, 0, mRes.mWidth, mRes.mHeight, a.mBgColor, .0f, 1.0f, 1.0f);
  } else {
    a.OnDraw();
  }

  sw2::IntRect rcv(0, 0, mRes.mWidth, mRes.mHeight);
  rcv.offset((int)a.mPosX, (int)a.mPosY);

  return renderChilds(a, rcv, a.mRot, a.mXscale, a.mYscale);
}

void renderAll() const
{
  T *pThis = (T*)this;
  mDirty = false;
  mRenderState = true;
  if (mTexDirty) {
    pThis->gx.restoreSur();
    mTexDirty = false;
  }
  pThis->gx.beginDraw(mRes.mWidth, mRes.mHeight);
  render();                             // render() may caused dirty flag set, so it will trigger renderAll() next time.
  pThis->onRender();
  pThis->gx.endDraw();
  mRenderState = false;
}

ImgT getFixFontImage(int size, int ch) const
{
  char chrmap[128];

  bool chValid = ' ' <= ch && '~' >= ch;
  if (chValid) {
    sprintf(chrmap, "chrmap;fix;%d;%d", size, ch);
  } else {
    sprintf(chrmap, "chrmap;fix;%d;none", size);
  }

  if (ImgT::existImage(chrmap)) {
    return ImgT::getImage(chrmap);
  }

  CanvasT img;
  int w = img.calcTextWidth((const char*)&ch, 1, size);
  img.create(w, 8 * size, 4);
  img.fill(0);

  if (chValid) {
    img.drawText((const char*)&ch, 1, 0, 0, 0xffffffff, size);
  } else {
    img.rect(0xffffffff, 0, 0, img.w, img.h);
  }

  ImgT i = ImgT::getImage(chrmap, img);
  if (i.isValid()) {
    mDirty = true;
  }

  return i;
}

ImgT getImage(ActorT const &a, std::string const& name) const
{
  if (a.mImg.isValid()) {
    return a.mImg;
  } else {
    return a.mImg = getImage(name);
  }
}

ImgT getImage(std::string const& name) const
{
  if (0 == mAr) {
    return ImgT();
  }

  if (ImgT::existImage(name)) {
    return ImgT::getImage(name);
  }

  std::string s;
  loadFile(name, s);

  ImgT img = ImgT::getImage(name, s);
  if (img.isValid()) {
    mDirty = true;
  }

  return img;
}

ImgT getImage(int size, int ch) const
{
  if (GOOD_DRAW_TEXT_FIXED_FONT == getFont()) {
    return getFixFontImage(size, ch);
  } else {
    return getSysFontImage(size, ch);
  }
}

ImgT getSysFontImage(int size, int ch) const
{
  char chrmap[128];
  sprintf(chrmap, "chrmap;%d;%d;%d", size, ch, mAntiAlias);

  if (ImgT::existImage(chrmap)) {
    return ImgT::getImage(chrmap);
  }

  CanvasT img;
  if (!img.loadFromChar(size, ch, mAntiAlias)) {
    img.create(size, size, 4);
    img.fill(0).rect(0xffffffff, 0, 0, img.w, img.h);;
  }

  ImgT i = ImgT::getImage(chrmap, img);
  if (i.isValid()) {
    mDirty = true;
  }

  return i;
}

// end of render.h
