
//
// UtilWnd.h
// Common window implementation.
//
// Copyright (c) 2014 Waync Cheng.
// All Rights Reserved.
//
// 2014/06/20 Waync created.
//

#pragma once

template<class T>
class CScrollEraseBkgndImpl
{
public:

  BEGIN_MSG_MAP_EX(CScrollEraseBkgndImpl)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
  END_MSG_MAP()

  LRESULT OnEraseBkgnd(CDCHandle dc)
  {
    T* pt = static_cast<T*>(this);

    RECT rcClient;
    pt->GetClientRect(&rcClient);

    int x = 0, y = 0;
    if (pt->m_sizeAll.cx < rcClient.right) {
      x = (rcClient.right - pt->m_sizeAll.cx) / 2;
    }

    if (pt->m_sizeAll.cy < rcClient.bottom) {
      y = (rcClient.bottom - pt->m_sizeAll.cy) / 2;
    }

    RECT rc = {0, 0, pt->m_sizeAll.cx, pt->m_sizeAll.cy};
    ::OffsetRect(&rc, 0 == x ? 0 : x, 0 == y ? 0 : y);

    if (rc.right - rc.left < rcClient.right) {
      RECT rcLeft = {0, 0, rc.left, rcClient.bottom};
      dc.FillRect(&rcLeft, COLOR_APPWORKSPACE);
      RECT rcRight = {rc.right, 0, rcClient.right, rcClient.bottom};
      dc.FillRect(&rcRight, COLOR_APPWORKSPACE);
    }

    if (rc.bottom - rc.top < rcClient.bottom) {
      RECT rcTop = {rc.left, 0, rc.right, rc.top};
      dc.FillRect(&rcTop, COLOR_APPWORKSPACE);
      RECT rcBottom = {rc.left, rc.bottom, rc.right, rcClient.bottom};
      dc.FillRect(&rcBottom, COLOR_APPWORKSPACE);
    }

    return TRUE;                        // Handled.
  }
};

class CGoodPane : public CPaneContainerImpl<CGoodPane>
{
public:
  enum {
    mCxButton = 23,
    mCyButton = 22
  };

  typedef CPaneContainerImpl<CGoodPane> BaseT;

  BEGIN_MSG_MAP_EX(CGoodPane)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
    CHAIN_MSG_MAP(CPaneContainerImpl<CGoodPane>)
  END_MSG_MAP()

  //
  // Message handler.
  //

  BOOL OnEraseBkgnd(CDCHandle dc)
  {
    return FALSE;                       // Force erase bkgnd.
  }

  //
  // Override.
  //

  void CalcSize()
  {
    CFontHandle font = BaseT::GetTitleFont();
    LOGFONT lf = { 0 };
    font.GetLogFont(lf);

    int cyFont = abs(lf.lfHeight) + m_cxyBorder + 2 * m_cxyTextOffset;
    int cyBtn = mCyButton + 2 * m_cxyBorder + m_cxyBtnOffset;
    m_cxyHeader = max(cyFont, cyBtn);
  }
};

template<class T>
class CTextureViewImpl : public CScrollWindowImpl<T>
{
public:

  int mId;                              // Texture ID.

  CTextureViewImpl() : mId(-1)
  {
  }

  CTextureViewImpl(int id) : mId(id)
  {
  }

  void SelectTexture(int id)
  {
    PrjT::TextureT& tex = PrjT::inst().getTex(id);

    ImgT img = ImgT::getImage(tex.mFileName);
    if (img.isValid()) {
      SIZE sz = {img.getWidth(), img.getHeight()};
      SetScrollOffset(0, 0, FALSE);
      SetScrollSize(sz);
      mId = id;
    }
  }

