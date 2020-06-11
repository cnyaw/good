// player.cpp : main source file for player.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atldlgs.h>
#include <atlcrack.h>
#include <atlmisc.h>

#include <cmath>

#include <algorithm>
#include <map>
#include <sstream>
#include <vector>

#ifndef _DEBUG
# define GOOD_SUPPORT_STGE
#endif

#define STGE_NO_ERROR_MSG

#include "../good/rt/rt.h"

#include "resource.h"

#ifdef GOOD_SUPPORT_SDL
# include "../good/app/sdl_app.h"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
  good::rt::SDLApplication::getInst().go(lpstrCmdLine);
  return 0;
}
#else

#include "../ed/resource1.h"

void ShellOpen(const char *path)
{
  SHELLEXECUTEINFO si = {sizeof(SHELLEXECUTEINFO)};
  si.lpVerb = TEXT("open");
  si.lpFile = path;
  si.nShow = SW_SHOWNORMAL;
  ShellExecuteEx(&si);
}

#include "../ed/DlgAbout.h"

CAppModule _Module;

#ifdef GOOD_SUPPORT_IMGP_GX
#define GOOD_SUPPORT_GDIPLUS
#define GOOD_WTL_PLAYER
#include "../good/gx/imgp_gx.h"
#include "../good/snd/openal_snd.h"

class CPlayerWindow :
  public CFrameWindowImpl<CPlayerWindow>,
  public CMessageFilter,
  public good::rt::Application<CPlayerWindow, good::gx::ImgpImage, good::snd::ALSound, good::gx::Imgp>
{
  CPlayerWindow() : gx(scr)
  {
  }
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  good::gx::Imgp scr;
  good::gx::ImgpGraphics gx;

  sw2::IntPoint lastMousePt;

  static CPlayerWindow& getInst()
  {
    static CPlayerWindow i;
    return i;
  }

  virtual BOOL PreTranslateMessage(MSG* pMsg)
  {
    return CFrameWindowImpl<CPlayerWindow>::PreTranslateMessage(pMsg);
  }

  void Exit()
  {
    if (mExit) {
      DestroyWindow();
    }
  }

  int GetKeyState() const
  {
    int ks = 0;

    if (::GetKeyState(VK_UP) & 0x8000) {
      ks |= GOOD_KEYS_UP;
    }

    if (::GetKeyState(VK_DOWN) & 0x8000) {
      ks |= GOOD_KEYS_DOWN;
    }

    if (::GetKeyState(VK_LEFT) & 0x8000) {
      ks |= GOOD_KEYS_LEFT;
    }

    if (::GetKeyState(VK_RIGHT) & 0x8000) {
      ks |= GOOD_KEYS_RIGHT;
    }

    if (::GetKeyState(VK_RETURN) & 0x8000) {
      ks |= GOOD_KEYS_RETURN;
    }

    if (::GetKeyState(VK_ESCAPE) & 0x8000) {
      ks |= GOOD_KEYS_ESCAPE;
    }

    if (::GetKeyState('Z') & 0x8000) {
      ks |= GOOD_KEYS_BTN_A;
    }

    if (::GetKeyState('X') & 0x8000) {
      ks |= GOOD_KEYS_BTN_B;
    }

    if (::GetKeyState(VK_LBUTTON) & 0x80000) {
      ks |= GOOD_KEYS_LBUTTON;
    }

    if (::GetKeyState(VK_RBUTTON) & 0x80000) {
      ks |= GOOD_KEYS_RBUTTON;
    }

    return ks;
  }

  //
  // Good APP.
  //

  bool doInit(std::string const& name)
  {
    scr.release();
    scr.create(mRes.mWidth, mRes.mHeight, 4);

    ResizeClient(mRes.mWidth, mRes.mHeight);

    return true;
  }

  void doUninit()
  {
    good::gx::ImgpImageResource::inst().clear();
  }

  bool doOpenUrl(std::string const& url) const
  {
    ShellOpen(url.c_str());
    return true;
  }

  void onPackageChanged()
  {
    if (GOOD_LOGO_NAME != mRes.mName) {
      SetWindowText(mRes.mName.c_str());
    }
  }

  BEGIN_MSG_MAP_EX(CPlayerWindow)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_ERASEBKGND(OnEraseBkgnd)
    MSG_WM_KEYDOWN(OnKeyDown)
    MSG_WM_PAINT(OnPaint)
    MSG_WM_TIMER(OnTimer)
    CHAIN_MSG_MAP(CFrameWindowImpl<CPlayerWindow>)
  END_MSG_MAP()

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    SetMenu(NULL);

    //
    // Register object for message filtering and idle updates.
    //

    ResizeClient(320, 240);

    SetTimer(1, 1000/60);

    onAppCreate();
    SetMsgHandled(false);
    return 0;
  }

  void OnDestroy()
  {
    uninit();

    onAppDestroy();
    SetMsgHandled(false);
  }

  BOOL OnEraseBkgnd(CDCHandle dc)
  {
    return TRUE;
  }

  void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    if (VK_ESCAPE == nChar && 0 == (mHandledKeys & GOOD_KEYS_ESCAPE)) {
      Exit();
    }
  }

  void OnPaint(CDCHandle)
  {
    renderAll();
    CPaintDC dc(m_hWnd);
    scr.blt(dc, 0, 0);
  }

  void OnTimer(UINT_PTR nIDEvent)
  {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);
    sw2::IntPoint mp(pt.x, pt.y);
    RECT RcWnd;
    GetClientRect(&RcWnd);
    mp.x = (int)(mp.x * mRes.mWidth / (float)RcWnd.right);
    mp.y = (int)(mp.y * mRes.mHeight / (float)RcWnd.bottom);
    lastMousePt = mp;

    trigger(GetKeyState(), lastMousePt);
    RedrawWindow();
  }
};

