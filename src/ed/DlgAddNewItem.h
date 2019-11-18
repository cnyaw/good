
//
// DlgAddNewItem.h
// Add new item wizard.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/11/25 Waync created.
//

#pragma once

class CDlgPageMapProp :
  public CPropertyPageImpl<CDlgPageMapProp>,
  public CWinDataExchange<CDlgPageMapProp>
{
public:
  enum { IDD = IDD_NEWMAP };

  CString mName;

  int mTexture;

  int mWidth, mHeight;
  int mTileWidth, mTileHeight;

  std::vector<good::ed::GridLine> mVertGrid, mHorzGrid;

  //
  // Override.
  //

  BOOL OnKillActive()
  {
    if (!DoDataExchange(TRUE)) {
      return FALSE;
    }

    if (-1 == mTexture) {
      ::SetFocus(GetDlgItem(IDC_EDIT7));
      goto failed;
    }

    mName.TrimRight();
    mName.TrimLeft();

    return TRUE;

failed:
    ::MessageBeep((UINT)-1);

    return FALSE;
  }

  int OnSetActive()
  {
    SetWizardButtons(PSWIZB_NEXT);
    return TRUE;
  }

  int OnWizardNext()
  {
    return IDD_MAPPROP_GRID;
  }

  BEGIN_MSG_MAP_EX(CDlgPageMapProp)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_ID_HANDLER_EX(IDC_BUTTON2, OnSelTexture)
    CHAIN_MSG_MAP(CPropertyPageImpl<CDlgPageMapProp>)
  END_MSG_MAP()

  BEGIN_DDX_MAP(CDlgPageMapProp)
    DDX_TEXT(IDC_EDIT1, mName)
    DDX_UINT_RANGE(IDC_EDIT2, mWidth, 1, 1024)
    DDX_UINT_RANGE(IDC_EDIT3, mHeight, 1, 1024)
    DDX_UINT_RANGE(IDC_EDIT4, mTileWidth, 8, 1024)
    DDX_UINT_RANGE(IDC_EDIT5, mTileHeight, 8, 1024)
  END_DDX_MAP()

  //
  // Message handler.
  //

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    GetPropertySheet().CenterWindow(GetPropertySheet().GetParent());

    mName = _T("");
    mTexture = 0;
    mWidth = 10, mHeight = 10;
    mTileWidth = 32, mTileHeight = 32;

    good::ed::GridLine gl;
    gl.range = 1;
    gl.color = RGB(128,128,128);

    mVertGrid.clear(), mVertGrid.push_back(gl);
    mHorzGrid.clear(), mHorzGrid.push_back(gl);

    DoDataExchange(FALSE);              // Init, apply to controls.

    return TRUE;
  }

  //
  // Command handler.
  //

  void OnSelTexture(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CDlgTexturePicker dlg;
    if (IDOK != dlg.DoModal()) {
      return;
    }

    mTexture = dlg.mId;
    if (-1 == mTexture) {
      return;
    }

    SetDlgItemText(IDC_EDIT7, PrjT::inst().getTex(mTexture).getName().c_str());
  }
};

template<class MainT, class PropT>
class CDlgPageMapGrid :
  public CPropertyPageImpl<CDlgPageMapGrid<MainT, PropT> >,
  public CDlgPageMapGridImpl<CDlgPageMapGrid<MainT, PropT> >
{
public:
  enum { IDD = IDD_MAPPROP_GRID };

  PropT& mProp;

  CDlgPageMapGrid(PropT& p) : CDlgPageMapGridImpl<CDlgPageMapGrid>(
                                p.mWidth,
                                p.mHeight,
                                p.mTileWidth,
                                p.mTileHeight,
                                p.mVertGrid,
                                p.mHorzGrid), mProp(p)
  {
  }

  //
  // Override.
  //

  int OnSetActive()
  {
    SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
    return TRUE;
  }

  int OnWizardBack()
  {
    return IDD_NEWMAP;
  }

  INT_PTR OnWizardFinish()
  {
    PrjT& prj = PrjT::inst();

    int idMap = prj.addMap<ImgT>((char const*)mProp.mName, mProp.mWidth, mProp.mHeight, mProp.mTileWidth, mProp.mTileHeight, mProp.mTexture);
    if (-1 == idMap) {
      return FALSE;
    }

    PrjT::MapT& map = prj.getMap(idMap);
    map.setGrid(mProp.mVertGrid, mProp.mHorzGrid); // Assign grid settings.

    MainT &m = MainT::inst();
    m.AddResourceItem(_T("Map"), map.getName(), idMap, GOOD_RESOURCE_MAP, m.CreateEditor<CMapEditor<MainT> >(idMap));

    return TRUE;
  }

  BEGIN_MSG_MAP_EX(CDlgPageMapGrid)
    CHAIN_MSG_MAP(CDlgPageMapGridImpl<CDlgPageMapGrid>)
    CHAIN_MSG_MAP(CPropertyPageImpl<CDlgPageMapGrid>)
  END_MSG_MAP()
};