  BEGIN_MSG_MAP_EX(CTextureViewImpl)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
    CHAIN_MSG_MAP(CScrollWindowImpl<T>)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnEraseBkgnd(CDCHandle dc)
  {
    RECT rect;
    GetClientRect(&rect);

    if(rect.right > m_sizeAll.cx) {
      RECT rc = rect;
      rc.left = m_sizeAll.cx;
      rc.bottom = m_sizeAll.cy;
      dc.FillRect(&rc, COLOR_APPWORKSPACE);
    }

    if(rect.bottom > m_sizeAll.cy) {
      RECT rc = rect;
      rc.top = m_sizeAll.cy;
      dc.FillRect(&rc, COLOR_APPWORKSPACE);
    }

    return TRUE;
  }

  //
  // Override.
  //

  void DoPaint(CDCHandle dc)
  {
    if (-1 == mId) {
      return;
    }

    PrjT::TextureT& tex = PrjT::inst().getTex(mId);

    ImgT img = ImgT::getImage(tex.mFileName);
    if (!img.isValid()) {
      return;
    }

    CDC memdc;
    memdc.CreateCompatibleDC(dc);
    memdc.SelectBitmap(img.mBmp);

    CDC memdc2;
    memdc2.CreateCompatibleDC(dc);

    CBitmap membmp2;
    membmp2.CreateCompatibleBitmap(dc, img.getWidth(), img.getHeight());
    memdc2.SelectBitmap(membmp2);

    if (img.hasAlphaChannel()) {

      //
      // Fill grid pattern.
      //

      CBitmap pat;
      pat.LoadBitmap(IDB_BKBRUSH);

      CBrush br;
      br.CreatePatternBrush(pat);

      RECT rcb = {0, 0, img.getWidth(), img.getHeight()};
      memdc2.FillRect(&rcb, br);

      //
      // Blend bmp.
      //

      BLENDFUNCTION bf;
      bf.BlendOp = AC_SRC_OVER;
      bf.BlendFlags = 0;
      bf.SourceConstantAlpha = 255;
      bf.AlphaFormat = AC_SRC_ALPHA;

      memdc2.AlphaBlend(0, 0, img.getWidth(), img.getHeight(), memdc, 0, 0, img.getWidth(), img.getHeight(), bf);
    } else {
      memdc2.BitBlt(0, 0, img.getWidth(), img.getHeight(), memdc, 0, 0, SRCCOPY);
    }

    dc.BitBlt(0, 0, img.getWidth(), img.getHeight(), memdc2, 0, 0, SRCCOPY);
  }
};

template<class EditorT>
class CGoodTilesetView : public CTextureViewImpl<CGoodTilesetView<EditorT> >
{
public:
  DECLARE_WND_CLASS(NULL)

  typedef CTextureViewImpl<CGoodTilesetView> BaseT;

  int mCurHot;
  int mLastCurTile;
  bool mMouseDown;

  int mTileWidth, mTileHeight;
  int mCxTile, mCyTile;

  EditorT& mEditor;

  CGoodTilesetView(EditorT& ed) : mEditor(ed), mCurHot(-1), mMouseDown(false)
  {
  }

  void SelectTileset(good::Tileset const& ts)
  {
    mEditor.mCurTile = mLastCurTile = 0;

    mTileWidth = ts.mTileWidth;
    mTileHeight = ts.mTileHeight;
    mCxTile = ts.mCxTile;
    mCyTile = ts.mCyTile;

    BaseT::SelectTexture(ts.mTextureId);
  }

  BEGIN_MSG_MAP_EX(CGoodTilesetView)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_LBUTTONUP(OnLButtonUp)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    MSG_WM_RBUTTONDOWN(OnLButtonDown)
    MSG_WM_RBUTTONUP(OnLButtonUp)
    CHAIN_MSG_MAP(BaseT)
  END_MSG_MAP()

  //
  // Message handler.
  //

  void OnLButtonDown(UINT nFlags, CPoint point)
  {
    if (-1 == BaseT::mId) {
      return;
    }

    SetCapture();

    mMouseDown = true;

    mEditor.mCurTile = mLastCurTile = mCurHot;
    mEditor.mCurPattern.clear();

    Invalidate(FALSE);
  }

