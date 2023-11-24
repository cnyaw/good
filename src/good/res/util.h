
//
// util.h
// Good utility.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/01/25 Waync created.
//

#pragma once

namespace good {

template<class T>
void assignListFromString(const std::string &s, std::vector<T> &v)
{
  sw2::Util::split(s, v);
}

template<class PoolT, class VecT, class MapT>
bool loadResources(PoolT& p, MapT& t, VecT& v, std::string const& name, sw2::Ini& sec, sw2::Ini const& ini)
{
  assignListFromString(sec[name].value, v);
  t.clear();

  for (size_t i = 0; i < v.size(); ++ i) {
    if (!t[v[i]].load(p, ini, v[i])) {
      return false;
    }
  }

  return true;
}

template<class PoolT, class VecT, class MapT>
bool loadStringTableResources(PoolT& p, MapT& t, VecT& v, std::string const& name, sw2::Ini const& ini)
{
  v.clear();
  t.clear();

  if (0 == ini.find(name)) {
    return true;
  }

  sw2::Ini const& sec = ini[name];
  for (size_t i = 0; i < sec.items.size(); ++i) {
    std::stringstream ss(sec.items[i].key);
    int id;
    ss >> id;
    if (id != p.alloc(id)) {
      return false;
    }
    v.push_back(id);
    t[id] = sec.items[i].value;
  }

  return true;
}

std::string getSecName(int id, char const* tag)
{
  char buff[64];
  sprintf(buff, "%s%d", tag, id);
  return std::string(buff);
}

template<class T>
std::string getName(T const& t, std::string const& tag)
{
  if (!t.mName.empty()) {
    return t.mName;
  } else {
    return getSecName(t.mId, tag.c_str());
  }
}

int loadRGB(std::string const& rgb)
{
  std::vector<int> v;
  assignListFromString(rgb, v);
  v.push_back(255);                     // Default value, if no keyColor assign.
  v.push_back(255);
  v.push_back(255);
  return (v[0] & 0xff) | ((v[1] & 0xff) << 8) | ((v[2] & 0xff) << 16);
}

std::string getFileName(const std::string &path)
{
  std::string name(path);
  std::replace(name.begin(), name.end(), '\\', '/');
  std::string::size_type pos = name.find_last_of('/');
  if (std::string::npos != pos) {
    return name.substr(pos + 1);
  } else {
    return name;
  }
}

std::string getPathName(const std::string &pathname)
{
  std::string name(pathname);
  std::replace(name.begin(), name.end(), '\\', '/');
  std::string path = name.substr(0, name.find_last_of('/') + 1);
  return path;
}

std::string compactPath(const std::string &orgPath)
{
  std::string path(orgPath);
  size_t pos;

  //
  // remove ../
  //

  while (true) {
    pos = path.find("../");
    if (path.npos == pos) {
      break;
    }
    std::string r = path.substr(pos + 3);
    path = path.substr(0, path.find_last_of('/', pos - 2) + 1) + r;
  }

  //
  // remove ./
  //

  while (true) {
    pos = path.find("./");
    if (path.npos == pos) {
      break;
    }
    std::string r = path.substr(pos + 2);
    path = path.substr(0, pos) + r;
  }

  return path;
}

template<class GxT, class MapT, class ImgT>
void CommonDrawMap(GxT& gx, MapT const& map, ImgT& img, int dx, int dy, int left, int top, int right, int bottom, unsigned int color)
{
  for (int y = top; y <= bottom; y++) {
    for (int x = left; x <= right; x++) {
      int tile = map.mData[map.getTilemapIndexFromXy(x, y)] - 1; // Tile 0 defines as empty tile.
      if (0 > tile) {
        continue;
      }
      int dstx = map.mTileset.mTileWidth * (x - left) + map.mTileset.mTileWidth - map.mTileset.mTileWidth;
      int dsty = map.mTileset.mTileHeight * (y - top);
      int srcx = map.mTileset.mTileWidth * (tile % map.mTileset.mCxTile);
      int srcy = map.mTileset.mTileHeight * (tile / map.mTileset.mCxTile);
      gx.drawImage(dx + dstx, dy + dsty, img, srcx, srcy, map.mTileset.mTileWidth, map.mTileset.mTileHeight, color);
    }
  }
}

template<class GxT, class MapT, class ImgT>
void CommonDrawMap(GxT &gx, const MapT &map, ImgT &img, int cx, int cy, const sw2::IntRect &rcv, sw2::IntRect &rcm, sw2::IntRect &rc, unsigned int color)
{
  sw2::IntPoint adj0(cx % map.mTileset.mTileWidth, cy % map.mTileset.mTileHeight);
  sw2::IntPoint adj(adj0.x + (map.mTileset.mTileWidth * (rcm.left / map.mTileset.mTileWidth)), adj0.y + (map.mTileset.mTileHeight * (rcm.top / map.mTileset.mTileHeight)));
  rcm.offset(-rcm.left + adj.x, -rcm.top + adj.y);

  int left = (int)((rcm.left - rc.left) / map.mTileset.mTileWidth);
  int right = (std::min)(map.mWidth - 1, (int)((rcm.right - rc.left) / map.mTileset.mTileWidth));
  int top = (int)((rcm.top - rc.top) / map.mTileset.mTileHeight);
  int bottom = (std::min)(map.mHeight - 1, (int)((rcm.bottom - rc.top) / map.mTileset.mTileHeight));

  if (0 < left) {                       // Draw an extra tile to avoid tear.
    left -= 1;
    rcm.left -= map.mTileset.mTileWidth;
  }

  if (0 < top) {
    top -= 1;
    rcm.top -= map.mTileset.mTileHeight;
  }

  CommonDrawMap(gx, map, img, rcm.left - rcv.left, rcm.top - rcv.top, left, top, right, bottom, color);
}

template<class SpriteT>
void CalcDrawSpriteTexOffset(const SpriteT &spr, int frame, int &srcx, int &srcy)
{
  int tile = spr.mFrame[frame];
  srcx = spr.mTileset.mTileWidth * (tile % spr.mTileset.mCxTile);
  srcy = spr.mTileset.mTileHeight * (tile / spr.mTileset.mCxTile);
}

template<class GxT, class SpriteT, class ImgT>
void CommonDrawSprite(GxT &gx, const SpriteT &spr, ImgT &img, int x, int y, int frame, unsigned int color = 0xffffffff, float rot = .0f, float xscale = 1.0f, float yscale = 1.0f)
{
  int srcx, srcy;
  CalcDrawSpriteTexOffset(spr, frame, srcx, srcy);
  gx.drawImage(x, y, img, srcx, srcy, spr.mTileset.mTileWidth, spr.mTileset.mTileHeight, color, rot, xscale, yscale);
}

unsigned int converBgColor(unsigned int c)
{
  return 0xff000000 | ((c & 0xff) << 16) | (((c >> 8) & 0xff) << 8) | ((c >> 16) & 0xff);
}

bool isGoodArchive(std::string const& name)
{
  return std::string::npos != name.find(".good") ||
          std::string::npos != name.find(".zip");
}

template<class PrjT, class LevelT, class ObjectT>
void getObjDim(const LevelT &lvl, const ObjectT &obj, sw2::IntRect &rc)
{
  PrjT::inst().getObjDim(obj, rc);
  if (rc.empty()) {
    rc = sw2::IntRect(0, 0, 32, 32);
    rc.offset(obj.mPosX, obj.mPosY);
  }
  int idParent = lvl.getParent(obj.mId);
  while (lvl.mId != idParent) {
    const ObjectT &objParent = lvl.getObj(idParent);
    rc.offset(objParent.mPosX, objParent.mPosY);
    idParent = lvl.getParent(idParent);
  }
}

void CalcDrawTileParam(bool repx, bool repy, int x, int y, int cx, int cy, sw2::IntRect const& rcbound, int& nx, int& ny, int& xbound, int& ybound)
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

} // namespace good

// end of util.h
