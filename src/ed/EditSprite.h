
//
// EditSprite.h
// Sprite editor.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/12/15 Waync created.
//

#pragma once

template<class MainT, class EditorT>
class CSpriteEditView : public CScrollWindowImpl<CSpriteEditView<MainT, EditorT> >
{
public:
  DECLARE_WND_CLASS(NULL)

  int mCurHot;                          // Current hot light frame index, -1 invalid.
  int mCurSel, mSelCnt;                 // Current selected frame index, -1 for invalid.

  CBrush brPat;

  int mCurFrame, mCurCnt;

  EditorT& mEditor;

  CSpriteEditView(EditorT& ed) : mEditor(ed), mCurFrame(0), mCurCnt(0)
  {
  }

  BEGIN_MSG_MAP_EX(CSpriteEditView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_LBUTTONDBLCLK(OnLButtonDown)
    MSG_WM_LBUTTONUP(OnLButtonUp)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    MSG_WM_SIZE(OnSize)
    MSG_WM_TIMER(OnTimer)
    CHAIN_MSG_MAP(CScrollWindowImpl<CSpriteEditView>)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    mCurSel = mCurHot = -1;

    UpdateLayout();
    SetScrollOffset(0, 0, FALSE);

    CBitmap pat;
    pat.LoadBitmap(IDB_BKBRUSH);

    brPat.CreatePatternBrush(pat);

    SetMsgHandled(FALSE);
    return 0;
  }

  LRESULT OnEraseBkgnd(CDCHandle dc)
  {
    return TRUE;
  }

  void OnLButtonDown(UINT nFlags, CPoint point)
  {
    if (GetFocus() != m_hWnd) {
      SetFocus();
    }

    SetCapture();

    switch (mEditor.mCurTool)
    {
    case EditorT::TOOL_DRAW:
      if (mCurHot != mCurSel) {
        mCurSel = mCurHot;
        mSelCnt = 1;
        UpdateFrameTime();
        Invalidate(FALSE);
      }
      break;

    case EditorT::TOOL_ERASE:
      if (-1 != mCurHot && PrjT::inst().getSprite(mEditor.mId).removeFrame(mCurHot)) {
        mCurSel = -1;
        Invalidate(FALSE);
      }
      break;
    }
  }

  void OnLButtonUp(UINT nFlags, CPoint point)
  {
    ReleaseCapture();

    if (0 > mSelCnt) {
      mCurSel += mSelCnt + 1;
      mSelCnt *= -1;
    }

    Invalidate(FALSE);
  }

  void OnMouseMove(UINT nFlags, CPoint point)
  {
    bool bBtnDown = GetCapture() == m_hWnd;

    RECT rcClient;
    GetClientRect(&rcClient);

    PrjT::SpriteT &spr = PrjT::inst().getSprite(mEditor.mId);

    int lastHot = mCurHot;
    size_t cxMaxTile = max(1, rcClient.right / spr.mTileset.mTileWidth);

    int cx = cxMaxTile;
    int cy = 1 + spr.mFrame.size() / cxMaxTile;

    RECT rcBound = {0, 0, cx * spr.mTileset.mTileWidth, cy * spr.mTileset.mTileHeight};

    if (PtInRect(&rcBound, point)) {
      int x = point.x / spr.mTileset.mTileWidth;
      int y = (point.y + m_ptOffset.y) / spr.mTileset.mTileHeight;
      mCurHot = x + y * cxMaxTile;
      if (spr.mFrame.size() <= (size_t)mCurHot) {
        if (!bBtnDown) {
          mCurHot = -1;
        } else {
          mCurHot = spr.mFrame.size() - 1;
        }
      }
    } else {
      if (!bBtnDown) {
        mCurHot = -1;
      }
    }

    int lastSelCnt = mSelCnt;
    if (bBtnDown) {
      if (EditorT::TOOL_ERASE == mEditor.mCurTool) {
        if (lastHot != mCurHot && -1 != mCurHot && spr.removeFrame(mCurHot)) {
          Invalidate(FALSE);
        }
      } else {
        mSelCnt = mCurHot - mCurSel;
        if (0 < mSelCnt) {
          mSelCnt += 1;
        } else {
          mSelCnt -= 1;
        }
      }
    }

    if (lastHot != mCurHot || lastSelCnt != mSelCnt) {
      CString str;
      if (-1 != mCurHot) {
        str.Format(_T("Frame %d (%d)"), mCurHot, spr.mFrame[mCurHot]);
      }
      MainT::inst().mStatus.SetPaneText(2, str);
      Invalidate(FALSE);
    }
  }

