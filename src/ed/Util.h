
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

std::string GetRelativePath(std::string const& path, std::string const& relPath)
{
  return good::ed::getRelativePath(good::getPathName(relPath), good::getPathName(path)) + good::getFileName(path);
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
  good::ed::getObjDim<PrjT>(lvl, obj, irc);
  rc.left = irc.left;
  rc.right = irc.right;
  rc.top = irc.top;
  rc.bottom = irc.bottom;
}

bool SelMultiFile(LPCTSTR lpszDefExt, LPCTSTR lpszFilter, std::vector<std::string> &paths)
{
  CMultiFileDialog dlg(lpszDefExt, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, lpszFilter);
  if (IDOK != dlg.DoModal()) {
    return false;
  }

  CString path;
  int nLength = dlg.GetDirectory(NULL, 0);
  if (0 < nLength && 0 < dlg.GetDirectory(path.GetBuffer(nLength), nLength)) {
    path.ReleaseBuffer(nLength - 1);
  } else {
    return false;
  }

  LPCTSTR pFile = dlg.GetFirstFileName();
  while (pFile) {
    CString n = path + _T("\\") + pFile;
    paths.push_back(std::string(n));
    pFile = dlg.GetNextFileName();
  }

  return true;
}

// end of Util.h
