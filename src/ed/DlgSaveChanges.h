
//
// DlgSaveChanges.h
// Save changes dialog box.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/11/07 Waync created.
//

#pragma once

template<class MainT>
class CDlgSaveChanges : public CDialogImpl<CDlgSaveChanges<MainT> >
{
public:
  enum { IDD = IDD_SAVECHANGES };

  std::map<int, int> id;                // Pageid, resid.
  std::map<int, int> changes;           // Pageid, bSave.

  CListBox mList;

  BOOL CheckSaveChanges()
  {
    PrjT& prj = PrjT::inst();

    MainT &m = MainT::inst();

    for (int i = 0; i < m.mTabView.GetPageCount(); ++i) { // Collect changed items.
      HWND h = m.mTabView.GetPageHWND(i);
      if (::SendMessage(h, WM_GOOD_CANSAVE, 0, 0)) {
        int aId = ::SendMessage(h, WM_GOOD_GETRESOURCEID, 0, 0);
        if (prj.mRes.isScript(aId) || prj.mRes.isStgeScript(aId)) {
          id[i] = aId;
        }
      }
    }

    if (!id.empty()) {
      switch (DoModal())
      {
      case IDYES:                       // Save changes.
        for (std::map<int, int>::const_iterator it = changes.begin();
             changes.end() != it;
             ++it) {
          if (it->second && !::SendMessage(m.mTabView.GetPageHWND(it->first), WM_GOOD_SAVE, 0, 0)) {
            MessageBox(_T("Save file failed!"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK | MB_ICONERROR);
            return FALSE;
          }
        }
        break;

      case IDNO:                        // Discard changes.
        break;

      case IDCANCEL:                    // Stop.
        return FALSE;
      }
    }

    return TRUE;
  }

  BEGIN_MSG_MAP_EX(CDlgSaveChanges)
    MSG_WM_COMMAND(OnCommand)
    MSG_WM_INITDIALOG(OnInitDialog)
  END_MSG_MAP()

  void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    switch (nID)
    {
    case IDCANCEL:
    case IDC_BUTTON3:                   // Cancel.
      EndDialog(IDCANCEL);
      break;

    case IDC_BUTTON1:                   // Yes.
      EndDialog(IDYES);
      break;

    case IDC_BUTTON2:                   // No.
      EndDialog(IDNO);
      break;

    case IDC_LIST1:
      if (LBN_SELCHANGE == uNotifyCode) {
        changes.clear();
        for (int i = 0; i < mList.GetCount(); ++i) {
          if (mList.GetSel(i)) {
            changes[mList.GetItemData(i)] = 1;
          }
        }
      }
      break;
    }
  }

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    mList = GetDlgItem(IDC_LIST1);

    PrjT const& prj = PrjT::inst();

    std::string path = good::getPathName(prj.mRes.mFileName);

    for (std::map<int, int>::const_iterator it = id.begin(); id.end() != it; ++it) {
      int id = it->second;
      int idx;
      if (prj.mRes.isScript(id)) {
        idx = mList.AddString((path + prj.mRes.getScript(id)).c_str());
      } else {
        idx = mList.AddString((path + prj.mRes.getStgeScript(id)).c_str());
      }
      mList.SetItemData(idx, it->first);
      changes[it->first] = 1;
    }

    mList.SetSel(-1, TRUE); // Select all.

    CenterWindow(GetParent());
    return TRUE;
  }
};

// end of DlgSaveChanges.h
