
//
// Explorer.h
// Explorer view.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/07/25 Waync created.
//

#pragma once

template<class MainT>
class CExplorerPropView : public CDialogImpl<CExplorerPropView<MainT> >
{
public:
  enum { IDD = IDD_PROPVIEW };

  CPropertyListCtrl mProp;
  CFont mBoldFont;

  int mResId;

  IResourceProperty* GetProperty(int id) const
  {
    if (-2 == id) {
      return &CResProjectProperty::inst();
    }

    PrjT const& prj = PrjT::inst();

    if (prj.mRes.isSnd(id)) {
      return &CResSoundProperty::inst(id);
    } else if (prj.mRes.isTex(id)) {
      return &CResTextureProperty::inst(id);
    } else if (prj.mRes.isMap(id)) {
      return &CResMapProperty::inst(id);
    } else if (prj.mRes.isSprite(id)) {
      return &CResSpriteProperty::inst(id);
    } else if (prj.mRes.isLevel(id)) {
      return &CResLevelProperty::inst(id);
    } else if (prj.mRes.isScript(id)) {
      return &CResScriptProperty::inst(id);
    } else if (prj.mRes.isStgeScript(id)) {
      return &CResParticleProperty::inst(id);
    } else if (prj.mRes.isDep(id)) {
      return &CResDependencyProperty::inst(id);
    } else {
      return &CResLevelObjectProperty::inst(id);
    }
  }

  void Reset()
  {
    mProp.ResetContent();
  }

  void SetProperty(int idItem)
  {
    Reset();

    if (-1 == idItem) {
      return;
    }

    mResId = idItem;

    IResourceProperty* prop = GetProperty(idItem);
    prop->FillProperty(mProp);
  }

  BEGIN_MSG_MAP_EX(CExplorerPropView)
    MSG_WM_CLOSE(OnClose)
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_SIZE(OnSize)
    NOTIFY_HANDLER_EX(IDC_LIST1, PIN_BROWSE, OnBrowse)
    NOTIFY_HANDLER_EX(IDC_LIST1, PIN_ITEMCHANGING, OnPropChanging)
    NOTIFY_HANDLER_EX(IDC_LIST1, PIN_ITEMCHANGED, OnPropChanged)
    REFLECT_NOTIFICATIONS()             // CPropertyListCtrl needs this.
  END_MSG_MAP()

  //
  // Message handler.
  //

  void OnClose()
  {
    Reset();

    SetMsgHandled(FALSE);
  }

  BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
  {
    mProp.SubclassWindow(GetDlgItem(IDC_LIST1));
    mProp.SetExtendedListStyle(PLS_EX_CATEGORIZED | PLS_EX_SINGLECLICKEDIT | PLS_EX_XPLOOK);

    CLogFont lf = AtlGetDefaultGuiFont();
    lf.MakeBolder();
    mBoldFont.CreateFontIndirect(&lf);

    return TRUE;
  }

  void OnSize(UINT nType, CSize size)
  {
    if (!mProp.IsWindow()) {
      return;
    }

    RECT rcClient;
    GetClientRect(&rcClient);
    mProp.MoveWindow(&rcClient);
    mProp.Invalidate(FALSE);
  }

  //
  // Notification.
  //

  LRESULT OnBrowse(LPNMHDR pnmh)
  {
    GetProperty(mResId)->UpdateProperty(mProp);
    return 0;
  }

  LRESULT OnPropChanging(LPNMHDR pnmh)
  {
    IResourceProperty *prop = GetProperty(mResId);
    return prop->OnPropChanging(mProp, (LPNMPROPERTYITEM)pnmh);
  }

  LRESULT OnPropChanged(LPNMHDR pnmh)
  {
    IResourceProperty* pres;
    pres = GetProperty(mResId);

    pres->OnPropChanged(mProp, (LPNMPROPERTYITEM)pnmh); // Item changed

    //
    // (Redraw view, change treeitem name, tab name) if necessary.
    //

    MainT::inst().UpdatePropChanged(pres);

    return 0;
  }
};

template<class MainT>
class CExplorerView : public CWindowImpl<CExplorerView<MainT> >
{
public:
  DECLARE_WND_CLASS_EX(NULL, 0, COLOR_BTNFACE)

  CHorSplitterWindow mSplit;

  CPaneContainer mPaneTree, mPaneProp;
  CTreeViewCtrlEx mTree;
  CExplorerPropView<MainT> mProp;

  CImageList mImages;

