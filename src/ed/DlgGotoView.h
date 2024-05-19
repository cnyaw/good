
//
// DlgGotoView.h
// Level editor goto view dialog.
//
// Copyright (c) 2019 Waync Cheng.
// All Rights Reserved.
//
// 2019/11/4 Waync created.
//

#pragma once

class CDlgGotoView : public CDialogImpl<CDlgGotoView>
{
public:
  enum { IDD = IDD_GOTOVIEW };

  int mViewCx, mViewCy;
  int mOffsetX, mOffsetY;

  CDlgGotoView(int cx, int cy, int x, int y) : mViewCx(cx), mViewCy(cy), mOffsetX(x), mOffsetY(y)
  {
  }

  BEGIN_MSG_MAP_EX(CDlgGotoView)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
  END_MSG_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    CString s;
    s.Format(_T("Offset X (0 - %d)"), mViewCx - 1);
    ::SetWindowText(GetDlgItem(IDC_STATIC1), s);

    CEdit edx = GetDlgItem(IDC_EDIT1);
    s.Format(_T("%d"), mOffsetX);
    edx.SetWindowText(s);

    s.Format(_T("Offset Y (0 - %d)"), mViewCy - 1);
    ::SetWindowText(GetDlgItem(IDC_STATIC2), s);

    CEdit edy = GetDlgItem(IDC_EDIT2);
    s.Format(_T("%d"), mOffsetY);
    edy.SetWindowText(s);

    CenterWindow(GetParent());
    return TRUE;
  }

  void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    BOOL b;
    INT x = ::GetDlgItemInt(m_hWnd, IDC_EDIT1, &b, FALSE);
    if (b) {
      mOffsetX = max(0, min(x, mViewCx - 1));
    }
    INT y = ::GetDlgItemInt(m_hWnd, IDC_EDIT2, &b, FALSE);
    if (b) {
      mOffsetY = max(0, min(y, mViewCy - 1));
    }

    EndDialog(nID);
  }
};

// end of DlgGotoView.h
