
//
// EditMap.h
// Map editor.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/12/03 Waync created.
//

#pragma once

template<class MainT, class EditorT>
class CMapEditView : public CScrollWindowImpl<CMapEditView<MainT, EditorT> >, public CScrollEraseBkgndImpl<CMapEditView<MainT, EditorT> >
{
public:
  DECLARE_WND_CLASS(NULL)

  int mCurHot;                          // Current hot light tile index.
  int mLastCurHot;                      // Previous hot light tile index.

  int mLastSel;                         // Mouse down tile index.

  bool mMouseDown;                      // Is L mouse down.
  bool mRMouseDown;                     // Is R mouse down.

  EditorT& mEditor;

  CMapEditView(EditorT& ed) : mEditor(ed)
  {
  }

  void DoScroll(int nType, int nScrollCode, int& cxyOffset, int cxySizeAll, int cxySizePage, int cxySizeLine)
  {
    CScrollWindowImpl<CMapEditView>::DoScroll(nType, nScrollCode, cxyOffset, cxySizeAll, cxySizePage, cxySizeLine);
    if (SB_ENDSCROLL == nScrollCode) {
      Invalidate(FALSE);
    }
  }

  BEGIN_MSG_MAP_EX(CMapEditView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_LBUTTONUP(OnLButtonUp)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
    MSG_WM_RBUTTONDOWN(OnRButtonDown)
    MSG_WM_RBUTTONUP(OnRButtonUp)
    CHAIN_MSG_MAP(CScrollEraseBkgndImpl<CMapEditView>)
    CHAIN_MSG_MAP(CScrollWindowImpl<CMapEditView>)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    PrjT::MapT const& map = PrjT::inst().getMap(mEditor.mId);

    //
    // Setup scroll view.
    //

    SIZE sz = {map.mWidth * map.mTileset.mTileWidth, map.mHeight * map.mTileset.mTileHeight};

    SetScrollOffset(0, 0, FALSE);
    SetScrollSize(sz);

    mCurHot = -1;
    mMouseDown = mRMouseDown = false;

    SetMsgHandled(FALSE);
    return 0;
  }