  void ChangeItemName(int id, std::string const& name)
  {
    //
    // Rename resource view tree item.
    //

    CTreeItem hItem = FindTreeItem(id);
    if (!hItem.IsNull()) {
      hItem.SetText(name.c_str());
    }
  }

  void FINDINUSEDMSG(std::string const& name)
  {
    SW2_TRACE_ERROR("Item is still used by %s", name.c_str());
  }

  bool SureRemoveRes(const std::string &name) const
  {
    CString msg;
    msg.Format(_T("Are you sure to remove resource item '%s'?"), name.c_str());
    return IDYES == ::MessageBox(m_hWnd, msg, CString((LPCTSTR)IDR_MAINFRAME), MB_YESNOCANCEL | MB_ICONQUESTION);
  }

  template<class MapT>
  bool FindIsUsedTex(MapT const& map, int idItem)
  {
    for (MapT::const_iterator it = map.begin(); map.end() != it; ++it) {
      if (it->second.mTileset.mTextureId == idItem) {
        FINDINUSEDMSG(it->second.getName());
        return true;
      }
    }

    return false;
  }

  bool FindIsUsedTex(std::map<int, typename PrjT::ObjectT> const& map, int idItem)
  {
    for (std::map<int, typename PrjT::ObjectT>::const_iterator it = map.begin();
         map.end() != it;
         ++it) {
      if (it->second.mTextureId == idItem) {
        FINDINUSEDMSG(it->second.getName());
        return true;
      }
    }

    return false;
  }

  template<class MapT>
  bool FindIsUsedMap(MapT const& map, int idItem)
  {
    for (MapT::const_iterator it = map.begin(); map.end() != it; ++it) {
      if (it->second.mMapId == idItem) {
        FINDINUSEDMSG(it->second.getName());
        return true;
      }
    }

    return false;
  }

  template<class MapT>
  bool FindIsUsedSpr(MapT const& map, int idItem)
  {
    for (MapT::const_iterator it = map.begin(); map.end() != it; ++it) {
      if (it->second.mSpriteId == idItem) {
        FINDINUSEDMSG(it->second.getName());
        return true;
      }
    }

    return false;
  }

  void Refresh(int id)
  {
    SetCurSel(-1);                      // Remove all prop.
    SetCurSel(id);                      // Add again, lvl will be the first visible prop.
  }

  void SetCurSel(int id)
  {
    CTreeItem hItem = FindTreeItem(id);

    //
    // Should check this, to avoid infinte setsel/selchange loop.
    //

    if (mTree.GetSelectedItem() != hItem) {
      mTree.SelectItem(hItem);
    }
  }

  void UpdateProperty(int id = -1)
  {
    if (-1 != id) {
      mProp.mResId = id;
    }
    mProp.GetProperty(mProp.mResId)->UpdateProperty(mProp.mProp);
  }

