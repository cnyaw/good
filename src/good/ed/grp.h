
//
// grp.h
// Editor, group.
//
// Copyright (c) 2019 Waync Cheng.
// All Rights Reserved.
//
// 2019/8/22 Waync created.
//

#pragma once

namespace good {

namespace ed {

class Group : public good::Base
{
public:
  std::vector<int> mObjId;

  template<class PoolT>
  bool load(PoolT& p, sw2::Ini& ini, int id)
  {
    std::string secName = good::getSecName(id, "group");
    sw2::Ini& sec = ini[secName];

    if (!Base::load(p, sec, id)) {
      return false;
    }

    std::stringstream ss(sec["objects"].value);
    mObjId.assign(std::istream_iterator<int>(ss), std::istream_iterator<int>());

    return true;
  }

  bool store(sw2::Ini& ini) const
  {
    std::string secName = good::getSecName(mId, "group");

    sw2::Ini& sec = ini[secName];       // Insert.

    if (!mName.empty()) {
      sec["name"] = mName;
    }

    if (!mObjId.empty()) {
      sec["objects"] = intVecToStr(mObjId);
    }

    return true;
  }
};

} // namespace ed

} // namespace good

// end of grp.h