  void OnLButtonUp(UINT nFlags, CPoint point)
  {
    if (-1 == BaseT::mId) {
      return;
    }

    if (GetCapture() == m_hWnd) {
      ReleaseCapture();
    }

    mMouseDown = false;

    int xSel = mEditor.mCurTile % mCxTile;
    int ySel = mEditor.mCurTile / mCxTile;
    int xSel2 = mLastCurTile % mCxTile;
    int ySel2 = mLastCurTile / mCxTile;

    if (xSel > xSel2) {
      std::swap(xSel, xSel2);
    }

    if (ySel > ySel2) {
      std::swap(ySel, ySel2);
    }

    mEditor.mSelRange.cx = xSel2 - xSel + 1;
    mEditor.mSelRange.cy = ySel2 - ySel + 1;

    if (mEditor.mCurTile != mLastCurTile) {
      for (int j = ySel; j <= ySel2; ++j) {
        for (int i = xSel; i <= xSel2; ++i) {
          mEditor.mCurPattern.push_back(j * mCxTile + i + 1);
        }
      }
    }
  }

  void OnMouseMove(UINT nFlags, CPoint point)
  {
    if (-1 == BaseT::mId) {
      mCurHot = -1;
      return;
    }

    POINT pt = {point.x + m_ptOffset.x, point.y + m_ptOffset.y};
    RECT rc = {0, 0, mCxTile * mTileWidth, mCyTile * mTileHeight};

    if (!PtInRect(&rc, pt)) {
      return;
    }

    int xSel = pt.x / mTileWidth;
    int ySel = pt.y / mTileHeight;

    int prevHot = mCurHot;

    mCurHot = xSel + ySel * mCxTile;

    if (mMouseDown && prevHot != mCurHot) {
      mLastCurTile = mCurHot;
      Invalidate(FALSE);
    }
  }

  //
  // Override.
  //

  void DoPaint(CDCHandle dc)
  {
    if (-1 == mId) {
      return;
    }

    CDC memdc;
    memdc.CreateCompatibleDC(dc);

    CBitmap membmp;
    membmp.CreateCompatibleBitmap(dc, mCxTile * mTileWidth, mCyTile * mTileHeight);
    memdc.SelectBitmap(membmp);

    BaseT::DoPaint((HDC)memdc);

    //
    // Draw selection.
    //

    if (-1 == mEditor.mCurTile) {
      return;
    }

    if (mEditor.mCurPattern.empty() && !mMouseDown) {
      mLastCurTile = mEditor.mCurTile;
    }

    int sel1 = mEditor.mCurTile, sel2 = mLastCurTile;

    int xSel = sel1 % mCxTile;
    int ySel = sel1 / mCxTile;
    int xSel2 = sel2 % mCxTile;
    int ySel2 = sel2 / mCxTile;

    if (xSel > xSel2) {
      std::swap(xSel, xSel2);
    }

    if (ySel > ySel2) {
      std::swap(ySel, ySel2);
    }

    RECT rc = {0, 0, (xSel2 - xSel + 1) * mTileWidth, (ySel2 - ySel + 1) * mTileHeight};
    ::OffsetRect(&rc, xSel * mTileWidth, ySel * mTileHeight);

    DrawAlphaRect((HDC)memdc, RGB(255, 0, 0), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 100);

    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(255,0,0));

    memdc.SelectPen(pen);
    memdc.SelectBrush((HBRUSH)::GetStockObject(NULL_BRUSH));
    memdc.Rectangle(rc.left, rc.top, rc.right, rc.bottom);

    RECT rcClient;
    GetClientRect(&rcClient);

    dc.BitBlt(m_ptOffset.x, m_ptOffset.y, rcClient.right, rcClient.bottom, memdc, m_ptOffset.x, m_ptOffset.y, SRCCOPY);
  }
};