  BEGIN_MSG_MAP_EX(CExplorerView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_SIZE(OnSize)
    NOTIFY_CODE_HANDLER_EX(NM_DBLCLK, OnTreeDblClk)
    NOTIFY_CODE_HANDLER_EX(TVN_BEGINDRAG, OnTreeBeginDrag)
    NOTIFY_CODE_HANDLER_EX(TVN_KEYDOWN, OnTreeKeyDown)
    NOTIFY_CODE_HANDLER_EX(TVN_SELCHANGED, OnTreeSelChanged)
    COMMAND_ID_HANDLER_EX(ID_PANE_CLOSE, OnCloseProjView)
  END_MSG_MAP()

  //
  // Message handler.
  //

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    mSplit.Create(m_hWnd);

    mPaneTree.Create(mSplit, _T("Resource"));
    mPaneTree.SetPaneContainerExtendedStyle(PANECNT_NOBORDER);
    mTree.Create(mPaneTree, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
    mPaneTree.SetClient(mTree);

    mPaneProp.Create(mSplit, _T("Property"));
    mPaneProp.SetPaneContainerExtendedStyle(PANECNT_NOBORDER | PANECNT_NOCLOSEBUTTON);
    mProp.Create(mPaneProp);
    mPaneProp.SetClient(mProp);

    mSplit.SetSplitterPanes(mPaneTree, mPaneProp);
    mSplit.SetActivePane(0);

    mImages.Create(IDB_BITMAP1, 16, 0, RGB(255,0,255));
    mTree.SetImageList(mImages, TVSIL_NORMAL);

    SetMsgHandled(FALSE);
    return 0;
  }

  void OnSize(UINT nType, CSize size)
  {
    mSplit.SetWindowPos(0, 2, 2, size.cx - 4, size.cy - 4, SWP_NOACTIVATE | SWP_NOZORDER);
    mSplit.SetSplitterPos(size.cy / 2);
  }

  //
  // Notification handler.
  //

  LRESULT OnTreeBeginDrag(LPNMHDR pnmh)
  {
    LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW)pnmh;

    HTREEITEM group = mTree.GetParentItem(lpnmtv->itemNew.hItem);
    if (NULL == group) {
      return 0;
    }

    RECT rcItem;
    TreeView_GetItemRect(mTree, lpnmtv->itemNew.hItem, &rcItem, TRUE);

    int offsetx = lpnmtv->ptDrag.x - rcItem.left + 18;
    int offsety = lpnmtv->ptDrag.y - rcItem.top;

    int typeItem = (int)mTree.GetItemData(group);
    int idItem = (int)mTree.GetItemData(lpnmtv->itemNew.hItem);

    RECT rcClient;
    mTree.GetClientRect(&rcClient);

    //
    // Drag loop.
    //

    CImageList img = TreeView_CreateDragImage(mTree, lpnmtv->itemNew.hItem);
    img.BeginDrag(0, 0, 0);
    img.DragEnter(mTree, lpnmtv->ptDrag.x - offsetx, lpnmtv->ptDrag.y - offsety);
    SetCapture();

    LPCTSTR cur = IDC_ARROW;

    HTREEITEM htiTarget = NULL;

    MSG msg;
    while (GetCapture() == m_hWnd) {

      if (!::GetMessage(&msg, NULL, 0, 0)) {
        ::PostQuitMessage(msg.wParam);
        break;
      }

      switch (msg.message)
      {
      case WM_MOUSEMOVE:
        {
          POINT point;
          point.x = LOWORD(msg.lParam);
          point.y = HIWORD(msg.lParam);
          ClientToScreen(&point);
          mTree.ScreenToClient(&point);
          img.DragMove(point.x - offsetx, point.y - offsety);
          img.DragShowNolock(FALSE);
          if (::PtInRect(&rcClient, point)) {
            TVHITTESTINFO tvht;
            tvht.pt.x = point.x;
            tvht.pt.y = point.y;
            if ((htiTarget = TreeView_HitTest(mTree, &tvht)) != NULL) {
              TreeView_SelectDropTarget(mTree, htiTarget);
              HTREEITEM groupTarget = mTree.GetParentItem(htiTarget);
              if (groupTarget) {
                int typeTarget = (int)mTree.GetItemData(groupTarget);
                if (typeTarget != typeItem) {
                  cur = IDC_NO;
                } else {
                  cur = IDC_ARROW;
                }
              } else if (htiTarget != group) {
                cur = IDC_NO;
              }
            }
          } else {
            cur = IDC_NO;
          }

          SetCursor(LoadCursor(NULL, cur));

          img.DragShowNolock(TRUE);
        }
        break;

      case WM_KEYDOWN:
        if (VK_ESCAPE != msg.wParam) {
          break;
        }

        //
        // Escape key pressed, end tracking.
        //

      case WM_LBUTTONUP:
        goto end;

      case WM_SYSKEYDOWN:
      case WM_LBUTTONDOWN:
        goto end;
      }

      ::DispatchMessage(&msg);          // Default.
    }
end:
    ReleaseCapture();

    img.EndDrag();                      // Destroy tmp image list.
    TreeView_SelectDropTarget(mTree, NULL);

    if (htiTarget != NULL && IDC_NO != cur) {
      int idCur = -1;
      if (mTree.GetParentItem(htiTarget) != NULL) {
        idCur = (int)mTree.GetItemData(htiTarget);
      } else if (group != htiTarget) {
        return 0;
      }
      if (PrjT::inst().insertAfter(idItem, idCur)) {
        CString name;
        mTree.GetItemText(lpnmtv->itemNew.hItem, name);
        CTreeItem ti = mTree.GetParentItem(lpnmtv->itemNew.hItem);
        if (-1 != idCur) {
          ti.InsertAfter(name, htiTarget, 2).SetData(idItem);
        } else {
          ti.AddHead(name, 2).SetData(idItem);
        }
        ti.Select();
        TreeView_DeleteItem(mTree, lpnmtv->itemNew.hItem); // Delete old item.
      }
    }

    return 0;
  }

