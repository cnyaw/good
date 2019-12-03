
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

template<class GxT>
void renderColorBg(GxT& gx, ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, unsigned int color, float rot, float xscale, float yscale) const
{
  int l, t, w, h;
  getDim_i(a, l, t, w, h);

  sw2::IntRect rc(0, 0, w, h);
  rc.offset((int)cx, (int)cy);

  sw2::IntRect rcm;
  if (!rc.intersect(rcv, rcm)) {
    return;
  }

  gx.setAnchor(a.mAnchorX, a.mAnchorY);
  gx.fillSolidColor(rcm.left - rcv.left, rcm.top - rcv.top, rcm.width(), rcm.height(), color, rot, xscale, yscale);
}

template<class GxT>
void renderMapBg(GxT& gx, ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, unsigned int color) const
{
  MapT const& map = mRes.getMap(a.mResId);

  int w = map.mWidth * map.mTileset.mTileWidth;
  int h = map.mHeight * map.mTileset.mTileHeight;

  int nx, ny, xbound, ybound;
  calcDrawTileParam(a.mRepX, a.mRepY, (int)cx, (int)cy, w, h, rcv, nx, ny, xbound, ybound);

  for (int ay = ny + rcv.top; -h <= ay && ay < ybound; ay += h) {
    for (int ax = nx + rcv.left; -w <= ax && ax < xbound; ax += w) {

      sw2::IntRect rc(0, 0, w, h);
      rc.offset(ax, ay);

      sw2::IntRect rcm;
      if (!rc.intersect(rcv, rcm)) {
        continue;
      }

      sw2::IntPoint adj0((int)cx % map.mTileset.mTileWidth, (int)cy % map.mTileset.mTileHeight);
      sw2::IntPoint adj(adj0.x + (map.mTileset.mTileWidth * (rcm.left / map.mTileset.mTileWidth)), adj0.y + (map.mTileset.mTileHeight * (rcm.top / map.mTileset.mTileHeight)));
      rcm.offset(-rcm.left + adj.x, -rcm.top + adj.y);

      int left = (int)((rcm.left - rc.left) / map.mTileset.mTileWidth);
      int right = (std::min)(map.mWidth - 1, (int)((rcm.right - rc.left) / map.mTileset.mTileWidth));
      int top = (int)((rcm.top - rc.top) / map.mTileset.mTileHeight);
      int bottom = (std::min)(map.mHeight - 1, (int)((rcm.bottom - rc.top) / map.mTileset.mTileHeight));

      if (0 < left) {                   // Draw an extra tile to avoid tear.
        left -= 1;
        rcm.left -= map.mTileset.mTileWidth;
      }

      if (0 < top) {
        top -= 1;
        rcm.top -= map.mTileset.mTileHeight;
      }

      ImgT img = getImage(a, mRes.getTex(map.mTileset.mTextureId).mFileName);
      if (!img.isValid()) {
        return;
      }

      gx.setAnchor(a.mAnchorX, a.mAnchorY);
      CommonDrawMap(gx, map, img, rcm.left - rcv.left, rcm.top - rcv.top, left, top, right, bottom, color);
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

template<class GxT>
void renderTexBg(GxT& gx, ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, unsigned int color, float rot, float xscale, float yscale) const
{
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
        gx.setAnchor(a.mAnchorX, a.mAnchorY);
        gx.drawImage((int)ax, (int)ay, img, a.mDim.left, a.mDim.top, w, h, color, rot, xscale, yscale);
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
        gx.setAnchor(a.mAnchorX, a.mAnchorY);
        gx.drawImage((int)ax, (int)ay, img, a.mDim.left, a.mDim.top, w, h, color, rot, xscale, yscale);
      }
    }
  }
}

template<class GxT>
void renderSprite(GxT& gx, ActorT const& a, float cx, float cy, sw2::IntRect const& rcv, unsigned int color, float rot, float xscale, float yscale) const
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

  int frame = a.mCurrFrame;
  int tile = spr.mFrame[frame];
  int srcx = spr.mTileset.mTileWidth * (tile % spr.mTileset.mCxTile);
  int srcy = spr.mTileset.mTileHeight * (tile / spr.mTileset.mCxTile);

  int offsetx = rcm.left - rc.left;
  int offsety = rcm.top - rc.top;

  gx.setAnchor(a.mAnchorX, a.mAnchorY);
  gx.drawImage(rcm.left - rcv.left, rcm.top - rcv.top, img, srcx + offsetx, srcy + offsety, rcm.width(), rcm.height(), color, rot, xscale, yscale);
}

template<class GxT>
bool renderChilds(GxT& gx, ActorT const& a, sw2::IntRect const& rcv, float rot, float xscale, float yscale) const
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
      renderChilds(gx, child, rcv, arot, axscale, ayscale);
      continue;
    }

    float cx, cy;
    child.getPos(cx, cy);

    float childAlpha = alpha * (((child.mBgColor >> 24) & 0xff) / (float)0xff);
    unsigned int childBgColor = (child.mBgColor & 0xffffff) | (((unsigned int)(childAlpha * 0xff) & 0xff) << 24);

    switch (child.getType())
    {
    case ActorT::TYPES_COLBG:           // Solid color background.
      renderColorBg(gx, child, cx, cy, rcv, childBgColor, arot, axscale, ayscale);
      break;

    case ActorT::TYPES_MAPBG:           // Tilemap background.
      renderMapBg(gx, child, cx, cy, rcv, childBgColor);
      break;

    case ActorT::TYPES_TEXBG:           // Texture background.
      renderTexBg(gx, child, cx - rcv.left, cy - rcv.top, rcv, childBgColor, arot, axscale, ayscale);
      break;

    case ActorT::TYPES_SPRITE:          // Object instance.
      renderSprite(gx, child, cx, cy, rcv, childBgColor, arot, axscale, ayscale);
      break;
    }

    renderChilds(gx, child, rcv, arot, axscale, ayscale);
  }

  return true;
}

template<class GxT>
bool render(GxT& gx) const
{
  if (!mActors.isUsed(mRoot)) {
    return false;
  }

  ActorT const& a = mActors[mRoot];
  if (ActorT::TYPES_LEVEL != a.mResType) {
    return false;
  }

  if (!a.mOwnerDraw) {
    gx.setAnchor(a.mAnchorX, a.mAnchorY);
    gx.fillSolidColor(0, 0, mRes.mWidth, mRes.mHeight, a.mBgColor, .0f, 1.0f, 1.0f);
  } else {
    a.OnDraw();
  }

  sw2::IntRect rcv(0, 0, mRes.mWidth, mRes.mHeight);
  rcv.offset((int)a.mPosX, (int)a.mPosY);

  return renderChilds(gx, a, rcv, a.mRot, a.mXscale, a.mYscale);
}

template<class GxT>
void renderAll(GxT& gx) const
{
  mDirty = false;
  mRenderState = true;
  if (mTexDirty) {
    gx.restoreSur();
    mTexDirty = false;
  }
  gx.beginDraw(mRes.mWidth, mRes.mHeight);
  render(gx);                           // render may caused dirty flag set, so it will trigger renderAll process next time.
  ((T*)this)->onRender();
  gx.endDraw();
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
    return ImgT(0);
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
  }

  char chrmap[128];
  sprintf(chrmap, "chrmap;%d;%d;%d", size, ch, mAntiAlias);

  if (ImgT::existImage(chrmap)) {
    return ImgT::getImage(chrmap);
  }

  ImgT img = ImgT::getImage(chrmap, size, ch, mAntiAlias);
  if (img.isValid()) {
    mDirty = true;
  }

  return img;
}

// end of render.h
