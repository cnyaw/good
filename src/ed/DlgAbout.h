
//
// DlgAbout.h
// About box.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/10/14 Waync created.
//

#pragma once

void ShellOpen(const char *path)
{
  SHELLEXECUTEINFOA si = {sizeof(SHELLEXECUTEINFOA)};
  si.lpVerb = "open";
  si.lpFile = path;
  si.nShow = SW_SHOWNORMAL;
  ShellExecuteExA(&si);
}

class CDlgAbout : public CDialogImpl<CDlgAbout>
{
public:
  enum { IDD = IDD_ABOUTBOX };

  BEGIN_MSG_MAP_EX(CDlgAbout)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_RANGE_HANDLER_EX(IDC_BUTTON1, IDC_BUTTON2, OnUrl)
    COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
    COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
  END_MSG_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    CenterWindow(GetParent());
    return TRUE;
  }

  void OnUrl(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    const char* URL[] = {
      "https://good-ed.blogspot.com",
      "https://github.com/cnyaw/good"
    };
    ShellOpen(URL[nID - IDC_BUTTON1]);
  }

  void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    EndDialog(nID);
  }
};

// end of DlgAbout.h
