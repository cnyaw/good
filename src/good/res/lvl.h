
//
// lvl.h
// Good level.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/01/25 Waync created.
//

#pragma once

namespace good {

template<class ObjectT>
class Level : public ObjectT
{
public:

  //
  // Level size, in pixel.
  //

  int mWidth, mHeight;

  //
  // Object instance.
  //

  std::map<int, ObjectT> mObj;

  //
  // Name property.
  //

  std::string getName() const
  {
    return good::getName(*this, "level");
  }

  //
  // Get object instance.
  //

  bool isObj(int id) const
  {
    typename std::map<int, ObjectT>::const_iterator it = mObj.find(id);
    if (mObj.end() == it) {
      return false;
    } else {
      return true;
    }
  }

  ObjectT& getObj(int id)
  {
    return const_cast<ObjectT&>(static_cast<Level const &>(*this).getObj(id));
  }

  ObjectT const& getObj(int id) const
  {
    typename std::map<int, ObjectT>::const_iterator it = mObj.find(id);
    if (mObj.end() == it) {
      throw std::range_error("getObj");
    } else {
      return it->second;
    }
  }

  //
  // Load level.
  //

  void getWndSize(sw2::Ini const& ini, int &w, int &h) const
  {
    w = 640;
    h = 480;

    sw2::Ini sec = ini["good"];

    std::vector<int> v;
    {
      std::stringstream ss(sec["window"].value);
      v.assign(std::istream_iterator<int>(ss), std::istream_iterator<int>());
    }

    if (1 <= v.size()) {
      w = v[0];
    }

    if (2 <= v.size()) {
      h = v[1];
    }
  }

  template<class PoolT>
  bool load(PoolT& p, sw2::Ini const& ini, int id)
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(id, "level");

    if (!ini.find(secName)) {
      return false;
    }

    sw2::Ini sec = ini[secName];

    //
    // Property.
    //

    if (!ObjectT::load_i(p, ini, sec, id, mObj)) {
      return false;
    }

    ObjectT::mType = ObjectT::mSpriteId = ObjectT::mTextureId = ObjectT::mMapId = -1;

    int pw, ph;
    getWndSize(ini, pw, ph);

    if (!sec.find("width")) {
      mWidth = pw;
    } else {
      mWidth = sec["width"];
    }
    if (!sec.find("height")) {
      mHeight = ph;
    } else {
      mHeight = sec["height"];
    }

    return true;
  }
};

} // namespace good

// end of lvl.h
