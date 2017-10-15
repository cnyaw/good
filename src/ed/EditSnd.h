
//
// EditSnd.h
// Sound editor.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/08/08 Waync created.
//

#pragma once

template<class MainT>
class CSoundEditor : public CWindowImpl<CSoundEditor<MainT> >, public CUpdateUI<CSoundEditor<MainT> >, public CIdleHandler
{
public:

  int mId;

  CGoodPane mPreviewPane;
  CToolBarCtrl mTBPreview;

  CSoundEditor(int id) : mId(id)
  {
  }

  virtual void OnFinalMessage(HWND)     // Delete self when window destroy.
  {
    delete this;
  }

  virtual BOOL OnIdle()                 // Update UI states.
  {
    PrjT::SoundT const& snd = PrjT::inst().getSnd(mId);

    UISetCheck(ID_SPRITEPREVIEW_LOOP, snd.mLoop);
    UIEnable(ID_SPRITEPREVIEW_PLAY, FALSE);
    UIEnable(ID_SPRITEPREVIEW_PAUSE, FALSE);
    UIEnable(ID_SPRITEPREVIEW_STOP, FALSE);

    UIUpdateToolBar();

    return FALSE;
  }

  BEGIN_UPDATE_UI_MAP(CSoundEditor)
    UPDATE_ELEMENT(ID_SPRITEPREVIEW_LOOP, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_SPRITEPREVIEW_PLAY, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_SPRITEPREVIEW_PAUSE, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_SPRITEPREVIEW_STOP, UPDUI_TOOLBAR)
  END_UPDATE_UI_MAP()

  BEGIN_MSG_MAP_EX(CSoundEditor)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_GOOD(OnGoodMessage)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    MSG_WM_SIZE(OnSize)
    FORWARD_TOOLTIP_GETDISPINFO()
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    ModifyStyleEx(0, WS_EX_STATICEDGE, 0);

    mPreviewPane.Create(m_hWnd);
    mPreviewPane.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);

    mTBPreview = GoodCreateSimpleToolbar(mPreviewPane, IDR_TB_SPRITEPREVIEW, 4);
    UIAddToolBar(mTBPreview);

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
    MainT::inst().ResetStatusBar();

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    if (bShow) {
      pLoop->AddIdleHandler(this);
    } else {
      //Stop();
      pLoop->RemoveIdleHandler(this);
    }

    SetMsgHandled(FALSE);
  }

  void OnSize(UINT nType, CSize size)
  {
    mPreviewPane.SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOACTIVATE | SWP_NOZORDER);
  }
};

// end of EditSnd.h
