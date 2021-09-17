
//
// Property.h
// Good resource property interface/implementation.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/07/26 Waync created.
//

#pragma once

//
// Interface.
//

class IResourceProperty
{
public:

  int mId;                              // Res ID, editor ID.
  std::map<std::string, HPROPERTY> propmap;

  virtual ~IResourceProperty()
  {
  }

  //
  // Overrideables.
  //

  virtual int GetType() const           // Property item type.
  {
    return -1;
  }

  virtual int GetTreeId() const         // Item resource id in the resource tree.
  {
    return mId;
  }

  virtual std::string GetName() const   // Display name in the resource tree or editor tab page.
  {
    return "";
  }

  virtual void FillProperty(CPropertyListCtrl& prop) // Fill all propertys into the list of the resource item.
  {
  }

  virtual void UpdateProperty(CPropertyListCtrl& prop) // Update changes of the resource item.
  {
  }

  virtual bool SetColor(unsigned int newColor)
  {
    return false;
  }

  virtual bool SetSprite(int newSprite)
  {
    return false;
  }

  virtual bool SetTexture(int newTexture)
  {
    return false;
  }

  virtual LRESULT OnPropChanging(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    //
    // Before property item change, get a chance to avoid change(allow change
    // return 0).
    //

    return 1;
  }

  virtual void OnPropChanged(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi) // After property item changed.
  {
  }

  //
  // Util.
  //

  bool GetChangedProp(LPNMPROPERTYITEM ppi, LPCTSTR name, int& i) const
  {
    if (::lstrcmp(name, ppi->prop->GetName())) {
      return false;
    }

    VARIANT var = {VT_INT};
    ppi->prop->GetValue(&var);
    i = var.intVal;

    return true;
  }

  bool GetChangedProp(LPNMPROPERTYITEM ppi, LPCTSTR name, float& f) const
  {
    if (::lstrcmp(name, ppi->prop->GetName())) {
      return false;
    }

    TCHAR str[256];
    if (!ppi->prop->GetDisplayValue(str, sizeof(str)/sizeof(TCHAR))) {
      return false;
    }

    std::string s = str;

    f = (float)::atof(s.c_str());

    return true;
  }

  bool GetChangedProp(LPNMPROPERTYITEM ppi, LPCTSTR name, bool& b) const
  {
    if (::lstrcmp(name, ppi->prop->GetName())) {
      return false;
    }

    VARIANT var = {VT_BOOL};
    ppi->prop->GetValue(&var);
    b = 0 != var.boolVal;

    return true;
  }

  bool GetChangedProp(LPNMPROPERTYITEM ppi, LPCTSTR name, std::string& s) const
  {
    if (::lstrcmp(name, ppi->prop->GetName())) {
      return false;
    }

    TCHAR str[256];
    if (!ppi->prop->GetDisplayValue(str, sizeof(str)/sizeof(TCHAR))) {
      return false;
    }

    s = str;

    return true;
  }

  bool GetChangingProp(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi, LPCTSTR name, int& i) const
  {
    if (::lstrcmp(name, ppi->prop->GetName())) {
      return false;
    }

    TCHAR str[256];
    str[::GetWindowText(prop.m_hwndInplace, str, 256)] = TCHAR('\0');
    i = ::atoi(str);

    return true;
  }

  bool GetChangingProp(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi, LPCTSTR name, float& f) const
  {
    if (::lstrcmp(name, ppi->prop->GetName())) {
      return false;
    }

    TCHAR str[256];
    str[::GetWindowText(prop.m_hwndInplace, str, 256)] = TCHAR('\0');
    f = (float)::atof(str);

    return true;
  }

  bool GetChangingProp(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi, LPCTSTR name, std::string& s) const
  {
    if (::lstrcmp(name, ppi->prop->GetName())) {
      return false;
    }

    TCHAR str[256];
    str[::GetWindowText(prop.m_hwndInplace, str, 256)] = TCHAR('\0');
    s = str;

    return true;
  }

  bool GetChangingProp(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi, LPCTSTR name, bool& b) const
  {
    if (::lstrcmp(name, ppi->prop->GetName())) {
      return false;
    }

    TCHAR str[256];
    str[::GetWindowText(prop.m_hwndInplace, str, 256)] = TCHAR('\0');
    b = 0 == ::lstrcmp(str, _T("True"));

    return true;
  }

  HPROPERTY AddProp(CPropertyListCtrl& prop, HPROPERTY p)
  {
    prop.AddItem(p);
    propmap[p->GetName()] = p;
    return p;
  }
};

HPROPERTY PropCreateColorPicker(LPCTSTR pstrName, COLORREF color, IResourceProperty* prop);

//
// Texture.
//

class CResTextureProperty : public IResourceProperty
{
  CResTextureProperty()
  {
  }

public:

