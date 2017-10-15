
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
