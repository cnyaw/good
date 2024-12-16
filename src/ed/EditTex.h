
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

class CTextureEditView : public CTextureViewImpl<CTextureEditView>
{
public:
  DECLARE_WND_CLASS(NULL)

  CTextureEditView(int id) : CTextureViewImpl<CTextureEditView>(id)
  {
  }
};

template<class MainT>
class CTextureEditor : public CWindowImpl<CTextureEditor<MainT> >, public CIdleHandler
{
public:

  int mId;                              // Texture ID.

  CTextureEditView mEditView;

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
      str.Format(_T("%d x %d (px)"), img.getWidth(), img.getHeight());
    }

    MainT::inst().mStatus.SetPaneText(1, str);

    return FALSE;
  }

  BEGIN_MSG_MAP_EX(CTextureEditor)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    MSG_WM_SIZE(OnSize)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    mEditView.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
    mEditView.SelectTexture(mId);

    SetMsgHandled(FALSE);
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
