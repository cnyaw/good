
//
// obj.h
// Editor, good level object instance.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/08/20 Waync created.
//

#pragma once

namespace good {

namespace ed {

template<class PrjT>
class Object : public good::Object
{
public:

  //
  // Release undo command resource.
  //

  void clear()
  {
  }

  //
  // Zorder.
  //

  int getObjIndex(int id) const
  {
    for (size_t i = 0; i < mObjIdx.size(); ++i) {
      if (mObjIdx[i] == id) {
        return i;
      }
    }
    return -1;
  }

  //
  // Modify property.
  //

  bool setBgColor(unsigned int newColor)
  {
    if (mBgColor == newColor) {
      return false;
    }

    mBgColor = newColor;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setDim(int newx, int newy, int neww, int newh)
  {
    sw2::IntRect dim(newx, newy, newx + neww, newy + newh);

    if (dim == mDim) {
      return false;
    }

    mDim = dim;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setPos(int newPosX, int newPosY)
  {
    if (newPosX == mPosX && mPosY == newPosY) {
      return false;
    }

    mPosX = newPosX;
    mPosY = newPosY;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setRot(float rot)
  {
    if (rot == mRot) {
      return false;
    }

    mRot = rot;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setRepeat(bool repx, bool repy)
  {
    if (repx == mRepX && repy == mRepY) {
      return false;
    }

    mRepX = repx;
    mRepY = repy;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setScale(float xscale, float yscale)
  {
    if (xscale == mScaleX && yscale == mScaleY) {
      return false;
    }

    mScaleX = xscale;
    mScaleY = yscale;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setAnchor(float xpivot, float ypivot)
  {
    if (xpivot == mAnchorX && ypivot == mAnchorY) {
      return false;
    }

    mAnchorX = xpivot;
    mAnchorY = ypivot;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setScript(std::string const& script)
  {
    if (script == mScript) {
      return false;
    }

    mScript = script;

    PrjT::inst().mModified = true;

    return true;
  }

  bool setVisible(bool visible)
  {
    if (visible == mVisible) {
      return false;
    }

    mVisible = visible;

    PrjT::inst().mModified = true;

    return true;
  }

  //
  // Store level object instance.
  //

  bool store_i(sw2::Ini& ini, const std::string& secName, const std::map<int, Object>& Objs) const
  {
    //
    // Property.
    //

    sw2::Ini sec;
    sec.key = secName;

    if (!mName.empty()) {
      sec["name"] = mName;
    }

    if (0xffffff != (mBgColor & 0xffffff)) {
      sec["bgColor"] = storeRGB(mBgColor);
    }

    switch (mType)
    {
    case TYPE_SPRITE:
      sec["sprite"] = mSpriteId;
      break;

    case TYPE_TEXBG:
      sec["texture"] = mTextureId;
      break;

    case TYPE_MAPBG:
      sec["map"] = mMapId;
      break;

    case TYPE_DUMMY:
      sec["sprite"] = 0xff;
      sec["texture"] = 0xff;
      sec["map"] = 0xff;
      break;

    case TYPE_LVLOBJ:
      sec["sprite"] = getLevelObjId();
      sec["texture"] = 0xfe;
      sec["map"] = 0xfe;
      break;

    default:

      //
      // Store nothing.
      //

      break;
    }

    if (TYPE_TEXBG == mType || TYPE_MAPBG == mType) {
      if (mRepX) {
        sec["repX"] = mRepX;
      }
      if (mRepY) {
        sec["repY"] = mRepY;
      }
    }

    if (.0f != mRot) {
      sec["rot"] = mRot;
    }

    if (1.0f != mScaleX) {
      sec["xscale"] = mScaleX;
    }

    if (1.0f != mScaleY) {
      sec["yscale"] = mScaleY;
    }

    if (.0f != mAnchorX) {
      sec["xpivot"] = mAnchorX;
    }

    if (.0f != mAnchorY) {
      sec["ypivot"] = mAnchorY;
    }

    if (!mVisible) {
      sec["visible"] = mVisible;
    }

    if (0 != mPosX) {
      sec["x"] = mPosX;
    }

    if (0 != mPosY) {
      sec["y"] = mPosY;
    }

    if (!mScript.empty()) {
      sec["script"] = mScript;
    }

    if (!mDim.empty()) {
      std::vector<int> v;
      v.push_back(mDim.left);
      v.push_back(mDim.top);
      v.push_back(mDim.width());
      v.push_back(mDim.height());
      sec["dim"] = intVecToStr(v);
    }

    if (!mObjIdx.empty()) {
      sec["objects"] = intVecToStr(mObjIdx);
    }

    ini[secName] = sec;                 // Insert new section to ini.

    if (!mObjIdx.empty()) {
      for (size_t i = 0; i < mObjIdx.size(); ++ i) {
        std::map<int, Object>::const_iterator it = Objs.find(mObjIdx[i]);
        if (Objs.end() == it || !it->second.store(ini, Objs)) {
          return false;
        }
      }
    }

    return true;
  }

  bool store(sw2::Ini& ini, const std::map<int, Object>& Objs) const
  {
    //
    // Settings.
    //

    std::string secName = good::getSecName(mId, "object");

    return store_i(ini, secName, Objs);
  }
};

} // namespace ed

} // namespace good

// end of obj.h