template<class MainT>
class CDlgPageSpriteProp :
  public CPropertyPageImpl<CDlgPageSpriteProp<MainT> >,
  public CWinDataExchange<CDlgPageSpriteProp<MainT> >
{
public:
  enum { IDD = IDD_NEWSPRITE };

  CString mName;
  int mTexture;
  int mTileWidth, mTileHeight;

  //
  // Override.
  //

  int OnSetActive()
  {
    SetWizardButtons(PSWIZB_FINISH);
    return TRUE;
  }

  INT_PTR OnWizardFinish()
  {
    if (-1 == mTexture) {
      ::SetFocus(GetDlgItem(IDC_EDIT3)); // Fixme, cant set focus to this ctrl.
      return FALSE;
    }

    if (!DoDataExchange(TRUE)) {
      return FALSE;
    }

    mName.TrimRight();
    mName.TrimLeft();

    PrjT& prj = PrjT::inst();

    int idSprite = prj.addSprite<ImgT>((char const*)mName, mTileWidth, mTileHeight, mTexture);
    if (-1 == idSprite) {
      return FALSE;
    }

    MainT &m = MainT::inst();
    m.AddResourceItem(_T("Sprite"), prj.getSprite(idSprite).getName(), idSprite, GOOD_RESOURCE_SPRITE, m.CreateEditor<CSpriteEditor<MainT> >(idSprite));

    return TRUE;
  }

  BEGIN_MSG_MAP_EX(CDlgPageSpriteProp)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_ID_HANDLER_EX(IDC_BUTTON1, OnSelTexture)
    CHAIN_MSG_MAP(CPropertyPageImpl<CDlgPageSpriteProp>)
  END_MSG_MAP()

  BEGIN_DDX_MAP(CDlgPageSpriteProp)
    DDX_TEXT(IDC_EDIT6, mName)
    DDX_UINT_RANGE(IDC_EDIT1, mTileWidth, 8, 1024)
    DDX_UINT_RANGE(IDC_EDIT2, mTileHeight, 8, 1024)
  END_DDX_MAP()

  //
  // Message handler.
  //

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    GetPropertySheet().CenterWindow(GetPropertySheet().GetParent());

    mName = _T("");
    mTexture = -1;
    mTileWidth = mTileHeight = 32;

    DoDataExchange(FALSE);              // Init, apply to controls.
    return TRUE;
  }

  //
  // Command handler.
  //

  void OnSelTexture(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CDlgTexturePicker dlg;
    if (IDOK != dlg.DoModal()) {
      return;
    }

    mTexture = dlg.mId;
    if (-1 == mTexture) {
      return;
    }

    SetDlgItemText(IDC_EDIT3, PrjT::inst().getTex(mTexture).getName().c_str());
  }
};

template<class MainT>
class CDlgAddNewItem : public CPropertySheetImpl<CDlgAddNewItem<MainT> >
{
public:

  int mId;                              // Dummy.

  CDlgPageMapProp mMapProp;
  CDlgPageMapGrid<MainT, CDlgPageMapProp> mMapGrid;
  CDlgPageSpriteProp<MainT> mSpriteProp;

  CDlgAddNewItem(int res) : mMapGrid(mMapProp)
  {
    SetWizardMode();

    AddPage(mMapProp);
    AddPage(mMapGrid);
    AddPage(mSpriteProp);

    switch (res)
    {
    case GOOD_RESOURCE_MAP:
      SetActivePage(0);
      break;

    case GOOD_RESOURCE_SPRITE:
      SetActivePage(2);
      break;
    }
  }

  BEGIN_MSG_MAP_EX(CDlgAddNewItem)
    CHAIN_MSG_MAP(CPropertySheetImpl<CDlgAddNewItem>)
  END_MSG_MAP()
};

// end of DlgAddNewItem.h