template<class MainT>
class CResourceListView : public CScrollWindowImpl<CResourceListView<MainT> >
{
public:

  CPen mPenSelBorder;

  int mCurHot, mCurSel;
  int CX_THUMB, CY_THUMB, CXY_BORDER;

  std::map<int, good::gx::GxImage> mThumbImg;

  virtual ~CResourceListView()
  {
    FreeImage();
  }

  void Blend(HDC hdc, int x, int y, const good::gx::GxImage &gx) const
  {
    if (0 == gx.dat) {
      return;
    }

    HDC memdc = CreateCompatibleDC(hdc);
    HBITMAP membmp = CreateCompatibleBitmap(hdc, gx.w, gx.h);
    membmp = (HBITMAP)SelectObject(memdc, membmp);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = gx.w;
    bmi.bmiHeader.biHeight = -gx.h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biClrImportant = BI_RGB;
    bmi.bmiHeader.biXPelsPerMeter = bmi.bmiHeader.biYPelsPerMeter = 1;
    SetDIBitsToDevice(memdc, 0, 0, gx.w, gx.h, 0, 0, 0, gx.h, gx.dat, &bmi, DIB_RGB_COLORS);

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;
    AlphaBlend(hdc, x, y, gx.w, gx.h, memdc, 0, 0, gx.w, gx.h, bf);

    DeleteObject(SelectObject(memdc, membmp));
    DeleteDC(memdc);
  }

  void FreeImage()
  {
    std::map<int, good::gx::GxImage>::iterator it = mThumbImg.begin();
    for (; mThumbImg.end() != it; ++it) {
      it->second.release();
    }
    mThumbImg.clear();
  }

  virtual int GetResCount() const=0;
  virtual int GetResId(int sel) const=0;
  virtual std::string GetResName(int id) const=0;
  virtual int GetResType() const=0;
  virtual bool LoadResImage(int id, good::gx::GxImage &img) const=0;

  void SetList()
  {
    mCurHot = mCurSel = -1;
    mThumbImg.clear();
    RECT rcClient;
    GetClientRect(&rcClient);
    CSize sz(rcClient.right, rcClient.bottom);
    OnSize(0, sz);
  }

