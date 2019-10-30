
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
#include "font2.h"

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
  gx::GLImage mFont;

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

  int maxDrawCalls, maxActors;

  CPlayerWindowImpl() : showFPS(false), showOutput(false), showTexInfo(false), mFont(0), maxDrawCalls(0), maxActors(0), caretTimer(0), iCmdHist(0)
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
    logs.push_back(s);

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

    iCmdHist = 0;
    cmdHist.clear();
    cmdLine = "";

    mPackageStack.clear();
    mCallStack.clear();

    showOutput = false;
    showTexInfo = false;

    isTipShown = false;
    timeTip = 0;
    maxDrawCalls = maxActors = 0;

    mStartLevel = StartLevel;

    mFont = gx::GLImage(0);
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
  }

  void OnInit()
  {
    BaseT::OnInit();

    //
    // Load font for messages.
    //

    datFont = std::string((char*)FONT2_MOD, sizeof(FONT2_MOD));
  }

  void onPackageChanged()
  {
    BaseT::onPackageChanged();
    mFont = gx::GLImage(0);
  }

  void SimpleDrawText(int x, int y, std::string const &s, sw2::uint color = 0xffffffff)
  {
    if (!mFont.isValid()) {
      gx::GLImageResource &gx = gx::GLImageResource::inst();
      mFont.mSur = gx.getImage("good.ed.texture.font2.bmp", datFont);
    }

    for (size_t i = 0; i < s.size(); i++) {
      int idx = s[i] - ' ';
      int srcx = CX_FONT2 * (idx % CCH_ROW_FONT2);
      int srcy = CY_FONT2 * (idx / CCH_ROW_FONT2);
      gx.drawImage(x + CX_FONT2 * (int)i, y, mFont, srcx, srcy, CX_FONT2, CY_FONT2, color, .0f, 1.0f, 1.0f);
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
    }
  }

  bool Outbound_i(int x, int y, int rcSize, int width, int height) const
  {
    return (1 + x) * rcSize > width || (1 + y) * rcSize > height;
  }

  void DrawTexInfo_i()
  {
    int size = mRes.mWidth;
    if (mRes.mWidth > mRes.mHeight) {
      size = mRes.mHeight;
    }
    gx::GLImageResource &ir = gx::GLImageResource::inst();
    int count = 1;                    // Number of tex display on a row.
    for (int i = 0;; i++) {
      int x = (ir.GetTextureCount() - 1) % count;
      int y = (ir.GetTextureCount() - 1) / count;
      if (!Outbound_i(x, y, size, mRes.mWidth, mRes.mHeight)) {
        break;
      }
      if (mRes.mWidth > mRes.mHeight) {
        size = mRes.mHeight / (1 + i);
      } else {
        size = mRes.mWidth / (1 + i);
      }
      count = mRes.mWidth / size;
    }
    char buff[512];
    for (int i = 0; i < ir.GetTextureCount(); i++) {
      int x = size * (i % count);
      int y = size * (i / count);
      gx::GL_Surface<gx::GLImageResource> *sur = ir.GetTex(i);
      gx.drawTex(x, y, sur, size, size, 0xffffffff);
      sprintf(buff, "%d:%d", i, sur->size());
      SimpleDrawText(x, y, buff, 0xffff0000);
    }
  }

  void DrawFpsInfo_i()
  {
    static int FPS = 0;
    static int framesPerSecond = 0;     // This will store our FPS.
    static DWORD LastTime = 0;          // This will hold the time from the last frame.
    DWORD CurrentTime = GetTickCount();
    framesPerSecond += 1;

    if (CurrentTime - LastTime >= 1000) {
      LastTime = CurrentTime;
      FPS = framesPerSecond;
      framesPerSecond = 0;
    }

    //
    // Draw FPS info.
    //

    maxDrawCalls = max(maxDrawCalls, gx.nLastDrawCalls);
    maxActors = max(maxActors, mActors.size());

    char buff[512];
    sprintf(buff, "%d,d%d/%d,o%d/%d", FPS, gx.nLastDrawCalls, maxDrawCalls, mActors.size(), maxActors);
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
      doLuaScript(lazyRunCmdLine.c_str());
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
      iCmdHist = cmdHist.size();
      lazyRunCmdLine = cmdLine;
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
    if (!showOutput) {
      return;
    }

    const int nPage = getNumLogsPerPage();
    switch (nChar)
    {
    case VK_PRIOR:                      // Prev page.
      if (nPage <= iLogs) {
        iLogs -= nPage;
      }
      break;
    case VK_NEXT:                       // Next page.
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
    case VK_UP:                         // Prev cmd line.
      if (0 < iCmdHist) {
        cmdLine = cmdHist[--iCmdHist];
      }
      break;
    case VK_DOWN:                       // Next cmd line.
      if ((int)cmdHist.size() > iCmdHist + 1) {
        cmdLine = cmdHist[++iCmdHist];
      }
      break;
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
