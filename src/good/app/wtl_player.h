
//
// wtl_player.h
// Player implementation for editor.
//
// Copyright (c) 2013 Waync Cheng.
// All Rights Reserved.
//
// 2013/11/02 Waync created.
//

#pragma once

#include "wtl_app.h"
#include "font2.h"                      // Generate font2.h from font2.bmp as uchar array FONT2_MOD by bin2c tool.

namespace good {

namespace rt {

template<class PlayerT>
class CPlayerWindowImpl : public WtlApplicationImpl<PlayerT>
{
public:

  enum {
    CX_FONT2 = 8,
    CY_FONT2 = 16,
    CCH_ROW_FONT2 = 15,
    COLOR_TRACE = 0x90ffffff,
    SHOW_TIP_TIME = 6 * 60,             // 6 seconds.
    DEFAULT_LOG_LINES_PER_PAGE = 30,
    COLOR_CMD_LINE = 0xffffffff,
    MAX_CMD_LINE_HIST = 100,
  };

  typedef WtlApplicationImpl<PlayerT> BaseT;

  std::string datFont;

  bool showFPS, showOutput, showTexInfo;

  std::string tip;
  bool isTipShown;
  int timeTip;

  int caretTimer;
  std::string cmdLine, lazyRunCmdLine;
  int iCmdHist;
  std::vector<std::string> cmdHist;

  int iLogs;
  std::vector<std::string> logs;
  std::vector<int> idxLineLogs;

  int iTexs;
  int maxDrawCalls, maxActors;

  CPlayerWindowImpl() : showFPS(true), showOutput(false), showTexInfo(false), maxDrawCalls(0), maxActors(0), caretTimer(0), iCmdHist(0)
  {
    tip = "Press Ctrl+Alt+O to toggle trace messages";
  }

  BEGIN_MSG_MAP_EX(CPlayerWindowImpl)
    MSG_WM_CHAR(OnChar)
    MSG_WM_KEYUP(OnKeyUp)
#ifdef ID_VIEW_OUTPUTWINDOW
    COMMAND_ID_HANDLER_EX(ID_VIEW_OUTPUTWINDOW, OnViewOutputWindow)
#endif
#ifdef ID_VIEW_TEXTURE
    COMMAND_ID_HANDLER_EX(ID_VIEW_TEXTURE, OnViewTextureInfo)
#endif
    CHAIN_MSG_MAP(BaseT)
  END_MSG_MAP()

  void doTrace(const char *s)
  {
    int idxLine = (int)logs.size();
    std::string ss;
    while (SimpleBreakLine(&s, gx.SCREEN_W, ss)) {
      logs.push_back(ss);
      idxLineLogs.push_back(idxLine);
    }

    int nPage = getNumLogsPerPage();

    if (iLogs + nPage < (int)logs.size()) {
      iLogs = (int)(logs.size() - (logs.size() % nPage)); // Move to end page.
    }

    if (!isTipShown && 0 == timeTip) {
      timeTip = SHOW_TIP_TIME;
    }
  }

  void doUserIntEvent(int i)
  {
    char buff[128];
    sprintf(buff, "* User INT event: %d", i);
    doTrace(buff);
  }

  int getNumLogsPerPage() const
  {
    int nPage = mRes.mHeight / CY_FONT2 - 1;
    if (0 >= nPage) {
      nPage = DEFAULT_LOG_LINES_PER_PAGE;
    }
    return nPage;
  }

  void Reset(int StartLevel = -1)
  {
    iLogs = 0;
    logs.clear();
    idxLineLogs.clear();

    iCmdHist = 0;
    cmdHist.clear();
    cmdLine = "";

    mPkgCallStack.clear();
    mNextPlayPkg.clear();

    showOutput = false;
    showTexInfo = false;

    isTipShown = false;
    timeTip = 0;
    iTexs = maxDrawCalls = maxActors = 0;

    mStartLevel = StartLevel;
  }

  void ToggleFps()
  {
    showFPS = !showFPS;
  }

  void ToggleOutput()
  {
    showOutput = !showOutput;
    isTipShown = true;
    timeTip = 0;
    if (showOutput && showTexInfo) {
      showTexInfo = false;
    }
    mDirty = true;
  }

  void ToggleTexInfo()
  {
    showTexInfo = !showTexInfo;
    if (showTexInfo && showOutput) {
      showOutput = false;
      isTipShown = true;
      timeTip = 0;
    }
    mDirty = true;
  }

