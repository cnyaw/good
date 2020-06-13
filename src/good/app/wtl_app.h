
//
// wtl_app.h
// OpenGL WTL application player window.
//
// Copyright (c) 2009 Waync Cheng.
// All Rights Reserved.
//
// 2009/07/15 Waync created.
//

#pragma once

#include "AtlOpengl.h"

#define GOOD_SUPPORT_GDIPLUS
#define GOOD_WTL_PLAYER

#include "../gx/opengl_gx.h"
#include "../gx/imgp_gx.h"
#include "../snd/audiere_snd.h"

namespace good {

namespace rt {

template<class AppT, class GxT = gx::GLGraphics, class ImgT = gx::GLImage, class SndT = snd::AudiereSound>
class WtlApplicationImpl :
  public CFrameWindowImpl<AppT>,
  public CIdleHandler,
  public CMessageFilter,
  public COpenGL<AppT>,
  public Application<AppT, ImgT, SndT, gx::Imgp>
{
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  CGLMessageLoop theLoop;

  GxT gx;
  sw2::IntPoint lastMousePt;

  DWORD nextTime;
  bool bAlwaysUpdate;

  WtlApplicationImpl() : bAlwaysUpdate(false)
  {
  }

  virtual BOOL OnIdle()
  {
    if (GetFocus() == m_hWnd || bAlwaysUpdate) {

      if (GetFocus() == m_hWnd) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(&pt);
        sw2::IntPoint mp(pt.x, pt.y);
        RECT RcWnd;
        GetClientRect(&RcWnd);
        mp.x = (int)(mp.x * mRes.mWidth / (float)RcWnd.right);
        mp.y = (int)(mp.y * mRes.mHeight / (float)RcWnd.bottom);
        lastMousePt = mp;
      }

      CClientDC dc(m_hWnd);
      dc.wglMakeCurrent(m_hRC);
      trigger(GetKeyState(), lastMousePt);
      dc.wglMakeCurrent(NULL);

      if (mExit) {
        Exit();
        return TRUE;
      }

      if (mDirty || mTexDirty) {
        RedrawWindow();
      }
    }

    ::Sleep(TimeLeft());

    return FALSE;
  }

  virtual BOOL PreTranslateMessage(MSG* pMsg)
  {
    return CFrameWindowImpl<AppT>::PreTranslateMessage(pMsg);
  }

  void Exit()
  {
    if (mExit) {
      DestroyWindow();
    }
  }

  DWORD TimeLeft()
  {
    int const TICK = (int)(1000/(float)mRes.mFps);

    DWORD now = ::GetTickCount();

    DWORD left = 0;
    if(nextTime > now) {
      left = nextTime - now;
    }

    nextTime += TICK;

    if ((int)(nextTime - now) > 3 * TICK) {
      nextTime = now + TICK;
      left = 0;
    }

    return left;
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
  // Implement COpenGL<>.
  //

  void OnInit(void)
  {
    gx.init();
  }

  void OnRender(void)
  {
    renderAll();
  }

  void OnResize(int width, int height)
  {
    gx.resize(width, height);
  }

  //
  // Good APP.
  //

  bool doInit(std::string const& name)
  {
    gx.SCREEN_W = mRes.mWidth;
    gx.SCREEN_H = mRes.mHeight;

    ResizeClient(mRes.mWidth, mRes.mHeight);

    return true;
  }

  void doUninit()
  {
    gx::GLImageResource::inst().clear();
    snd::AudiereSoundResource::inst().free();
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

  BEGIN_MSG_MAP_EX(WTLApplication)
    MSG_WM_ACTIVATE(OnActivate)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_KEYDOWN(OnKeyDown)
    CHAIN_MSG_MAP(CFrameWindowImpl<AppT>)
    CHAIN_MSG_MAP(COpenGL<AppT>)
  END_MSG_MAP()

  void OnActivate(UINT nState, BOOL bMinimized, CWindow wndOther)
  {
    static float volMus = SndT::getMusicVolume();
    static float volSnd = SndT::getSoundVolume();

    if (WA_INACTIVE == nState) {
      volMus = SndT::getMusicVolume();
      volSnd = SndT::getSoundVolume();
      SndT::setMusicVolume(.0f);
      SndT::setSoundVolume(.0f);
      AppT::getInst().onAppPause();
    } else {
      SndT::setMusicVolume(volMus);
      SndT::setSoundVolume(volSnd);
    }

    int const TICK = (int)(1000/(float)mRes.mFps);
    nextTime = ::GetTickCount() + TICK;
  }

  int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    SetMenu(NULL);

    //
    // Register object for message filtering and idle updates.
    //

    theLoop.AddIdleHandler(this);

    ResizeClient(320, 240);

    AppT::getInst().onAppCreate();
    SetMsgHandled(false);
    return 0;
  }

  void OnDestroy()
  {
    theLoop.RemoveIdleHandler(this);

    uninit();

    AppT::getInst().onAppDestroy();
    SetMsgHandled(false);
  }

  void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    if (VK_ESCAPE == nChar && 0 == (mHandledKeys & GOOD_KEYS_ESCAPE)) {
      Exit();
    }
  }
};

} // namespace rt

} // namespace good

// end of wtl_app.h
