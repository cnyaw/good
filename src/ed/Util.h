
//
// Util.h
// Good editor, utilities.
//
// Copyright (c) 2014 Waync Cheng.
// All Rights Reserved.
//
// 2014/06/17 Waync created.
//

#pragma once

void DrawAlphaRect(CDCHandle dc, COLORREF clr, int x, int y, int w, int h, int step)
{
  CDC memdc;
  memdc.CreateCompatibleDC(dc);

  CBitmap bmp;
  bmp.CreateCompatibleBitmap(dc, w, h);
  memdc.SelectBitmap(bmp);

  RECT rc = {0, 0, w, h};
  memdc.FillSolidRect(&rc, clr);

  BLENDFUNCTION bf = {0};
  bf.SourceConstantAlpha = step;
  dc.AlphaBlend(x, y, w, h, memdc, 0, 0, w, h, bf);
}

RECT DrawTrackFocusRect(HDC dc, POINT a, POINT b)
{
  if (a.x > b.x) {
    std::swap(a.x, b.x);
  }

  if (a.y > b.y) {
    std::swap(a.y, b.y);
  }

  RECT r = {a.x, a.y, b.x, b.y};
  CDCHandle(dc).DrawFocusRect(&r);

  return r;
}

ImgT LoadTileset(std::string const& path)
{
  ImgT img = ImgT::getImage(path);
  if (img.isValid()) {
    return img;
  } else {
    return ImgT();
  }
}

template<class T>
void UpdateRelativePath(T& cont, std::string const& org, std::string const& path)
{
  for (typename T::iterator it = cont.begin(); cont.end() != it; ++it) {
    std::string p = org + it->second.mFileName;
    good::normalizePath(p);
    it->second.mFileName = GetRelativePath(p, path);
  }
}

template<class T>
void UpdateRelativePath2(T& cont, std::string const& org, std::string const& path)
{
  for (typename T::iterator it = cont.begin(); cont.end() != it; ++it) {
    std::string p = org + it->second;
    good::normalizePath(p);
    it->second = GetRelativePath(p, path);
  }
}

std::string GetRelativePath(std::string const& path, std::string const& relPath)
{
  return good::getRelativePath(good::getPathName(relPath), good::getPathName(path)) + good::getFileName(path);
}

std::string ExtractFileName(std::string const& path)
{
  return good::getFileName(path);
}

std::string ExtractFilePath(std::string const& path)
{
  std::string::size_type pos = path.find_last_of('/', path.length() - 2);
  if (std::string::npos != pos) {
    return path.substr(pos + 1);
  } else {
    return path;
  }
}

std::string CompleteFileExt(std::string const& path, std::string const& ext)
{
  std::string name = path;

  std::string lname = path;
  sw2::Util::toLowerString(lname);

  if (lname.npos == lname.find(ext)) {
    name += ext;
  }

  return name;
}

HWND GoodCreateSimpleToolbar(HWND hParent, UINT idRes, int nBtn = -1)
{
  CToolBarCtrl tb = CFrameWindowImplBase<>::CreateSimpleToolBarCtrl(hParent, idRes, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
  tb.SetWindowPos(NULL, 3, 3, (-1 == nBtn ? tb.GetButtonCount() : nBtn) * 23, 22, SWP_NOZORDER | SWP_NOACTIVATE);

  return tb;
}

void GetObjDim(const PrjT::LevelT &lvl, const PrjT::ObjectT &obj, RECT &rc)
{
  sw2::IntRect irc;
  good::getObjDim<PrjT>(lvl, obj, irc);
  rc.left = irc.left;
  rc.right = irc.right;
  rc.top = irc.top;
  rc.bottom = irc.bottom;
}

bool SelMultiFile(LPCTSTR lpszDefExt, LPCTSTR lpszFilter, std::vector<std::string> &paths)
{
  CFileDialog dlg(TRUE, lpszDefExt, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, lpszFilter);

  const int LenBuff = 4096;
  TCHAR Buff[LenBuff] = {0};
  dlg.m_ofn.lpstrFile = Buff;
  dlg.m_ofn.nMaxFile = LenBuff;

  if (IDOK != dlg.DoModal()) {
    return false;
  }

  if (0 != Buff[dlg.m_ofn.nFileOffset - 1]) {
    Buff[dlg.m_ofn.nFileOffset - 1] = 0;
  }

  CString path = Buff;
  for (int i = path.GetLength() + 1, s = i; i < LenBuff; i++) {
    if (0 != Buff[i]) {
      continue;
    }
    CString n(Buff + s, i - s);
    n = path + _T("\\") + n;
    paths.push_back(std::string(n));
    if (0 == Buff[i + 1]) {
      break;
    }
    s = i + 1;
  }

  return true;
}

// end of Util.h