  void OnInit()
  {
    BaseT::OnInit();

    //
    // Load font for messages.
    //

    datFont = std::string((char*)FONT2_MOD, sizeof(FONT2_MOD));
  }

  bool SimpleBreakLine(const char **s, int maxW, std::string &subs) const
  {
    subs = "";
    int w = 0;
    const char *p = *s;
    while (*p) {
      subs += *p++;
      w += CX_FONT2;
      if (w >= maxW) {
        break;
      }
    }
    *s = p;
    return !subs.empty();
  }

  void SimpleDrawText(int x, int y, std::string const &s, sw2::uint color = 0xffffffff)
  {
    gx::GLImage font = gx::GLImageResource::inst().getImage("good.ed.texture.font2.bmp", datFont);
    if (!font.isValid()) {
      return;
    }
    for (size_t i = 0; i < s.size(); i++) {
      int idx = s[i] - ' ';
      int srcx = CX_FONT2 * (idx % CCH_ROW_FONT2);
      int srcy = CY_FONT2 * (idx / CCH_ROW_FONT2);
      gx.drawImage(x + CX_FONT2 * (int)i, y, font, srcx, srcy, CX_FONT2, CY_FONT2, color, .0f, 1.0f, 1.0f);
    }
  }

  void DrawCmdLine_i()
  {
    std::string s(cmdLine);
    if (10 <= caretTimer) {
      s += "|";
    }
    if (!s.empty()) {
      const int nPage = getNumLogsPerPage();
      SimpleDrawText(0, nPage * CY_FONT2, s, COLOR_CMD_LINE);
    }
  }

  void DrawLogInfo_i()
  {
    caretTimer = (caretTimer + 1) % 60;
    if ((showOutput || 0 < timeTip) && !logs.empty()) {
      if (isTipShown) {
        const int nPage = getNumLogsPerPage();
        for (int i = iLogs; i < (int)logs.size() && (i - iLogs) < nPage; i++) {
          SimpleDrawText(0, (i - iLogs) * CY_FONT2, logs[i], COLOR_TRACE);
        }
      } else if (0 < timeTip) {
        SimpleDrawText(0, 0, tip, COLOR_TRACE);
        if (0 == --timeTip) {
          isTipShown = true;
        }
      }
    }
    if (showOutput) {
      DrawCmdLine_i();
      mDirty = true;                    // Force to update caret timer.
    }
  }

  void DrawTexInfo_i()
  {
    gx::GLImageResource &ir = gx::GLImageResource::inst();
    const int nMaxTexs = ir.GetTextureCount();
    if (0 >= nMaxTexs) {
      return;
    }
    gx::GL_Surface *sur = ir.GetTex(iTexs);
    int size = mRes.mWidth;
    if (mRes.mWidth > mRes.mHeight) {
      size = mRes.mHeight;
    }
    gx.drawTex(0, 0, sur, size, size, 0xffffffff);
    char buff[512];
    sprintf(buff, "%d/%d,t%d", iTexs, nMaxTexs - 1, sur->size());
    SimpleDrawText(0, 0, buff, 0xffff0000);
  }

  void DrawFpsInfo_i()
  {
    //
    // Draw FPS info.
    //

    maxDrawCalls = max(maxDrawCalls, gx.nLastDrawCalls);
    maxActors = max(maxActors, mActors.size());

    char buff[512], buff2[128];;
    sprintf(buff, "%d,d%d/%d,o%d/%d", BaseT::fps.getFps(), gx.nLastDrawCalls, maxDrawCalls, mActors.size(), maxActors);
    if (mCanvas.size()) {
      sprintf(buff2, ",c%d", mCanvas.size());
      strcat(buff, buff2);
    }
#ifdef GOOD_SUPPORT_STGE
    int nActions = 0, nParticles = 0;
    for (int i = 0; i < GOOD_MAX_STGE_OBJ_MGR; i++) {
      nActions += mObjMgr[i].actions.size();
      nParticles += mObjMgr[i].objects.size();
    }
    if (nActions) {
      sprintf(buff2, ",a%d", nActions);
      strcat(buff, buff2);
    }
    if (nParticles) {
      sprintf(buff2, ",b%d", nParticles);
      strcat(buff, buff2);
    }
#endif
    SimpleDrawText(mRes.mWidth - CX_FONT2 * (int)strlen(buff), 0, buff, COLOR_TRACE);
  }