int Run(LPTSTR lpstrCmdLine, int nCmdShow = SW_SHOWDEFAULT)
{
  //
  // Start the app.
  //

  CMessageLoop theLoop;
  _Module.AddMessageLoop(&theLoop);

  CPlayerWindow& wndMain = CPlayerWindow::getInst();

  if (wndMain.CreateEx() == NULL) {
    ATLTRACE(_T("Main gl window creation failed!\n"));
    return 0;
  }

  if (!wndMain.init(lpstrCmdLine)) {
    ATLTRACE(_T("Init good failed!\n"));
    return 0;
  }

  wndMain.ShowWindow(SW_SHOW);

  timeBeginPeriod(1);
  int nRet = theLoop.Run();
  timeEndPeriod(1);

  _Module.RemoveMessageLoop();
  return nRet;
}
#else
#include "../good/app/wtl_app.h"

class CPlayerWindow : public good::rt::WtlApplicationImpl<CPlayerWindow>
{
public:
  typedef good::rt::WtlApplicationImpl<CPlayerWindow> BaseT;

  static CPlayerWindow& getInst()
  {
    static CPlayerWindow inst;
    return inst;
  } // getInst

  BEGIN_MSG_MAP_EX(CPlayerWindow)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_SYSCOMMAND(OnSysCommand)
    CHAIN_MSG_MAP(BaseT)
  END_MSG_MAP()

  void OnInit()
  {
    BaseT::OnInit();

    HMENU sys = ::GetSystemMenu(m_hWnd, FALSE);
    ::AppendMenu(sys, MF_SEPARATOR, 0, 0);
    ::AppendMenu(sys, 0, ID_APP_ABOUT, "&About");
  }

  void OnSysCommand(UINT nID, CPoint)
  {
    if (ID_APP_ABOUT == nID) {
      CDlgAbout().DoModal();
      return;
    }

    SetMsgHandled(FALSE);
  }
};

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
  //
  // Load good package resouce stream.
  //

  HRSRC hRes = FindResourceEx(
                 _Module.GetResourceInstance(),
                 _T("GDPRES"),
                 _T("101"),
                 MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
  if (NULL == hRes) {
    CDlgAbout().DoModal();
    return 0;
  }

  int len = SizeofResource(_Module.GetResourceInstance(), hRes);
  HGLOBAL lpres = LoadResource(_Module.GetResourceInstance(), hRes);

  if (0 >= len || 0 == lpres) {
    CDlgAbout().DoModal();
    return 0;
  }

  //
  // Start the app.
  //

  CGLMessageLoop theLoop;
  _Module.AddMessageLoop(&theLoop);

  CPlayerWindow& wndMain = CPlayerWindow::getInst();

  std::stringstream ss;
  ss.write((char const*)LockResource(lpres), len);

  if (wndMain.CreateEx() == NULL) {
    ATLTRACE(_T("Main gl window creation failed!\n"));
    return 0;
  }

  if (!wndMain.init(ss)) {
    ATLTRACE(_T("Init good failed!\n"));
    return 0;
  }

  wndMain.ShowWindow(SW_SHOW);

  timeBeginPeriod(1);
  int nRet = theLoop.Run();
  timeEndPeriod(1);

  _Module.RemoveMessageLoop();
  return nRet;
}
#endif // GOOD_SUPPORT_IMGP_GX

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//    HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_BAR_CLASSES);    // add flags to support other controls

    hRes = _Module.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    int nRet = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();
    ::CoUninitialize();

    return nRet;
}
#endif // GOOD_SUPPORT_SDL
