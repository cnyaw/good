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
# include "AtlOpengl.h"
# include "../good/app/wtl_player.h"

#include "../ed/resource1.h"
#include "../ed/DlgAbout.h"

CAppModule _Module;

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
    MSG_WM_SYSCOMMAND(OnSysCommand)
    CHAIN_MSG_MAP(BaseT)
  END_MSG_MAP()

  void OnInit(void)
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
  int nRet = wndMain.theLoop.Run();
  timeEndPeriod(1);

  _Module.RemoveMessageLoop();
  return nRet;
}

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
#endif
