
//
// EditStgeScript.h
// STGE script editor.
//
// Copyright (c) 2011 Waync Cheng.
// All Rights Reserved.
//
// 2011/09/27 Waync created.
//

#pragma once

sw2::IntRect mBounding;
sw2::IntRect mOutterBounding;

template<class ObjMgrT>
class MyObj : public stge::Object
{
public:

  bool init(ObjMgrT& fac, int idAction, int idNewObj)
  {
    //
    // Return false if init failed, and clear this object.
    //

    return true;
  }

  template<typename PlayerT>
  bool update(float fElapsed, ObjMgrT& om, PlayerT& player)
  {
    //
    // Return false, and clear this object.
    //

    stge::Object::update(fElapsed, om, player);
    return mOutterBounding.ptInRect(sw2::IntPoint((int)x, (int)y));
  }
};

class MyObjectManager : public stge::ObjectManager<MyObjectManager, MyObj<MyObjectManager> >
{
public:
  static int getWindowWidth()
  {
    return PrjT::inst().mRes.mWidth;
  }

  static int getWindowHeight()
  {
    return PrjT::inst().mRes.mHeight;
  }
};

template<class MainT>
class CStgeView : public CWindowImpl<CStgeView<MainT> >, public CDoubleBufferImpl<CStgeView<MainT> >
{
public:
  DECLARE_WND_CLASS_EX(NULL, 0, COLOR_BTNTEXT)

  enum { FPS = 60 };

  stge::ScriptManager mScm;
  MyObjectManager mOm;
  stge::Object mPlayer;
  sw2::FpsHelper mFps;
  good::gx::Imgp mScr;

  CStgeView()
  {
    mOm.init(&mScm);
    mFps.start(FPS);

    int w = PrjT::inst().mRes.mWidth/2, h = PrjT::inst().mRes.mHeight/2;
    mBounding = sw2::IntRect(-w, -h, w, h);

    mOutterBounding = mBounding;
    mOutterBounding.inflate(mBounding.width()/10, mBounding.height()/10);
  }

  void updateAndDraw(CDCHandle dc)
  {
    RECT rc;
    GetClientRect(&rc);

    POINT p;
    GetCursorPos(&p);
    ScreenToClient(&p);

    // Update STGE.
    mPlayer.x = (float)(p.x - rc.right/2);
    mPlayer.y = (float)(p.y - rc.bottom/2);
    mOm.update(1/(float)FPS, mPlayer);

    // Draw.
    char buff[128];
    sprintf(buff, "%d a%u o%u", mFps.getFps(), mOm.actions.size(), mOm.objects.size());
    mScr.fill(0).drawText(buff, -1, 0, 0, 0xffffffff, 2);

    mScr.rect(0xffffffff, mBounding.left + rc.right/2, mBounding.top + rc.bottom/2, mBounding.width(), mBounding.height());
    mScr.rect(0xff808080, mOutterBounding.left + rc.right/2, mOutterBounding.top + rc.bottom/2, mOutterBounding.width(), mOutterBounding.height());

    for (int i = mOm.objects.first(); -1 != i; i = mOm.objects.next(i)) {
      stge::Object const& o = mOm.objects[i];
      sw2::POINT_t<int> pt((int)o.x, (int)o.y);
      if (mOutterBounding.ptInRect(pt)) {
        mScr.rect(mBounding.ptInRect(pt) ? 0xffffffff : 0xff808080, rc.right / 2 + pt.x - 3, rc.bottom / 2 + pt.y - 3, 6, 6);
      }
    }

    // Draw to screen.
    mScr.blt(dc, 0, 0);
  }

  BEGIN_MSG_MAP(CStgeView)
    MSG_WM_SIZE(OnSize)
    CHAIN_MSG_MAP(CDoubleBufferImpl<CStgeView>)
  END_MSG_MAP()

  void OnSize(UINT nType, CSize size)
  {
    mScr.release();
    mScr.create(size.cx, size.cy, 4);
  }

  void DoPaint(CDCHandle dc)
  {
    mFps.tick();
    updateAndDraw(dc);
    mFps.wait();
    Invalidate(FALSE);
  }
};

template<class MainT>
class CStgeScriptEditor : public CWindowImpl<CStgeScriptEditor<MainT> >
{
public:

  CGoodPane mPane;
  CToolBarCtrl mToolbar;

  CSplitterWindow mSplit;
  CListBox mList;
  CStgeView<MainT> mStge;

  int mId;
  int mPos;

  CStgeScriptEditor(int id) : mId(id), mPos(220)
  {
  }

  virtual void OnFinalMessage(HWND)     // Delete self when window destroy.
  {
    delete this;
  }

  void Compile()
  {
    PrjT const& prj = PrjT::inst();
    std::string path = good::getPathName(prj.mRes.mFileName);
    path += prj.getStgeScript(mId);

    stge::ScriptManager scm;
    if (!stge::Parser::parse(path, scm)) {
      SW2_TRACE_ERROR("%s", stge::Parser::getLastError().c_str());
      CString s = stge::Parser::getLastError().c_str();
      MessageBox(s, _T("Error"), MB_OK|MB_ICONERROR);
      return;
    }

    mList.ResetContent();
    for (stge::ScriptManager::const_iterator it = scm.begin(); scm.end() != it; ++it) {
      mList.AddString((LPCTSTR)it->first.c_str());
    }

    mStge.mOm.reset();
    mStge.mScm = scm;
  }

  BEGIN_MSG_MAP_EX(CStgeScriptEditor)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_SIZE(OnSize)
    COMMAND_ID_HANDLER_EX(ID_STGESCRIPT_SOURCE, OnSource)
    COMMAND_ID_HANDLER_EX(ID_STGESCRIPT_COMPILE, OnCompile)
    COMMAND_CODE_HANDLER_EX(LBN_SELCHANGE, OnSelChange)
    FORWARD_TOOLTIP_GETDISPINFO()
  END_MSG_MAP()

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    //
    // Create UI.
    //

    mPane.Create(m_hWnd);
    mPane.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);

    mToolbar = GoodCreateSimpleToolbar(mPane, IDR_TB_STGESCRIPT);

    mSplit.Create(mPane, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    mList.Create(mSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY, WS_EX_CLIENTEDGE);

    mPane.SetClient(mSplit);
    mStge.Create(mSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_STATICEDGE);
    mSplit.SetSplitterPanes(mList, mStge);

    //
    // Load script.
    //

    Compile();

    SetMsgHandled(FALSE);
    return 0;
  }

  void OnSize(UINT nType, CSize size)
  {
    mPane.SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOACTIVATE | SWP_NOZORDER);
    mSplit.SetSplitterPos(mPos);
  }

  void OnCompile(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    Compile();
  }

  void OnSource(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    const PrjT &prj = PrjT::inst();
    std::string path = good::getPathName(prj.mRes.mFileName);
    path += prj.getStgeScript(mId);
    ShellOpen(path.c_str());
  }

  void OnSelChange(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CString s;
    mList.GetText(mList.GetCurSel(), s);
    mStge.mOm.reset();
    mStge.mOm.run(std::string(s), 0, 0);
  }
};

// end of EditStgeScript.h
