
//
// DlgGotoLine.h
// Source editor goto line dialog.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/11/08 Waync created.
//

#pragma once

class CDlgGotoLine : public CDialogImpl<CDlgGotoLine>
{
public:
  enum { IDD = IDD_GOTOLINE };

  int mLineCount;
  int mLineNum;

  CDlgGotoLine(int lc, int ln) : mLineCount(lc), mLineNum(ln)
  {
  }

  BEGIN_MSG_MAP_EX(CDlgGotoLine)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
    COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
  END_MSG_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    CString s;
    s.Format(_T("Line number (1 - %d)"), mLineCount);

    ::SetWindowText(GetDlgItem(IDC_STATIC1), s);

    s.Format(_T("%d"), mLineNum + 1);

    CEdit ed = GetDlgItem(IDC_EDIT1);
    ed.SetWindowText(s);
    ed.SetSelAll();

    CenterWindow(GetParent());
    return TRUE;
  }

  void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    BOOL b;
    INT i = ::GetDlgItemInt(m_hWnd, IDC_EDIT1, &b, FALSE);
    if (b) {
      mLineNum = max(1, min(i, mLineCount));
    }

    EndDialog(nID);
  }
};

// end of DlgGotoLine.h
