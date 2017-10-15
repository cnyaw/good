
//
// EditScript.h
// Script editor.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/03/27 Waync created.
//

#pragma once

CFindReplaceDialog** m_gFindReplaceDialog = NULL;

class CScriptView :
  public CWindowImpl<CScriptView, CEdit>,
  public CEditCommands<CScriptView>,
  public CEditFindReplaceImpl<CScriptView>
{
public:
  enum { MARGIN = 16 };

  CCursor mCursor;
  bool bFindText;

  BOOL FindTextSimple(LPCTSTR lpszFind, BOOL bMatchCase, BOOL bWholeWord, BOOL bFindDown = TRUE)
  {
    bFindText = true;
    BOOL ret = CEditFindReplaceImpl<CScriptView>::FindTextSimple(lpszFind, bMatchCase, bWholeWord, bFindDown);
    bFindText = false;
    return ret;
  }

  void GetSel(int& nStartChar, int& nEndChar) const
  {
    CEdit::GetSel(nStartChar, nEndChar);
    if (bFindText) {
      int n = 0;
      LPCTSTR lpsz = LockBuffer(), lpszStart = lpsz;
      while (n < nStartChar) {
        lpsz = CharNext(lpsz);
        n += 1;
      }
      UnlockBuffer();
      nEndChar = nEndChar - nStartChar + lpsz - lpszStart;
      nStartChar = lpsz - lpszStart;
    }
  }

  void SetSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE)
  {
    if (bFindText) {
      int n = 0;
      LPCTSTR lpsz = LockBuffer(), lpszEnd = lpsz + nStartChar;
      while (lpsz < lpszEnd) {
        lpsz = CharNext(lpsz);
        n += 1;
      }
      UnlockBuffer();
      nEndChar = n + nEndChar - nStartChar;
      nStartChar = n;
    }
    CEdit::SetSel(nStartChar, nEndChar, bNoScroll);
  }

  void CloseFindDlg()
  {
    if (m_pFindReplaceDialog) {
      m_pFindReplaceDialog->SendMessage(WM_CLOSE);
      ATLASSERT(m_pFindReplaceDialog == NULL);
    }
  }

  void RegFindDlg()
  {
    m_gFindReplaceDialog = &m_pFindReplaceDialog;
  }

  void UnregFindDlg()
  {
    m_gFindReplaceDialog = NULL;
  }

  bool GetText(std::ostream& stream)
  {
    char buff[2048];

    int nl = GetLineCount();
    for (int i = 0; i < nl; ++i) {
      int n = GetLine(i, buff, sizeof(buff));
      if (n) {
        stream.write(buff, n);
      }
      if (i != nl - 1) {
        stream << "\n";
      }
    }

    return true;
  }

  bool LoadText(std::string const& fname)
  {
    //
    // Load text.
    //

    std::ifstream ifs(fname.c_str());
    if (!ifs) {
      return false;
    }

    std::stringstream ss;
    std::string line;
    while (std::getline(ifs, line)) {
      ss << line << "\r\n";
    }

    SetWindowText(ss.str().c_str());
    SetModify(FALSE);

    //
    // Update margin.
    //

    SetMargins(MARGIN, 0);

    return true;
  }

  bool SaveText(std::string const& fname)
  {
    std::stringstream ss;
    if (!GetText(ss)) {
      return false;
    }

    std::ofstream ofs(fname.c_str());
    if (!ofs) {
      return false;
    }

    ofs << ss.rdbuf();
    ofs.close();

    SetModify(FALSE);

    return true;
  }

  void GetCursorLocation(int& Row, int& Col)
  {
    int nStartPos, nEndPos;
    GetSel(nStartPos, nEndPos);

    Row = LineFromChar(nEndPos) + 1;
    Col = nEndPos - LineIndex(Row - 1) + 1;
  }

  BEGIN_MSG_MAP_EX(CScriptView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_SETCURSOR(OnSetCursor)
    COMMAND_ID_HANDLER_EX(ID_EDIT_GOTO, OnEditGoto)
    CHAIN_MSG_MAP_ALT(CEditCommands<CScriptView>, 1)
    CHAIN_MSG_MAP_ALT(CEditFindReplaceImpl<CScriptView>, 1)
  END_MSG_MAP()

  //
  // Message handler.
  //

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    bFindText = false;
    mCursor.LoadCursor(IDC_POINTER);

    SetMsgHandled(FALSE);
    return 0;
  }

  BOOL OnSetCursor(CWindow wnd, UINT nHitTest, UINT message)
  {
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(&p);

    if (MARGIN > p.x) {
      SetCursor(mCursor);
      return TRUE;
    }

    SetMsgHandled(FALSE);
    return 0;
  }

  void OnEditGoto(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    int nStartPos, nEndPos;
    GetSel(nStartPos, nEndPos);

    CDlgGotoLine dlg(GetLineCount(), LineFromChar(nEndPos));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    LineScroll(dlg.mLineNum - 1);

    int c = LineIndex(dlg.mLineNum - 1);
    SetSel(c, c);
  }
};