  static CResTextureProperty& inst(int id)
  {
    static CResTextureProperty i;
    i.mId = id;
    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_TEXTURE;
  }

  virtual std::string GetName() const
  {
    return PrjT::inst().getTex(mId).getName();
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    PrjT::TextureT const& o = PrjT::inst().getTex(mId);

    AddProp(prop, PropCreateCategory("Texture"));
    AddProp(prop, PropCreateSimple("Id", o.mId))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Name", o.getName().c_str()));
    AddProp(prop, PropCreateSimple("File", o.mFileName.c_str()))->SetEnabled(FALSE);
  }

  virtual LRESULT OnPropChanging(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::TextureT const& o = PrjT::inst().getTex(mId);
    std::string s;

    if (GetChangingProp(prop, ppi, _T("Name"), s)) {
      return !(s != o.getName());
    }

    return 1;                           // NOT allow change
  }

  virtual void OnPropChanged(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::TextureT& o = PrjT::inst().getTex(mId);
    std::string s;

    if (GetChangedProp(ppi, _T("Name"), s)) {
      PrjT::inst().rename(o, s);
      if (s.empty()) {                  // Special case.
        ppi->prop->SetValue(CComVariant(o.getName().c_str()));
      }
    }
  }
};

//
// Map.
//

class CResMapProperty : public IResourceProperty
{
  CResMapProperty()
  {
  }

public:

  static CResMapProperty& inst(int id)
  {
    static CResMapProperty i;
    i.mId = id;
    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_MAP;
  }

  virtual std::string GetName() const
  {
    return PrjT::inst().getMap(mId).getName();
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    PrjT::MapT const& o = PrjT::inst().getMap(mId);
    std::string tex = PrjT::inst().getTex(o.mTileset.mTextureId).getName();

    AddProp(prop, PropCreateCategory("Map"));
    AddProp(prop, PropCreateSimple("Id", o.mId))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Name", o.getName().c_str()));
    AddProp(prop, PropCreateSimple("Width", o.mWidth))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Height", o.mHeight))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Tile Width", o.mTileset.mTileWidth))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Tile Height", o.mTileset.mTileHeight))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Texture", tex.c_str()))->SetEnabled(FALSE);
  }

  virtual LRESULT OnPropChanging(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::MapT const& o = PrjT::inst().getMap(mId);
    std::string s;

    if (GetChangingProp(prop, ppi, _T("Name"), s)) {
      return !(s != o.getName());
    }

    return 1;                           // NOT allow change.
  }

  virtual void OnPropChanged(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::MapT& o = PrjT::inst().getMap(mId);
    std::string s;

    if (GetChangedProp(ppi, _T("Name"), s)) {
      PrjT::inst().rename(o, s);
      if (s.empty()) {                  // Special case.
        ppi->prop->SetValue(CComVariant(o.getName().c_str()));
      }
    }
  }
};

//
// Sprite.
//

class CResSpriteProperty : public IResourceProperty
{
  CResSpriteProperty()
  {
  }

public:

  static CResSpriteProperty& inst(int id)
  {
    static CResSpriteProperty i;
    i.mId = id;
    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_SPRITE;
  }

  virtual std::string GetName() const
  {
    return PrjT::inst().getSprite(mId).getName();
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    PrjT const& prj = PrjT::inst();
    PrjT::SpriteT const& o = prj.getSprite(mId);
    std::string tex = prj.getTex(o.mTileset.mTextureId).getName();

    AddProp(prop, PropCreateCategory("Sprite"));
    AddProp(prop, PropCreateSimple("Id", o.mId))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Name", o.getName().c_str()));
    AddProp(prop, PropCreateSimple("Tile Width", o.mTileset.mTileWidth))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Tile Height", o.mTileset.mTileHeight))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Texture", tex.c_str()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Loop", o.mLoop));
    AddProp(prop, PropCreateSimple("OffsetX", o.mOffsetX));
    AddProp(prop, PropCreateSimple("OffsetY", o.mOffsetY));
  }

  virtual void UpdateProperty(CPropertyListCtrl& prop)
  {
    PrjT::SpriteT const& o = PrjT::inst().getSprite(mId);

    prop.SetItemValue(propmap["Loop"], &CComVariant((int)o.mLoop));
  }

  virtual LRESULT OnPropChanging(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::SpriteT const& o = PrjT::inst().getSprite(mId);
    std::string s;
    bool b; int i;

    if (GetChangingProp(prop, ppi, _T("Name"), s)) {
      return !(s != o.getName());
    } else if (GetChangingProp(prop, ppi, _T("Loop"), b)) {
      return !(b != o.mLoop);
    } else if (GetChangingProp(prop, ppi, _T("OffsetX"), i)) {
      return !(i != o.mOffsetX);
    } else if (GetChangingProp(prop, ppi, _T("OffsetY"), i)) {
      return !(i != o.mOffsetY);
    }

    return 1;                           // NOT allow change.
  }