  LRESULT OnTreeDblClk(LPNMHDR pnmh)
  {
    if (mTree != pnmh->hwndFrom) {
      return 0;
    }

    MainT& m = MainT::inst();

    POINT pt;
    ::GetCursorPos(&pt);
    mTree.ScreenToClient(&pt);

    UINT flag;
    HTREEITEM sel = mTree.HitTest(pt, &flag);
    if (NULL == sel || !(flag & TVHT_ONITEM)) {
      return 0;
    }

    HTREEITEM group = mTree.GetParentItem(sel);
    if (NULL == group) {
      return 0;
    }

    int typeItem = (int)mTree.GetItemData(group);
    int idItem = (int)mTree.GetItemData(sel);

    //
    // Find exist view.
    //

    for (int i = 0; i < m.mTabView.GetPageCount(); ++i) {
      HWND hwnd = m.mTabView.GetPageHWND(i);
      int id = (int)::SendMessage(hwnd, WM_GOOD_GETRESOURCEID, 0, 0);
      if (id == idItem) {
        m.mTabView.SetActivePage(i);
        return 1;
      }
    }

    //
    // Open new view.
    //

    PrjT &prj = PrjT::inst();

    switch (typeItem)
    {
    case GOOD_RESOURCE_AUDIO:
      m.AddResourceEditor(prj.getSnd(idItem).getName(), idItem, m.CreateEditor<CSoundEditor<CMainFrame> >(idItem));
      return 1;

    case GOOD_RESOURCE_TEXTURE:
      m.AddResourceEditor(prj.getTex(idItem).getName(), idItem, m.CreateEditor<CTextureEditor<CMainFrame> >(idItem));
      return 1;

    case GOOD_RESOURCE_MAP:
      m.AddResourceEditor(prj.getMap(idItem).getName(), idItem, m.CreateEditor<CMapEditor<CMainFrame> >(idItem));
      return 1;

    case GOOD_RESOURCE_SPRITE:
      m.AddResourceEditor(prj.getSprite(idItem).getName(), idItem, m.CreateEditor<CSpriteEditor<CMainFrame> >(idItem));
      return 1;

    case GOOD_RESOURCE_LEVEL:
      m.AddResourceEditor(prj.getLevel(idItem).getName(), idItem, m.CreateEditor<CLevelEditor<CMainFrame> >(idItem));
      return 1;

    case GOOD_RESOURCE_SCRIPT:
    case GOOD_RESOURCE_PARTICLE:
      {
        std::string name;
        if (GOOD_RESOURCE_SCRIPT == typeItem) {
          name = prj.getScript(idItem);
        } else {
          name = prj.getStgeScript(idItem);
        }

        std::string::size_type pos = name.find_last_of('/');
        if (std::string::npos != pos) {
          name = name.substr(pos + 1);
        }

        if (GOOD_RESOURCE_SCRIPT == typeItem) {
          m.AddResourceEditor(name, idItem, m.CreateEditor<CScriptEditor<CMainFrame> >(idItem));
        } else {
          m.AddResourceEditor(name, idItem, m.CreateEditor<CStgeScriptEditor<CMainFrame> >(idItem));
        }
      }
      return 1;
    }

    return 0;
  }