template<class MainT>
class CScriptEditor : public CWindowImpl<CScriptEditor<MainT> >, public CIdleHandler
{
public:

  int mId;
  CScriptView mView;

  CScriptEditor(int id) : mId(id)
  {
  }

  virtual void OnFinalMessage(HWND)     // Delete self when window destroy.
  {
    delete this;
  }

  virtual BOOL OnIdle()                 // Update UI states.
  {
    //
    // Update status bar.
    //

    MainT& mf = MainT::inst();

    int Row, Col;
    mView.GetCursorLocation(Row, Col);

    CString str;
    str.Format(_T("Total Lines:%d   Ln:%d   Col:%d"), mView.GetLineCount(), Row, Col);
    mf.mStatus.SetPaneText(1, str);

    return FALSE;
  }

  BEGIN_MSG_MAP_EX(CScriptEditor)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_GOOD(OnGoodMessage)
    MSG_WM_SETFOCUS(OnSetFocus)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    MSG_WM_SIZE(OnSize)
    MSG_WM_TIMER(OnTimer)
    CHAIN_COMMANDS_MEMBER(mView)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    mView.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN, WS_EX_CLIENTEDGE);
    mView.SetFont(AtlGetStockFont(SYSTEM_FIXED_FONT));

    //
    // Load script.
    //

    PrjT const& prj = PrjT::inst();
    std::string path = good::getPathName(prj.mRes.mFileName);
    path += prj.getScript(mId);

    mView.LoadText(path);

    SetMsgHandled(FALSE);
    return 0;
  }

  int OnGoodMessage(int uMsg, WPARAM wParam, LPARAM lParam)
  {
    switch (uMsg)
    {
    case WM_GOOD_GETRESOURCEID:
      return mId;

    case WM_GOOD_CANUNDO:
      return mView.CanUndo();

    case WM_GOOD_UNDO:
      mView.Undo();
      return true;

    case WM_GOOD_CANSAVE:
      return mView.GetModify();

    case WM_GOOD_SAVE:
      {
        PrjT const& prj = PrjT::inst();
        std::string path = good::getPathName(prj.mRes.mFileName);
        path += prj.getScript(mId);
        return mView.SaveText(path);
      }
    }

    return 0;
  }

  void OnSetFocus(CWindow wndOld)
  {
    mView.SetFocus();
  }

  void OnShowWindow(BOOL bShow, UINT nStatus)
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    MainT::inst().ResetStatusBar();

    if (bShow) {
      pLoop->AddIdleHandler(this);
      mView.RegFindDlg();
      SetTimer(1, 100);                 // To fixed focus issue.
    } else {
      pLoop->RemoveIdleHandler(this);
      mView.CloseFindDlg();
      mView.UnregFindDlg();
    }

    SetMsgHandled(FALSE);
  }

  void OnSize(UINT nType, CSize size)
  {
    mView.SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOACTIVATE | SWP_NOZORDER);
  }

  void OnTimer(UINT_PTR nIDEvent)
  {
    KillTimer(1);

    if (GetFocus() != mView) {
      mView.SetFocus();
    }
  }
};

// end of EditScript.h
