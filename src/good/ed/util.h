
//
// util.h
// Editor, util.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/12/08 Waync created.
//

#pragma once

namespace good {

namespace ed {

struct GridLine
{
  //
  // Grid line range(in x or y depends on line type).
  //

  int range;

  //
  // Line color.
  //

  unsigned int color;

  bool operator==(GridLine const& gl) const
  {
    return gl.range == range && gl.color == color;
  }
};

std::string intVecToStr(std::vector<int> const &v)
{
  std::stringstream ss;
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(ss, " "));
  return sw2::Util::trim(ss.str());
}

bool loadGrid(sw2::Ini& sec, std::string const& sgrid, int range, std::vector<GridLine>& lines)
{
  std::vector<int> v;
  assignListFromString(sec[sgrid].value, v);

  for (size_t i = 0; i < v.size(); ) {

    if (i + 3 >= v.size()) {
      break;
    }

    GridLine gl;
    gl.range = (std::max)(0, (std::min)(range / 2, v[i]));
    gl.color = (v[i + 1] & 0xff) |
               ((v[i + 2] & 0xff) << 8) |
               ((v[i + 3] & 0xff) << 16);

    if (0 < gl.range && range / 2 > gl.range) {
      lines.push_back(gl);
    }

    i += 4;
  }

  return true;
}

bool storeGrid(sw2::Ini& ini, std::string const& secName, std::string const& sgrid, std::vector<GridLine> const& lines)
{
  std::vector<int> v;
  for (size_t i = 0; i < lines.size(); i++) {
    GridLine const& gl = lines[i];
    v.push_back(gl.range);
    v.push_back(gl.color & 0xff);       // R.
    v.push_back((gl.color >> 8) & 0xff); // G.
    v.push_back((gl.color >> 16) & 0xff); // B.
  }

  if (v.empty()) {
    return true;
  }

  sw2::Ini& sec = ini[secName];
  sec[sgrid] = intVecToStr(v);

  return true;
}

template<class VecT, class MapT>
bool storeResources(MapT const& t, VecT const& v, std::string const& name, sw2::Ini& sec, sw2::Ini& ini)
{
  if (!v.empty()) {
    sec[name] = intVecToStr(v);
    for (size_t i = 0; i < v.size(); ++ i) {
      typename MapT::const_iterator it = t.find(v[i]);
      if (!it->second.store(ini)) {
        return false;
      }
    }
  }

  return true;
}

template<class VecT, class MapT>
bool storeStringTableResource(MapT const& t, VecT const& v, std::string const& name, sw2::Ini& ini)
{
  if (v.empty()) {
    return true;
  }

  std::stringstream ss;

  sw2::Ini& sec = ini[name];

  for (size_t i = 0; i < v.size(); ++i) {
    ss.str("");
    ss << v[i];
    sec[ss.str()] = t.find(v[i])->second; // Insert back.
  }

  return true;
}

std::string storeRGB(unsigned int clr)
{
  std::vector<int> v;
  v.push_back(clr & 0xff);              // R.
  v.push_back((clr >> 8) & 0xff);       // G.
  v.push_back((clr >> 16) & 0xff);      // B.

  return intVecToStr(v);
}

} // namespace ed

} // namespace good

// end of util.h