  void OnSize(UINT nType, CSize size)
  {
    UpdateLayout(size.cx, size.cy);
    SetMsgHandled(FALSE);
  }

  void OnTimer(UINT_PTR nIDEvent)
  {
    PrjT::SpriteT const& spr = PrjT::inst().getSprite(mEditor.mId);

    if (spr.mTime.empty()) {
      return;
    }

    mCurCnt -= 1;

    if (0 >= mCurCnt) {

      mCurFrame += 1;
      if ((int)spr.mFrame.size() <= mCurFrame) {
        mCurFrame = 0;
      }

      mCurCnt = spr.mTime[mCurFrame];

      //
      // Update preview tile.
      //

      RECT rcClient;
      GetClientRect(&rcClient);

      PrjT::SpriteT const& spr = PrjT::inst().getSprite(mEditor.mId);

      size_t cxMaxTile = max(1, rcClient.right / spr.mTileset.mTileWidth);

      int x = 0;
      int y = 1 + (spr.mFrame.size() / cxMaxTile);

      RECT rc = {0, 0, spr.mTileset.mTileWidth, spr.mTileset.mTileHeight};
      ::OffsetRect(&rc, spr.mTileset.mTileWidth * x, spr.mTileset.mTileHeight * y + 10);

      InvalidateRect(&rc, FALSE);       // Only invalid preview tile area.
    }
  }

  //
  // Override.
  //