  virtual void OnPropChanged(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::SpriteT& o = PrjT::inst().getSprite(mId);
    std::string s;
    bool b; int i;

    if (GetChangedProp(ppi, _T("Name"), s)) {
      PrjT::inst().rename(o, s);
      if (s.empty()) {                  // Special case.
        ppi->prop->SetValue(CComVariant(o.getName().c_str()));
      }
    } else if (GetChangedProp(ppi, _T("Loop"), b)) {
      o.setLoop(b);
    } else if (GetChangedProp(ppi, _T("OffsetX"), i)) {
      o.setOffset(i, o.mOffsetY);
    } else if (GetChangedProp(ppi, _T("OffsetY"), i)) {
      o.setOffset(o.mOffsetX, i);
    }
  }
};

//
// Level.
//

class CResLevelObjectProperty : public IResourceProperty
{
  CResLevelObjectProperty()
  {
  }

  char const* FloatToStr(float f) const
  {
    static char buff[64];
    sprintf(buff, "%g", f);
    return buff;
  }

  std::string GetLevelObjInfo(int id) const
  {
    std::string s;
    const PrjT &prj = PrjT::inst();
    for (size_t i = 0; i < prj.mRes.mLevelIdx.size(); ++i) {
      const PrjT::LevelT &lvl = prj.getLevel(prj.mRes.mLevelIdx[i]);
      if (GetLevelObjInfo(lvl, lvl.mObjIdx, id, s)) {
        return s;
      }
    }
    return s;
  }

  bool GetLevelObjInfo(const PrjT::LevelT &lvl, const std::vector<int> &v, int id, std::string &s) const
  {
    for (size_t i = 0; i < v.size(); i++) {
      int idObj = v[i];
      const PrjT::ObjectT &o = lvl.getObj(idObj);
      if (idObj == id) {
        char buff[256];
        sprintf(buff, "%s/%s(%d)", lvl.getName().c_str(), o.getName().c_str(), id);
        s = buff;
        return true;
      }
      if (GetLevelObjInfo(lvl, o.mObjIdx, id, s)) {
        return true;
      }
    }
    return false;
  }

public:

  int mObjId;

  static CResLevelObjectProperty& inst(int id)
  {
    static CResLevelObjectProperty i;
    i.mObjId = id;

    PrjT const& prj = PrjT::inst();
    for (std::map<int, PrjT::LevelT>::const_iterator it = prj.mRes.mLevel.begin();
         prj.mRes.mLevel.end() != it;
         ++it) {
      if (it->second.isObj(id)) {
        i.mId = it->first;
        break;
      }
    }

    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_LEVEL_OBJECT;
  }

  virtual int GetTreeId() const
  {
    return mObjId;
  }

