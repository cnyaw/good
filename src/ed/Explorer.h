
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
class CTextureResListView : public CResourceListView<MainT>
{
public:
  CTextureResListView()
  {
    CX_THUMB = CY_THUMB = 74;
    CXY_BORDER = 14;
  }

  virtual int GetResCount() const
  {
    return (int)PrjT::inst().mRes.mTexIdx.size();
  }

  virtual int GetResId(int sel) const
  {
    return PrjT::inst().mRes.mTexIdx[sel];
  }

  virtual std::string GetResName(int id) const
  {
    typename const PrjT::TextureT &tex = PrjT::inst().getTex(id);
    return tex.getName();
  }

  virtual int GetResType() const
  {
    return GOOD_RESOURCE_TEXTURE;
  }

  virtual bool LoadResImage(int id, good::gx::GxImage &img) const
  {
    typename const PrjT::TextureT &tex = PrjT::inst().getTex(id);
    return img.load(tex.mFileName);
  }
};

template<class MainT>
class CSpriteResListView : public CResourceListView<MainT>
{
public:
  CSpriteResListView()
  {
    CX_THUMB = CY_THUMB = 74;
    CXY_BORDER = 14;
  }

  virtual int GetResCount() const
  {
    return (int)PrjT::inst().mRes.mSpriteIdx.size();
  }

  virtual int GetResId(int sel) const
  {
    return PrjT::inst().mRes.mSpriteIdx[sel];
  }

  virtual std::string GetResName(int id) const
  {
    typename const PrjT::SpriteT &spr = PrjT::inst().getSprite(id);
    return spr.getName();
  }

  virtual int GetResType() const
  {
    return GOOD_RESOURCE_SPRITE;
  }

  virtual bool LoadResImage(int id, good::gx::GxImage &img) const
  {
    typename const PrjT::SpriteT &spr = PrjT::inst().getSprite(id);
    if (!img.create(spr.mTileset.mTileWidth, spr.mTileset.mTileHeight, 4)) {
      return false;
    }
    typename const PrjT::TextureT &tex = PrjT::inst().getTex(spr.mTileset.mTextureId);
    if (!spr.mFrame.empty()) {
      good::gx::GxImage imgTex;
      if (!imgTex.load(tex.mFileName)) {
        return false;
      }
      int tile = spr.mFrame[0];
      int srcx = spr.mTileset.mTileWidth * (tile % spr.mTileset.mCxTile);
      int srcy = spr.mTileset.mTileHeight * (tile / spr.mTileset.mCxTile);
      img.draw(0, 0, imgTex, srcx, srcy, spr.mTileset.mTileWidth, spr.mTileset.mTileHeight);
    }
    return true;
  }
};

template<class MainT>
class CExplorerView : public CWindowImpl<CExplorerView<MainT> >
{
public:
  DECLARE_WND_CLASS_EX(NULL, 0, COLOR_BTNFACE)

  CHorSplitterWindow mSplit;

  CTabView mTabView;
  CPaneContainer mPaneProp;
  CTreeViewCtrlEx mTree;
  CTextureResListView<MainT> mTexRes;
  CSpriteResListView<MainT> mSprRes;
  CExplorerPropView<MainT> mProp;

  CImageListManaged mImages;

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

  void FINDINUSEDMSG(std::string const& name, std::string const& owner)
  {
    if (owner.empty()) {
      SW2_TRACE_ERROR("Item is still used by %s", name.c_str());
    } else {
      SW2_TRACE_ERROR("Item is still used by %s of %s", name.c_str(), owner.c_str());
    }
  }

  template<int ResType, class ResT, class IdxT>
  void FillResourceTree(ResT const& res, IdxT const& idx, CString const& resName)
  {
    MainT& m = MainT::inst();
    for (size_t i = 0; i < idx.size(); ++i) {
      typename ResT::const_iterator it = res.find(idx[i]);
      m.AddResourceItem(resName, it->second.getName(), idx[i], ResType, NULL);
    }
  }