  void DoPaint(CDCHandle dc1)
  {
    RECT rcClient;
    GetClientRect(&rcClient);

    PrjT::SpriteT const& spr = PrjT::inst().getSprite(mEditor.mId);

    size_t cxMaxTile = max(1, rcClient.right / spr.mTileset.mTileWidth);

    CMemoryDC dc(dc1, rcClient);
    dc.FillRect(&rcClient, COLOR_APPWORKSPACE);

    ImgT img;
    img = LoadTileset(PrjT::inst().getTex(spr.mTileset.mTextureId).mFileName);

    //
    // Frames.
    //

    dc.SelectBrush((HBRUSH)::GetStockObject(NULL_BRUSH));

    CPen p1, p2;
    p1.CreatePen(PS_SOLID, 1, RGB(96,96,96));
    p2.CreatePen(PS_SOLID, 1, RGB(255,0,0));

    size_t i;
    for (i = 0; i < spr.mFrame.size() + 1; ++i) { // +1 for empty frame.
      size_t x = i % cxMaxTile, y = i / cxMaxTile;
      RECT rc = {0, 0, spr.mTileset.mTileWidth, spr.mTileset.mTileHeight};
      ::OffsetRect(&rc, spr.mTileset.mTileWidth * x, spr.mTileset.mTileHeight * y);

      if (spr.mFrame.size() != i) {
        dc.FillRect(&rc, brPat);
        if (img.isValid()) {
          int tile = spr.mFrame[i];
          int srcx = spr.mTileset.mTileWidth * (tile % spr.mTileset.mCxTile);
          int srcy = spr.mTileset.mTileHeight * (tile / spr.mTileset.mCxTile);
          GxT(dc).drawImage(
                    rc.left, rc.top,
                    img,
                    srcx, srcy,
                    spr.mTileset.mTileWidth, spr.mTileset.mTileHeight);
        }
        if (mCurHot == i) {
          DrawAlphaRect(
            (HDC)dc,
            EditorT::TOOL_DRAW == mEditor.mCurTool ? RGB(0, 255, 0) : RGB(255, 0, 0),
            rc.left, rc.top,
            spr.mTileset.mTileWidth, spr.mTileset.mTileHeight,
            120);
        }
        int CurSel = 0 > mSelCnt ? mCurSel + mSelCnt + 1 : mCurSel;
        int SelCnt = 0 > mSelCnt ? -mSelCnt : mSelCnt;
        if (-1 != mCurSel && CurSel <= (int)i && CurSel + SelCnt > (int)i) {
          dc.SelectPen(p2);
        } else {
          dc.SelectPen(p1);
        }
      } else {
        dc.SelectPen(-1 == mCurSel ? p2 : p1);
        dc.FillRect(&rc, COLOR_APPWORKSPACE);
      }

      dc.Rectangle(&rc);
    }

    //
    // Erase extra bkgnd.
    //

    size_t x = i % cxMaxTile, y = i / cxMaxTile;

    RECT rce = {0, 0, spr.mTileset.mTileWidth * (cxMaxTile - (i % cxMaxTile)), spr.mTileset.mTileHeight};
    ::OffsetRect(&rce, spr.mTileset.mTileWidth * x, spr.mTileset.mTileHeight * y);

    dc.FillRect(&rce, COLOR_APPWORKSPACE);

    //
    // Draw preview tile.
    //

    x = 0;
    y = 1 + (spr.mFrame.size() / cxMaxTile);

    RECT rc = {0, 0, spr.mTileset.mTileWidth, spr.mTileset.mTileHeight};
    ::OffsetRect(&rc, spr.mTileset.mTileWidth * x, spr.mTileset.mTileHeight * y + 10);

    dc.FillRect(&rc, brPat);

    if (!spr.mFrame.empty() && img.isValid()) {
      CommonDrawSprite(GxT(dc), spr, img, rc.left, rc.top, mCurFrame);
    }
  }

  //
  // Util.
  //

  void UpdateLayout()
  {
    RECT rcClient;
    GetClientRect(&rcClient);
    UpdateLayout(rcClient.right, rcClient.left);
  }

  void UpdateLayout(int cx, int cy)
  {
    PrjT &prj = PrjT::inst();

    if (!prj.mRes.isSprite(mEditor.mId)) {
      return;
    }

    PrjT::SpriteT const& spr = prj.getSprite(mEditor.mId);

    size_t cxMaxTile = max(1, cx / spr.mTileset.mTileWidth);
    int y = 1 + spr.mFrame.size() / cxMaxTile;

    SIZE sz = {cxMaxTile * spr.mTileset.mTileWidth, y * spr.mTileset.mTileHeight};
    SetScrollSize(sz);
  }

  void UpdateFrameTime()
  {
    if (-1 != mCurSel) {
      PrjT::SpriteT const& spr = PrjT::inst().getSprite(mEditor.mId);
      mEditor.mFrameTime.SetPos(spr.mTime[mCurSel]);
      CString s;
      s.Format(_T("%d"), spr.mTime[mCurSel]);
      mEditor.mFrameTimeTxt.SetWindowText(s);
      mEditor.mCurTile = spr.mFrame[mCurSel];
      mEditor.mTilesetView.Invalidate(FALSE);
    } else {
      mEditor.mFrameTimeTxt.SetWindowText(_T(""));
    }
    mEditor.mFrameTime.EnableWindow(-1 != mCurSel);
  }
};

template<class MainT>
class CSpriteEditor : public CWindowImpl<CSpriteEditor<MainT> >, public CUpdateUI<CSpriteEditor<MainT> >, public CIdleHandler
{
public:
  enum {
    TOOL_DRAW,
    TOOL_ERASE
  };

