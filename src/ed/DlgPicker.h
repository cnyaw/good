
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

template<class T>
class CDlgResourceItemPicker : public CDialogImpl<T>
{
public:

  int mId;
  CListBox mItem;

  CDlgResourceItemPicker() : mId(-1)
  {
  }

  BEGIN_MSG_MAP_EX(CDlgResourceItemPicker)
    MSG_WM_INITDIALOG(OnInitDialog)
    COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
    COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
    COMMAND_HANDLER_EX(IDC_LIST1, LBN_SELCHANGE, OnItemSelChange)
  END_MSG_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    CenterWindow(GetParent());
    ::EnableWindow(GetDlgItem(IDOK), FALSE);
    mItem = GetDlgItem(IDC_LIST1);
    static_cast<T*>(this)->DoInitListBox();
    return TRUE;
  }

  void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (IDOK == nID) {
      mId = (int)mItem.GetItemData(mItem.GetCurSel());
    }
    EndDialog(nID);
  }

  void OnItemSelChange(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    ::EnableWindow(GetDlgItem(IDOK), LB_ERR != mItem.GetCurSel() && (int)mItem.GetItemData(mItem.GetCurSel()) != mId);
  }
};

class CDlgTexturePicker : public CDlgResourceItemPicker<CDlgTexturePicker>
{
public:
  enum { IDD = IDD_CHOICETEXTURE };

  void DoInitListBox()
  {
    PrjT const& prj = PrjT::inst();
    for (size_t i = 0; i < prj.mRes.mTexIdx.size(); ++i) {
      int idx = mItem.AddString(prj.mRes.getTex(prj.mRes.mTexIdx[i]).getName().c_str());
      mItem.SetItemData(idx, prj.mRes.mTexIdx[i]);
    }
  }
};

class CDlgLevelObjPicker : public CDialogImpl<CDlgLevelObjPicker>
{
public:
  enum { IDD = IDD_CHOICELVLOBJ };

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
    COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
    COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)
    NOTIFY_CODE_HANDLER_EX(TVN_SELCHANGED, OnTreeSelChanged)
  END_MSG_MAP()

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    CenterWindow(GetParent());
    ::EnableWindow(GetDlgItem(IDOK), FALSE);
    InitTreeView();
    return TRUE;
  }

  void OnCloseCmd(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    EndDialog(nID);
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
