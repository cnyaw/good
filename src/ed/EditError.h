
//
// EditError.h
// Debug output.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/04/08 Waync created
//

#pragma once

template<class MainT>
class CErrorEditor : public CGoodPane
{
public:

  CToolBarCtrl mToolbar;

  CFont mFon;
  CEdit mEdit;

  bool mVisible;

  BEGIN_MSG_MAP_EX(CErrorEditor)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_GOOD(OnGoodMessage)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    if (WM_USER + 99887 == uMsg) {
      mVisible = false;                 // Force visible.
      MainT::inst().OnViewOutputWindow(0, 0, 0);
      return TRUE;
    }
    COMMAND_ID_HANDLER_EX(ID_EDIT_CLEAR_ALL, OnClearAll)
    CHAIN_MSG_MAP(CGoodPane)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);

    //
    // Toolbar.
    //

    mToolbar = GoodCreateSimpleToolbar(m_hWnd, IDR_TB_OUTPUT);

    //
    // Edit.
    //

    mEdit.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN, WS_EX_CLIENTEDGE);

    LOGFONT fon;
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &fon);

    ::lstrcpy(fon.lfFaceName, _T("Courier New"));
    fon.lfHeight = 15;

    mFon.CreateFontIndirect(&fon);

    mEdit.SetFont(mFon);

    SetClient(mEdit);

    mHwndOutputView = m_hWnd;
    mHwndOutputEdit = mEdit.m_hWnd;

    mVisible = false;

    SetMsgHandled(FALSE);
    return 0;
  }

  int OnGoodMessage(int uMsg, WPARAM wParam, LPARAM lParam)
  {
    switch (uMsg)
    {
    case WM_GOOD_CANUNDO:
      return mEdit.CanUndo();

    case WM_GOOD_UNDO:
      mEdit.Undo();
      return true;
    }

    return 0;
  }

  void OnShowWindow(BOOL bShow, UINT nStatus)
  {
    MainT::inst().ResetStatusBar();

    SetMsgHandled(FALSE);
  }

  void OnClearAll(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mEdit.SetSel(0, -1);
    mEdit.Clear();
  }
};

// end of EditError.h