  template<int ResType, class ResT, class IdxT>
  void FillResourceTree2(ResT const& res, IdxT const& idx, CString const& resName)
  {
    MainT& m = MainT::inst();
    for (size_t i = 0; i < idx.size(); ++i) {
      typename ResT::const_iterator it = res.find(idx[i]);
      std::string name = it->second;
      name = ExtractFileName(name);
      if (name.empty()) {               // This is a search path.
        name = it->second;
      }
      m.AddResourceItem(resName, name, idx[i], ResType, NULL);
    }
  }

  void FillTree()
  {
    PrjT::ResT const& res = PrjT::inst().mRes;
    FillResourceTree<GOOD_RESOURCE_AUDIO>(res.mSnd, res.mSndIdx, _T("Audio"));
    FillResourceTree<GOOD_RESOURCE_TEXTURE>(res.mTex, res.mTexIdx, _T("Texture"));
    FillResourceTree<GOOD_RESOURCE_SPRITE>(res.mSprite, res.mSpriteIdx, _T("Sprite"));
    FillResourceTree<GOOD_RESOURCE_MAP>(res.mMap, res.mMapIdx, _T("Map"));
    FillResourceTree<GOOD_RESOURCE_LEVEL>(res.mLevel, res.mLevelIdx, _T("Level"));
    FillResourceTree2<GOOD_RESOURCE_SCRIPT>(res.mScript, res.mScriptIdx, _T("Script"));
    FillResourceTree2<GOOD_RESOURCE_PARTICLE>(res.mStgeScript, res.mStgeScriptIdx, _T("Particle"));
    FillResourceTree2<GOOD_RESOURCE_DEPENDENCY>(res.mDep, res.mDepIdx, _T("Dependency"));
    mTree.InsertItem(_T("Project"), 3, 3, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_PROJECT); // Project info.
    mTexRes.SetList();
    mSprRes.SetList();
  }

