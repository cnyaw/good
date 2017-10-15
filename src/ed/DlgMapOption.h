
//
// DlgMapOption.h
// Map option dialog.
//
// Copyright (c) 2014 Waync Cheng.
// All Rights Reserved.
//
// 2014/06/19 Waync created.
//

#pragma once

template<class MainT>
class CDlgMapOptionGrid :
  public CPropertyPageImpl<CDlgMapOptionGrid<MainT> >,
  public CDlgPageMapGridImpl<CDlgMapOptionGrid<MainT> >
{
public:
  enum { IDD = IDD_MAPPROP_GRID };

  int mId; // mapid

  CDlgMapOptionGrid(int id, int& w, int& h, int& tw, int& th, std::vector<good::ed::GridLine>& vg, std::vector<good::ed::GridLine>& hg)
      : CDlgPageMapGridImpl<CDlgMapOptionGrid>(w, h, tw, th, vg, hg), mId(id)
  {
  }

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
    PrjT& prj = PrjT::inst();

    PrjT::MapT& map = prj.getMap(mId);
    map.setGrid(mVertGrid, mHorzGrid);

    return TRUE;
  }

  BEGIN_MSG_MAP_EX(CDlgPageMapGrid)
    MSG_WM_INITDIALOG(OnInitDialog)
    CHAIN_MSG_MAP(CDlgPageMapGridImpl<CDlgMapOptionGrid>)
    CHAIN_MSG_MAP(CPropertyPageImpl<CDlgMapOptionGrid>)
  END_MSG_MAP()

  //
  // Message handler.
  //

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    GetPropertySheet().CenterWindow(GetPropertySheet().GetParent());

    SetMsgHandled(FALSE);
    return TRUE;
  }
};

template<class MainT>
class CDlgMapOption : public CPropertySheetImpl<CDlgMapOption<MainT> >
{
public:

  CDlgMapOptionGrid<MainT> mMapGrid;

  int mWidth, mHeight;
  int mTileWidth, mTileHeight;

  std::vector<good::ed::GridLine> mVertGrid, mHorzGrid;

  CDlgMapOption(int id) :
    mMapGrid(id, mWidth, mHeight, mTileWidth, mTileHeight, mVertGrid, mHorzGrid)
  {
    SetWizardMode();

    AddPage(mMapGrid);
    SetActivePage(0);

    PrjT& prj = PrjT::inst();

    PrjT::MapT const& map = prj.getMap(id);

    mWidth = map.mWidth;
    mHeight = map.mHeight;
    mTileWidth = map.mTileset.mTileWidth;
    mTileHeight = map.mTileset.mTileHeight;
    mVertGrid = map.mVertGrid;
    mHorzGrid = map.mHorzGrid;
  }

  BEGIN_MSG_MAP_EX(CDlgMapOption)
    CHAIN_MSG_MAP(CPropertySheetImpl<CDlgMapOption>)
  END_MSG_MAP()
};

// end of DlgMapOption.h
