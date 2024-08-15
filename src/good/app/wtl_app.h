
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

#define GOOD_SUPPORT_GDIPLUS_IMG

#include "../gx/opengl_gx.h"
#include "../gx/imgp_gx.h"
#include "../snd/audiere_snd.h"

namespace good {

namespace rt {

template<class AppT, class SndT = snd::AudiereSound>
class WtlApplicationImpl :
  public CFrameWindowImpl<AppT>,
  public CIdleHandler,
  public CMessageFilter,
  public COpenGL<AppT>,
  public Application<AppT, gx::GLImage, SndT>
{
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  CGLMessageLoop theLoop;
  sw2::FpsHelper fps;

  gx::GLGraphics gx;
  sw2::IntPoint lastMousePt;

  bool bAlwaysUpdate;

  WtlApplicationImpl() : bAlwaysUpdate(false)
  {
  }

  virtual BOOL OnIdle()
  {
    if (GetFocus() == m_hWnd || bAlwaysUpdate) {

      int ks = 0;
      if (GetFocus() == m_hWnd) {
        ks = GetKeyState();
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
      trigger(ks, lastMousePt);
      dc.wglMakeCurrent(NULL);

      if (mExit) {
        Exit();
        return TRUE;
      }

      if (mDirty || mTexDirty) {
        RedrawWindow();
      }
    }

    fps.wait();

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

  int GetKeyState() const
  {
    return GetWin32ToGoodKeyStates();
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
    fps.tick();
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
    fps.start(mRes.mFps);

    return true;
  }

  void doUninit()
  {
    gx::GLImage::clear();
    snd::AudiereSoundResource::inst().free();
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