  void onRender(void)
  {
    DrawLogInfo_i();                    // Draw output message.

    if (showTexInfo) {
      DrawTexInfo_i();                  // Draw texture list.
    }

    if (showFPS) {
      DrawFpsInfo_i();                  // Calc FPS.
    }

    if (!lazyRunCmdLine.empty()) {
      std::string::size_type n = 0;
      while (std::string::npos != (n = lazyRunCmdLine.find('%', n))) {
        lazyRunCmdLine.replace(n, 1, "%%");
        n += 2;
      }
      doLuaScript(lazyRunCmdLine.c_str()); // doLuaScript will do format string, so convert format char % to %% to prevent format error.
      lazyRunCmdLine = "";
    }
  }

  void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    if (!showOutput) {
      return;
    }

    switch (nChar)
    {
    case 8:                           // Backspace.
      if (!cmdLine.empty()) {
        cmdLine.erase(cmdLine.length() - 1);
      }
      break;
    case 13:                          // Enter.
      if (cmdLine.empty()) {
        break;
      }
      cmdHist.push_back(cmdLine);
      if (MAX_CMD_LINE_HIST < cmdHist.size()) {
        cmdHist.erase(cmdHist.begin());
      }
      iCmdHist = (int)cmdHist.size();
      lazyRunCmdLine = cmdLine;
      doTrace((std::string(">") + cmdLine).c_str());
      cmdLine = "";
      break;
    case 27:                          // ESC.
      // NOP.
      break;
    default:
      cmdLine += (char)nChar;
      break;
    }
  }

  void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
#ifndef ID_VIEW_OUTPUTWINDOW
    if (('o' == nChar || 'O' == nChar) && (::GetKeyState(VK_LCONTROL) & 0x8000) && (::GetKeyState(VK_LMENU) & 0x8000)) {
      OnViewOutputWindow(0, 0, 0);
      return;
    }
#endif
#ifndef ID_VIEW_TEXTURE
    if (('t' == nChar || 'T' == nChar) && (::GetKeyState(VK_LCONTROL) & 0x8000) && (::GetKeyState(VK_LMENU) & 0x8000)) {
      OnViewTextureInfo(0, 0, 0);
      return;
    }
#endif

    if (showOutput) {
      const int nPage = getNumLogsPerPage();
      switch (nChar)
      {
      case VK_PRIOR:                    // Prev page.
        if (nPage <= iLogs) {
          iLogs -= nPage;
        }
        break;
      case VK_NEXT:                     // Next page.
        if (iLogs + nPage < (int)logs.size()) {
          iLogs += nPage;
        }
        break;
      case VK_END:
        iLogs = (int)(logs.size() - (logs.size() % nPage));
        break;
      case VK_HOME:
        iLogs = 0;
        break;
      case VK_UP:                       // Prev cmd line.
        if (0 < iCmdHist) {
          cmdLine = cmdHist[--iCmdHist];
        }
        break;
      case VK_DOWN:                     // Next cmd line.
        if ((int)cmdHist.size() > iCmdHist + 1) {
          cmdLine = cmdHist[++iCmdHist];
        }
        break;
      }
    } else if (showTexInfo) {
      const int prev = iTexs;
      const int nMaxTexs = gx::GLImageResource::inst().GetTextureCount();
      switch (nChar)
      {
      case VK_PRIOR:
        if (0 < iTexs) {
          iTexs -= 1;
        }
        break;
      case VK_NEXT:
        if (nMaxTexs - 1 > iTexs) {
          iTexs += 1;
        }
        break;
      case VK_END:
        iTexs = nMaxTexs - 1;
        break;
      case VK_HOME:
        iTexs = 0;
        break;
      }
      if (prev != iTexs) {
        mDirty = true;
      }
    }
  }

  void OnViewOutputWindow(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    ToggleOutput();
    Invalidate();
  }

  void OnViewTextureInfo(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    ToggleTexInfo();
    Invalidate();
  }
};

class CPlayerWindow : public CPlayerWindowImpl<CPlayerWindow>
{
public:
  static CPlayerWindow& getInst()
  {
    static CPlayerWindow inst;
    return inst;
  }
};

} // namespace rt

} // namespace good

// end of wtl_player.h