  LRESULT OnTreeKeyDown(LPNMHDR pnmh)
  {
    if (mTree != pnmh->hwndFrom) {
      return 0;
    }

    MainT& m = MainT::inst();

    LPNMTVKEYDOWN pkey = (LPNMTVKEYDOWN)pnmh;

    if (VK_DELETE == pkey->wVKey) {   // Delete item.

      //
      // Get selected item.
      //

      HTREEITEM sel = mTree.GetSelectedItem();
      if (NULL == sel) {
        return 0;
      }

      PrjT& prj = PrjT::inst();

      HTREEITEM group = mTree.GetParentItem(sel);
      if (NULL == group) {

        //
        // Delete all group is not allowed.
        //

      } else {

        //
        // Delete single item.
        //

        int typeItem = (int)mTree.GetItemData(group);
        int idItem = (int)mTree.GetItemData(sel);

        switch (typeItem)               // Delete resource item.
        {
        case GOOD_RESOURCE_AUDIO:
          if (!SureRemoveRes(prj.getSnd(idItem).getName())) {
            return 0;
          }
          if (!prj.removeSnd(idItem)) {
            return 0;
          }
          break;

        case GOOD_RESOURCE_TEXTURE:
          {
            if (!SureRemoveRes(prj.getTex(idItem).getName())) {
              return 0;
            }

            if (FindIsUsedTex(prj.mRes.mMap, idItem) ||
                FindIsUsedTex(prj.mRes.mSprite, idItem)) {
              return 0;
            }

            for (std::map<int, PrjT::LevelT>::const_iterator it = prj.mRes.mLevel.begin();
                 prj.mRes.mLevel.end() != it;
                 ++it) {
              typename PrjT::LevelT const& lvl = it->second;
              if (FindIsUsedTex(lvl.mObj, idItem)) {
                return 0;
              }
            }

            if (!prj.removeTex(idItem)) {
              return 0;
            }
          }
          break;

        case GOOD_RESOURCE_MAP:
          {
            if (!SureRemoveRes(prj.getMap(idItem).getName())) {
              return 0;
            }

            for (std::map<int, PrjT::LevelT>::const_iterator it = prj.mRes.mLevel.begin();
                 prj.mRes.mLevel.end() != it;
                 ++it) {
              typename PrjT::LevelT const& lvl = it->second;
              if (FindIsUsedMap(lvl.mObj, idItem)) {
                return 0;
              }
            }

            if (!prj.removeMap(idItem)) {
              return 0;
            }
          }
          break;

        case GOOD_RESOURCE_SPRITE:
          {
            if (!SureRemoveRes(prj.getSprite(idItem).getName())) {
              return 0;
            }

            for (std::map<int, PrjT::LevelT>::const_iterator it = prj.mRes.mLevel.begin();
                 prj.mRes.mLevel.end() != it;
                 ++it) {
              typename PrjT::LevelT const& lvl = it->second;
              if (FindIsUsedSpr(lvl.mObj, idItem)) {
                return 0;
              }
            }

            if (!prj.removeSprite(idItem)) {
              return 0;
            }
          }
          break;

        case GOOD_RESOURCE_LEVEL:
          if (!SureRemoveRes(prj.getLevel(idItem).getName())) {
            return 0;
          }
          if (!prj.removeLevel(idItem)) {
            return 0;
          }
          break;

        case GOOD_RESOURCE_SCRIPT:
          if (!SureRemoveRes(prj.getScript(idItem))) {
            return 0;
          }
          if (!prj.removeScript(idItem)) {
            return 0;
          }
          break;

        case GOOD_RESOURCE_PARTICLE:
          if (!SureRemoveRes(prj.getStgeScript(idItem))) {
            return 0;
          }
          if (!prj.removeStgeScript(idItem)) {
            return 0;
          }
          break;

        case GOOD_RESOURCE_DEPENDENCY:
          if (!SureRemoveRes(prj.getDep(idItem))) {
            return 0;
          }
          if (!prj.removeDep(idItem)) {
            return 0;
          }
          break;
        }

        for (int i = 0; i < m.mTabView.GetPageCount(); ++i) { // Delete tab item.
          HWND hwnd = m.mTabView.GetPageHWND(i);
          int id = (int)::SendMessage(hwnd, WM_GOOD_GETRESOURCEID, 0, 0);
          if (id == idItem) {
            m.mTabView.RemovePage(i);
            break;
          }
        }

        mTree.DeleteItem(sel);          // Delete tree item.
      }
    }

    return 0;
  }

  LRESULT OnTreeSelChanged(LPNMHDR pnmh)
  {
    if (mTree != pnmh->hwndFrom) {
      return 0;
    }

    HTREEITEM sel = mTree.GetSelectedItem();
    if (NULL == sel) {
      mProp.Reset();
      return 0;
    }

    HTREEITEM group = mTree.GetParentItem(sel);
    if (NULL == group) {
      if (GOOD_RESOURCE_PROJECT == (int)mTree.GetItemData(sel)) {
        mProp.SetProperty(-2);          // -2 for dummy.
      } else {
        mProp.Reset();
      }
      return 0;
    }

    int idItem = (int)mTree.GetItemData(sel);

    mProp.SetProperty(idItem);

    //
    // Set page active if this resource item is opened.
    //

    MainT::inst().SetActivePage(idItem);
    mTree.SetFocus();

    return 0;
  }

  //
  // Command handler.
  //

  void OnCloseProjView(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    MainT& m = MainT::inst();

    m.mSplit.SetSinglePaneMode(SPLIT_PANE_RIGHT);
    m.UISetCheck(ID_VIEW_PROJVIEW, FALSE);
  }

  //
  // Impl helper.
  //

  CTreeItem FindTreeItem(int id) const
  {
    CTreeItem hItem = NULL;
    CTreeItem hGroup = mTree.GetChildItem(NULL); // First group.
    while (hGroup) {
      hItem = hGroup.GetChild();
      while (hItem) {
        if (id == (int)hItem.GetData()) {
          break;
        }
        hItem = hItem.GetNextSibling();
      }
      if (hItem) {                      // Found!
        break;
      }
      hGroup = hGroup.GetNextSibling(); // Next group.
    }

    return hItem;
  }
};

// end of Explorer.h
