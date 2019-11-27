
//
// obj.h
// Good level object instance.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/08/20 Waync created.
//

#pragma once

namespace good {

class Object : public Base
{
public:
  enum TYPE {
    TYPE_SPRITE,
    TYPE_COLBG,
    TYPE_TEXBG,
    TYPE_MAPBG,
    TYPE_DUMMY,
    TYPE_LVLOBJ,                        // External child obj of a lvl.
  };

  //
  // Type of the object.
  // Decision prioty: sprite -> texture -> map -> color -> dummy.
  //

  int mType;

  //
  // Is visible.
  //

  bool mVisible;

  //
  // Position.
  //

  int mPosX, mPosY;

  //
  // Script name.
  //

  std::string mScript;

  //
  // Dimension, x y w h.
  //

  sw2::IntRect mDim;

  //
  // Sprite ID.
  //

  int mSpriteId;

  //
  // Texture ID.
  //

  int mTextureId;

  //
  // Map ID.
  //

  int mMapId;

  //
  // Solid color.
  //

  unsigned int mBgColor;

  //
  // Repeat property, only valid for tex & map bkgnd.
  //

  bool mRepX, mRepY;

  //
  // Rotation.
  //

  float mRot;

  //
  // Scale.
  //

  float mScaleX, mScaleY;

  //
  // Anchor point.
  //

  float mAnchorX, mAnchorY;

  //
  // Child obj id.
  //

  std::vector<int> mObjIdx;

  //
  // Property.
  //

  int getLevelObjId() const
  {
    if (TYPE_LVLOBJ == mType) {
      return mSpriteId;
    } else {
      return 0;
    }
  }

  std::string getName() const
  {
    switch (mType)
    {
    case TYPE_SPRITE:
      return good::getName(*this, "sprite");

    case TYPE_COLBG:
      return good::getName(*this, "color");

    case TYPE_TEXBG:
      return good::getName(*this, "tex");

    case TYPE_MAPBG:
      return good::getName(*this, "map");

    case TYPE_DUMMY:
      return good::getName(*this, "dummy");

    case TYPE_LVLOBJ:
      return good::getName(*this, "lvobj");

    default:
      return good::getName(*this, "object");
    }
  }

  //
  // Load object instance.
  //

  template<class PoolT, class ObjectT>
  bool load_i(PoolT& p, sw2::Ini const& ini, sw2::Ini& sec, int id, std::map<int, ObjectT>& Objs)
  {
    if (!Base::load(p, sec, id)) {
      return false;
    }

    if (sec.find("visible")) {
      mVisible = sec["visible"];
    } else {
      mVisible = true;
    }

    mPosX = sec["x"];
    mPosY = sec["y"];
    mScript = sec["script"].value;
    mBgColor = loadRGB(sec["bgColor"].value);

    mType = TYPE_DUMMY;
    mSpriteId = mTextureId = mMapId = -1;

    if (!sec["sprite"].value.empty()) {
      mType = TYPE_SPRITE;
      mSpriteId = sec["sprite"];
    }

    if (!sec["texture"].value.empty()) {
      mType = TYPE_TEXBG;
      mTextureId = sec["texture"];
    }

    if (!sec["map"].value.empty()) {
      mType = TYPE_MAPBG;
      mMapId = sec["map"];
    }

    if (0xff == mSpriteId && 0xff == mTextureId && 0xff == mMapId) {
      mType = TYPE_DUMMY;
    } else if (0xfe == mTextureId && 0xfe == mMapId) {
      mType = TYPE_LVLOBJ;              // mSpriteId as lvl object id.
    } else if (!sec["bgColor"].value.empty() && TYPE_DUMMY == mType) {
      mType = TYPE_COLBG;
    }

    mRepX = sec["repX"];
    mRepY = sec["repY"];

    mRot = sec["rot"];

    mScaleX = sec["xscale"];
    mScaleY = sec["yscale"];

    if (.0f == mScaleX) {
      mScaleX = 1.0f;
    }

    if (.0f == mScaleY) {
      mScaleY = 1.0f;
    }

    mAnchorX = sec["xpivot"];
    mAnchorY = sec["ypivot"];

    {
      std::stringstream ss(sec["dim"].value);
      std::vector<int> v;
      v.assign(std::istream_iterator<int>(ss), std::istream_iterator<int>());

      v.push_back(0);                   // Default value, if no assign.
      v.push_back(0);
      v.push_back(0);
      v.push_back(0);

      mDim.left = v[0];
      mDim.top = v[1];
      mDim.right = v[0] + v[2];
      mDim.bottom = v[1] + v[3];
    }

    {
      std::stringstream ss(sec["objects"].value);
      mObjIdx.assign(std::istream_iterator<int>(ss), std::istream_iterator<int>());

      for (size_t i = 0; i < mObjIdx.size(); ++ i) {
        if (!Objs[mObjIdx[i]].load(p, ini, mObjIdx[i], Objs)) {
          return false;
        }
      }
    }

    return true;
  }

  template<class PoolT, class ObjectT>
  bool load(PoolT& p, sw2::Ini const& ini, int id, std::map<int, ObjectT>& Objs)
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(id, "object");

    if (!ini.find(secName)) {
      return false;
    }

    sw2::Ini sec = ini[secName];

    //
    // Property.
    //

    return load_i(p, ini, sec, id, Objs);
  }
};

} // namespace good

// end of obj.h
