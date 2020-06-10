
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
class CStgeView : public CWindowImpl<CStgeView<MainT> >, public COpenGL<CStgeView<MainT> >
{
public:
  DECLARE_WND_CLASS_EX(NULL, 0, COLOR_BTNTEXT)

  enum { FPS = 60 };

  stge::ScriptManager mScm;
  MyObjectManager mOm;
  stge::Object mPlayer;

  float mDepth;

  CStgeView()
  {
    mOm.init(&mScm);

    int w = PrjT::inst().mRes.mWidth/2, h = PrjT::inst().mRes.mHeight/2;
    mBounding = sw2::IntRect(-w, -h, w, h);

    mOutterBounding = mBounding;
    mOutterBounding.inflate(mBounding.width()/10, mBounding.height()/10);
  }

  DWORD TimeLeft()
  {
    int const TICK = (int)(1000/(float)FPS);
    static DWORD nextTime = ::GetTickCount() + TICK;

    DWORD now = ::GetTickCount();

    DWORD left = 0;
    if (nextTime > now) {
      left = nextTime - now;
    }

    nextTime += (int)(1000/(float)FPS);

    if ((int)(nextTime - now) > 3 * TICK) {
      nextTime = now + TICK;
      left = 0;
    }

    return left;
  }

  bool IsMainWndActive() const
  {
    TCHAR ClassName[256];
    UINT cch;
    HWND hWnd = GetActiveWindow();
    while (hWnd) {
      cch = 256;
      ClassName[RealGetWindowClass(hWnd, ClassName, cch)] = TCHAR('\0');
      if (!lstrcmp(ClassName, _T("#32770"))) { // A dialog?
        return false;
      }
      if (MainT::inst() == hWnd) {
        return true;
      }
      hWnd = ::GetParent(hWnd);
    }

    return false;
  }

  BEGIN_MSG_MAP(CStgeView)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
    CHAIN_MSG_MAP(COpenGL<CStgeView>)
  END_MSG_MAP()

  BOOL OnEraseBkgnd(CDCHandle dc)
  {
    return TRUE;
  }

  //
  // COpenGL<>
  //

  void OnInit(void)
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    //
    // Load font.
    //

    std::string datFont = std::string((char*)FONT2_MOD, sizeof(FONT2_MOD));

    good::gx::GxImage img;
    if (img.loadFromStream(datFont)) {
      img.expand(256, 256);
      img.flip();
      GLuint texture = 0;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.w, img.h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.dat);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }
  }

  void OnRender(void)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // Bounding.
    //

    glLoadIdentity();
    glTranslatef(0, 0, -mDepth);
    glScalef(mBounding.width()/2.0f, mBounding.height()/2.0f, 1);
    glColor3ub(255, 255, 255);
    DrawRect();

    glLoadIdentity();
    glTranslatef(0, 0, -mDepth);
    glScalef(mOutterBounding.width()/2.0f, mOutterBounding.height()/2.0f, 1);
    glColor3ub(128, 128, 128);
    DrawRect();

    //
    // Draw bullet.
    //

    for (int i = mOm.objects.first(); -1 != i; i = mOm.objects.next(i)) {
      stge::Object const& o = mOm.objects[i];
      sw2::POINT_t<int> pt((int)o.x, (int)o.y);

      if (!mOutterBounding.ptInRect(pt)) {
        continue;
      }

      glLoadIdentity();
      glTranslatef(o.x, -o.y, -mDepth);
      glScalef(3, 3, 3);

      if (mBounding.ptInRect(pt)) {
        glColor3ub(255, 255, 255);
      } else {
        glColor3ub(128, 128, 128);
      }

      DrawRect();
    }

    //
    // Draw stats.
    //

    RECT rc;
    GetClientRect(&rc);

    std::stringstream ss;
    ss << "a" << mOm.actions.size() << " o" << mOm.objects.size();
    DrawString(-rc.right/2.0f, rc.bottom/2.0f, ss.str());

    //
    // Trigger next draw.
    //

    if (IsWindowVisible() && IsMainWndActive()) {
      ::Sleep(TimeLeft());

      //
      // Update.
      //

      RECT rc;
      GetClientRect(&rc);

      POINT p;
      GetCursorPos(&p);
      ScreenToClient(&p);

      mPlayer.x = (float)(p.x - rc.right/2);
      mPlayer.y = (float)(p.y - rc.bottom/2);

      mOm.update(1/(float)FPS, mPlayer);

      //
      // Trigger redraw.
      //

      Invalidate();
    }
  }

  void OnResize(int width, int height)
  {
    if (height == 0) {
      height = 1;
    }

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //
    // Calculate The Aspect Ratio Of The Window.
    //

    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);

    float deg2pi = 3.1415926f / 180.0f;
    mDepth = (height / 2.0f) / tanf((45.0f/2.0f) * deg2pi);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  //
  // Helper.
  //

  void DrawChar(int c)
  {
    int idx = c - ' ';

    int cw = 8, ch = 16;
    int cch = 15;

    int x = cw * (idx % cch);
    int y = ch * (idx / cch);

    float x0 = x / 256.0f, y0 = (256 - y) / 256.0f;
    float x1 = (x + cw) / 256.0f, y1 = (256 - y - ch) / 256.0f;

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
      glTexCoord2f(x0,y1);
      glVertex3f(-1,-1,0);
      glTexCoord2f(x1,y1);
      glVertex3f(1,-1,0);
      glTexCoord2f(x1,y0);
      glVertex3f(1,1,0);
      glTexCoord2f(x0,y0);
      glVertex3f(-1,1,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
  }

  void DrawRect()
  {
    glBegin(GL_LINE_LOOP);
      glVertex2f(-1, 1);
      glVertex2f(1, 1);
      glVertex2f(1, -1);
      glVertex2f(-1, -1);
    glEnd();
  }

  void DrawString(float x, float y, std::string const& str)
  {
    x += 8, y -= 8;
    for (size_t i = 0; i < str.size(); ++i) {
      glLoadIdentity();
      glTranslatef(x + 10 * i, y, -mDepth);
      glScalef(5, 6, 1);
      glColor3ub(255, 255, 255);
      DrawChar(str[i]);
    }
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

    std::ifstream ifs(path.c_str());
    if (!ifs) {
      MessageBox(_T("Load file failed\n"), _T("Error"), MB_OK | MB_ICONERROR);
      return;
    }

    stge::ScriptManager scm;
    if (!stge::Parser::parse(ifs, scm)) {
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
    MSG_WM_GOOD(OnGoodMessage)
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

  int OnGoodMessage(int uMsg, WPARAM wParam, LPARAM lParam)
  {
    switch (uMsg)
    {
    case WM_GOOD_GETRESOURCEID:
      return mId;
    }

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
