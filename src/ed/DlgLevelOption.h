
//
// DlgLevelOption.h
// Level option dialog.
//
// Copyright (c) 2019 Waync Cheng.
// All Rights Reserved.
//
// 2019/11/19 Waync created.
//

#pragma once

template<class MainT>
class CDlgLevelOptionGrid : public CPropertyPageImpl<CDlgLevelOptionGrid<MainT> >, public CDlgPageMapGridImpl<CDlgLevelOptionGrid<MainT> >
{
public:
  enum { IDD = IDD_MAPPROP_GRID };

  int mId; // lvlid

  CDlgLevelOptionGrid(int id, int& w, int& h, int& tw, int& th, std::vector<good::ed::GridLine>& vg, std::vector<good::ed::GridLine>& hg)
      : CDlgPageMapGridImpl<CDlgLevelOptionGrid>(w, h, tw, th, vg, hg), mId(id)
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

    PrjT::LevelT& lvl = prj.getLevel(mId);
    lvl.setGrid(mVertGrid, mHorzGrid);

    return TRUE;
  }

  BEGIN_MSG_MAP_EX(CDlgPageMapGrid)
    MSG_WM_INITDIALOG(OnInitDialog)
    CHAIN_MSG_MAP(CDlgPageMapGridImpl<CDlgLevelOptionGrid>)
    CHAIN_MSG_MAP(CPropertyPageImpl<CDlgLevelOptionGrid>)
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
class CDlgLevelOption : public CPropertySheetImpl<CDlgLevelOption<MainT> >
{
public:

  CDlgLevelOptionGrid<MainT> mLevelGrid;

  int mWidth, mHeight;
  int mTempTileWidth, mTempTileHeight;

  std::vector<good::ed::GridLine> mVertGrid, mHorzGrid;

  CDlgLevelOption(int id) : mLevelGrid(id, mWidth, mHeight, mTempTileWidth, mTempTileHeight, mVertGrid, mHorzGrid)
  {
    SetWizardMode();

    AddPage(mLevelGrid);
    SetActivePage(0);

    PrjT& prj = PrjT::inst();

    PrjT::LevelT const& lvl = prj.getLevel(id);

    mWidth = lvl.mWidth;
    mHeight = lvl.mHeight;
    mVertGrid = lvl.mVertGrid;
    mHorzGrid = lvl.mHorzGrid;
  }

  BEGIN_MSG_MAP_EX(CDlgLevelOption)
    CHAIN_MSG_MAP(CPropertySheetImpl<CDlgLevelOption>)
  END_MSG_MAP()
};

// end of DlgLevelOption.h
