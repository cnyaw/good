
#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atldlgs.h>
#include <atlcrack.h>
#include <atlmisc.h>

#include <algorithm>
#include <map>
#include <sstream>
#include <vector>

#ifndef _DEBUG
# define GOOD_SUPPORT_STGE
#endif

#define STGE_NO_ERROR_MSG

#include "../good/rt/rt.h"

#include "../ed/resource.h"
#include "../ed/resource1.h"

# include "../good/app/wtl_player.h"

#include "../ed/DlgAbout.h"

CAppModule _Module;

class CPlayerWindow : public good::rt::CPlayerWindowImpl<CPlayerWindow>
{
  CPlayerWindow()
  {
    tip = "Press O to toggle trace messages";
    showFPS = true;
  }

public:

  enum {
    DISP_MAX_TEX_SIZE = 64,             // Max display size in pixel of texture.
    DISP_TEX_BORDER_SIZE = 2,
  };

  typedef good::rt::CPlayerWindowImpl<CPlayerWindow> BaseT;
  int m_ListY;

  static CPlayerWindow& getInst()
  {
    static CPlayerWindow inst;
    return inst;
  } // getInst

  template<class ResT, class IdxT>
  void ListTexItems(ResT const& res, IdxT const& idx, const std::string &ResName)
  {
    const size_t cxMaxTile = (std::max)(1, mRes.mWidth / (DISP_MAX_TEX_SIZE + DISP_TEX_BORDER_SIZE));
    SimpleDrawText(0, m_ListY, ResName);
    m_ListY += 16;
    for (size_t i = 0, j = 0; i < idx.size(); ++i) {
      typename ResT::const_iterator it = res.find(idx[i]);
      typename const TextureT &tex = it->second;
      good::gx::GLImage img = getImage(tex.mFileName);
      int imgw = img.getWidth(), imgh = img.getHeight();
      float scale = 1.0f;
      if (DISP_MAX_TEX_SIZE < imgw || DISP_MAX_TEX_SIZE < imgh) {
        float dw = DISP_MAX_TEX_SIZE / (float)imgw;
        float dh = DISP_MAX_TEX_SIZE / (float)imgh;
        scale = (std::min)(dw, dh) ;
      }
      int x = (int)j * (DISP_MAX_TEX_SIZE + DISP_TEX_BORDER_SIZE);
      drawImage(-1, x, m_ListY, idx[i], 0, 0, imgw, imgh, 0xffffffff, .0f, scale, scale);
      j += 1;
      if (cxMaxTile == j) {
        j = 0;
        m_ListY += (DISP_MAX_TEX_SIZE + DISP_TEX_BORDER_SIZE);
      }
    }
    m_ListY += (DISP_MAX_TEX_SIZE + DISP_TEX_BORDER_SIZE);
  }

  template<class ResT, class IdxT>
  void ListResItems(ResT const& res, IdxT const& idx, const std::string &ResName)
  {
    SimpleDrawText(0, m_ListY, ResName);
    m_ListY += 16;
    for (size_t i = 0; i < idx.size(); ++i) {
      typename ResT::const_iterator it = res.find(idx[i]);
      SimpleDrawText(0, m_ListY, it->second.getName().c_str());
      m_ListY += 16;
    }
  }

  void ListRes()
  {
    m_ListY = 16;                       // Skip proj name title.
    ListTexItems(mRes.mTex, mRes.mTexIdx, "-Texture-");
    ListResItems(mRes.mLevel, mRes.mLevelIdx, "-Level-");
  }

  BEGIN_MSG_MAP_EX(CPlayerWindow)
    MSG_WM_SYSCOMMAND(OnSysCommand)
    CHAIN_MSG_MAP(BaseT)
  END_MSG_MAP()

  virtual BOOL OnIdle()
  {
    // TODO:Edit mode.

    return BaseT::OnIdle();
  }

  void OnInit(void)
  {
    BaseT::OnInit();

    HMENU sys = ::GetSystemMenu(m_hWnd, FALSE);
    ::AppendMenu(sys, MF_SEPARATOR, 0, 0);
    ::AppendMenu(sys, 0, ID_APP_ABOUT, TEXT("&About"));
  }

  void OnSysCommand(UINT nID, CPoint)
  {
    if (ID_APP_ABOUT == nID) {
      CDlgAbout().DoModal();
      return;
    }

    SetMsgHandled(FALSE);
  }

  void onRender(void)
  {
    BaseT::onRender();
    drawText(-1, 0, 0, mRes.mName.c_str(), 16, 0xffffff00);
    ListRes();
  }
};

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
  CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Good Project Files(*.txt)\0*.txt\0"));
  if (IDOK != dlg.DoModal()) {
    return 0;
  }

  CGLMessageLoop theLoop;
  _Module.AddMessageLoop(&theLoop);

  CPlayerWindow& wndMain = CPlayerWindow::getInst();

  if (wndMain.CreateEx() == NULL) {
    ATLTRACE(_T("Main gl window creation failed!\n"));
    return 0;
  }

  if (!wndMain.init(dlg.m_szFileName)) {
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
