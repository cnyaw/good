
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

template<class T>
class CDlgPageMapGridImpl : public COwnerDraw<T>, public CDoubleBufferImpl<T>
{
public:
  enum DRAG_TYPE {
    DRAG_NEWVERTLINE,
    DRAG_NEWHORZLINE
  };

  struct FindGridRange
  {
    int range;

    FindGridRange(int r) : range(r)
    {
    }

    bool operator()(good::ed::GridLine const& gl)
    {
      return gl.range == range;
    }
  };

  struct SortGridLine
  {
    bool operator()(good::ed::GridLine const& gl1, good::ed::GridLine const& gl2) const
    {
      return gl1.range < gl2.range;
    }
  };

  bool bGridChanged;
  unsigned int mColor;                  // Current select color.

  bool mDragging;                       // Is dragging.
  int mDragType;                        // DRAG_TYPE.
  int mDragPos;                         // In tile,

  RECT mRcGrid, mRcNewVertLn, mRcNewHorzLn, mRcVertLn, mRcHorzLn;

  int &mWidth, &mHeight;
  int &mTileWidth, &mTileHeight;

  std::vector<good::ed::GridLine> &mVertGrid, &mHorzGrid;

  CDlgPageMapGridImpl(int& w, int& h, int& tw, int& th, std::vector<good::ed::GridLine>& vg, std::vector<good::ed::GridLine>& hg) :
    mWidth(w),
    mHeight(h),
    mTileWidth(tw),
    mTileHeight(th),
    mVertGrid(vg),
    mHorzGrid(hg)
  {
    bGridChanged = false;
    mColor = 0;
    mDragging = false;
  }

  BEGIN_MSG_MAP_EX(CDlgPageMapGridImpl)
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_LBUTTONUP(OnLButtonUp)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    COMMAND_ID_HANDLER_EX(IDC_COLOR, OnColor)
    CHAIN_MSG_MAP(COwnerDraw<T>)
    CHAIN_MSG_MAP(CDoubleBufferImpl<T>)
  END_MSG_MAP()

  //
  // Message handler.
  //

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    T* pThis = static_cast<T*>(this);

    ::GetWindowRect(pThis->GetDlgItem(IDC_GRID), &mRcGrid);
    pThis->ScreenToClient(&mRcGrid);

    RECT rcColor;
    ::GetWindowRect(pThis->GetDlgItem(IDC_COLOR), &rcColor);
    pThis->ScreenToClient(&rcColor);

    ::SetRect(&mRcNewVertLn, rcColor.left, mRcGrid.top, rcColor.right, mRcGrid.bottom);
    ::SetRect(&mRcNewHorzLn, mRcGrid.left, rcColor.top, mRcGrid.right, rcColor.bottom);
    ::SetRect(&mRcVertLn, mRcGrid.left, mRcGrid.bottom + 5, mRcGrid.right, mRcGrid.bottom + 35);
    ::SetRect(&mRcHorzLn, mRcGrid.right + 5, mRcGrid.top, mRcGrid.right + 35, mRcGrid.bottom);

