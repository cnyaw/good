
//
// tex.h
// Texture.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/02/25 Waync created.
//

#pragma once

namespace good {

class Texture : public Base
{
public:

  //
  // Resource file name(tileset), relative to resource project file.
  //

  std::string mFileName;

  //
  // Name property.
  //

  std::string getName() const
  {
    return good::getName(*this, "tex");
  }

  //
  // Load texture.
  //

  template<class PoolT>
  bool load(PoolT& p, sw2::Ini const& ini, int id)
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(id, "tex");

    if (!ini.find(secName)) {
      return false;
    }

    sw2::Ini sec = ini[secName];

    //
    // Property.
    //

    if (!Base::load(p, sec, id)) {
      return false;
    }

    mFileName = sec["fileName"].value;

    return true;
  }
};

} // namespace good

// end of tex.h
