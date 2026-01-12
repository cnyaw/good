
#include "stdafx.h"

#include "swinc.h"

#include <cstdarg>

#include "good.h"

#include "MainFrm.h"

HWND mHwndOutputView;
HWND mHwndOutputEdit;

void NotifyImageManagerSurfaceReset()
{
  good::rt::CPlayerWindow::getInst().handleImageManagerSurfaceReset();
}

void GoodTraceTool(int level, const char* format, va_list args)
{
  char buf[1024];
  vsnprintf(buf, sizeof(buf), format, args);
  good::rt::CPlayerWindow& player = good::rt::CPlayerWindow::getInst();
  if (-1 == player.theLoop.m_aMsgFilter.Find(&player)) {
    if (!::IsWindow(mHwndOutputEdit)) {
      return;
    }
    strcat(buf, "\r\n");
    CEdit(mHwndOutputEdit).AppendText(CString(buf));
    if (::IsWindow(mHwndOutputView)) {    // Hack, force show output view.
      ::SendMessage(mHwndOutputView, WM_USER + 99887, 0, 0);
    }
  } else {
    player.trace(buf);
  }
} // GoodTraceTool

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
  CMessageLoop theLoop;
  _Module.AddMessageLoop(&theLoop);

  CMainFrame& wndMain = CMainFrame::inst();

  if(wndMain.CreateEx() == NULL) {
    ATLTRACE(_T("Main window creation failed!\n"));
    return 0;
  }

  sw2::Ini ini;
  if (ini.load(wndMain.GetConfigName())) {

    //
    // Restore window settings.
    //

    sw2::Ini &secWnd = ini["Window"];

    WINDOWPLACEMENT wp;
    wp.length = secWnd["length"];

    if (sizeof(WINDOWPLACEMENT) == wp.length) {

      wp.flags = secWnd["flags"];
      wp.showCmd = secWnd["showCmd"];
      wp.ptMinPosition.x = secWnd["ptMin.x"];
      wp.ptMinPosition.y = secWnd["ptMin.y"];
      wp.ptMaxPosition.x = secWnd["ptMax.x"];
      wp.ptMaxPosition.y = secWnd["ptMax.y"];
      wp.rcNormalPosition.left = secWnd["rcPos.l"];
      wp.rcNormalPosition.top = secWnd["rcPos.t"];
      wp.rcNormalPosition.right = secWnd["rcPos.r"];
      wp.rcNormalPosition.bottom = secWnd["rcPos.b"];

      if (SW_SHOWMINIMIZED == wp.showCmd) {
        wp.showCmd = SW_SHOWNORMAL;
      }

      wndMain.SetWindowPlacement(&wp);
    }

    //
    // Restore editor config.
    //

    sw2::Ini &secCfg = ini["Config"];

    if (secCfg.find("PrjView")) {
      bool bShowPrjView = secCfg["PrjView"];
      wndMain.mSplit.SetSinglePaneMode(bShowPrjView ? SPLIT_PANE_NONE : SPLIT_PANE_RIGHT);
      wndMain.UISetCheck(ID_VIEW_PROJVIEW, bShowPrjView);
    }

    if (secCfg.find("OutView")) {
      bool bShowOutputView = secCfg["OutView"];
      if (bShowOutputView) {
        wndMain.OnViewOutputWindow(0, 0, NULL);
      }
    }

    int SplitPos = secCfg["SplitPos"];
    if (0 < SplitPos) {
      wndMain.mSplit.SetSplitterPos(SplitPos);
    }

  } else {
    wndMain.ShowWindow(nCmdShow);
  }

  int nRet = theLoop.Run();

  _Module.RemoveMessageLoop();
  return nRet;
} // Run

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
  sw2::Util::setUnhandledExceptionFilter();

  SW2_TRACE_FUNC(GoodTraceTool);        // Override do trace func.

  HRESULT hRes = ::CoInitialize(NULL);

  // If you are running on NT 4.0 or higher you can use the following call instead to
  // make the EXE free threaded. This means that calls come in on a random RPC thread.
  //    HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

  ATLASSERT(SUCCEEDED(hRes));

  //
  // This resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used.
  //

  ::DefWindowProc(NULL, 0, 0, 0L);

  AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES); // Add flags to support other controls.

  hRes = _Module.Init(NULL, hInstance);
  ATLASSERT(SUCCEEDED(hRes));

  int nRet = Run(lpstrCmdLine, nCmdShow);

  _Module.Term();
  ::CoUninitialize();

  return nRet;
} // _tWinMain
