
//
// MainFrm.h
// Application main frame window.
//
// Copyright (c) 2007 Waync Cheng.
// All Rights Reserved.
//
// 2007/10/14 WTL wizard created, Waync updated.
//

#pragma once

class CMainFrame :
  public CFrameWindowImpl<CMainFrame>,
  public CUpdateUI<CMainFrame>,
  public CMessageFilter,
  public CIdleHandler
{
  CMainFrame()
  {
  }

public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  CCommandBarCtrl mCmdBar;
  CMultiPaneStatusBarCtrl mStatus;

  CSplitterWindow mSplit;

  CExplorerView<CMainFrame> mExpView;
  CTabView mTabView;

  CErrorEditor<CMainFrame> mOutput;

  std::string mFileName;                // Project file name.

  CRecentDocumentList mMru;

  static CMainFrame& inst()
  {
    static CMainFrame instance;
    return instance;
  }

  virtual BOOL PreTranslateMessage(MSG* pMsg)
  {
    if (WM_MOUSEWHEEL == pMsg->message) { // Mouse wheel on view.
      POINT pt;
      ::GetCursorPos(&pt);
      HWND hwnd = ::WindowFromPoint(pt);
      ::SendMessage(hwnd, WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
      return TRUE;
    }

    if (WM_LBUTTONDBLCLK == pMsg->message && pMsg->hwnd == mTabView.m_tab) {

      //
      // Double-click on tab to close tab page.
      //

      TCHAR n[64];
      n[::GetClassName(pMsg->hwnd, n, 64)] = TCHAR('\0');

      if (!::lstrcmp(n, _T("ATL:SysTabControl32"))) {
        OnWindowClose(0,0,0);
        return TRUE;
      }
    }

    if (mTabView.PreTranslateMessage(pMsg)) {
      return TRUE;
    }

    if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg)) {
      return TRUE;
    }

    return FALSE;
  }

  virtual BOOL OnIdle()
  {
    PrjT& prj = PrjT::inst();

    int nActivePage = mTabView.GetActivePage();

    UIEnable(
      ID_FILE_SAVE,
      !mFileName.empty() && (prj.canStore() ||
      (-1 != nActivePage && ::SendMessage(mTabView.GetPageHWND(nActivePage),
      WM_GOOD_CANSAVE, 0, 0))));
    UIEnable(ID_FILE_SAVE_AS, !mFileName.empty());
    UIEnable(ID_FILE_ADDNEWAUDIO, !mFileName.empty());
    UIEnable(ID_FILE_ADDNEWTEXTURE, !mFileName.empty());
    UIEnable(ID_FILE_ADDNEWMAP, !mFileName.empty());
    UIEnable(ID_FILE_ADDNEWSPRITE, !mFileName.empty());
    UIEnable(ID_FILE_ADDNEWOBJECT, !mFileName.empty());
    UIEnable(ID_FILE_ADDNEWLEVEL, !mFileName.empty());
    UIEnable(ID_FILE_ADDNEWSCRIPT, !mFileName.empty());
    UIEnable(ID_FILE_ADDNEWSTGE, !mFileName.empty());
    UIEnable(ID_FILE_ADDNEWDEPENDENCY, !mFileName.empty());
    UIEnable(ID_PROJECT_NEWSEARCHPATH, !mFileName.empty());
    UIEnable(ID_PROJECT_RUNTHEGAME, !mFileName.empty());
    UIEnable(ID_PROJECT_RUNTHISLEVEL, !mFileName.empty() && -1 != GetPlayThisLevelId());
    UIEnable(ID_PROJECT_CREATEPACKAGE, !mFileName.empty());
    UIEnable(ID_PROJECT_CREATEZIPPACKAGE, !mFileName.empty());
    UIEnable(
      ID_EDIT_UNDO,
      -1 != nActivePage &&
      ::SendMessage(mTabView.GetPageHWND(nActivePage), WM_GOOD_CANUNDO, 0, 0));
    UIEnable(
      ID_EDIT_REDO,
      -1 != nActivePage &&
      ::SendMessage(mTabView.GetPageHWND(nActivePage), WM_GOOD_CANREDO, 0, 0));
    UISetCheck(ID_VIEW_OUTPUTWINDOW, mOutput.mVisible);
    UISetCheck(ID_VIEW_SHOWFPS, good::rt::CPlayerWindow::getInst().showFPS);
    UIEnable(ID_WINDOW_CLOSE, -1 != nActivePage);
    UIEnable(ID_WINDOW_CLOSE_ALL, 0 < mTabView.GetPageCount());

    UIUpdateToolBar();

    return FALSE;
  }

  BEGIN_UPDATE_UI_MAP(CMainFrame)
    UPDATE_ELEMENT(ID_FILE_SAVE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_SAVE_AS, UPDUI_MENUPOPUP)
    UPDATE_ELEMENT(ID_FILE_ADDNEWAUDIO, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_ADDNEWTEXTURE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_ADDNEWMAP, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_ADDNEWSPRITE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_ADDNEWOBJECT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_ADDNEWLEVEL, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_ADDNEWSCRIPT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_ADDNEWSTGE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_FILE_ADDNEWDEPENDENCY, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_PROJECT_NEWSEARCHPATH, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_EDIT_UNDO, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_EDIT_REDO, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_PROJECT_RUNTHEGAME, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_PROJECT_RUNTHISLEVEL, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_PROJECT_CREATEPACKAGE, UPDUI_MENUPOPUP)
    UPDATE_ELEMENT(ID_PROJECT_CREATEZIPPACKAGE, UPDUI_MENUPOPUP)
    UPDATE_ELEMENT(ID_VIEW_PROJVIEW, UPDUI_MENUPOPUP| UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_VIEW_OUTPUTWINDOW, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_VIEW_SHOWFPS, UPDUI_MENUPOPUP)
    UPDATE_ELEMENT(ID_WINDOW_CLOSE, UPDUI_MENUPOPUP)
    UPDATE_ELEMENT(ID_WINDOW_CLOSE_ALL, UPDUI_MENUPOPUP)
  END_UPDATE_UI_MAP()

  BEGIN_MSG_MAP_EX(CMainFrame)
    MSG_WM_CLOSE(OnClose)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_ENDSESSION(OnEndSession)
    MSG_WM_QUERYENDSESSION(OnQueryEndSession)
    COMMAND_ID_HANDLER_EX(ID_APP_EXIT, OnFileExit)
    COMMAND_ID_HANDLER_EX(ID_FILE_NEW, OnFileNew)
    COMMAND_ID_HANDLER_EX(ID_FILE_OPEN, OnFileOpen)
    COMMAND_ID_HANDLER_EX(ID_FILE_SAVE, OnFileSave)
    COMMAND_ID_HANDLER_EX(ID_FILE_SAVE_AS, OnFileSaveAs)
    COMMAND_ID_HANDLER_EX(ID_FILE_ADDNEWAUDIO, OnAddNewSound)
    COMMAND_ID_HANDLER_EX(ID_FILE_ADDNEWTEXTURE, OnAddNewTex)
    COMMAND_ID_HANDLER_EX(ID_FILE_ADDNEWSCRIPT, OnAddNewScript)
    COMMAND_ID_HANDLER_EX(ID_FILE_ADDNEWSTGE, OnAddNewStgeScript)
    COMMAND_ID_HANDLER_EX(ID_FILE_ADDNEWDEPENDENCY, OnAddNewDep)
    COMMAND_ID_HANDLER_EX(ID_PROJECT_NEWSEARCHPATH, OnAddNewPath)
    COMMAND_ID_HANDLER_EX(ID_EDIT_REDO, OnEditRedo)
    COMMAND_ID_HANDLER_EX(ID_EDIT_UNDO, OnEditUndo)
    COMMAND_ID_HANDLER_EX(ID_PROJECT_RUNTHEGAME, OnRunTheGame)
    COMMAND_ID_HANDLER_EX(ID_PROJECT_RUNTHISLEVEL, OnRunThisLevel)
    COMMAND_ID_HANDLER_EX(ID_PROJECT_PLAYFILE, OnPlayPackage)
    COMMAND_ID_HANDLER_EX(ID_PROJECT_CREATEPACKAGE, OnCreatePackage)
    COMMAND_ID_HANDLER_EX(ID_PROJECT_CREATEZIPPACKAGE, OnCreateZipPackage)
    COMMAND_ID_HANDLER_EX(ID_VIEW_PROJVIEW, OnViewProjView)
    COMMAND_ID_HANDLER_EX(ID_VIEW_OUTPUTWINDOW, OnViewOutputWindow)
    COMMAND_ID_HANDLER_EX(ID_VIEW_SHOWFPS, OnViewShowFps)
    COMMAND_ID_HANDLER_EX(ID_HELP_GOODAPIREFERENCE, OnShowApiRef)
    COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
    COMMAND_ID_HANDLER_EX(ID_HELP_CHECKFORUPDATE, OnCheckUpdate)
    COMMAND_ID_HANDLER_EX(ID_WINDOW_CLOSE, OnWindowClose)
    COMMAND_ID_HANDLER_EX(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
    COMMAND_RANGE_HANDLER_EX(ID_FILE_ADDNEWMAP, ID_FILE_ADDNEWLEVEL, OnAddNewItem)
    COMMAND_RANGE_HANDLER_EX(ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate)
    COMMAND_RANGE_HANDLER_EX(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnMruMenuItem)
    NOTIFY_CODE_HANDLER_EX(TBVN_PAGEACTIVATED, OnPageActivated)
    CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
    CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    if (WM_COMMAND == uMsg) {
      int nActivePage = mTabView.GetActivePage();
      if (-1 != nActivePage) {
        HWND hWnd;
        if (nActivePage != mTabView.PageIndexFromHwnd(mOutput)) {
          hWnd = mTabView.GetPageHWND(nActivePage);
        } else {
          hWnd = mOutput.mEdit;
        }
        ::SendMessage(hWnd, uMsg, wParam, lParam);
      }
    }
  END_MSG_MAP()

  //
  // Message handler.
  //

  void OnClose()
  {
    OnEndSession(OnQueryEndSession(0,0), 0);
  }

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    //
    // Create command bar window.
    //

    HWND hWndCmdBar = mCmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
    mCmdBar.AttachMenu(GetMenu());
    SetMenu(NULL);

    //
    // Set up MRU stuff.
    //

    CMenuHandle menu = mCmdBar.GetMenu();
    CMenuHandle menuFile = menu.GetSubMenu(0);
    CMenuHandle menuMru = menuFile.GetSubMenu(5);
    mMru.SetMenuHandle(menuMru);
    mMru.SetMaxEntries(12);

    LoadConfig();

    //
    // Create toolbar & statusbar.
    //

    HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

    CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
    AddSimpleReBarBand(hWndCmdBar);
    AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

    CreateSimpleStatusBar();
    mStatus.SubclassWindow(m_hWndStatusBar);

    int arrPanes[] = {ID_DEFAULT_PANE, 1, 2};
    mStatus.SetPanes(arrPanes, sizeof(arrPanes) / sizeof(int), false);

    int arrWidths[] = {0, 200, 160};
    SetPaneWidths(arrWidths, sizeof(arrWidths) / sizeof(int));

    UIAddToolBar(hWndToolBar);
    UISetCheck(ID_VIEW_PROJVIEW, TRUE);
    UISetCheck(ID_VIEW_TOOLBAR, TRUE);
    UISetCheck(ID_VIEW_STATUS_BAR, TRUE);

    CReBarCtrl(m_hWndToolBar).LockBands(true); // Lock toolbar.

    //
    // Create views.
    //

    m_hWndClient = mSplit.Create(m_hWnd);

    mExpView.Create(mSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    mTabView.Create(mSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    mSplit.SetSplitterPanes(mExpView, mTabView);
    mSplit.SetActivePane(1);

    mOutput.Create(mTabView);

    //
    // Register object for message filtering and idle updates.
    //

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    //
    // Setup tabview.
    //

    CMenuHandle menuMain = mCmdBar.GetMenu();
    mTabView.SetWindowMenu(menuMain.GetSubMenu(WINDOW_MENU_POSITION));

    UpdateLayout();
    mSplit.SetSplitterPos(210);

    CString cap;
    cap.Format(_T("%s - Untitled"), CString((LPCTSTR)IDR_MAINFRAME));
    SetWindowText(cap);

    mTabView.SetTitleBarWindow(m_hWnd);

    PrjT::inst().closeAll();

    InitTree();
    mExpView.mTree.InsertItem(_T("Project"), 3, 3, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_PROJECT); // Project info.

    SystemParametersInfo(SPI_SETLISTBOXSMOOTHSCROLLING, 0, FALSE, SPIF_SENDCHANGE);

    SetMsgHandled(FALSE);
    return 0;
  }

  void OnDestroy()
  {
    //
    // Unregister message filtering and idle updates.
    //

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

    SetMsgHandled(FALSE);
  }

  void OnEndSession(BOOL bEnding, UINT uLogOff)
  {
    SaveConfig();

    if (bEnding) {
      DestroyWindow();
    }
  }

  LRESULT OnQueryEndSession(UINT nSource, UINT uLogOff)
  {
    if (mFileName.empty()) {
      return TRUE;
    }

    PrjT& prj = PrjT::inst();

    if (prj.canStore()) {
      switch (MessageBox(_T("Save Changes?"), CString((LPCTSTR)IDR_MAINFRAME), MB_YESNOCANCEL | MB_ICONQUESTION))
      {
      case IDYES:                       // Do save.
        if (mFileName.empty()) {
          CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Good Project Files(*.txt)\0*.txt\0"));
          if (IDOK == dlg.DoModal()) {
            mFileName = CompleteFileExt(dlg.m_szFileName, ".txt");
          } else {
            return FALSE;
          }
        }
        if (!prj.store(mFileName)) {
          MessageBox(_T("Save File Failed!"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK | MB_ICONERROR);
          return FALSE;
        }
        break;
      case IDNO:                        // Skip changes.
        break;
      case IDCANCEL:                    // Stop.
        return FALSE;
      }
    }

    return TRUE;
  }

  //
  // Command handler.
  //

  void OnAddNewDep(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(TRUE, NULL, NULL, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Good Package Files(*.txt,*.zip,*.good)\0*.txt;*.zip;*.good\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    std::string name = GetRelativePath(dlg.m_szFileName, mFileName);

    PrjT& prj = PrjT::inst();
    int id = prj.addDep(name);
    if (-1 != id) {
      AddResourceItem(_T("Dependency"), ExtractFileName(name), id, GOOD_RESOURCE_DEPENDENCY, NULL);
    }
  }

  void OnAddNewPath(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFolderDialog dlg;
    if (IDOK != dlg.DoModal()) {
      return;
    }

    std::string name = GetRelativePath(dlg.GetFolderPath(), mFileName);
    name += "/";

    PrjT& prj = PrjT::inst();
    int id = prj.addDep(name);
    if (-1 != id) {
      AddResourceItem(_T("Dependency"), name, id, GOOD_RESOURCE_DEPENDENCY, NULL);
    }
  }

  void OnAddNewScript(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(TRUE, _T("lua"), NULL, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Script Files(*.lua)\0*.lua\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    std::string name = GetRelativePath(dlg.m_szFileName, mFileName);

    PrjT& prj = PrjT::inst();
    int id = prj.addScript(name);
    if (-1 != id) {
      AddResourceItem(_T("Script"), ExtractFileName(name), id, GOOD_RESOURCE_SCRIPT, NULL);
    }
  }

  void OnAddNewSound(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(TRUE, NULL, NULL, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Sound Files(*wav,*.ogg)\0*.wav;*.ogg\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    std::string name = GetRelativePath(dlg.m_szFileName, mFileName);

    PrjT& prj = PrjT::inst();
    int id = prj.addSnd(name);
    if (-1 != id) {
      AddResourceItem(_T("Audio"), prj.getSnd(id).getName(), id, GOOD_RESOURCE_AUDIO, NULL);
    }
  }

  void OnAddNewStgeScript(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(TRUE, _T("stge"), NULL, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Stge Script Files(*.stge)\0*.stge\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    std::string name = GetRelativePath(dlg.m_szFileName, mFileName);

    PrjT& prj = PrjT::inst();
    int id = prj.addStgeScript(name);
    if (-1 != id) {
      AddResourceItem(_T("Particle"), ExtractFileName(name), id, GOOD_RESOURCE_PARTICLE, CreateEditor<CStgeScriptEditor<CMainFrame> >(id));
    }
  }

  void OnAddNewTex(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(TRUE, NULL, NULL, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, _T("Image Files(*.bmp,*.jpg,*.gif,*.png)\0*.bmp;*.jpg;") _T("*.gif;*.png\0"));

    const int LenBuff = 4096;
    TCHAR Buff[LenBuff] = {0};
    dlg.m_ofn.lpstrFile = Buff;
    dlg.m_ofn.nMaxFile = LenBuff;

    if (IDOK != dlg.DoModal()) {
      return;
    }

    if (0 != Buff[dlg.m_ofn.nFileOffset - 1]) {
      Buff[dlg.m_ofn.nFileOffset - 1] = 0;
    }

    PrjT& prj = PrjT::inst();
    CString path = Buff;
    for (int i = path.GetLength() + 1, s = i; i < LenBuff; i++) {
      if (0 != Buff[i]) {
        continue;
      }
      CString n(Buff + s, i - s);
      n = path + _T("\\") + n;
      std::string name = GetRelativePath((const char*)n, mFileName);
      int id = prj.addTex<ImgT>(name);
      if (-1 != id) {
        AddResourceItem(_T("Texture"), prj.getTex(id).getName(), id, GOOD_RESOURCE_TEXTURE, CreateEditor<CTextureEditor<CMainFrame> >(id));
      }
      if (0 == Buff[i + 1]) {
        break;
      }
      s = i + 1;
    }
  }

  void OnAddNewItem(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT& prj = PrjT::inst();

    if (ID_FILE_ADDNEWLEVEL == nID) {
      int idLevel = prj.addLevel("", prj.mRes.mWidth, prj.mRes.mHeight);
      if (-1 != idLevel) {
        AddResourceItem(_T("Level"), prj.getLevel(idLevel).getName(), idLevel, GOOD_RESOURCE_LEVEL, CreateEditor<CLevelEditor<CMainFrame> >(idLevel));
      }
    } else {
      CDlgAddNewItem<CMainFrame>(nID - ID_FILE_ADDNEWMAP + GOOD_RESOURCE_MAP).DoModal();
    }
  }

  void OnAppAbout(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CDlgAbout().DoModal();
  }

  void OnCheckUpdate(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    char temp[MAX_PATH];
    ::tmpnam(temp);

    HRESULT hr = URLDownloadToFile(NULL, GOOD_CHECK_UPDATE_URL, temp, 0, NULL);
    if (FAILED(hr)) {
      MessageBox(_T("Connection error!"), _T("Error"), MB_OK | MB_ICONERROR);
      return;
    }

    std::ifstream ifs(temp);
    if (!ifs) {
      MessageBox(_T("Check update failed!"), _T("Error"), MB_OK | MB_ICONERROR);
      return;
    }

    std::stringstream ss;
    ss << ifs.rdbuf();

    ifs.close();
    ::remove(temp);

    std::string s = ss.str();

    if (CURRENT_GOOD_EDITOR_VERSION != s) {
      CString str;
      str.Format(_T("New version '%s' is available on good-ed forum."), s.c_str());
      MessageBox(str, CString((LPCTSTR)IDR_MAINFRAME), MB_OK);
    } else {
      MessageBox(_T("You are using the latest version."), CString((LPCTSTR)IDR_MAINFRAME), MB_OK);
    }
  }

  void OnCreatePackage(LPCTSTR msg, std::string const &ext, LPCTSTR title, bool encrypt)
  {
    CFileDialog dlg(FALSE, NULL, NULL, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, msg);
    if (IDOK != dlg.DoModal()) {
      return;
    }

    std::string name = CompleteFileExt(dlg.m_szFileName, ext);

    std::ofstream ofs(name.c_str(), std::ios_base::binary);
    if (!PrjT::inst().createPackage(ofs, encrypt)) {
      ofs.close();
      remove(name.c_str());
      MessageBox(title, CString((LPCTSTR)IDR_MAINFRAME), MB_OK | MB_ICONERROR);
    }
  }

  void OnCreatePackage(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    OnCreatePackage(_T("Good Package Files(*.good)\0*.good\0"), ".good", _T("Create Good Package Failed!"), true);
  }

  void OnCreateZipPackage(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    OnCreatePackage(_T("Good Zip Package Files(*.zip)\0*.zip\0"), ".zip", _T("Create Good Zip Package Failed!"), false);
  }

  void OnEdit(UINT Msg, UINT Msg2)
  {
    int nActivePage = mTabView.GetActivePage();
    if (-1 != nActivePage && ::SendMessage(mTabView.GetPageHWND(nActivePage), Msg, 0, 0)) {
      ::SendMessage(mTabView.GetPageHWND(nActivePage), Msg2, 0, 0);
    }
  }

  void OnEditRedo(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    OnEdit(WM_GOOD_CANREDO, WM_GOOD_REDO);
  }

  void OnEditUndo(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    OnEdit(WM_GOOD_CANUNDO, WM_GOOD_UNDO);
  }

  void OnFileClose(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (!OnQueryEndSession(0, 0)) {      // Save changes if necessary.
      return;
    }

    PrjT::inst().closeAll();
    good::gx::GdiImageResource::inst().clear();
    good::snd::AudiereSoundResource::inst().free();

    mFileName.clear();                  // Set un-titiled.
    mExpView.mTree.DeleteAllItems();
    mExpView.mProp.Reset();
    OnWindowCloseAll(0, 0, 0);

    CString cap;
    cap.Format(_T("%s - Untitled"), CString((LPCTSTR)IDR_MAINFRAME));
    SetWindowText(cap);

    mTabView.SetTitleBarWindow(m_hWnd);
  }

  void OnFileNew(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (!OnQueryEndSession(0, 0)) {     // Save changes if necessary.
      return;
    }

    CDlgNewProj dlg;
    if (IDOK != dlg.DoModal()) {
      return;
    }

    //
    // Reset proj.
    //

    InitTree();
    mExpView.mTree.InsertItem(_T("Project"), 3, 3, TVI_ROOT, TVI_LAST).SetData(GOOD_RESOURCE_PROJECT); // Project info.
    mExpView.mProp.Reset();
    OnWindowCloseAll(0, 0, 0);

    PrjT& prj = PrjT::inst();

    prj.closeAll();
    good::gx::GdiImageResource::inst().clear();

    //
    // Store new proj.
    //

    std::string name = dlg.mPath;

    prj.mRes.mFileName = name;
    prj.mRes.mName = dlg.mName;
    prj.mRes.mWidth = dlg.mWidth;
    prj.mRes.mHeight = dlg.mHeight;

    if (!prj.store(name)) {
      MessageBox(_T("Store File Failed!"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK | MB_ICONERROR);
      return;
    }

    mMru.AddToList(dlg.mPath);
    SaveConfig();

    mFileName = name;

    SetCaption();
  }

  void OnFileExit(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PostMessage(WM_CLOSE);
  }

  void OnFileOpen(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (!OnQueryEndSession(0, 0)) {
      return;
    }

    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Good Project Files(*.txt)\0*.txt\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    PrjT& prj = PrjT::inst();

    if (dlg.m_szFileName == mFileName) {
      return;
    }

    if (!prj.load(dlg.m_szFileName)) {
      MessageBox(_T("Load File Failed!"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK | MB_ICONERROR);
      return;
    }

    mMru.AddToList(dlg.m_szFileName);
    SaveConfig();

    InitTree();
    mExpView.mProp.Reset();
    OnWindowCloseAll(0, 0, 0);

    mFileName = dlg.m_szFileName;
    SetCaption();

    FillTree();
  }

  void OnFileSave(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    int nActivePage = mTabView.GetActivePage();
    if (-1 != nActivePage && ::SendMessage(mTabView.GetPageHWND(nActivePage), WM_GOOD_CANSAVE, 0, 0)) {
      ::SendMessage(mTabView.GetPageHWND(nActivePage), WM_GOOD_SAVE, 0, 0);
    } else {

      PrjT& prj = PrjT::inst();

      std::string name(mFileName);
      if (name.empty()) {
        CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Good Project Files(*.txt)\0*.txt\0"));
        if (IDOK != dlg.DoModal()) {
          return;
        }
        name = CompleteFileExt(dlg.m_szFileName, ".txt");
      } else if (!prj.canStore()) {
        return;
      }

      if (!prj.store(name)) {
        MessageBox(_T("Store File Failed!"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK | MB_ICONERROR);
        return;
      }

      prj.mRes.mFileName = name;
      mFileName = name;

      SetCaption();
    }
  }

  void OnFileSaveAs(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Good Project Files(*.txt)\0*.txt\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    std::string name = CompleteFileExt(dlg.m_szFileName, ".txt");

    PrjT& prj = PrjT::inst();
    if (!prj.store(name)) {
      MessageBox(_T("Store File Failed!"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK | MB_ICONERROR);
      return;
    }

    //
    // Update resource relative paths.
    //

    if (!mFileName.empty()) {
      std::string orgpath = good::getPathName(prj.mRes.mFileName);
      std::string newpath = good::getPathName(name);
      if (orgpath != newpath) {

        newpath = name;

        UpdateRelativePath(prj.mRes.mSnd, orgpath, newpath);
        UpdateRelativePath(prj.mRes.mTex, orgpath, newpath);

        UpdateRelativePath2(prj.mRes.mScript, orgpath, newpath);
        UpdateRelativePath2(prj.mRes.mStgeScript, orgpath, newpath);
        UpdateRelativePath2(prj.mRes.mDep, orgpath, newpath);

        //
        // Save again.
        //

        (void)prj.store(name);
      }
    }

    prj.mRes.mFileName = name;

    mFileName = name;
    SetCaption();

    mExpView.UpdateProperty(-1);
  }

  void OnMruMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CString sFile;
    if (!mMru.GetFromList(nID, sFile)) {
      return;
    }

    if (!OnQueryEndSession(0, 0)) {
      return;
    }

    if ((char const*)sFile == mFileName) {
      return;
    }

    PrjT& prj = PrjT::inst();
    if (!prj.load((char const*)sFile)) {
      MessageBox(_T("Load File Failed!"), CString((LPCTSTR)IDR_MAINFRAME), MB_OK | MB_ICONERROR);
      mMru.RemoveFromList(nID);
      return;
    }

    TCHAR CurrPath[MAX_PATH];
    lstrcpy(CurrPath, sFile);

    PathRemoveFileSpec(CurrPath);
    PathAddBackslash(CurrPath);
    SetCurrentDirectory(CurrPath);

    mMru.MoveToTop(nID);
    SaveConfig();

    InitTree();
    mExpView.mProp.Reset();
    OnWindowCloseAll(0, 0, 0);

    mFileName = sFile;
    SetCaption();

    FillTree();
  }

  void OnPlayPackage(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Good Package Files(*.txt,*.zip,*.good)\0*.txt;*.zip;*.good\0"));
    if (IDOK != dlg.DoModal()) {
      return;
    }

    PlayGame(dlg.m_szFileName, -1);
  }

  void OnRunTheGame(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (mFileName.empty()) {
      return;
    }

    if (!OnQueryEndSession(0, 0)) {
      return;
    }

    PlayGame(mFileName, -1);
  }

  void OnRunThisLevel(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (mFileName.empty()) {
      return;
    }

    if (!OnQueryEndSession(0, 0)) {
      return;
    }

    int idLevel = GetPlayThisLevelId();
    if (-1 != idLevel) {
      PlayGame(mFileName, idLevel);
    }
  }

  void OnShowApiRef(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    ShellOpen(_T("http://good-ed.smallworld.idv.tw/wiki/index.php?title=Good_API_Reference"));
  }

  void OnViewShowFps(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    good::rt::CPlayerWindow& mf = good::rt::CPlayerWindow::getInst();
    mf.showFPS = !mf.showFPS;
  }

  void OnViewOutputWindow(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    if (mOutput.mVisible) {
      mOutput.mVisible = false;
      mTabView.m_bDestroyPageOnRemove = false;
      mTabView.RemovePage(mTabView.PageIndexFromHwnd(mOutput));
      mTabView.m_bDestroyPageOnRemove = true;
    } else {
      mOutput.mVisible = true;
      int idx = mTabView.PageIndexFromHwnd(mOutput);
      if (-1 == idx) {
        mTabView.AddPage(mOutput, _T("Output"));
      } else {
        mTabView.SetActivePage(idx);
      }
    }
  }

  void OnViewProjView(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    bool bHide = SPLIT_PANE_RIGHT == mSplit.GetSinglePaneMode();
    bHide = !bHide;
    mSplit.SetSinglePaneMode(bHide ? SPLIT_PANE_RIGHT : SPLIT_PANE_NONE);
    UISetCheck(ID_VIEW_PROJVIEW, !bHide);
  }

  void OnWindowClose(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    int nActivePage = mTabView.GetActivePage();
    if(nActivePage == -1) {
      return;
    }

    if (::SendMessage(mTabView.GetPageHWND(nActivePage), WM_GOOD_CANSAVE, 0, 0)) {
      switch (MessageBox(_T("Save Changes?"), CString((LPCTSTR)IDR_MAINFRAME), MB_YESNOCANCEL | MB_ICONQUESTION))
      {
      case IDYES:                       // Do save.
        if (!::SendMessage(mTabView.GetPageHWND(nActivePage), WM_GOOD_SAVE, 0, 0)) {
          return;
        }
        break;
      case IDNO:                        // Skip changes.
        break;
      case IDCANCEL:                    // Stop.
        return;
      }
    }

    bool bIsOuput = mTabView.GetPageHWND(nActivePage) == mOutput;
    if (bIsOuput) {
      mOutput.mVisible = false;
      mTabView.m_bDestroyPageOnRemove = false;
    }

    mTabView.RemovePage(nActivePage);
    if (bIsOuput) {
      mTabView.m_bDestroyPageOnRemove = true;
    }
  }

  void OnWindowCloseAll(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    //
    // It may cause crash because of window redraw that is caused by calling
    // RemoveAllPages when open new project. So destroy page windows manaully.
    //

    bool visible = mOutput.mVisible;
    if (visible) {
      mOutput.mVisible = false;
    }

    int nh = mTabView.GetPageCount();
    HWND h[128];

    for (int i = 0; i < nh; i++) {
      h[i] = mTabView.GetPageHWND(i);
    }

    mTabView.m_bDestroyPageOnRemove = false;
    mTabView.RemoveAllPages();
    mTabView.m_bDestroyPageOnRemove = true;

    for (int i = 0; i < nh; i++) {
      if (h[i] != mOutput) {
        ::DestroyWindow(h[i]);
      }
    }

    if (visible) {
      OnViewOutputWindow(0, 0, 0);
    }
  }

  void OnWindowActivate(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    int nPage = nID - ID_WINDOW_TABFIRST;
    mTabView.SetActivePage(nPage);
  }

  //
  // Notification handler.
  //

  LRESULT OnPageActivated(LPNMHDR pnmh)
  {
    if (-1 == pnmh->idFrom) {

      //
      // Clear, this happens when all page close.
      //

      mExpView.SetCurSel(-1);
    } else {
      mExpView.SetCurSel((int)::SendMessage(mTabView.GetPageHWND(pnmh->idFrom), WM_GOOD_GETRESOURCEID, 0, 0));
    }

    return 0;
  }

  //
  // Util.
  //

  template<class EditorT>
  HWND CreateEditor(int resId) const
  {
    EditorT* pEditor = new EditorT(resId);
    pEditor->Create(mTabView, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
    return pEditor->m_hWnd;
  }

  void AddResourceItem(CString const& groupName, std::string const& resName, int resId, int ResType, HWND hEditor)
  {
    CTreeViewCtrlEx& tree = mExpView.mTree;
    CTreeItem ti = tree.GetChildItem(NULL);
    while (ti) {                        // Find group item.
      if (ResType == (int)ti.GetData()) {
        break;
      }
      ti = tree.GetNextSiblingItem(ti);
    }

    if (ti.IsNull()) {
      return;
    }

    if (hEditor) {
      AddResourceEditor(resName, resId, hEditor);
    }

    ti.AddTail(resName.c_str(), 2).SetData(resId);
    ti.Expand();
  }

  void AddResourceEditor(std::string const& resName, int resId, HWND hEditor)
  {
    mTabView.AddPage(hEditor, resName.c_str());
  }

  void FillTree()
  {
    mExpView.FillTree();
  }

  void InitTree()
  {
    mExpView.InitTree();
  }

  LPTSTR GetConfigName() const
  {
    static TCHAR ModName[MAX_PATH];
    ModName[GetModuleFileName(NULL, ModName, MAX_PATH)] = TCHAR('\0');

    PathRemoveFileSpec(ModName);
    PathAppend(ModName, CONFIG_FILE_NAME);

    return ModName;
  }

  void LoadConfig()
  {
    LPTSTR ModName = GetConfigName();

    sw2::Ini ini;
    if (!ini.load(ModName)) {
      return;
    }

    //
    // Restore editor config.
    //

    good::rt::CPlayerWindow::getInst().showFPS = ini["Config"]["ShowFps"];

    //
    // Restore MRU.
    //

    sw2::Ini &sec = ini["MRU"];
    for (int i = sec.size() - 1; i >= 0; i--) {
      mMru.AddToList(sec.items[i].value.c_str());
    }
  }

  void PlayGame(std::string const& name, int StartLevel)
  {
    ShowWindow(SW_HIDE);
#ifdef GOOD_SUPPORT_SDL
    good::rt::SDLApplication::getInst().go(name);
#else
    good::rt::CPlayerWindow& wndMain = good::rt::CPlayerWindow::getInst();

    if (wndMain.CreateEx() == NULL) {
      SW2_TRACE_ERROR("Main gl window creation failed!\n");
    } else {

      wndMain.Reset(StartLevel);

      if (!wndMain.init(name)) {
        SW2_TRACE_ERROR("Init good failed!\n");
      }

      wndMain.ShowWindow(SW_SHOW);

      wndMain.theLoop.AddMessageFilter(&wndMain);
      timeBeginPeriod(1);
      wndMain.theLoop.Run();
      timeEndPeriod(1);
      wndMain.theLoop.RemoveMessageFilter(&wndMain);

      for (size_t i = 0; i < wndMain.logs.size(); i++) {
        SW2_TRACE_MESSAGE(wndMain.logs[i].c_str());
      }
    }

#endif
    ShowWindow(SW_SHOW);
  }

  void ResetStatusBar()
  {
    mStatus.SetPaneText(1, _T(""));
    mStatus.SetPaneText(2, _T(""));
  }

  void SaveConfig()
  {
    LPTSTR ModName = GetConfigName();

    sw2::Ini ini;

    //
    // Save window config.
    //

    WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);

    sw2::Ini &secWnd = ini["Window"];
    secWnd["length"] = wp.length;
    secWnd["flags"] = wp.flags;
    secWnd["showCmd"] = wp.showCmd;
    secWnd["ptMin.x"] = wp.ptMinPosition.x;
    secWnd["ptMin.y"] = wp.ptMinPosition.y;
    secWnd["ptMax.x"] = wp.ptMaxPosition.x;
    secWnd["ptMax.y"] = wp.ptMaxPosition.y;
    secWnd["rcPos.l"] = wp.rcNormalPosition.left;
    secWnd["rcPos.t"] = wp.rcNormalPosition.top;
    secWnd["rcPos.r"] = wp.rcNormalPosition.right;
    secWnd["rcPos.b"] = wp.rcNormalPosition.bottom;

    //
    // Save editor config.
    //

    sw2::Ini &secCfg = ini["Config"];
    secCfg["ShowFps"] = good::rt::CPlayerWindow::getInst().showFPS;
    secCfg["PrjView"] = SPLIT_PANE_RIGHT != mSplit.GetSinglePaneMode();
    secCfg["OutView"] = mOutput.mVisible;
    secCfg["SplitPos"] = mSplit.m_xySplitterPos;

    //
    // Save MRU.
    //

    char idx[16];
    for (int i = ID_FILE_MRU_FIRST; i <= ID_FILE_MRU_LAST; i++) {
      CString name;
      if (!mMru.GetFromList(i, name)) {
        break;
      }
      sprintf(idx, "%d", i - ID_FILE_MRU_FIRST);
      ini["MRU"][idx] = (char const*)name;
    }

    ini.store(ModName);
  }

  void SetActivePage(int id)
  {
    int idx = -1;
    for (int i = 0; i < mTabView.GetPageCount(); ++i) {
      if ((int)::SendMessage(mTabView.GetPageHWND(i), WM_GOOD_GETRESOURCEID, 0, 0) == id) {
        idx = i;
        break;
      }
    }

    if (-1 != idx && mTabView.GetActivePage() != idx) {
      mTabView.SetActivePage(idx);
    }
  }

  void SetCaption()
  {
    PrjT const& prj = PrjT::inst();

    CString cap;
    if (prj.mRes.mName.empty()) {
      cap.Format(_T("%s - %s"), CString((LPCTSTR)IDR_MAINFRAME), mFileName.c_str());
    } else {
      cap.Format(_T("%s - %s (%s)"), CString((LPCTSTR)IDR_MAINFRAME), mFileName.c_str(), prj.mRes.mName.c_str());
    }

    SetWindowText(cap);
    mTabView.SetTitleBarWindow(m_hWnd);
  }

  void SetPaneWidths(int* arrWidths, int nPanes)
  {
    //
    // Find the size of the borders.
    //

    int arrBorders[3];
    mStatus.GetBorders(arrBorders);

    //
    // Calculate right edge of default pane (0).
    //

    arrWidths[0] += arrBorders[2];
    for (int i = 1; i < nPanes; i++) {
      arrWidths[0] += arrWidths[i];
    }

    //
    // Calculate right edge of remaining panes (1 thru nPanes-1).
    //

    for (int j = 1; j < nPanes; j++) {
      arrWidths[j] += arrBorders[2] + arrWidths[j - 1];
    }

    //
    // Set the pane widths.
    //

    mStatus.SetParts(mStatus.m_nPanes, arrWidths);

    mStatus.SetPaneText(1, _T(""));
    mStatus.SetPaneText(2, _T(""));
  }

  void UpdatePropChanged(IResourceProperty* pres)
  {
    if (GOOD_RESOURCE_PROJECT == pres->GetType()) {
      SetCaption();
      return;
    }

    //
    // Try to rename resource tree item.
    //

    std::string name = pres->GetName();
    mExpView.ChangeItemName(pres->GetTreeId(), name);

    if (-1 != mTabView.GetActivePage()) {
      HWND hwnd = mTabView.GetPageHWND(mTabView.GetActivePage());
      int id = (int)::SendMessage(hwnd, WM_GOOD_GETRESOURCEID, 0, 0);
      if (pres->mId == id) {            // Editor id, try to redraw view.
        ::SendMessage(hwnd, WM_GOOD_UPDATE, 0, 0);
      }
      if (pres->GetTreeId() == id) {    // Changed item id, try to rename tab page item.
        mTabView.SetPageTitle(mTabView.GetActivePage(), name.c_str());
      } else {
        ::SendMessage(hwnd, WM_GOOD_UPDATE, pres->GetTreeId(), 0); // Sub item id, try to update it.
      }
    }
  }

  int GetPlayThisLevelId() const
  {
    HTREEITEM sel = mExpView.mTree.GetSelectedItem();
    if (NULL == sel) {
      return -1;
    }

    HTREEITEM group = mExpView.mTree.GetParentItem(sel);
    if (NULL == group) {
      return -1;
    }

    int idItem = (int)mExpView.mTree.GetItemData(sel);
    if (PrjT::inst().mRes.isLevel(idItem)) {
      return idItem;
    }

    return -1;
  }
};

// MainFrm.h
