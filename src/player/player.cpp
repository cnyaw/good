// player.cpp : main source file for player.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlcrack.h>
#include <atlmisc.h>

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
#include "../ed/DlgAbout.h"

CAppModule _Module;

#ifdef GOOD_SUPPORT_IMGP_GX
#define GOOD_SUPPORT_GDIPLUS_IMG
#include "../good/gx/imgp_gx.h"
#include "../good/snd/openal_snd.h"

class CPlayer :
  public CFrameWindowImpl<CPlayer>, public CDoubleBufferImpl<CPlayer>,
  public CMessageFilter,
  public good::rt::Application<CPlayer, good::gx::ImgpImage, good::snd::ALSound, good::gx::Imgp>
{
  CPlayer()
  {
  }
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  good::gx::ImgpGraphics gx;

  sw2::IntPoint lastMousePt;

  static CPlayer& getInst()
  {
    static CPlayer i;
    return i;
  }

  virtual BOOL PreTranslateMessage(MSG* pMsg)
  {
    return CFrameWindowImpl<CPlayer>::PreTranslateMessage(pMsg);
  }

  void Exit()
  {
    if (mExit) {
      DestroyWindow();
    }
  }

  int GetKeyState() const
  {
    return GetWin32ToGoodKeyStates();
  }

  //
  // Good APP.
  //

  bool doInit(std::string const& name)
  {
    gx.release();
    gx.create(mRes.mWidth, mRes.mHeight, 4);

    ResizeClient(mRes.mWidth, mRes.mHeight);

    return true;
  }

  void doUninit()
  {
    good::gx::ImgpImage::clear();
  }

  void onPackageChanged()
  {
    if (GOOD_LOGO_NAME != mRes.mName) {
      SetWindowText(mRes.mName.c_str());
    }
  }

  BEGIN_MSG_MAP_EX(CPlayer)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_KEYDOWN(OnKeyDown)
    MSG_WM_TIMER(OnTimer)
    CHAIN_MSG_MAP(CFrameWindowImpl<CPlayer>)
    CHAIN_MSG_MAP(CDoubleBufferImpl<CPlayer>)
  END_MSG_MAP()

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    SetMenu(NULL);

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);

    ResizeClient(320, 240);

    SetTimer(1, 1000/60);

    onAppCreate();
    SetMsgHandled(false);
    return 0;
  }

  void OnDestroy()
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);

    uninit();

    onAppDestroy();
    SetMsgHandled(false);
  }

  void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    if (VK_ESCAPE == nChar && 0 == (mHandledKeys & GOOD_KEYS_ESCAPE)) {
      Exit();
    }
  }

  void DoPaint(CDCHandle memdc)
  {
    renderAll();
    gx.blt(memdc, 0, 0);
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

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
  CMessageLoop theLoop;
  return good::rt::WinMainPlay<CPlayer>(hInstance, lpstrCmdLine, nCmdShow, lpstrCmdLine, &theLoop);
}
#else
#include "../good/app/wtl_app.h"

class CPlayer : public good::rt::WtlApplicationImpl<CPlayer>
{
public:
  typedef good::rt::WtlApplicationImpl<CPlayer> BaseT;

  static CPlayer& getInst()
  {
    static CPlayer inst;
    return inst;
  } // getInst

  BEGIN_MSG_MAP_EX(CPlayer)
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

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
  return good::rt::WinMainPlay<CPlayer>(hInstance, lpstrCmdLine, nCmdShow, lpstrCmdLine, &CPlayer::getInst().theLoop);
}
#endif // GOOD_SUPPORT_IMGP_GX
#endif // GOOD_SUPPORT_SDL