  int mPos;
  CSplitterWindow mSplit;

  CGoodPane mEditPane, mTilesetPane;    // Panes with toolbar.
  CToolBarCtrl mTBEdit, mTBTileset;     // Toolbars.

  CSpriteEditView<MainT, CSpriteEditor> mEditView;
  CGoodTilesetView<CSpriteEditor> mTilesetView;

  CTrackBarCtrl mFrameTime;             // Frame duration.
  CTrackBarCtrl mFrameTimeAdd;          // Sprite pane, frame duration.

  CStatic mFrameTimeTxt, mFrameTimeAddTxt;

  int mId;                              // ID of sprite.

  int mCurTool;                         // Selected tool, 0:draw, 1:erase, 2:fill, 3:sel, 4:move.
  int mCurTile;                         // Single tile, valid if mCurPattern.empty() and not -1.

  std::vector<int> mCurPattern;         // Pattern.
  SIZE mSelRange;                       // Selection range(w,h).

  bool mPreview;

  CSpriteEditor(int id) :
    mPos(210),
    mEditView(*this),
    mTilesetView(*this),
    mId(id),
    mPreview(true)
  {
  }

  virtual void OnFinalMessage(HWND)     // Delete self when window destroy.
  {
    MainT::inst().ResetStatusBar();
    delete this;
  }

  virtual BOOL OnIdle()                 // Update UI states.
  {
    PrjT::SpriteT const& spr = PrjT::inst().getSprite(mId); // This sprite.

    UISetCheck(ID_SPRITEPREVIEW_PLAY, mPreview);
    UISetCheck(ID_SPRITEEDIT_DRAW, TOOL_DRAW == mCurTool);
    UISetCheck(ID_SPRITEEDIT_ERASE, TOOL_ERASE == mCurTool);

    UIEnable(ID_SPRITETILESET_INSERT, TOOL_DRAW == mCurTool && (-1 != mCurTile || !mCurPattern.empty()));

    UIUpdateToolBar();

    //
    // Update status bar.
    //

    MainT& mf = MainT::inst();

    CString str;
    str.Format(_T("%d (frames)"), spr.mFrame.size());

    mf.mStatus.SetPaneText(1, str);

    str.Empty();
    if (-1 != mEditView.mCurHot) {
      str.Format(_T("Frame %d (%d)"), mEditView.mCurHot, spr.mFrame[mEditView.mCurHot]);
    }

    mf.mStatus.SetPaneText(2, str);

    //
    // Save splitter pos.
    //

    RECT rcClient;
    GetClientRect(&rcClient);

    mPos = rcClient.right - mSplit.GetSplitterPos();

    return FALSE;
  }

  BEGIN_UPDATE_UI_MAP(CSpriteEditor)
    UPDATE_ELEMENT(ID_SPRITEPREVIEW_PLAY, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_SPRITEEDIT_DRAW, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_SPRITEEDIT_ERASE, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_SPRITETILESET_INSERT, UPDUI_TOOLBAR)
  END_UPDATE_UI_MAP()

  BEGIN_MSG_MAP_EX(CSpriteEditor)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_HSCROLL(OnHScroll)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    MSG_WM_SIZE(OnSize)
    MSG_WM_GOOD(OnGoodMessage)
    COMMAND_ID_HANDLER_EX(ID_SPRITEPREVIEW_PLAY, OnPreviewPlay)
    COMMAND_ID_HANDLER_EX(ID_SPRITEEDIT_DRAW, OnDrawTool)
    COMMAND_ID_HANDLER_EX(ID_SPRITEEDIT_ERASE, OnEraseTool)
    COMMAND_ID_HANDLER_EX(ID_SPRITETILESET_INSERT, OnInsertSprite)
    CHAIN_MSG_MAP(CUpdateUI<CSpriteEditor>)
    FORWARD_TOOLTIP_GETDISPINFO()
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    PrjT& prj = PrjT::inst();
    PrjT::SpriteT const& spr = prj.getSprite(mId); // This sprite.

