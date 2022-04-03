
//
// EditTex.h
// Texture editor.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/02/26 Waync created.
//

#pragma once

template<class MainT>
class CTextureEditView : public CTextureViewImpl<CTextureEditView<MainT> >
{
public:
  DECLARE_WND_CLASS(NULL)

  typedef CTextureViewImpl<CTextureEditView> BaseT;

  CTextureEditView(int id) : BaseT(id)
  {
  }

  BEGIN_MSG_MAP_EX(CTextureEditView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    CHAIN_MSG_MAP(BaseT)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    SelectTexture(mId);

    SetMsgHandled(FALSE);
    return 0;
  }

  void OnMouseMove(UINT nFlags, CPoint point)
  {
    MainT &m = MainT::inst();

    POINT pt = {point.x, point.y};
    if (pt.x > m_sizeAll.cx || pt.y > m_sizeAll.cy) {
      m.mStatus.SetPaneText(2, TEXT(""));
      return;
    }

    PrjT::TextureT& tex = PrjT::inst().getTex(mId);

    ImgT img = ImgT::getImage(tex.mFileName);
    if (!img.isValid()) {
      return;
    }

    unsigned int clr = img.getPixel(pt.x, pt.y);

    CString str;
    str.Format(_T("R:%d G:%d B:%d (#%06X)"), GetBValue(clr), GetGValue(clr), GetRValue(clr), clr);

    m.mStatus.SetPaneText(2, str);
  }
};

template<class MainT>
class CTextureEditor : public CWindowImpl<CTextureEditor<MainT> >, public CIdleHandler
{
public:

  int mId;                              // Texture ID.

  CTextureEditView<MainT> mEditView;

  CTextureEditor(int id) : mId(id), mEditView(id)
  {
  }

  virtual void OnFinalMessage(HWND)     // Delete self when window destroy.
  {
    MainT::inst().ResetStatusBar();
    delete this;
  }

  virtual BOOL OnIdle()                 // Update UI states.
  {
    PrjT::TextureT& tex = PrjT::inst().getTex(mId);
    ImgT img = ImgT::getImage(tex.mFileName);

    //
    // Update status bar.
    //

    CString str;
    if (img.isValid()) {
      str.Format(_T("%d x %d"), img.getWidth(), img.getHeight());
    }

    MainT::inst().mStatus.SetPaneText(1, str);

    return FALSE;
  }

  BEGIN_MSG_MAP_EX(CTextureEditor)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_GOOD(OnGoodMessage)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    MSG_WM_SIZE(OnSize)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    mEditView.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

    SetMsgHandled(FALSE);
    return 0;
  }

  int OnGoodMessage(int uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (WM_GOOD_GETRESOURCEID == uMsg) {
      return mId;
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
    mEditView.SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOACTIVATE | SWP_NOZORDER);
  }
};

// end of EditTex.h