  void OnLButtonDown(UINT nFlags, CPoint point)
  {
    if (GetFocus() != m_hWnd) {
      SetFocus();
    }

    if (-1 == mCurHot) {
      return;
    }

    PrjT::MapT& map = PrjT::inst().getMap(mEditor.mId);

    if ((-1 == mEditor.mCurTile && mEditor.mCurPattern.empty() &&
        (EditorT::TOOL_ERASE != mEditor.mCurTool &&
         EditorT::TOOL_FILL != mEditor.mCurTool))) {
      MessageBox(_T("Please Select a Tile First"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK | MB_ICONWARNING);
      return;
    }

    SetCapture();
    mMouseDown = true;

    map.beginDraw();

    ApplyMouseTool();
  }

  void OnLButtonUp(UINT nFlags, CPoint point)
  {
    if (GetCapture() == m_hWnd) {
      ReleaseCapture();
    }

    mMouseDown = false;

    PrjT::inst().getMap(mEditor.mId).endDraw();
  }

  void OnMouseMove(UINT nFlags, CPoint point)
  {
    int xySel = GetTileIndexFromPoint(point);

    if (mCurHot != xySel) {
      mCurHot = xySel;
      if (-1 != xySel) {
        mLastCurHot = xySel;
      }

      //
      // This is safe, because there is always only one WM_PAINT been generated?
      //

      Invalidate(FALSE);

      //
      // Update status bar.
      //

      PrjT::MapT const& map = PrjT::inst().getMap(mEditor.mId); // This map.

      CString str;
      if (-1 != mCurHot) {
        int tile = map.mData[mCurHot];
        str.Format(_T("%d : %d (index %d)"), mCurHot % map.mWidth, mCurHot / map.mWidth, mCurHot);
        if (0 != tile) {
          CString str2;
          str2.Format(_T(" [tile %d]"), tile);
          str += str2;
        }
      }

      MainT::inst().mStatus.SetPaneText(2, str);
    }

    if (mMouseDown && EditorT::TOOL_FILL != mEditor.mCurTool) {
      ApplyMouseTool();
    }
  }

  LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
  {
    CScrollWindowImpl<CMapEditView>::OnMouseWheel(uMsg, wParam, lParam, bHandled);
    Invalidate(FALSE);

    return 0;
  }

  void OnRButtonDown(UINT nFlags, CPoint point)
  {
    if (GetFocus() != m_hWnd) {
      SetFocus();
    }

    PrjT::MapT const& map = PrjT::inst().getMap(mEditor.mId);

    if (EditorT::TOOL_DRAW != mEditor.mCurTool && EditorT::TOOL_ERASE != mEditor.mCurTool) {
      return;
    }

    mEditor.mCurPattern.clear();

    int prevSelTile = mEditor.mCurTile;

    if (-1 != mCurHot) {
      mEditor.mCurTile = map.mData[mCurHot] - 1;
      mLastSel = mLastCurHot = mCurHot;
      mRMouseDown = true;
    } else {
      mEditor.mCurTile = -1;
    }

    if (prevSelTile != mEditor.mCurTile) {
      mEditor.mTilesetView.Invalidate(FALSE);
    }
  }

  void OnRButtonUp(UINT nFlags, CPoint point)
  {
    if (!mRMouseDown) {
      return;
    }

    if (mLastSel != mLastCurHot) {
      PrjT::MapT const& map = PrjT::inst().getMap(mEditor.mId);

      int dx = mLastSel % map.mWidth, dy = mLastSel / map.mWidth; // Down xy.
      int cx = mLastCurHot % map.mWidth, cy = mLastCurHot / map.mWidth; // Last valid hot xy.
      if (dx < cx) std::swap(dx, cx);
      if (dy < cy) std::swap(dy, cy);

      mEditor.mSelRange.cx = dx - cx + 1;
      mEditor.mSelRange.cy = dy - cy + 1;

      for (int j = cy; j <= dy; j++) {
        for (int i = cx; i <= dx; i++) {
          int tile;
          if (EditorT::TOOL_ERASE == mEditor.mCurTool) {
            tile = 0;
          } else {
            tile = map.mData[i + j * map.mWidth];
          }
          mEditor.mCurPattern.push_back(tile);
        }
      }

      mEditor.mCurTile = -1;
      mEditor.mTilesetView.Invalidate(FALSE);
    }

    mEditor.mEditView.Invalidate(FALSE);

    mRMouseDown = false;
  }

  //
  // Override.
  //

  void DoPaint(CDCHandle dc)
  {
    PrjT& prj = PrjT::inst();
    PrjT::MapT const& map = prj.getMap(mEditor.mId);

    //
    // Calc and center log rect.
    //

    RECT rcClient;
    GetClientRect(&rcClient);

    int x = 0, y = 0;
    if (m_sizeAll.cx < rcClient.right) {
      x = (rcClient.right - m_sizeAll.cx) / 2;
    }

    if (m_sizeAll.cy < rcClient.bottom) {
      y = (rcClient.bottom - m_sizeAll.cy) / 2;
    }

    POINT pt = {x + m_ptOffset.x, y + m_ptOffset.y};

    RECT rc = {0, 0, m_sizeAll.cx, m_sizeAll.cy};
    ::OffsetRect(&rc, 0 == x ? 0 : pt.x, 0 == y ? 0 : pt.y);

    //
    // Calc device cell boundary.
    //

    RECT rcb = rc;
    OffsetRect(&rcb, -m_ptOffset.x, -m_ptOffset.y);

    int left = 0, top = 0, right = map.mWidth - 1, bottom = map.mHeight - 1; // Cell bound.
    if (0 > rcb.left || rcClient.right < rcb.right) {
      left = (int)(- rcb.left / map.mTileset.mTileWidth);
      right = min(map.mWidth - 1, (int)((rcClient.right - rcb.left) / map.mTileset.mTileWidth));
    }

    if (0 > rcb.top || rcClient.bottom < rcb.bottom) {
      top = (int)(- rcb.top / map.mTileset.mTileHeight);
      bottom = min(map.mHeight - 1, (int)((rcClient.bottom - rcb.top) / map.mTileset.mTileHeight));
    }

    //
    // Create mem DC.
    //

    CDC memdc;
    memdc.CreateCompatibleDC(dc);

    int bw = map.mTileset.mTileWidth * (right - left + 1);
    int bh = map.mTileset.mTileHeight * (bottom - top + 1);

    CBitmap membmp;
    membmp.CreateCompatibleBitmap(dc, bw, bh);
    memdc.SelectBitmap(membmp);

    {
      //
      // Block, erase bkgnd.
      //

      RECT rcbk = {0, 0, bw, bh};

      CBitmap pat;
      pat.LoadBitmap(IDB_BKBRUSH);

      CBrush br;
      br.CreatePatternBrush(pat);

      memdc.FillRect(&rcbk, br);
    }

    //
    // Draw map.
    //

    ImgT img = LoadTileset(PrjT::inst().getTex(map.mTileset.mTextureId).mFileName);
    if (img.isValid()) {
      CommonDrawMap(GxT(memdc), map, img, 0, 0, left, top, right, bottom, 0xffffffff);
    }

    //
    // Draw grid.
    //

    if (mEditor.mShowGrid) {

      //
      // Horz line(s).
      //

      for (size_t i = 0; i < map.mHorzGrid.size(); ++i) {
        good::ed::GridLine const& gl = map.mHorzGrid[i];
        CPen pen;
        pen.CreatePen(PS_SOLID, 1, gl.color);
        memdc.SelectPen(pen);
        int offset = gl.range - (top % gl.range);
        for (int j = -gl.range; j < bottom - top + 1; )  {
          memdc.MoveTo(0, map.mTileset.mTileHeight * (offset + j));
          memdc.LineTo(bw, map.mTileset.mTileHeight * (offset + j));
          j += gl.range;
        }
      }

      //
      // Vert line(s).
      //

      for (size_t i = 0; i < map.mVertGrid.size(); ++i) {
        good::ed::GridLine const& gl = map.mVertGrid[i];
        CPen pen;
        pen.CreatePen(PS_SOLID, 1, gl.color);
        memdc.SelectPen(pen);
        int offset = gl.range - (left % gl.range);
        for (int j = -gl.range; j < right - left + 1; ) {
          memdc.MoveTo(map.mTileset.mTileWidth * (offset + j), 0);
          memdc.LineTo(map.mTileset.mTileWidth * (offset + j), bh);
          j += gl.range;
        }
      }
    }

    //
    // Draw hot light tile.
    //

    if (mRMouseDown && (EditorT::TOOL_DRAW == mEditor.mCurTool || EditorT::TOOL_ERASE == mEditor.mCurTool)) {

      //
      // Selection tracking.
      //

      int dx = mLastSel % map.mWidth, dy = mLastSel / map.mWidth; // Down xy.
      int cx = mLastCurHot % map.mWidth, cy = mLastCurHot / map.mWidth; // Last valid hot xy.
      if (dx < cx) std::swap(dx, cx);
      if (dy < cy) std::swap(dy, cy);

      RECT rcd = {0, 0, (dx - cx + 1) * map.mTileset.mTileWidth, (dy - cy + 1) * map.mTileset.mTileHeight};
      OffsetRect(&rcd, map.mTileset.mTileWidth * (cx - left), map.mTileset.mTileHeight * (cy - top));

      unsigned int clr = RGB(0, 255, 0); // Draw tool.
      if (EditorT::TOOL_ERASE == mEditor.mCurTool) {
        clr = RGB(255, 0, 0);
      }

      DrawAlphaRect(
        CDCHandle((HDC)memdc),
        clr,
        rcd.left,
        rcd.top,
        rcd.right - rcd.left,
        rcd.bottom - rcd.top,
        120);
    } else if (-1 != mCurHot &&
               !mEditor.mCurPattern.empty() &&
               (EditorT::TOOL_DRAW == mEditor.mCurTool ||
                EditorT::TOOL_ERASE == mEditor.mCurTool)) {

      //
      // Has brush pattern.
      //

      int cx = mCurHot % map.mWidth, cy = mCurHot / map.mWidth; // cur hot xy

      RECT rcd = {0, 0, mEditor.mSelRange.cx * map.mTileset.mTileWidth, mEditor.mSelRange.cy * map.mTileset.mTileHeight};
      OffsetRect(&rcd, map.mTileset.mTileWidth * (cx - mEditor.mSelRange.cx / 2 - left), map.mTileset.mTileHeight * (cy - mEditor.mSelRange.cy / 2 - top));

      unsigned int clr = RGB(0, 255, 0); // Draw tool.
      if (EditorT::TOOL_ERASE == mEditor.mCurTool) {
        clr = RGB(255, 0, 0);
      }

      DrawAlphaRect(
        CDCHandle((HDC)memdc),
        clr,
        rcd.left,
        rcd.top,
        rcd.right - rcd.left,
        rcd.bottom - rcd.top,
        120);
    } else if (-1 != mCurHot) {

      //
      // Normal case.
      //

      int cx = mCurHot % map.mWidth, cy = mCurHot / map.mWidth; // Cur hot xy.

      RECT rcd = {0, 0, map.mTileset.mTileWidth, map.mTileset.mTileHeight};

      OffsetRect(&rcd, map.mTileset.mTileWidth * (cx - left), map.mTileset.mTileHeight * (cy - top));

      unsigned int clr = RGB(0, 255, 0); // Draw tool.
      if (1 == mEditor.mCurTool) {      // Erase tool.
        clr = RGB(255, 0, 0);
      } else if (2 == mEditor.mCurTool) { // Fill tool
        clr = RGB(255, 255, 0);
      }

      DrawAlphaRect(
        CDCHandle((HDC)memdc),
        clr,
        rcd.left,
        rcd.top,
        map.mTileset.mTileWidth,
        map.mTileset.mTileHeight,
        120);
    }

    //
    // Flip.
    //

    dc.BitBlt(rc.left + map.mTileset.mTileWidth * left, rc.top + map.mTileset.mTileHeight * top, bw, bh, memdc, 0, 0, SRCCOPY);
  }

  //
  // Util.
  //

  void ApplyMouseTool()
  {
    if (-1 == mCurHot) {
      return;
    }

    PrjT::MapT& map = PrjT::inst().getMap(mEditor.mId);

    int x = mCurHot % map.mWidth;
    int y = mCurHot / map.mWidth;

    bool bUpdate = false;

    switch (mEditor.mCurTool)
    {
    case EditorT::TOOL_DRAW:
      if (mEditor.mCurPattern.empty()) {
        bUpdate = map.draw(x, y, mEditor.mCurTile + 1);
      } else {
        bUpdate = map.draw(x, y, mEditor.mCurPattern, mEditor.mSelRange.cx, mEditor.mSelRange.cy);
      }
      break;

    case EditorT::TOOL_ERASE:
      if (mEditor.mCurPattern.empty()) {
        bUpdate = map.draw(x, y, 0);
      } else {
        bUpdate = map.draw(x, y, mEditor.mCurPattern, mEditor.mSelRange.cx, mEditor.mSelRange.cy);
      }
      break;

    case EditorT::TOOL_FILL:
      bUpdate = map.fill(x, y, mEditor.mCurTile + 1);
      break;
    }

    if (bUpdate) {
      mEditor.mEditView.Invalidate(FALSE);
    }

  }

  int GetTileIndexFromPoint(CPoint& point) const
  {
    RECT rcClient;
    GetClientRect(&rcClient);

    int x = 0, y = 0;
    if (m_sizeAll.cx < rcClient.right) {
      x = (rcClient.right - m_sizeAll.cx) / 2;
    }

    if (m_sizeAll.cy < rcClient.bottom) {
      y = (rcClient.bottom - m_sizeAll.cy) / 2;
    }

    RECT rc = {0, 0, m_sizeAll.cx, m_sizeAll.cy};
    ::OffsetRect(&rc, x - m_ptOffset.x, y - m_ptOffset.y);

    if (!PtInRect(&rc, point)) {
      return -1;
    }

    PrjT::MapT const& map = PrjT::inst().getMap(mEditor.mId);

    int w = (int)map.mTileset.mTileWidth;
    int h = (int)map.mTileset.mTileHeight;

    int xSel = (point.x - rc.left) / w;
    int ySel = (point.y - rc.top) / h;
    int xySel = xSel + ySel * map.mWidth;

    return xySel;
  }
};

template<class MainT>
class CMapEditor : public CWindowImpl<CMapEditor<MainT> >, public CUpdateUI<CMapEditor<MainT> >, public CIdleHandler
{
public:
  enum {
    TOOL_DRAW = 0,
    TOOL_ERASE,
    TOOL_FILL
  };