  virtual bool SetColor(unsigned int newColor)
  {
    return PrjT::inst().getLevel(mId).setObjBgColor(mObjId, newColor);
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    PrjT const& prj = PrjT::inst();
    PrjT::ObjectT const& o = prj.getLevel(mId).getObj(mObjId);

    AddProp(prop, PropCreateCategory(("Level Object - " + o.getTypeStr()).c_str()));
    AddProp(prop, PropCreateSimple("Id", o.mId))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Name", o.getName().c_str()));

    if (PrjT::ObjectT::TYPE_MAPBG == o.mType) {
      std::string name = prj.getMap(o.mMapId).getName();
      AddProp(prop, PropCreateSimple("Map", name.c_str()))->SetEnabled(FALSE);
    } else if (PrjT::ObjectT::TYPE_TEXBG == o.mType) {
      std::string name = prj.getTex(o.mTextureId).getName();
      AddProp(prop, PropCreateSimple("Texture", name.c_str()))->SetEnabled(FALSE);
    } else if (PrjT::ObjectT::TYPE_SPRITE == o.mType) {
      std::string name = prj.getSprite(o.mSpriteId).getName();
      AddProp(prop, PropCreateSimple("Sprite", name.c_str()))->SetEnabled(FALSE);
    }

    AddProp(prop, PropCreateSimple("Visible", o.mVisible));
    AddProp(prop, PropCreateSimple("PosX", o.mPosX));
    AddProp(prop, PropCreateSimple("PosY", o.mPosY));

    if (PrjT::ObjectT::TYPE_LVLOBJ == o.mType) {
      std::string oi = GetLevelObjInfo(o.getLevelObjId());
      AddProp(prop, PropCreateSimple("Object", oi.c_str()))->SetEnabled(FALSE);
      return;
    } else if (PrjT::ObjectT::TYPE_TEXT == o.mType) {
      AddProp(prop, PropCreateSimple("Text", o.mText.c_str()));
      AddProp(prop, PropCreateSimple("TextSize", o.mTextSize));
    } else if (PrjT::ObjectT::TYPE_MAPBG == o.mType) {
      AddProp(prop, PropCreateSimple("RepeatX", o.mRepX));
      AddProp(prop, PropCreateSimple("RepeatY", o.mRepY));
    } else if (PrjT::ObjectT::TYPE_TEXBG == o.mType) {
      AddProp(prop, PropCreateSimple("OffsetX", o.mDim.left));
      AddProp(prop, PropCreateSimple("OffsetY", o.mDim.top));
      if (o.mDim.empty()) {
        ImgT img = LoadTileset(prj.getTex(o.mTextureId).mFileName);
        if (img.isValid()) {
          AddProp(prop, PropCreateSimple("Width", img.getWidth()));
          AddProp(prop, PropCreateSimple("Height", img.getHeight()));
        }
      } else {
        AddProp(prop, PropCreateSimple("Width", o.mDim.width()));
        AddProp(prop, PropCreateSimple("Height", o.mDim.height()));
      }
      AddProp(prop, PropCreateSimple("RepeatX", o.mRepX));
      AddProp(prop, PropCreateSimple("RepeatY", o.mRepY));
    } else if (PrjT::ObjectT::TYPE_COLBG == o.mType) {
      AddProp(prop, PropCreateSimple("Width", o.mDim.empty() ? 32 : o.mDim.width()));
      AddProp(prop, PropCreateSimple("Height", o.mDim.empty() ? 32 : o.mDim.height()));
    }

    if (PrjT::ObjectT::TYPE_MAPBG != o.mType && PrjT::ObjectT::TYPE_TEXT != o.mType) {
      AddProp(prop, PropCreateSimple("Rot", FloatToStr(o.mRot)));
      AddProp(prop, PropCreateSimple("ScaleX", FloatToStr(o.mScaleX)));
      AddProp(prop, PropCreateSimple("ScaleY", FloatToStr(o.mScaleY)));
      AddProp(prop, PropCreateSimple("AnchorX", FloatToStr(o.mAnchorX)));
      AddProp(prop, PropCreateSimple("AnchorY", FloatToStr(o.mAnchorY)));
    }
    AddProp(prop, PropCreateColorPicker("Color", o.mBgColor, this));
    AddProp(prop, PropCreateSimple("Script", o.mScript.c_str()));
  }

  virtual void UpdateProperty(CPropertyListCtrl& prop)
  {
    PrjT::ObjectT const& o = PrjT::inst().getLevel(mId).getObj(mObjId);

    prop.SetItemValue(propmap["Name"], &CComVariant(o.getName().c_str()));
    prop.SetItemValue(propmap["Visible"], &CComVariant((int)o.mVisible));
    prop.SetItemValue(propmap["PosX"], &CComVariant(o.mPosX));
    prop.SetItemValue(propmap["PosY"], &CComVariant(o.mPosY));

    if (PrjT::ObjectT::TYPE_LVLOBJ == o.mType) {
      return;
    } if (PrjT::ObjectT::TYPE_COLBG == o.mType) {
      prop.SetItemValue(propmap["Width"], &CComVariant(o.mDim.empty() ? 32 : o.mDim.width()));
      prop.SetItemValue(propmap["Height"], &CComVariant(o.mDim.empty() ? 32 : o.mDim.height()));
      prop.SetItemValue(propmap["Rot"], &CComVariant(o.mRot));
      prop.SetItemValue(propmap["ScaleX"], &CComVariant(o.mScaleX));
      prop.SetItemValue(propmap["ScaleY"], &CComVariant(o.mScaleY));
      prop.SetItemValue(propmap["AnchorX"], &CComVariant(o.mAnchorX));
      prop.SetItemValue(propmap["AnchorY"], &CComVariant(o.mAnchorY));
    } else {
      if (PrjT::ObjectT::TYPE_TEXBG == o.mType) {
        prop.SetItemValue(propmap["OffsetX"], &CComVariant(o.mDim.left));
        prop.SetItemValue(propmap["OffsetY"], &CComVariant(o.mDim.top));
        if (!o.mDim.empty()) {
          prop.SetItemValue(propmap["Width"], &CComVariant(o.mDim.width()));
          prop.SetItemValue(propmap["Height"], &CComVariant(o.mDim.height()));
        }
        prop.SetItemValue(propmap["RepeatX"], &CComVariant((int)o.mRepX));
        prop.SetItemValue(propmap["RepeatY"], &CComVariant((int)o.mRepY));
        prop.SetItemValue(propmap["Rot"], &CComVariant(o.mRot));
        prop.SetItemValue(propmap["ScaleX"], &CComVariant(o.mScaleX));
        prop.SetItemValue(propmap["ScaleY"], &CComVariant(o.mScaleY));
        prop.SetItemValue(propmap["AnchorX"], &CComVariant(o.mAnchorX));
        prop.SetItemValue(propmap["AnchorY"], &CComVariant(o.mAnchorY));
      } else if (PrjT::ObjectT::TYPE_SPRITE == o.mType) {
        prop.SetItemValue(propmap["Rot"], &CComVariant(o.mRot));
        prop.SetItemValue(propmap["ScaleX"], &CComVariant(o.mScaleX));
        prop.SetItemValue(propmap["ScaleY"], &CComVariant(o.mScaleY));
        prop.SetItemValue(propmap["AnchorX"], &CComVariant(o.mAnchorX));
        prop.SetItemValue(propmap["AnchorY"], &CComVariant(o.mAnchorY));
      } else if (PrjT::ObjectT::TYPE_TEXT == o.mType) {
        prop.SetItemValue(propmap["TextSize"], &CComVariant(o.mTextSize));
        prop.SetItemValue(propmap["Text"], &CComVariant(o.mText.c_str()));
      } else if (PrjT::ObjectT::TYPE_MAPBG == o.mType) {
        prop.SetItemValue(propmap["RepeatX"], &CComVariant((int)o.mRepX));
        prop.SetItemValue(propmap["RepeatY"], &CComVariant((int)o.mRepY));
      }
    }

    prop.SetItemValue(propmap["Color"], &CComVariant(o.mBgColor));
    prop.SetItemValue(propmap["Script"], &CComVariant(o.mScript.c_str()));
  }

