
//
// DlgNewProj.h
// New project dialog.
//
// Copyright (c) 2014 Waync Cheng.
// All Rights Reserved.
//
// 2014/1/2 Waync created.
//

#pragma once

class CDlgNewProj : public CDialogImpl<CDlgNewProj>, public CWinDataExchange<CDlgNewProj>
{
public:
  enum { IDD = IDD_NEWPROJ };

  CString mName, mPath;
  int mWidth, mHeight;

  CDlgNewProj() : mWidth(640), mHeight(480)
  {
  }

  BEGIN_MSG_MAP_EX(CDlgNewProj)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_ID_HANDLER_EX(IDC_BUTTON1, OnBrowsePath)
    COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
  END_MSG_MAP()

  BEGIN_DDX_MAP(CDlgNewProj)
    DDX_TEXT(IDC_EDIT1, mName)
    DDX_TEXT(IDC_EDIT2, mPath)
    DDX_UINT_RANGE(IDC_EDIT3, mWidth, 32, 4096)
    DDX_UINT_RANGE(IDC_EDIT4, mHeight, 32, 4096)
  END_DDX_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    DoDataExchange(FALSE);              // Init, apply to controls.
    CenterWindow(GetParent());
    return TRUE;
  }

  void OnBrowsePath(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Good Project Files(*.txt)\0*.txt\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    std::string name = dlg.m_szFileName;
    name = CompleteFileExt(name, ".txt");

    ::SetWindowText(GetDlgItem(IDC_EDIT2), name.c_str());
  }

  void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (IDOK == nID) {
      if (!DoDataExchange(TRUE)) {
        return;
      }
      if (mPath.IsEmpty()) {
        ::SetFocus(GetDlgItem(IDC_EDIT2));
        return;
      }
    }

    EndDialog(nID);
  }
};

// end of DlgNewProj.h
