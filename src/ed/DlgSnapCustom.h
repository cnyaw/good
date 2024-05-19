
//
// DlgSnapCustom.h
// Snap customize dialog.
//
// Copyright (c) 2014 Waync Cheng.
// All Rights Reserved.
//
// 2014/6/20 Waync created.
//

#pragma once

class CDlgSnapCustomize : public CDialogImpl<CDlgSnapCustomize>, public CWinDataExchange<CDlgSnapCustomize>
{
public:
  enum { IDD = IDD_SNAPCUSTOMIZE };

  int mWidth, mHeight;

  CDlgSnapCustomize(int w, int h) : mWidth(w), mHeight(h)
  {
  }

  BEGIN_MSG_MAP_EX(CDlgSnapCustomize)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
  END_MSG_MAP()

  BEGIN_DDX_MAP(CDlgSnapCustomize)
    DDX_UINT_RANGE(IDC_EDIT1, mWidth, 1, 1024)
    DDX_UINT_RANGE(IDC_EDIT2, mHeight, 1, 1024)
  END_DDX_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    DoDataExchange(FALSE);              // Init, apply to controls.
    CenterWindow(GetParent());

    return TRUE;
  }

  void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (IDOK == nID && !DoDataExchange(TRUE)) {
      return;
    }

    EndDialog(nID);
  }
};

// end of DlgSnapCustom.h