  virtual LRESULT OnPropChanging(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::ObjectT& o = PrjT::inst().getLevel(mId).getObj(mObjId);
    std::string s; int i; bool b; float f;

    if (GetChangingProp(prop, ppi, _T("Name"), s)) {
      return !(s != o.getName());
    } else if (GetChangingProp(prop, ppi, _T("Visible"), b)) {
      return !(b != o.mVisible);
    } else if (GetChangingProp(prop, ppi, _T("PosX"), i) ||
               GetChangingProp(prop, ppi, _T("PosY"), i)) {
      return !(-2048 <= i && 2048 >= i);
    } else if (GetChangingProp(prop, ppi, _T("OffsetX"), i) ||
               GetChangingProp(prop, ppi, _T("OffsetY"), i)) {
      return !(0 <= i && 2048 >= i);
    } else if (GetChangingProp(prop, ppi, _T("Width"), i) ||
               GetChangingProp(prop, ppi, _T("Height"), i)) {
      return !(1 <= i && 2048 >= i);
    } else if (GetChangingProp(prop, ppi, _T("Rot"), f)) {
      return !(f != o.mRot);
    } else if (GetChangingProp(prop, ppi, _T("ScaleX"), f)) {
      return !(f != o.mScaleX);
    } else if (GetChangingProp(prop, ppi, _T("ScaleY"), f)) {
      return !(f != o.mScaleY);
    } else if (GetChangingProp(prop, ppi, _T("AnchorX"), f)) {
      return !(f != o.mAnchorX);
    } else if (GetChangingProp(prop, ppi, _T("AnchorY"), f)) {
      return !(f != o.mAnchorY);
    } else if (GetChangingProp(prop, ppi, _T("RepeatX"), b)) {
      return !(b != o.mRepX);
    } else if (GetChangingProp(prop, ppi, _T("RepeatY"), b)) {
      return !(b != o.mRepY);
    } else if (GetChangingProp(prop, ppi, _T("Script"), s)) {
      return !(s != o.mScript);
    } if (GetChangingProp(prop, ppi, _T("TextSize"), i)) {
      return !(i != o.mTextSize);
    } if (GetChangingProp(prop, ppi, _T("Text"), s)) {
      return !(s != o.mText);
    }

    return 1;                           // NOT allow change.
  }