  int mPos;
  CSplitterWindow mSplit;

  CGoodPane mEditPane;
  CToolBarCtrl mTBEdit;

  CMapEditView<MainT, CMapEditor> mEditView;
  CGoodTilesetView<CMapEditor> mTilesetView;

  int mId;                              // Map ID.

  int mCurTool;                         // Selected tool, 0:draw, 1:erase, 2:fill, 3:sel, 4:move.
  bool mShowGrid;

  int mCurTile;                         // Single tile, valid if mCurPattern.empty() and not -1.

  std::vector<int> mCurPattern;         // Pattern.
  SIZE mSelRange;                       // Selection range(w,h).

  CMapEditor(int id) : mPos(210), mEditView(*this), mTilesetView(*this), mId(id)
  {
  }

  virtual void OnFinalMessage(HWND)     // Delete self when window destroy.
  {
    MainT::inst().ResetStatusBar();
    delete this;
  }

  virtual BOOL OnIdle()                 // Update UI states.
  {
    PrjT::MapT const& map = PrjT::inst().getMap(mId); // This map.

    //
    // Update toolbar.
    //

    UIEnable(ID_MAPEDIT_FILL, mCurPattern.empty());

    UISetCheck(ID_MAPEDIT_DRAW, TOOL_DRAW == mCurTool);
    UISetCheck(ID_MAPEDIT_ERASE, TOOL_ERASE == mCurTool);
    UISetCheck(ID_MAPEDIT_FILL, TOOL_FILL == mCurTool);

    UISetCheck(ID_MAPEDIT_GRID, mShowGrid);

    UIUpdateToolBar();

    //
    // Update status bar.
    //

    MainT& mf = MainT::inst();

    CString str;
    str.Format(_T("%d x %d (tile) %d x %d (pixel)"), map.mWidth, map.mHeight, map.mTileset.mTileWidth, map.mTileset.mTileHeight);
    mf.mStatus.SetPaneText(1, str);

    str.Empty();
    if (-1 != mEditView.mCurHot) {
      int tile = map.mData[mEditView.mCurHot];
      str.Format(_T("%d : %d (index %d)"), mEditView.mCurHot % map.mWidth, mEditView.mCurHot / map.mWidth, mEditView.mCurHot);
      if (0 != tile) {
        CString str2;
        str2.Format(_T(" [tile %d]"), tile);
        str += str2;
      }
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

  BEGIN_UPDATE_UI_MAP(CMapEditor)
    UPDATE_ELEMENT(ID_MAPEDIT_DRAW, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_MAPEDIT_ERASE, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_MAPEDIT_FILL, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_MAPEDIT_GRID, UPDUI_TOOLBAR)
  END_UPDATE_UI_MAP()

  BEGIN_MSG_MAP_EX(CMapEditor)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    MSG_WM_SIZE(OnSize)
    MSG_WM_GOOD(OnGoodMessage)
    COMMAND_ID_HANDLER_EX(ID_MAPEDIT_DRAW, OnDrawTool)
    COMMAND_ID_HANDLER_EX(ID_MAPEDIT_ERASE, OnEraseTool)
    COMMAND_ID_HANDLER_EX(ID_MAPEDIT_FILL, OnFillTool)
    COMMAND_ID_HANDLER_EX(ID_MAPEDIT_GRID, OnToggleShowGrid)
    COMMAND_ID_HANDLER_EX(ID_MAPEDIT_OPTION, OnOption)
    CHAIN_MSG_MAP(CUpdateUI<CMapEditor>)
    FORWARD_TOOLTIP_GETDISPINFO()
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    PrjT& prj = PrjT::inst();
    PrjT::MapT const& map = prj.getMap(mId); // This map.

    //
    // Property.
    //

    mCurTool = TOOL_DRAW;
    mShowGrid = true;
    mCurTile = -1;

    //
    // Create splits and panes.
    //

    mEditPane.Create(m_hWnd);
    mEditPane.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);

    mSplit.Create(mEditPane);
    mEditPane.SetClient(mSplit);

    //
    // Create toolbars.
    //

    mTBEdit = GoodCreateSimpleToolbar(mEditPane, IDR_TB_MAPEDIT);
    mTBEdit.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
    UIAddToolBar(mTBEdit);

    //
    // Create views.
    //

    mEditView.Create(mSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_STATICEDGE);
    mTilesetView.Create(mSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_STATICEDGE);

    mSplit.SetSplitterPanes(mEditView, mTilesetView);
    mSplit.SetActivePane(0);

    //
    // Init res.
    //

    mTilesetView.SelectTileset(map.mTileset);

    SetMsgHandled(FALSE);
    return 0;
  }

