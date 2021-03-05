
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
  std::stringstream ss(s);
  v.assign(std::istream_iterator<T>(ss), std::istream_iterator<T>());
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

bool loadFileBinaryContent(const char *pFileName, std::string &outs)
{
  FILE *f = fopen(pFileName, "rb");
  if (!f) {
    return false;
  }
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);
  outs.resize(len);
  fread((void*)outs.data(), 1, len, f);
  fclose(f);
  return true;
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

std::string getPathName(std::string const& pathname)
{
  std::string name(pathname);
  std::replace(name.begin(), name.end(), '\\', '/');

  std::string path = name.substr(0, name.find_last_of('/') + 1);
  if (path.empty()) {
    path = pathname;
  }

  return path;
}

template<class GxT, class MapT, class ImgT>
void CommonDrawMap(GxT& gx, MapT const& map, ImgT& img, int dx, int dy, int left, int top, int right, int bottom, unsigned int color)
{
  for (int y = top; y <= bottom; y++) {
    for (int x = left; x <= right; x++) {
      int tile = map.mData[x + map.mWidth * y] - 1; // Tile 0 defines as empty tile.
      if (0 > tile) {
        continue;
      }

      int dstx = map.mTileset.mTileWidth * (x - left) +
                 map.mTileset.mTileWidth - map.mTileset.mTileWidth;
      int dsty = map.mTileset.mTileHeight * (y - top);
      int srcx = map.mTileset.mTileWidth * (tile % map.mTileset.mCxTile);
      int srcy = map.mTileset.mTileHeight * (tile / map.mTileset.mCxTile);

      gx.drawImage(
            dx + dstx, dy + dsty,
            img,
            srcx, srcy,
            map.mTileset.mTileWidth, map.mTileset.mTileHeight,
            color,
            .0f,
            1.0f, 1.0f);
    }
  }
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

} // namespace good

#ifdef WIN32
void ShellOpen(const char *path)
{
  SHELLEXECUTEINFOA si = {sizeof(SHELLEXECUTEINFOA)};
  si.lpVerb = "open";
  si.lpFile = path;
  si.nShow = SW_SHOWNORMAL;
  ShellExecuteExA(&si);
}
#endif

// end of util.h