  void InitTree()
  {
    mTree.DeleteAllItems();
    mTree.InsertItem(_T("Audio"), 0, 0, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_AUDIO);
    mTree.InsertItem(_T("Texture"), 0, 0, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_TEXTURE);
    mTree.InsertItem(_T("Sprite"), 0, 0, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_SPRITE);
    mTree.InsertItem(_T("Map"), 0, 0, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_MAP);
    mTree.InsertItem(_T("Level"), 0, 0, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_LEVEL);
    mTree.InsertItem(_T("Script"), 0, 0, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_SCRIPT);
    mTree.InsertItem(_T("Particle"), 0, 0, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_PARTICLE);
    mTree.InsertItem(_T("Dependency"), 0, 0, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_DEPENDENCY);
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

  bool SureRemoveRes(const std::string &name) const
  {
    CString msg;
    msg.Format(_T("Are you sure to remove resource item '%s'?"), name.c_str());
    return IDYES == ::MessageBox(m_hWnd, msg, CString((LPCTSTR)IDR_MAINFRAME), MB_YESNOCANCEL | MB_ICONQUESTION);
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

    mTabView.Create(mSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    mTree.Create(mTabView, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
    mTabView.AddPage(mTree, _T("Resource"));
    mTexRes.Create(mTabView, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
    mTabView.AddPage(mTexRes, _T("Texture"));
    mSprRes.Create(mTabView, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
    mTabView.AddPage(mSprRes, _T("Sprite"));
    mTabView.SetActivePage(0);

    mPaneProp.Create(mSplit, _T("Property"));
    mPaneProp.SetPaneContainerExtendedStyle(PANECNT_NOBORDER | PANECNT_NOCLOSEBUTTON);
    mProp.Create(mPaneProp);
    mPaneProp.SetClient(mProp);

    mSplit.SetSplitterPanes(mTabView, mPaneProp);
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

    CImageListManaged img = TreeView_CreateDragImage(mTree, lpnmtv->itemNew.hItem);
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

  template<class EditorT>
  void AddResourceEditor(int idItem, const std::string &name)
  {
    MainT& m = MainT::inst();
    m.AddResourceEditor(name, idItem, m.CreateEditor<EditorT>(idItem));
  }

  bool AddEditorView(int typeItem, int idItem)
  {
    PrjT &prj = PrjT::inst();

    switch (typeItem)
    {
    case GOOD_RESOURCE_AUDIO:
      AddResourceEditor<CSoundEditor<MainT> >(idItem, prj.getSnd(idItem).getName());
      return true;

    case GOOD_RESOURCE_TEXTURE:
      AddResourceEditor<CTextureEditor<MainT> >(idItem, prj.getTex(idItem).getName());
      return true;

    case GOOD_RESOURCE_MAP:
      AddResourceEditor<CMapEditor<MainT> >(idItem, prj.getMap(idItem).getName());
      return true;

    case GOOD_RESOURCE_SPRITE:
      AddResourceEditor<CSpriteEditor<MainT> >(idItem, prj.getSprite(idItem).getName());
      return true;

    case GOOD_RESOURCE_LEVEL:
      AddResourceEditor<CLevelEditor<MainT> >(idItem, prj.getLevel(idItem).getName());
      return true;

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
          AddResourceEditor<CScriptEditor<MainT> >(idItem, name);
        } else {
          AddResourceEditor<CStgeScriptEditor<MainT> >(idItem, name);
        }
      }
      return true;
    }

    return false;
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

    if (AddEditorView(typeItem, idItem)) {
      return 1;
    }

    return 0;
  }

  bool RemoveResItem(int typeItem, int idItem)
  {
    PrjT& prj = PrjT::inst();

    switch (typeItem)               // Delete resource item.
    {
    case GOOD_RESOURCE_AUDIO:
      if (!SureRemoveRes(prj.getSnd(idItem).getName())) {
        return false;
      }
      if (!prj.removeSnd(idItem)) {
        return false;
      }
      break;

    case GOOD_RESOURCE_TEXTURE:
      {
        if (!SureRemoveRes(prj.getTex(idItem).getName())) {
          return false;
        }
        std::string lvlName, oName;
        if (prj.isTexUsed(idItem, lvlName, oName)) {
          FINDINUSEDMSG(oName, lvlName);
          return false;
        }
        if (!prj.removeTex(idItem)) {
          return false;
        }
      }
      break;

    case GOOD_RESOURCE_MAP:
      {
        if (!SureRemoveRes(prj.getMap(idItem).getName())) {
          return false;
        }
        std::string lvlName, oName;
        if (prj.isMapUsed(idItem, lvlName, oName)) {
          FINDINUSEDMSG(oName, lvlName);
          return false;
        }
        if (!prj.removeMap(idItem)) {
          return false;
        }
      }
      break;

    case GOOD_RESOURCE_SPRITE:
      {
        if (!SureRemoveRes(prj.getSprite(idItem).getName())) {
          return false;
        }
        std::string lvlName, oName;
        if (prj.isSpriteUsed(idItem, lvlName, oName)) {
          FINDINUSEDMSG(oName, lvlName);
          return false;
        }
        if (!prj.removeSprite(idItem)) {
          return false;
        }
      }
      break;

    case GOOD_RESOURCE_LEVEL:
      if (!SureRemoveRes(prj.getLevel(idItem).getName())) {
        return false;
      }
      if (!prj.removeLevel(idItem)) {
        return false;
      }
      break;

    case GOOD_RESOURCE_SCRIPT:
      if (!SureRemoveRes(prj.getScript(idItem))) {
        return false;
      }
      if (!prj.removeScript(idItem)) {
        return false;
      }
      break;

    case GOOD_RESOURCE_PARTICLE:
      if (!SureRemoveRes(prj.getStgeScript(idItem))) {
        return false;
      }
      if (!prj.removeStgeScript(idItem)) {
        return false;
      }
      break;

    case GOOD_RESOURCE_DEPENDENCY:
      if (!SureRemoveRes(prj.getDep(idItem))) {
        return false;
      }
      if (!prj.removeDep(idItem)) {
        return false;
      }
      break;
    }

    return true;
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

        if (!RemoveResItem(typeItem, idItem)) {
          return 0;
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