  int OnGoodMessage(int uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (WM_GOOD_GETRESOURCEID == uMsg) {
      return mId;
    }

    PrjT::MapT& map = PrjT::inst().getMap(mId); // This map.

    switch (uMsg)
    {
    case WM_GOOD_CANUNDO:
      return map.canUndo();

    case WM_GOOD_UNDO:
      if (map.undo()) {
        mEditView.Invalidate(FALSE);
      }
      return true;

    case WM_GOOD_CANREDO:
      return map.canRedo();

    case WM_GOOD_REDO:
      if (map.redo()) {
        mEditView.Invalidate(FALSE);
      }
      return true;
    }

    return 0;
  }

  void OnShowWindow(BOOL bShow, UINT nStatus)
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    if (bShow) {
      pLoop->AddIdleHandler(this);
    } else {
      pLoop->RemoveIdleHandler(this);
    }

    SetMsgHandled(FALSE);
  }

  void OnSize(UINT nType, CSize size)
  {
    mEditPane.SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOACTIVATE | SWP_NOZORDER);
    mSplit.SetSplitterPos(size.cx - mPos);
  }

  //
  // Command handler.
  //

  void OnDrawTool(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mCurTool = TOOL_DRAW;
    mCurPattern.clear();
  }

  void OnEraseTool(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mCurTool = TOOL_ERASE;
    mCurPattern.clear();
  }

  void OnFillTool(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mCurTool = TOOL_FILL;
  }

  void OnOption(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CDlgMapOption<MainT> dlg(mId);
    if (IDOK == dlg.DoModal()) {
      mEditView.Invalidate(FALSE);
    }
  }

  void OnToggleShowGrid(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mShowGrid = !mShowGrid;
    mEditView.Invalidate(FALSE);
  }
};

// end of EditMap.h