  virtual void OnPropChanged(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT& prj = PrjT::inst();
    PrjT::LevelT& lvl = prj.getLevel(mId);
    PrjT::ObjectT& o = lvl.getObj(mObjId);
    std::string s; int i; bool b; float f;

    if (GetChangedProp(ppi, _T("Name"), s)) {
      lvl.setObjName(mObjId, s);
      if (s.empty()) {                  // Special case.
        ppi->prop->SetValue(CComVariant(o.getName().c_str()));
      }
    } else if (GetChangedProp(ppi, _T("Visible"), b)) {
      lvl.setObjProp(mObjId, b, o.mRot, o.mScaleX, o.mScaleY, o.mAnchorX, o.mAnchorY, o.mRepX, o.mRepY);
    } else if (GetChangedProp(ppi, _T("PosX"), i)) {
      std::vector<int> v;
      v.push_back(mObjId);
      lvl.moveObj(v, i - o.mPosX, 0);
    } else if (GetChangedProp(ppi, _T("PosY"), i)) {
      std::vector<int> v;
      v.push_back(mObjId);
      lvl.moveObj(v, 0, i - o.mPosY);
    } else if (GetChangedProp(ppi, _T("Rot"), f)) {
      lvl.setObjProp(mObjId, o.mVisible, f, o.mScaleX, o.mScaleY, o.mAnchorX, o.mAnchorY, o.mRepX, o.mRepY);
    } else if (GetChangedProp(ppi, _T("ScaleX"), f)) {
      lvl.setObjProp(mObjId, o.mVisible, o.mRot, f, o.mScaleY, o.mAnchorX, o.mAnchorY, o.mRepX, o.mRepY);
    } else if (GetChangedProp(ppi, _T("ScaleY"), f)) {
      lvl.setObjProp(mObjId, o.mVisible, o.mRot, o.mScaleX, f, o.mAnchorX, o.mAnchorY, o.mRepX, o.mRepY);
    } else if (GetChangedProp(ppi, _T("AnchorX"), f)) {
      lvl.setObjProp(mObjId, o.mVisible, o.mRot, o.mScaleX, o.mScaleY, f, o.mAnchorY, o.mRepX, o.mRepY);
    } else if (GetChangedProp(ppi, _T("AnchorY"), f)) {
      lvl.setObjProp(mObjId, o.mVisible, o.mRot, o.mScaleX, o.mScaleY, o.mAnchorX, f, o.mRepX, o.mRepY);
    } else if (GetChangedProp(ppi, _T("RepeatX"), b)) {
      lvl.setObjProp(mObjId, o.mVisible, o.mRot, o.mScaleX, o.mScaleY, o.mAnchorX, o.mAnchorY, b, o.mRepY);
    } else if (GetChangedProp(ppi, _T("RepeatY"), b)) {
      lvl.setObjProp(mObjId, o.mVisible, o.mRot, o.mScaleX, o.mScaleY, o.mAnchorX, o.mAnchorY, o.mRepX, b);
    } else if (GetChangedProp(ppi, _T("Script"), s)) {
      lvl.setObjScript(mObjId, s);
    } else if (GetChangedProp(ppi, _T("TextSize"), i)) {
      lvl.setObjTextSize(mObjId, i);
    } else if (GetChangedProp(ppi, _T("Text"), s)) {
      lvl.setObjText(mObjId, s);
    } else {

      int w = 32, h = 32;               // Set def sz.
      if (0 < o.mTextureId) {
        ImgT img = LoadTileset(prj.getTex(o.mTextureId).mFileName);
        if (img.isValid()) {
          w = img.getWidth(), h = img.getHeight();
        }
      }

      if (0 != o.mDim.width()) {
        w = o.mDim.width();
      }

      if (0 != o.mDim.height()) {
        h = o.mDim.height();
      }

      if (GetChangedProp(ppi, _T("OffsetX"), i)) {
        lvl.setObjDim(mObjId, i, o.mDim.top, w, h);
      } else if (GetChangedProp(ppi, _T("OffsetY"), i)) {
        lvl.setObjDim(mObjId, o.mDim.left, i, w, h);
      } else if (GetChangedProp(ppi, _T("Width"), i)) {
        lvl.setObjDim(mObjId, o.mDim.left, o.mDim.top, i, h);
      } else if (GetChangedProp(ppi, _T("Height"), i)) {
        lvl.setObjDim(mObjId, o.mDim.left, o.mDim.top, w, i);
      }
    }
  }
};

class CResLevelProperty : public IResourceProperty
{
  CResLevelProperty()
  {
  }

public:

  static CResLevelProperty& inst(int id)
  {
    static CResLevelProperty i;
    i.mId = id;
    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_LEVEL;
  }

  virtual std::string GetName() const
  {
    return PrjT::inst().getLevel(mId).getName();
  }

  virtual bool SetColor(unsigned int newColor)
  {
    return PrjT::inst().getLevel(mId).setObjBgColor(mId, newColor);
  }

  virtual void UpdateProperty(CPropertyListCtrl& prop)
  {
    PrjT::LevelT const& o = PrjT::inst().getLevel(mId);
    prop.SetItemValue(propmap["PosX"], &CComVariant(o.mPosX));
    prop.SetItemValue(propmap["PosY"], &CComVariant(o.mPosY));
    prop.SetItemValue(propmap["Width"], &CComVariant(o.mWidth));
    prop.SetItemValue(propmap["Height"], &CComVariant(o.mHeight));
    prop.SetItemValue(propmap["Color"], &CComVariant(o.mBgColor));
    prop.SetItemValue(propmap["Script"], &CComVariant(o.mScript.c_str()));
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    PrjT::LevelT const& o = PrjT::inst().getLevel(mId);

    AddProp(prop, PropCreateCategory("Level"));
    AddProp(prop, PropCreateSimple("Id", o.mId))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Name", o.getName().c_str()));
    AddProp(prop, PropCreateSimple("PosX", o.mPosX));
    AddProp(prop, PropCreateSimple("PosY", o.mPosY));
    AddProp(prop, PropCreateSimple("Width", o.mWidth));
    AddProp(prop, PropCreateSimple("Height", o.mHeight));
    AddProp(prop, PropCreateColorPicker("Color", o.mBgColor, this));
    AddProp(prop, PropCreateSimple("Script", o.mScript.c_str()));
  }