    return TRUE;
  }

  void OnLButtonDown(UINT nFlags, CPoint point)
  {
    T* pThis = static_cast<T*>(this);

    if (::PtInRect(&mRcNewVertLn, point)) {
      mDragging = true;
      mDragType = DRAG_NEWVERTLINE;
      mDragPos = -1;
      pThis->SetCapture();
      pThis->Invalidate();
    } else if (::PtInRect(&mRcNewHorzLn, point)) {
      mDragging = true;
      mDragType = DRAG_NEWHORZLINE;
      mDragPos = -1;
      pThis->SetCapture();
      pThis->Invalidate();
    } else if (::PtInRect(&mRcVertLn, point)) {
      float cw = (mRcGrid.right - mRcGrid.left) / (float)(mWidth / 2);
      for (int i = (int)mVertGrid.size() - 1; 0 <= i; --i) {
        float t2 = cw * mVertGrid[i].range;
        RECT rcTxt = {0, 0, 20, 30};
        ::OffsetRect(&rcTxt, mRcGrid.left + (int)t2 - 10, mRcGrid.bottom + 5);
        if (::PtInRect(&rcTxt, point)) {
          mVertGrid.erase(mVertGrid.begin() + i);
          bGridChanged = true;
          pThis->Invalidate();
          break;
        }
      }
    } else if (::PtInRect(&mRcHorzLn, point)) {
      float ch = (mRcGrid.bottom - mRcGrid.top) / (float)(mHeight / 2);
      for (int i = (int)mHorzGrid.size() - 1; 0 <= i; --i) {
        float t2 = ch * mHorzGrid[i].range;
        RECT rcTxt = {0, 0, 30, 20};
        ::OffsetRect(&rcTxt, mRcGrid.right + 5, mRcGrid.top + (int)t2 - 10);
        if (::PtInRect(&rcTxt, point)) {
          mHorzGrid.erase(mHorzGrid.begin() + i);
          bGridChanged = true;
          pThis->Invalidate();
          break;
        }
      }
    }
  }

  void OnLButtonUp(UINT nFlags, CPoint point)
  {
    T* pThis = static_cast<T*>(this);

    if (*pThis == GetCapture()) {

      ReleaseCapture();

      if (-1 != mDragPos) {

        std::vector<good::ed::GridLine>& v = DRAG_NEWVERTLINE == mDragType ? mVertGrid : mHorzGrid;

        std::vector<good::ed::GridLine>::iterator it;
        it = std::find_if(v.begin(), v.end(), FindGridRange(mDragPos));

        if (v.end() == it) {

          //
          // Add new.
          //

          good::ed::GridLine gl;
          gl.color = mColor;
          gl.range = mDragPos;

          v.push_back(gl);
          std::sort(v.begin(), v.end(), SortGridLine());
          bGridChanged = true;

        } else {

          //
          // Update exist.
          //

          good::ed::GridLine& gl = *it;
          gl.color = mColor;
        }
      }

      pThis->Invalidate();
    }

    mDragging = false;
  }

  void OnMouseMove(UINT nFlags, CPoint point)
  {
    T* pThis = static_cast<T*>(this);

    if (!mDragging) {
      return;
    }

    int posLast = mDragPos;
    if (::PtInRect(&mRcGrid, point)) {
      int w = mRcGrid.right - mRcGrid.left;
      int h = mRcGrid.bottom - mRcGrid.top;
      float cw = w / (float)(mWidth / 2), ch = h / (float)(mHeight / 2);
      if (DRAG_NEWVERTLINE == mDragType) {
        mDragPos = (int)((point.x - mRcGrid.left) / cw);
        if (0 >= mDragPos || (mWidth / 2) < mDragPos) {
          mDragPos = -1;
        }
      } else {
        mDragPos = (int)((point.y - mRcGrid.top) / ch);
        if (0 >= mDragPos || (mHeight / 2) < mDragPos) {
          mDragPos = -1;
        }
      }
    } else {
      mDragPos = -1;
    }

    if (posLast != mDragPos) {
      pThis->Invalidate();
    }
  }

  void DoPaint(CDCHandle memdc)
  {
    T* pThis = static_cast<T*>(this);

    RECT rcClient;
    pThis->GetClientRect(&rcClient);

    memdc.FillSolidRect(&rcClient, ::GetSysColor(COLOR_BTNFACE));

    memdc.DrawFocusRect(&mRcGrid);
    memdc.DrawFocusRect(&mRcNewVertLn);
    memdc.DrawFocusRect(&mRcNewHorzLn);

    memdc.SetBkMode(TRANSPARENT);
    memdc.DrawText(CString((LPCTSTR)IDS_HELP_ADDGRIDLINE), -1, &mRcNewHorzLn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    int w = mRcGrid.right - mRcGrid.left;
    int h = mRcGrid.bottom - mRcGrid.top;
    float cw = w / (float)(mWidth / 2), ch = h / (float)(mHeight / 2);

    //
    // Horz line.
    //

    for (size_t i = 0; i < mHorzGrid.size(); ++i) {

      good::ed::GridLine const& gl = mHorzGrid[i];

      CPen pen;
      pen.CreatePen(PS_SOLID, 1, gl.color);
      memdc.SelectPen(pen);

      float t2 = ch * gl.range;
      memdc.MoveTo(mRcGrid.left, mRcGrid.top + (int)t2);
      memdc.LineTo(mRcGrid.right + 10, mRcGrid.top + (int)t2);

      RECT rcTxt = {0, 0, 30, 20};
      ::OffsetRect(&rcTxt, mRcGrid.right + 5, mRcGrid.top + (int)t2 - 10);

      memdc.SelectPen((HPEN)::GetStockObject(BLACK_PEN));
      memdc.Rectangle(&rcTxt);

      TCHAR s[32];
      memdc.DrawText(s, ::wsprintf(s, _T("%d"), gl.range), &rcTxt, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }

    //
    // Vert line.
    //

    for (size_t i = 0; i < mVertGrid.size(); ++i) {

      good::ed::GridLine const& gl = mVertGrid[i];

      CPen pen;
      pen.CreatePen(PS_SOLID, 1, gl.color);
      memdc.SelectPen(pen);

      float t2 = cw * gl.range;
      memdc.MoveTo(mRcGrid.left + (int)t2, mRcGrid.top);
      memdc.LineTo(mRcGrid.left + (int)t2, mRcGrid.bottom + 5);

      RECT rcTxt = {0, 0, 20, 30};
      ::OffsetRect(&rcTxt, mRcGrid.left + (int)t2 - 10, mRcGrid.bottom + 5);

      memdc.SelectPen((HPEN)::GetStockObject(BLACK_PEN));
      memdc.Rectangle(&rcTxt);

      TCHAR s[32];
      memdc.DrawText(s, ::wsprintf(s, _T("%d"), gl.range), &rcTxt, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }

    if (mDragging && -1 != mDragPos) {

      if (DRAG_NEWVERTLINE == mDragType) {

        CPen pen;
        pen.CreatePen(PS_SOLID, 1, mColor);
        memdc.SelectPen(pen);

        memdc.Rectangle(&mRcNewVertLn);

        float t2 = cw * mDragPos;
        memdc.MoveTo(mRcGrid.left + (int)t2, mRcGrid.top);
        memdc.LineTo(mRcGrid.left + (int)t2, mRcGrid.bottom);

        char s[32];
        memdc.DrawText(s, ::wsprintf(s, _T("%d"), mDragPos), &mRcNewVertLn, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      } else if (DRAG_NEWHORZLINE == mDragType) {

        CPen pen;
        pen.CreatePen(PS_SOLID, 1, mColor);
        memdc.SelectPen(pen);

        memdc.Rectangle(&mRcNewHorzLn);

        float t2 = ch * mDragPos;
        memdc.MoveTo(mRcGrid.left, mRcGrid.top + (int)t2);
        memdc.LineTo(mRcGrid.right, mRcGrid.top + (int)t2);

        char s[32];
        memdc.DrawText(s, ::wsprintf(s, _T("%d"), mDragPos), &mRcNewHorzLn, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      }
    }
  }

  //
  // Command handler.
  //

  void OnColor(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CColorDialog dlg(mColor, CC_FULLOPEN);
    if (IDOK == dlg.DoModal()) {
      mColor = dlg.m_cc.rgbResult;
      ::InvalidateRect(wndCtl, NULL, FALSE);
      ::UpdateWindow(wndCtl);
    }
  }

  //
  // Override, COwnerDraw.
  //

  void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
  {
    if (IDC_COLOR == lpDrawItemStruct->CtlID) { // Color button.

      CDCHandle dc(lpDrawItemStruct->hDC);
      dc.Rectangle(&lpDrawItemStruct->rcItem);
      ::InflateRect(&lpDrawItemStruct->rcItem, -2, -2);

      if (lpDrawItemStruct->itemState & ODS_SELECTED) {
        ::InflateRect(&lpDrawItemStruct->rcItem, -1, -1);
      }

      dc.FillSolidRect(&lpDrawItemStruct->rcItem, mColor);
    }
  }
};

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
