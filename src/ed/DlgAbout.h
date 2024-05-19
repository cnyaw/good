
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

class CDlgAbout : public CSimpleDialog<IDD_ABOUTBOX>
{
public:

  BEGIN_MSG_MAP_EX(CDlgAbout)
    COMMAND_RANGE_HANDLER_EX(IDC_BUTTON1, IDC_BUTTON2, OnUrl)
    CHAIN_MSG_MAP(CSimpleDialog)
  END_MSG_MAP()

  void OnUrl(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    const char* URL[] = {
      "https://good-ed.blogspot.com",
      "https://github.com/cnyaw/good"
    };
    ShellOpen(URL[nID - IDC_BUTTON1]);
  }
};

// end of DlgAbout.h