  virtual LRESULT OnPropChanging(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::LevelT const& o = PrjT::inst().getLevel(mId);
    std::string s; int i;

    if (GetChangingProp(prop, ppi, _T("Name"), s)) {
      return !(s != o.getName());
    } else if (GetChangingProp(prop, ppi, _T("PosX"), i) ||
               GetChangingProp(prop, ppi, _T("PosY"), i)) {
      return !(-1024 * 1024 <= i && 1024 * 1024 >= i);
    } else if (GetChangingProp(prop, ppi, _T("Width"), i) ||
               GetChangingProp(prop, ppi, _T("Height"), i)) {
      return !(1 <= i && 1024 * 1024 >= i);
    } else if (GetChangingProp(prop, ppi, _T("Script"), s)) {
      return !(s != o.mScript);
    }

    return 1;                           // NOT allow change.
  }

  virtual void OnPropChanged(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::LevelT& lvl = PrjT::inst().getLevel(mId);
    std::string s; int i;

    if (GetChangedProp(ppi, _T("Name"), s)) {
      PrjT::inst().rename(lvl, s);
      if (s.empty()) {                  // Special case.
        ppi->prop->SetValue(CComVariant(lvl.getName().c_str()));
      }
    } else if (GetChangedProp(ppi, _T("PosX"), i)) {
      lvl.setLevelPos(i, lvl.mPosY);
    } else if (GetChangedProp(ppi, _T("PosY"), i)) {
      lvl.setLevelPos(lvl.mPosX, i);
    } else if (GetChangedProp(ppi, _T("Width"), i)) {
      lvl.setSize(i, lvl.mHeight);
    } else if (GetChangedProp(ppi, _T("Height"), i)) {
      lvl.setSize(lvl.mWidth, i);
    } else if (GetChangedProp(ppi, _T("Script"), s)) {
      lvl.setObjScript(mId, s);
    }
  }
};

//
// Project.
//

class CResProjectProperty : public IResourceProperty
{
  CResProjectProperty()
  {
  }

public:

  static CResProjectProperty& inst()
  {
    static CResProjectProperty i;
    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_PROJECT;
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    PrjT::ResT const& res = PrjT::inst().mRes;

    AddProp(prop, PropCreateCategory("Project"));
    AddProp(prop, PropCreateSimple("Name", res.mName.c_str()));
    AddProp(prop, PropCreateSimple("File", res.mFileName.c_str()))->SetEnabled(FALSE);

    AddProp(prop, PropCreateCategory("Window"));
    AddProp(prop, PropCreateSimple("Frame Rate", res.mFps))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Width", res.mWidth));
    AddProp(prop, PropCreateSimple("Height", res.mHeight));

    AddProp(prop, PropCreateCategory("Summary"));
    AddProp(prop, PropCreateSimple("#Audio", (int)res.mSnd.size()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("#Texture", (int)res.mTex.size()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("#Map", (int)res.mMap.size()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("#Sprite", (int)res.mSprite.size()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("#Level", (int)res.mLevel.size()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("#Script", (int)res.mScript.size()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("#Particle", (int)res.mStgeScript.size()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("#Dependency", (int)res.mDep.size()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("#ID", res.mId.size()))->SetEnabled(FALSE);
  }

  virtual void UpdateProperty(CPropertyListCtrl& prop)
  {
    std::string file = PrjT::inst().mRes.mFileName;
    prop.SetItemValue(propmap["File"], &CComVariant(file.c_str()));
  }

  virtual LRESULT OnPropChanging(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    int i;

    if (GetChangingProp(prop, ppi, _T("Width"), i) ||
        GetChangingProp(prop, ppi, _T("Height"), i)) {
      return !(1 <= i && 2048 >= i);  // Range check.
    } else if (GetChangingProp(prop, ppi, _T("Color Bits"), i)) {
      return !(8 == i || 16 == i || 24 == i || 32 == i);
    } else {                          // Allow change.
      return 0;
    }
  }

  virtual void OnPropChanged(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT& prj = PrjT::inst();
    PrjT::ResT const &res = prj.mRes;

    std::string s; int i;

    if (GetChangedProp(ppi, _T("Name"), s)) {
      prj.setName(s);
    } else if (GetChangedProp(ppi, _T("Width"), i)) {
      prj.setWindowSettings(i, res.mHeight);
    } else if (GetChangedProp(ppi, _T("Height"), i)) {
      prj.setWindowSettings(res.mWidth, i);
    }
  }
};

//
// Script.
//

class CResScriptProperty : public IResourceProperty
{
  CResScriptProperty()
  {
  }

public:

  static CResScriptProperty& inst(int id)
  {
    static CResScriptProperty i;
    i.mId = id;
    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_SCRIPT;
  }

  virtual std::string GetName() const
  {
    return PrjT::inst().getScript(mId);
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    std::string script = PrjT::inst().getScript(mId);

    AddProp(prop, PropCreateCategory("Script"));
    AddProp(prop, PropCreateSimple("Id", mId))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("File", script.c_str()))->SetEnabled(FALSE);
  }
};

//
// Particle, STGE script.
//

class CResParticleProperty : public IResourceProperty
{
  CResParticleProperty()
  {
  }

public:

  static CResParticleProperty& inst(int id)
  {
    static CResParticleProperty i;
    i.mId = id;
    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_PARTICLE;
  }

  virtual std::string GetName() const
  {
    return PrjT::inst().getScript(mId);
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    std::string script = PrjT::inst().getStgeScript(mId);

    AddProp(prop, PropCreateCategory("Particle"));
    AddProp(prop, PropCreateSimple("Id", mId))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("File", script.c_str()))->SetEnabled(FALSE);
  }
};

//
// Sound.
//

class CResSoundProperty : public IResourceProperty
{
  CResSoundProperty()
  {
  }

public:

  static CResSoundProperty& inst(int id)
  {
    static CResSoundProperty i;
    i.mId = id;
    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_AUDIO;
  }

  virtual std::string GetName() const
  {
    return PrjT::inst().getSnd(mId).getName();
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    PrjT::SoundT const& snd = PrjT::inst().getSnd(mId);

    AddProp(prop, PropCreateCategory("Sound"));
    AddProp(prop, PropCreateSimple("Id", snd.mId))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Name", snd.getName().c_str()));
    AddProp(prop, PropCreateSimple("File", snd.mFileName.c_str()))->SetEnabled(FALSE);
    AddProp(prop, PropCreateSimple("Stream", snd.mStream));
    AddProp(prop, PropCreateSimple("Loop", snd.mLoop));
  }

  virtual LRESULT OnPropChanging(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::SoundT const& snd = PrjT::inst().getSnd(mId);
    std::string s; bool b;

    if (GetChangingProp(prop, ppi, _T("Name"), s)) {
      return !(s != snd.getName());
    } else if (GetChangingProp(prop, ppi, _T("Stream"), b)) {
      return !(b != snd.mStream);
    } else if (GetChangingProp(prop, ppi, _T("Loop"), b)) {
      return !(b != snd.mLoop);
    }

    return 1;                           // NOT allow change.
  }

  virtual void OnPropChanged(CPropertyListCtrl& prop, LPNMPROPERTYITEM ppi)
  {
    PrjT::SoundT& snd = PrjT::inst().getSnd(mId);
    std::string s; bool b;

    if (GetChangedProp(ppi, _T("Name"), s)) {
      PrjT::inst().rename(snd, s);
      if (s.empty()) {                  // Special case.
        ppi->prop->SetValue(CComVariant(snd.getName().c_str()));
      }
    } else if (GetChangedProp(ppi, _T("Stream"), b)) {
      snd.setStream(b);
    } else if (GetChangedProp(ppi, _T("Loop"), b)) {
      snd.setLoop(b);
    }
  }
};

//
// Dependency.
//

class CResDependencyProperty : public IResourceProperty
{
  CResDependencyProperty()
  {
  }

public:

  static CResDependencyProperty& inst(int id)
  {
    static CResDependencyProperty i;
    i.mId = id;
    return i;
  }

  virtual int GetType() const
  {
    return GOOD_RESOURCE_DEPENDENCY;
  }

  virtual std::string GetName() const
  {
    return PrjT::inst().getDep(mId);
  }

  virtual void FillProperty(CPropertyListCtrl& prop)
  {
    AddProp(prop, PropCreateCategory("Dependency"));
    AddProp(prop, PropCreateSimple("Id", mId))->SetEnabled(FALSE);

    std::string name = PrjT::inst().getDep(mId);
    if ('/' == name[name.length() - 1]) {
      AddProp(prop, PropCreateSimple("Path", name.c_str()))->SetEnabled(FALSE);
    } else {
      AddProp(prop, PropCreateSimple("File", name.c_str()))->SetEnabled(FALSE);
    }
  }
};

// end of Property.h
