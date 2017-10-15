
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
    mEditor.mCurTile = -1;
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

// end of UtilWnd.h
