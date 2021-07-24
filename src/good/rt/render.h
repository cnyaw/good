
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

void renderMapBg(ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, unsigned int color) const
{
  MapT const& map = mRes.getMap(a.mResId);

  int w = map.mWidth * map.mTileset.mTileWidth;
  int h = map.mHeight * map.mTileset.mTileHeight;

  int nx, ny, xbound, ybound;
  calcDrawTileParam(a.mRepX, a.mRepY, (int)cx, (int)cy, w, h, rcv, nx, ny, xbound, ybound);

  T *pThis = (T*)this;
  for (int ay = ny + rcv.top; -h <= ay && ay < ybound; ay += h) {
    for (int ax = nx + rcv.left; -w <= ax && ax < xbound; ax += w) {
      sw2::IntRect rc(0, 0, w, h);
      rc.offset(ax, ay);
      sw2::IntRect rcm;
      if (!rc.intersect(rcv, rcm)) {
        continue;
      }
      ImgT img = getImage(a, mRes.getTex(map.mTileset.mTextureId).mFileName);
      if (!img.isValid()) {
        return;
      }
      pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
      CommonDrawMap(pThis->gx, map, img, (int)cx, (int)cy, rcv, rcm, rc, color);
    }
  }
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

  calcDrawTileParam(a.mRepX, a.mRepY, (int)cx, (int)cy, sw, sh, rcv, nx, ny, xbound, ybound);

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
    for (int ay = ny + rcv.top; -sh <= ay && ay < ybound; ay += sh) {
      for (int ax = nx + rcv.left; -sw <= ax && ax < xbound; ax += sw) {
        pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
        pThis->gx.drawImage((int)ax, (int)ay, img, a.mDim.left, a.mDim.top, w, h, color, rot, xscale, yscale);
      }
    }
  } else {
    int w = a.mDim.width();
    int h = a.mDim.height();
    int sw, sh, nx, ny, xbound, ybound;
    calcRenderTexBgParam(a, cx, cy, rcv, xscale, yscale, w, h, sw, sh, nx, ny, xbound, ybound);
    ImgT img;
    for (int ay = ny + rcv.top; -sh <= ay && ay < ybound; ay += sh) {
      for (int ax = nx + rcv.left; -sw <= ax && ax < xbound; ax += sw) {
        if (!img.isValid()) {
          img = getTexBgImg_i(a);
          if (!img.isValid()) {         // Later validate img.
            return;
          }
        }
        pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
        pThis->gx.drawImage((int)ax, (int)ay, img, a.mDim.left, a.mDim.top, w, h, color, rot, xscale, yscale);
      }
    }
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

  T *pThis = (T*)this;
  pThis->gx.setAnchor(a.mAnchorX, a.mAnchorY);
  CommonDrawSprite(pThis->gx, spr, img, rcm.left - rcv.left, rcm.top - rcv.top, a.mCurrFrame, rot, xscale, yscale);
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
      renderTexBg(child, cx - rcv.left, cy - rcv.top, rcv, childBgColor, arot, axscale, ayscale);
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

void calcDrawTileParam(bool repx, bool repy, int x, int y, int cx, int cy, sw2::IntRect const& rcbound, int& nx, int& ny, int& xbound, int& ybound) const
{
  nx = x - rcbound.left;
  ny = y - rcbound.top;
  xbound = (std::min)(rcbound.right, x + cx);
  ybound = (std::min)(rcbound.bottom, y + cy);

  if (repx) {
    if (0 < nx) {
      nx = (nx % cx) - cx;
    } else if (0 > nx) {
      nx = - (-nx % cx);
    }
    xbound = rcbound.right;
  }

  if (repy) {
    if (0 < ny) {
      ny = (ny % cy) - cy;
    } else if (0 > ny) {
      ny = - (-ny % cy);
    }
    ybound = rcbound.bottom;
  }
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

  std::stringstream ss;
  loadFile(name, ss);

  ImgT img = ImgT::getImage(name, ss.str());
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
