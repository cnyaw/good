
//
// DlgPicker.h
// Picker dialogs.
//
// Copyright (c) 2019 Waync Cheng.
// All Rights Reserved.
//
// 2019/11/27 Waync created.
//

#pragma once

template<class MainT>
class CDlgTexturePicker : public CDialogImpl<CDlgTexturePicker<MainT> >
{
public:
  enum { IDD = IDD_CHOICETEXTURE };

  int mId;
  CTextureResListView<MainT> mTexRes;

  CDlgTexturePicker() : mId(-1)
  {
  }

  BEGIN_MSG_MAP_EX(CDlgTexturePicker)
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_TIMER(OnTimer)
    COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
  END_MSG_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    CenterWindow(GetParent());
    ::EnableWindow(GetDlgItem(IDOK), FALSE);
    HWND hList = GetDlgItem(IDC_LIST1);
    RECT rcClient;
    ::GetClientRect(hList, &rcClient);
    mTexRes.Create(m_hWnd, rcClient, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
    ::DestroyWindow(hList);
    SetTimer(1, 1000);
    return TRUE;
  }

  void OnTimer(UINT_PTR nIDEvent)
  {
    if (-1 != mTexRes.mCurSel) {
      ::EnableWindow(GetDlgItem(IDOK), TRUE);
      KillTimer(1);
    }
  }

  void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (IDOK == nID) {
      mId = mTexRes.GetResId(mTexRes.mCurSel);
    }
    EndDialog(nID);
  }
};

class CDlgLevelObjPicker : public CSimpleDialog<IDD_CHOICELVLOBJ>
{
public:
  int mId;
  CImageListManaged mImages;
  CTreeViewCtrlEx mTree;

  CDlgLevelObjPicker() : mId(-1)
  {
  }

  void InitTreeItem(PrjT::LevelT const& lvl, CTreeItem &tnode, const std::vector<int> &v)
  {
    for (size_t i = 0; i < v.size(); i++) {
      int idObj = v[i];
      PrjT::ObjectT const &o = lvl.getObj(idObj);
      CTreeItem ti = tnode.AddTail(o.getName().c_str(), o.mType + 1);
      ti.SetData(idObj);
      InitTreeItem(lvl, ti, o.mObjIdx);
    }
  }

  void InitTreeView()
  {
    mTree = GetDlgItem(IDC_TREE1);
    mImages.Create(IDB_LVLOBJ, 16, 0, RGB(255,0,255));
    mTree.SetImageList(mImages, TVSIL_NORMAL);

    CTreeItem tiRoot = mTree.GetChildItem(NULL);
    PrjT const& prj = PrjT::inst();
    for (size_t i = 0; i < prj.mRes.mLevelIdx.size(); ++i) {
      const PrjT::LevelT &lvl = prj.getLevel(prj.mRes.mLevelIdx[i]);
      CTreeItem ti = tiRoot.AddTail(lvl.getName().c_str(), 0);
      ti.SetData(-lvl.mId);
      InitTreeItem(lvl, ti, lvl.mObjIdx);
    }
    tiRoot.Expand();
  }

  BEGIN_MSG_MAP_EX(CDlgLevelObjPicker)
    MSG_WM_INITDIALOG(OnInitDialog)
    NOTIFY_CODE_HANDLER_EX(TVN_SELCHANGED, OnTreeSelChanged)
    CHAIN_MSG_MAP(CSimpleDialog)
  END_MSG_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    CenterWindow(GetParent());
    ::EnableWindow(GetDlgItem(IDOK), FALSE);
    InitTreeView();
    return TRUE;
  }

  LRESULT OnTreeSelChanged(LPNMHDR pnmh)
  {
    HTREEITEM sel = mTree.GetSelectedItem();
    if (NULL != sel) {
      mId = (int)mTree.GetItemData(sel);
      ::EnableWindow(GetDlgItem(IDOK), 0 < mId);
    }
    return 0;
  }
};

// end of DlgPicker.h
