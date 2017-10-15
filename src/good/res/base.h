
//
// base.h
// Base class.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/03/25 Waync created.
//

#pragma once

namespace good {

class Base
{
public:

  //
  // ID.
  //

  int mId;

  //
  // Name(optional).
  //

  std::string mName;

  //
  // Load texture.
  //

  template<class PoolT>
  bool load(PoolT& p, sw2::Ini& sec, int id)
  {
    mId = p.alloc(id);
    if (mId != id) {
      return false;
    }

    mName = sec["name"].value;

    return true;
  }
};

} // namespace good

// end of base.h