  BEGIN_MSG_MAP_EX(CResourceListView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    MSG_WM_SIZE(OnSize)
    CHAIN_MSG_MAP(CScrollWindowImpl<CResourceListView>)
  END_MSG_MAP()

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    mCurHot = mCurSel = -1;
    mPenSelBorder.CreatePen(PS_SOLID, 3, GetSysColor(COLOR_HIGHLIGHT));
    SetClassLong(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE) | CS_DBLCLKS);
    SetMsgHandled(FALSE);
    return 0;
  }

  BOOL OnEraseBkgnd(CDCHandle dc)
  {
    return FALSE;
  }

  void OnLButtonDblClk(UINT nFlags, CPoint point)
  {
    if (-1 == mCurSel) {
      return;
    }
    MainT::inst().mExpView.AddEditorView(GetResType(), GetResId(mCurSel));
  }

  void OnLButtonDown(UINT nFlags, CPoint point)
  {
    if (-1 == mCurHot || mCurSel == mCurHot) {
      return;
    }
    mCurSel = mCurHot;
    Invalidate(FALSE);
    MainT::inst().mExpView.SetCurSel(GetResId(mCurSel));
  }

  void OnMouseMove(UINT nFlags, CPoint point)
  {
    int nItem = GetResCount();

    RECT rcClient;
    GetClientRect(&rcClient);

    int lastHot = mCurHot;
    size_t cxMaxTile = max(1, rcClient.right / CX_THUMB);

    int cx = cxMaxTile;
    int cy = 1 + nItem / cxMaxTile;

    RECT rcBound = {0, 0, cx * CX_THUMB, cy * CY_THUMB};
    if (PtInRect(&rcBound, point)) {
      int x = point.x / CX_THUMB;
      int y = (point.y + m_ptOffset.y) / CY_THUMB;
      mCurHot = x + y * cxMaxTile;
      if (nItem <= mCurHot) {
        mCurHot = -1;
      }
    } else {
      mCurHot = -1;
    }
  }

  void OnSize(UINT nType, CSize size)
  {
    int nItem = GetResCount();

    if (!IsWindow() || 0 >= nItem) {
      return;
    }

    size_t cxMaxTile = max(1, size.cx / CX_THUMB);
    int y = (0 != (nItem % cxMaxTile)) + (nItem / cxMaxTile);

    SIZE sz = {cxMaxTile * CX_THUMB, y * CY_THUMB};
    SetScrollSize(sz);

    SetMsgHandled(FALSE);
  }

  // Overrideables
  void DoPaint(CDCHandle dc)
  {
    RECT rcClient;
    GetClientRect(&rcClient);

    size_t cxMaxTile = max(1, rcClient.right / CX_THUMB);

    ::OffsetRect(&rcClient, m_ptOffset.x, m_ptOffset.y);

    CMemoryDC mdc(dc, rcClient);
    mdc.FillRect(&rcClient, COLOR_WINDOW);
    SelectObject(mdc, GetStockObject(DEFAULT_GUI_FONT));

    // draw thumb
    bool bLoadImageOneTime = false;

    mdc.SelectBrush((HBRUSH)::GetStockObject(NULL_BRUSH));

    for (int i = 0; i < GetResCount(); ++i) {
      size_t x = i % cxMaxTile, y = i / cxMaxTile;

      RECT rc = {0, 0, CX_THUMB, CY_THUMB};
      ::OffsetRect(&rc, CX_THUMB * x, CY_THUMB * y);

      RECT rcInt;
      if (!::IntersectRect(&rcInt, &rcClient, &rc)) {
        continue;
      }

      int id = GetResId(i);

      std::map<int, good::gx::GxImage>::iterator it = mThumbImg.find(id);

      // draw image border
      if (mCurSel == i) {
        mdc.SelectPen(mPenSelBorder);
        int w = CX_THUMB - CXY_BORDER/2;
        int h = CY_THUMB - CXY_BORDER;
        int x = rc.left + CXY_BORDER/4;
        int y = rc.top;
        mdc.Rectangle(x, y, x + w, y + h);
      }

      // draw cached image
      if (mThumbImg.end() != it) {
        Blend(mdc, rc.left + CXY_BORDER + (CX_THUMB - it->second.w - 2 * CXY_BORDER)/2, rc.top + (CY_THUMB - CXY_BORDER - it->second.h)/2, it->second);
      }

      // try to load a image(only this time)
      else if (!bLoadImageOneTime) {
        good::gx::GxImage img;
        if (LoadResImage(id, img)) {
          int ow = img.w, oh = img.h;
          if (CX_THUMB < ow || CY_THUMB < oh) {
            float dw = (CX_THUMB - 3 * CXY_BORDER) / (float)ow;
            float dh = (CY_THUMB - 2 * CXY_BORDER) / (float)oh;
            float scale = min(dw, dh) ;
            img.convert32();
            img.resize((int)(ow * scale), (int)(oh * scale));
          }
          mThumbImg[id] = img;
          img.dat = 0;
          bLoadImageOneTime = true;
          Invalidate(FALSE);
        }
      }

      // draw name
      rc.top = rc.bottom - CXY_BORDER;

      if (mCurSel == i) {
        mdc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
        mdc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
      } else {
        mdc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
        mdc.SetBkColor(::GetSysColor(COLOR_WINDOW));
      }

      std::string name = GetResName(id);
      mdc.DrawText(name.c_str(), name.size(), &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
    }
  }
};

// end of UtilWnd.h