    //
    // Property.
    //

    mCurTool = TOOL_DRAW;
    mCurTile = -1;

    //
    // Create splits and panes.
    //

    mSplit.Create(m_hWnd);

    mEditPane.Create(mSplit);
    mEditPane.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);

    mTilesetPane.Create(mSplit);
    mTilesetPane.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);

    mSplit.SetSplitterPanes(mEditPane, mTilesetPane);
    mSplit.SetActivePane(0);

    //
    // Create toolbars.
    //

    mTBEdit = GoodCreateSimpleToolbar(mEditPane, IDR_TB_SPRITEEDIT);
    UIAddToolBar(mTBEdit);

    mTBTileset = GoodCreateSimpleToolbar(mTilesetPane, IDR_TB_SPRITETILESET);
    UIAddToolBar(mTBTileset);

    //
    // Create views.
    //

    mEditView.Create(mEditPane, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_STATICEDGE);
    mEditPane.SetClient(mEditView);

    mTilesetView.Create(mTilesetPane, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_STATICEDGE);
    mTilesetPane.SetClient(mTilesetView);

    //
    // Create frame time edit & txt.
    //

    mFrameTime.Create(mEditPane, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBS_HORZ | TBS_AUTOTICKS);
    mFrameTime.SetFont(AtlGetDefaultGuiFont());
    mFrameTime.SetWindowPos(NULL, mEditPane.m_cxyBorder + mEditPane.m_cxyBtnOffset + (1 + mTBEdit.GetButtonCount()) * mEditPane.mCxButton, mEditPane.m_cxyBorder + mEditPane.m_cxyBtnOffset, 128, mEditPane.mCyButton, SWP_NOZORDER | SWP_NOACTIVATE);
    mFrameTime.SetRange(1,180);
    mFrameTime.EnableWindow(FALSE);

    mFrameTimeTxt.Create(mEditPane, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | SS_CENTERIMAGE);
    mFrameTimeTxt.SetFont(AtlGetDefaultGuiFont());
    mFrameTimeTxt.SetWindowPos(NULL, mEditPane.m_cxyBorder + mEditPane.m_cxyBtnOffset + (1 + mTBEdit.GetButtonCount()) * mEditPane.mCxButton + 130, mEditPane.m_cxyBorder + mEditPane.m_cxyBtnOffset, 128, mEditPane.mCyButton, SWP_NOZORDER | SWP_NOACTIVATE);

    mFrameTimeAdd.Create(mTilesetPane, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBS_HORZ | TBS_AUTOTICKS);
    mFrameTimeAdd.SetFont(AtlGetDefaultGuiFont());
    mFrameTimeAdd.SetWindowPos(NULL, mTilesetPane.m_cxyBorder + 2 * mTilesetPane.m_cxyBtnOffset + mEditPane.mCxButton, mTilesetPane.m_cxyBorder + mTilesetPane.m_cxyBtnOffset, 128, mTilesetPane.mCyButton, SWP_NOZORDER | SWP_NOACTIVATE);
    mFrameTimeAdd.SetRange(1,180);
    mFrameTimeAdd.SetPos(60);

    mFrameTimeAddTxt.Create(mTilesetPane, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | SS_CENTERIMAGE);
    mFrameTimeAddTxt.SetFont(AtlGetDefaultGuiFont());
    mFrameTimeAddTxt.SetWindowPos(NULL, mTilesetPane.m_cxyBorder + 2 * mTilesetPane.m_cxyBtnOffset + mEditPane.mCxButton + 130, mTilesetPane.m_cxyBorder + mTilesetPane.m_cxyBtnOffset, 128, mTilesetPane.mCyButton, SWP_NOZORDER | SWP_NOACTIVATE);
    mFrameTimeAddTxt.SetWindowText(_T("60"));

    //
    // Init res.
    //

    mTilesetView.SelectTileset(spr.mTileset);

    SetMsgHandled(FALSE);
    return 0;
  }

  int OnGoodMessage(int uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (WM_GOOD_GETRESOURCEID == uMsg) {
      return mId;
    }

    PrjT::SpriteT& spr = PrjT::inst().getSprite(mId);

    switch (uMsg)
    {
    case WM_GOOD_UNDO:
      if (spr.undo()) {
        mEditView.mCurSel = mEditView.mCurHot = -1;
        mEditView.mCurFrame = mEditView.mCurCnt = 0;
        UpdateLayout();
      }
      return true;

    case WM_GOOD_CANUNDO:
      return spr.canUndo();

    case WM_GOOD_CANREDO:
      return spr.canRedo();

    case WM_GOOD_REDO:
      if (spr.redo()) {
        UpdateLayout();
      }
      return true;
    }

    return 0;
  }

  void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar)
  {
    if (mFrameTime == pScrollBar) {
      if (-1 == mEditView.mCurSel) {
        return;
      }
      int pos = mFrameTime.GetPos();
      PrjT::SpriteT& spr = PrjT::inst().getSprite(mId); // This sprite.
      if (spr.setTime(mEditView.mCurSel, mEditView.mSelCnt, pos)) {
        CString s;
        s.Format(_T("%d"), pos);
        mFrameTimeTxt.SetWindowText(s);
      }
    } else if (mFrameTimeAdd == pScrollBar) {
      CString s;
      s.Format(_T("%d"), mFrameTimeAdd.GetPos());
      mFrameTimeAddTxt.SetWindowText(s);
    }
  }

  void OnShowWindow(BOOL bShow, UINT nStatus)
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    if (bShow) {
      pLoop->AddIdleHandler(this);
      if (mPreview) {
        mEditView.SetTimer(1, 1000 / PrjT::inst().mRes.mFps);
      }
    } else {
      pLoop->RemoveIdleHandler(this);
      if (mPreview) {
        mEditView.KillTimer(1);
      }
    }

    SetMsgHandled(FALSE);
  }

  void OnSize(UINT nType, CSize size)
  {
    mSplit.SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOACTIVATE | SWP_NOZORDER);
    mSplit.SetSplitterPos(size.cx - mPos);
  }

  //
  // Command handler.
  //

  void OnPreviewPlay(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mPreview = !mPreview;
    if (mPreview) {
      mEditView.SetTimer(1, 1000 / PrjT::inst().mRes.mFps);
    } else {
      mEditView.KillTimer(1);
    }
  }

  void OnDrawTool(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mCurTool = TOOL_DRAW;
    mEditView.UpdateFrameTime();
    mEditView.Invalidate(FALSE);
  }

  void OnEraseTool(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mCurTool = TOOL_ERASE;
    mEditView.mCurSel = -1;
    mEditView.UpdateFrameTime();
    mEditView.Invalidate(FALSE);
  }

  void OnInsertSprite(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::SpriteT& spr = PrjT::inst().getSprite(mId); // This sprite.

    int frameTime = mFrameTimeAdd.GetPos();

    int iIns = mEditView.mCurSel;
    if (-1 == iIns) {
      iIns = spr.mFrame.size();
    }

    std::vector<int> v;
    if (mCurPattern.empty()) {
      v.push_back(mCurTile);
    } else {
      for (size_t i = 0; i < mCurPattern.size(); ++i) {
        v.push_back(mCurPattern[i] - 1);
      }
    }

    if (spr.insertFrame(iIns, v, frameTime)) {
      UpdateLayout();
    }
  }

  //
  // Helper.
  //

  void UpdateLayout()
  {
    mEditView.UpdateFrameTime();
    mEditView.UpdateLayout();
  }
};

// end of EditSprite.h
