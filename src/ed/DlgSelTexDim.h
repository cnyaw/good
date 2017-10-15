
//
// DlgSelTexDim.h
// Level editor select texture dimenstion.
//
// Copyright (c) 2014 Waync Cheng.
// All Rights Reserved.
//
// 2014/06/18 Waync created.
//

#pragma once

class CSelTextureDimView : public CTextureViewImpl<CSelTextureDimView>
{
public:
  DECLARE_WND_CLASS(NULL)

  typedef CTextureViewImpl<CSelTextureDimView> BaseT;

  RECT mDim;

  bool mMouseDown, mResize;
  CPoint mPtDown;

  CSelTextureDimView(int id, int x, int y, int w, int h) : BaseT(id), mMouseDown(false)
  {
    SetRect(&mDim, 0, 0, w, h);
    OffsetRect(&mDim, x, y);
  }

  BEGIN_MSG_MAP_EX(CSelTextureDimView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_LBUTTONUP(OnLButtonUp)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    CHAIN_MSG_MAP(BaseT)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    SelectTexture(mId);

    if (IsRectEmpty(&mDim)) {
      PrjT::TextureT& tex = PrjT::inst().getTex(mId);
      ImgT img = ImgT::getImage(tex.mFileName);
      if (img.isValid()) {
        int x = mDim.left, y = mDim.top;
        int w = img.getWidth(), h = img.getHeight();
        SetRect(&mDim, 0, 0, w, h);
        OffsetRect(&mDim, x, y);
      }
    }

    return 0;
  }

  BOOL OnEraseBkgnd(CDCHandle dc)
  {
    return TRUE;
  }

  void OnLButtonDown(UINT nFlags, CPoint point)
  {
    point.x += m_ptOffset.x;
    point.y += m_ptOffset.y;

    if (!PtInRect(&mDim, point)) {
      return;
    }

    mMouseDown = true;
    mPtDown = point;

    RECT rcResize;
    SetRect(&rcResize, 0, 0, 10, 10);
    OffsetRect(&rcResize, mDim.right - 10, mDim.bottom - 10);

    mResize = 0 != PtInRect(&rcResize, point);
  }

  void OnLButtonUp(UINT nFlags, CPoint point)
  {
    mMouseDown = false;

    if (mDim.right < mDim.left) {
      std::swap(mDim.right, mDim.left);
    }

    if (mDim.bottom < mDim.top) {
      std::swap(mDim.bottom, mDim.top);
    }

    Invalidate(FALSE);
  }

  void OnMouseMove(UINT nFlags, CPoint point)
  {
    if (!mMouseDown) {
      return;
    }

    point.x += m_ptOffset.x;
    point.y += m_ptOffset.y;

    if (mResize) {
      if (mPtDown.x <= mDim.left || mPtDown.y <= mDim.top) {
        if (mPtDown.x <= mDim.left) {
          mDim.left = point.x;
        }
        if (mPtDown.y <= mDim.top) {
          mDim.top = point.y;
        }
      } else {
        mDim.right += point.x - mPtDown.x;
        mDim.bottom += point.y - mPtDown.y;
      }
    } else {
      OffsetRect(&mDim, point.x - mPtDown.x, point.y - mPtDown.y);
    }

    mPtDown = point;

    Invalidate(FALSE);
  }

  //
  // Override.
  //

  void DoPaint(CDCHandle dc)
  {
    //
    // Instead use GetClientRect, use CScrollWindowImpl::m_ptOffset,
    // m_sizeClient to get current client area.
    //

    RECT rcPaint;
    SetRect(&rcPaint, 0, 0, m_sizeClient.cx, m_sizeClient.cy);
    OffsetRect(&rcPaint, m_ptOffset.x, m_ptOffset.y);

    CMemoryDC memdc(dc, rcPaint);

    memdc.FillRect(&rcPaint, COLOR_APPWORKSPACE);

    //
    // Draw texture.
    //

    BaseT::DoPaint((HDC)memdc);

    //
    // Draw selection.
    //

    CPen redPen;
    redPen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

    memdc.SelectBrush((HBRUSH)::GetStockObject(NULL_BRUSH));
    memdc.SelectPen(redPen);

    RECT rc = mDim;
    memdc.Rectangle(&rc);

    rc.left = rc.right - 10;
    rc.top = rc.bottom - 10;

    CBrush br;
    br.CreateSolidBrush(RGB(255, 0, 0));

    memdc.FillRect(&rc, br);

    //
    // Draw info.
    //

    if (mMouseDown) {
      CString s;
      s.Format(_T("(%d, %d) - (%d, %d)"), mDim.left, mDim.top, abs(mDim.right - mDim.left), abs(mDim.bottom - mDim.top));
      memdc.TextOut(mDim.right + 2, mDim.bottom + 2, s);
    }
  }
};

class CDlgSelTextureDim : public CDialogImpl<CDlgSelTextureDim>
{
public:
  enum { IDD = IDD_SELTEXDIM };

  CSelTextureDimView mView;

  CDlgSelTextureDim(int id, int x, int y, int w, int h) : mView(id, x, y, w, h)
  {
  }

  BEGIN_MSG_MAP_EX(CDlgSelTextureDim)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
    COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
    COMMAND_ID_HANDLER_EX(IDC_BUTTON1, OnSelectAll)
  END_MSG_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    CenterWindow(GetParent());

    RECT rc;
    GetClientRect(&rc);

    rc.bottom -= 30;
    InflateRect(&rc, -16, -16);

    mView.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP, WS_EX_STATICEDGE);

    mView.SetFocus();

    return FALSE;                       // I want to control which is focus window.
  }

  void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (IDOK == nID) {
      PrjT::TextureT& tex = PrjT::inst().getTex(mView.mId);
      ImgT img = ImgT::getImage(tex.mFileName);
      if (img.isValid()) {
        RECT &dim = mView.mDim;
        if (0 == dim.left && 0 == dim.top &&
            img.getWidth() == dim.right - dim.left &&
            img.getHeight() == dim.bottom - dim.top) {
          SetRectEmpty(&dim);
        }
      }
    }

    EndDialog(nID);
  }

  void OnSelectAll(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::TextureT& tex = PrjT::inst().getTex(mView.mId);
    ImgT img = ImgT::getImage(tex.mFileName);
    if (img.isValid()) {
      SetRect(&mView.mDim, 0, 0, img.getWidth(), img.getHeight());
      mView.SetFocus();
      mView.Invalidate(FALSE);
    }
  }
};

// end of DlSelTexDim.h
