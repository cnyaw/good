
//
// EditLevel.h
// Level editor.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/01/31 Waync created.
//

#pragma once

template<class MainT, class EditorT>
class CLevelEditView : public CScrollWindowImpl<CLevelEditView<MainT, EditorT> >, public CScrollEraseBkgndImpl<CLevelEditView<MainT, EditorT> >
{
public:
  DECLARE_WND_CLASS(NULL)

  int mHot;                             // Cur hot item ID.
  bool mHotResizer;
  std::vector<int> mCurSel;             // Sel item ID.

  int mSnapSize;                        // x 8

  CImageListManaged mImgObjState;

  EditorT& mEditor;

  CLevelEditView(EditorT& ed) : mHot(-1), mEditor(ed), mSnapSize(2)
  {
  }

  void LoopCurSel(bool IsBackward)
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);
    for (size_t i = 0; i < lvl.mObjIdx.size(); i++) {
      if (lvl.mObjIdx[i] != mCurSel[0]) {
        continue;
      }
      mCurSel.clear();
      if (IsBackward) {
        if (0 == i) {
          mCurSel.push_back(lvl.mObjIdx[lvl.mObjIdx.size() - 1]);
        } else {
          mCurSel.push_back(lvl.mObjIdx[i - 1]);
        }
      } else {
        if (lvl.mObjIdx.size() - 1 == i) {
          mCurSel.push_back(lvl.mObjIdx[0]);
        } else {
          mCurSel.push_back(lvl.mObjIdx[i + 1]);
        }
      }
      break;
    }
  }

  void SetCurSelFirstObj(bool IsBackward)
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);
    if (IsBackward) {
      mCurSel.push_back(lvl.mObjIdx[lvl.mObjIdx.size() - 1]);
    } else {
      mCurSel.push_back(lvl.mObjIdx[0]);
    }
  }

  void TabSwitchCurSel(bool IsBackward)
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);
    if (lvl.mObjIdx.empty()) {
      return;
    }

    if (mCurSel.empty()) {
      SetCurSelFirstObj(IsBackward);
    } else {
      LoopCurSel(IsBackward);
    }

    SelItemChange(GOOD_RESOURCE_LEVEL_OBJECT, mCurSel[0]);
  }

  bool IsLevelObjUsed(const std::vector<int> &v) const
  {
    std::string lvlName, oName;
    if (PrjT::inst().isLevelObjUsed(v, lvlName, oName)) {
      SW2_TRACE_ERROR("Item is still used by %s of %s", oName.c_str(), lvlName.c_str());
      return true;
    }
    return false;
  }

  void DeleteObj(std::vector<int> const &v)
  {
    if (v.empty()) {
      return;
    }

    if (IsLevelObjUsed(v)) {
      return;
    }

    if (!PrjT::inst().removeLevelObj(mEditor.mId, v)) {
      return;
    }

    mEditor.DelTreeItem(v);
    mCurSel.clear();

    Invalidate();

    //
    // Remove all prop and add again, lvl will be the first visible prop.
    //

    MainT::inst().mExpView.Refresh(mEditor.mId);

    OnMouseMove(0, CPoint());
  }

  void MoveCurSel(int offsetx, int offsety)
  {
    if (mCurSel.empty()) {
      return;
    }

    PrjT::LevelT& lvl = PrjT::inst().getLevel(mEditor.mId);

    if (0 == (::GetKeyState(VK_LCONTROL) & 0x8000)) {
      offsetx *= lvl.getSnapWidth();
      offsety *= lvl.getSnapHeight();
    }

    if (::GetKeyState(VK_LSHIFT) & 0x8000) { // Resize.
      (void)lvl.resizeObj(mCurSel, offsetx, offsety);
    } else {                            // Normal move.
      (void)lvl.moveObj(mCurSel, offsetx, offsety);
    }

    Invalidate();

    MainT::inst().mExpView.UpdateProperty(); // Force update prop view.
  }

  POINT GetCurCursorPos(RECT &rcv) const
  {
    POINT ptCur;
    GetCursorPos(&ptCur);
    ScreenToClient(&ptCur);
    ptCur.x += m_ptOffset.x;
    ptCur.y += m_ptOffset.y;

    POINT pt, sz, ptDP;
    PrepareDraw(rcv, pt, sz, ptDP);

    if (0 != ptDP.x) {
      ptCur.x -= pt.x;
    }

    if (0 != ptDP.y) {
      ptCur.y -= pt.y;
    }

    return ptCur;
  }

  void AddNewObj()
  {
    RECT rcv;
    POINT ptCur = GetCurCursorPos(rcv);

    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);
    if (lvl.isShowSnap()) {
      int sw = lvl.getSnapWidth(), sh = lvl.getSnapHeight();
      ptCur.x = ptCur.x / sw * sw;
      ptCur.y = ptCur.y / sh * sh;
    }

    int id = PrjT::inst().addLevelObj<ImgT>(mEditor.mId, lvl.mAddObj, lvl.mAddMap, lvl.mAddTex, lvl.mAddCol, ptCur.x, ptCur.y);
    if (-1 == id) {
      return;
    }

    //
    // Add new item to obj tree.
    //

    mEditor.AddTreeItem(id);

    //
    // Force reset prop view.
    //

    mCurSel.clear();
    mCurSel.push_back(id);

    MainT::inst().mExpView.SetCurSel(-1);

    //
    // Force change cur sel prop to this editor.
    //

    MainT::inst().mExpView.SetCurSel(mEditor.mId);

    SelItemChange(GOOD_RESOURCE_LEVEL_OBJECT, id);

    SetFocus();
  }

  void ScrollObjToView(int idObj)
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);
    if (!lvl.isObj(idObj)) {
      return;
    }

    RECT rcv;
    POINT pt, sz, ptDP;
    PrepareDraw(rcv, pt, sz, ptDP);

    PrjT::ObjectT const& inst = lvl.getObj(idObj);

    RECT rcm, rc;
    GetObjDim(lvl, inst, rc);

    InflateRect(&rcv, -32, -32);
    if (!IntersectRect(&rcm, &rc, &rcv)) {
      RECT rcClient;
      GetClientRect(&rcClient);
      OffsetRect(&rcClient, rc.left - (rcClient.right - (rc.right - rc.left))/2, rc.top - (rcClient.bottom - (rc.bottom - rc.top))/2);
      ScrollToView(rcClient);
    }
  }

  BEGIN_MSG_MAP_EX(CLevelEditView)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_KEYDOWN(OnKeyDown)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    MSG_WM_RBUTTONDOWN(OnRButtonDown)
    CHAIN_MSG_MAP(CScrollEraseBkgndImpl<CLevelEditView>)
    CHAIN_MSG_MAP(CScrollWindowImpl<CLevelEditView>)
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);

    SIZE sz = {lvl.mWidth, lvl.mHeight};
    SetScrollOffset(0, 0, FALSE);
    SetScrollSize(sz);

    int szSnap[] = {8, 16, 24, 32, 40, 48, 56, 64};
    if (lvl.getSnapWidth() != lvl.getSnapHeight() || szSnap + 8 == std::find(szSnap, szSnap + 8, lvl.getSnapWidth())) {
      mSnapSize = ID_SNAP_CUSTOMIZE - ID_SNAP_8 + 1;
    } else {
      mSnapSize = lvl.getSnapWidth() / EDITOR_SNAP_SCALE;
    }

    mImgObjState.Create(IDB_OBJSTATE, 16, 0, RGB(255,0,255));

    SetMsgHandled(FALSE);
    return 0;
  }

  void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    switch (nChar)
    {
    case VK_TAB:
      TabSwitchCurSel(0 != (GetKeyState(VK_LSHIFT) & 0x8000));
      break;

    case VK_ESCAPE:
      PrjT::inst().getLevel(mEditor.mId).setMoveTool();
      break;

    case VK_DELETE:
      DeleteObj(mCurSel);
      break;

    case VK_LEFT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:
      {
        int const Movements[] = {-1, 0, 0, -1, 1, 0, 0, 1};
        int offsetx = Movements[2 * (nChar - VK_LEFT)];
        int offsety = Movements[2 * (nChar - VK_LEFT) + 1];
        MoveCurSel(offsetx, offsety);
      }
      break;
    }
  }

  void OnLButtonDown(UINT nFlags, CPoint point)
  {
    if (GetFocus() != m_hWnd) {
      SetFocus();
    }

    //
    // Force change cur sel prop to this editor.
    //

    MainT::inst().mExpView.SetCurSel(mEditor.mId);

    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);
    if (lvl.isMoveTool()) {
      HandleMouseDown();
    } else if (lvl.isRemoveTool()) {
      if (-1 != mHot) {
        std::vector<int> v;
        v.push_back(mHot);
        DeleteObj(v);
      }
    } else {
      AddNewObj();
    }
  }

  int FindHotObj(PrjT::ObjectT const& o, const RECT &rcv, const POINT &ptCur, RECT &rcm) const
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);

    //
    // Hittest, last draw first hit.
    //

    for (int i = (int)o.mObjIdx.size() - 1; i >= 0; --i) {

      int id = o.mObjIdx[i];
      PrjT::ObjectT const& inst = lvl.getObj(id);

      RECT rc;
      GetObjDim(lvl, inst, rc);

      int idHot = FindHotObj(inst, rcv, ptCur, rcm);
      if (-1 != idHot) {
        return idHot;
      }

      if (!::IntersectRect(&rcm, &rc, &rcv) || !PtInRect(&rcm, ptCur)) {
        continue;
      }

      return inst.mId;
    }

    return -1;
  }

  void OnMouseMove(UINT nFlags, CPoint point)
  {
    int prevHot = mHot;
    mHot = -1;
    mHotResizer = false;

    RECT rcv;
    POINT ptCur = GetCurCursorPos(rcv);

    //
    // Hittest.
    //

    MainT& mt = MainT::inst();
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);

    RECT rcm;
    mHot = FindHotObj(lvl, rcv, ptCur, rcm);

    if (-1 != mHot) {
      PrjT::ObjectT const& inst = lvl.getObj(mHot);
      RECT rc2 = {rcm.right - 10, rcm.bottom - 10, rcm.right, rcm.bottom};
      mHotResizer = PrjT::ObjectT::TYPE_TEXBG == inst.mType && PtInRect(&rc2, ptCur);
    }

    if (prevHot != mHot) {
      if (-1 != mHot) {
        PrjT::ObjectT const& inst = lvl.getObj(mHot);
        std::string str = GetObjectName(inst);
        mt.mStatus.SetPaneText(2, str.c_str());
      } else {
        if (!mCurSel.empty()) {
          CString str;
          str.Format(_T("%d select"), mCurSel.size());
          mt.mStatus.SetPaneText(2, str);
        } else {
          mt.mStatus.SetPaneText(2, _T(""));
        }
      }
      Invalidate(FALSE);
    }
  }

  void OnRButtonDown(UINT nFlags, CPoint point)
  {
    if (GetFocus() != m_hWnd) {
      SetFocus();
    }

    //
    // Force change cur sel prop to this editor.
    //

    MainT::inst().mExpView.SetCurSel(mEditor.mId);

    //
    // Reset current selection.
    //

    mCurSel.clear();

    Invalidate(FALSE);

    //
    // Changed cur sel res.
    //

    PrjT::inst().getLevel(mEditor.mId).setToolByLevelObjId(mHot);
  }

  //
  // Override.
  //

  void DoPaintHotObj(CDC &memdc, const PrjT::ObjectT& inst, const RECT &rc, CPen &pinkPen) const
  {
    //
    // Draw mouse hot rect.
    //

    memdc.SelectPen(pinkPen);
    memdc.Rectangle(&rc);

    //
    // Draw resizer on tex obj.
    //

    if (PrjT::ObjectT::TYPE_TEXBG == inst.mType) {
      CBrush br;
      br.CreateSolidBrush(RGB(255, 0, 0));
      RECT rc2 = {rc.right - 10, rc.bottom - 10, rc.right, rc.bottom};
      memdc.FillRect(&rc2, br);
    }
  }

  void DoPaintObjStateIcon(CDC &memdc, const PrjT::LevelT& lvl, const PrjT::ObjectT& inst, bool IsImgValid, const RECT &rc)
  {
    int id = inst.mId;
    int IconOffset = 0;
    if (!IsImgValid) {
      mImgObjState.Draw(memdc, 4, rc.left, rc.top, ILS_NORMAL);
      IconOffset += 16;
    }

    if (!inst.mScript.empty()) {
      mImgObjState.Draw(memdc, 0, rc.left + IconOffset, rc.top, ILS_NORMAL);
      IconOffset += 16;
    }

    if (!inst.mVisible) {
      mImgObjState.Draw(memdc, 1, rc.left + IconOffset, rc.top, ILS_NORMAL);
      IconOffset += 16;
    } else if (!lvl.isParentVisible(id)) {
      mImgObjState.Draw(memdc, 1, rc.left + IconOffset, rc.top, ILS_NORMAL);
      DrawAlphaRect(CDCHandle((HDC)memdc), RGB(255,0,0), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 45);
      IconOffset += 16;
    }

    if (inst.mRepX) {
      mImgObjState.Draw(memdc, 2, rc.left + IconOffset, rc.top, ILS_NORMAL);
      IconOffset += 16;
    }

    if (inst.mRepY) {
      mImgObjState.Draw(memdc, 3, rc.left + IconOffset, rc.top, ILS_NORMAL);
    }
  }

  void DoPaintChildObj(CDC &memdc, PrjT::LevelT const& lvl, std::vector<int> const& Objs, RECT &rcv, CPen &redPen, CPen &redDotPen, CPen &pinkPen, CPen &grayPen)
  {
    PrjT const& prj = PrjT::inst();

    for (size_t i = 0; i < Objs.size(); ++i) {

      int id = Objs[i];
      PrjT::ObjectT const& inst = lvl.getObj(id);

      RECT rcm, rc;
      GetObjDim(lvl, inst, rc);

      if (!::IntersectRect(&rcm, &rc, &rcv)) {
        DoPaintChildObj(memdc, lvl, inst.mObjIdx, rcv, redPen, redDotPen, pinkPen, grayPen);
        continue;
      }

      {
        RECT rcBaseFrame = rc;
        OffsetRect(&rcBaseFrame, -rcv.left, -rcv.top);
        memdc.SelectPen(grayPen);
        memdc.Rectangle(&rcBaseFrame);
      }

      GxT gx = GxT(memdc);
      bool IsImgValid = true;

      switch (inst.mType)
      {
      case PrjT::ObjectT::TYPE_MAPBG:
        {
          PrjT::MapT const& map = prj.getMap(inst.mMapId);

          ImgT img = LoadTileset(prj.getTex(map.mTileset.mTextureId).mFileName);
          if (!img.isValid()) {
            IsImgValid = false;
            break;
          }

          POINT adj0 = {rc.left % map.mTileset.mTileWidth, rc.top % map.mTileset.mTileHeight};

          POINT adj = {
            adj0.x + (map.mTileset.mTileWidth * (rcm.left / map.mTileset.mTileWidth)),
            adj0.y + (map.mTileset.mTileHeight * (rcm.top / map.mTileset.mTileHeight))
          };

          ::OffsetRect(&rcm, -rcm.left + adj.x, -rcm.top + adj.y);

          int left = (int)((rcm.left - rc.left) / map.mTileset.mTileWidth);
          int right = min(map.mWidth - 1, (int)((rcm.right - rc.left) / map.mTileset.mTileWidth));
          int top = (int)((rcm.top - rc.top) / map.mTileset.mTileHeight);
          int bottom = min(map.mHeight - 1, (int)((rcm.bottom - rc.top) / map.mTileset.mTileHeight));

          if (0 < left) {               // Draw an extra tile to avoid tear.
            left -= 1;
            rcm.left -= map.mTileset.mTileWidth;
          }

          if (0 < top) {
            top -= 1;
            rcm.top -= map.mTileset.mTileHeight;
          }

          CommonDrawMap(gx, map, img, rcm.left - rcv.left, rcm.top - rcv.top, left, top, right, bottom, 0xffffffff);
        }
        break;

      case PrjT::ObjectT::TYPE_TEXBG:
        {
          ImgT img = LoadTileset(prj.getTex(inst.mTextureId).mFileName);
          if (!img.isValid()) {
            IsImgValid = false;
            break;
          }

          int offsetx = rcm.left - rc.left;
          int offsety = rcm.top - rc.top;
          int w = min(rcm.right - rcm.left, img.getWidth() - abs(inst.mDim.left) - offsetx);
          int h = min(rcm.bottom - rcm.top, img.getHeight() - abs(inst.mDim.top) - offsety);

          gx.drawImage(rcm.left - rcv.left, rcm.top - rcv.top, img, inst.mDim.left + offsetx, inst.mDim.top + offsety, w, h);
        }
        break;

      case PrjT::ObjectT::TYPE_COLBG:
        {
          RECT r = rc;
          ::OffsetRect(&r, -rcv.left, -rcv.top);
          memdc.FillSolidRect(&r, inst.mBgColor & 0xffffff);
        }
        break;

      case PrjT::ObjectT::TYPE_SPRITE:
        {
          PrjT::SpriteT const& spr = prj.getSprite(inst.mSpriteId);

          ImgT img = LoadTileset(prj.getTex(spr.mTileset.mTextureId).mFileName);
          if (!img.isValid()) {
            IsImgValid = false;
            break;
          }

          int tile = spr.mFrame[0];
          int srcx = spr.mTileset.mTileWidth * (tile % spr.mTileset.mCxTile);
          int srcy = spr.mTileset.mTileHeight * (tile / spr.mTileset.mCxTile);

          int offsetx = rcm.left - rc.left;
          int offsety = rcm.top - rc.top;

          gx.drawImage(rcm.left - rcv.left, rcm.top - rcv.top, img, srcx + offsetx, srcy + offsety, rcm.right - rcm.left, rcm.bottom - rcm.top);
        }
        break;

      case PrjT::ObjectT::TYPE_DUMMY:
        {
          RECT r = rc;
          ::OffsetRect(&r, -rcv.left, -rcv.top);
          for (int i = 0; i < 3; i++) {
            memdc.Draw3dRect(&r, RGB(255, 0, 0), RGB(0, 0, 255));
            ::InflateRect(&r, -1, -1);
          }
        }
        break;

      case PrjT::ObjectT::TYPE_LVLOBJ:
        mEditor.mImages.Draw(memdc, 6, rc.left - rcv.left, rc.top - rcv.top, ILS_NORMAL);
        break;
      }

      ::OffsetRect(&rc, -rcv.left, -rcv.top);

      if (!mCurSel.empty()) {
        for (size_t j = 0; j < mCurSel.size(); ++j) {
          if (mCurSel[j] == id) {
            memdc.SelectPen(mCurSel.size() - 1 == j ? redPen : redDotPen);
            memdc.Rectangle(&rc);
            break;
          }
        }
      }

      //
      // Draw hot obj.
      //

      if (mHot == id) {
        DoPaintHotObj(memdc, inst, rc, pinkPen);
      }

      //
      // Draw object states icon on top-left corner.
      //

      DoPaintObjStateIcon(memdc, lvl, inst, IsImgValid, rc);

      //
      // Draw child objects.
      //

      DoPaintChildObj(memdc, lvl, inst.mObjIdx, rcv, redPen, redDotPen, pinkPen, grayPen);
    }
  }

  void DoPaintGridLines(CDC &memdc, const RECT &rcv) const
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);

    if (!lvl.isShowLine()) {
      return;
    }

    //
    // Horz line(s).
    //

    for (size_t i = 0; i < lvl.mHorzGrid.size(); ++i) {
      good::ed::GridLine const& gl = lvl.mHorzGrid[i];
      CPen pen;
      pen.CreatePen(PS_SOLID, 1, gl.color);
      memdc.SelectPen(pen);
      for (int j = 0; j < rcv.bottom; )  {
        memdc.MoveTo(-rcv.left, j - rcv.top);
        memdc.LineTo(lvl.mWidth - 1 - rcv.left, j - rcv.top);
        j += gl.range;
      }
    }

    //
    // Vert line(s).
    //

    for (size_t i = 0; i < lvl.mVertGrid.size(); ++i) {
      good::ed::GridLine const& gl = lvl.mVertGrid[i];
      CPen pen;
      pen.CreatePen(PS_SOLID, 1, gl.color);
      memdc.SelectPen(pen);
      for (int j = 0; j < rcv.right; ) {
        memdc.MoveTo(j - rcv.left, -rcv.top);
        memdc.LineTo(j - rcv.left, lvl.mHeight - 1 - rcv.top);
        j += gl.range;
      }
    }
  }

  void DoPaintGridDots(CDC &memdc, const RECT &rcv, const POINT &sz) const
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);
    if (lvl.isShowSnap()) {
      int sw = lvl.getSnapWidth(), sh = lvl.getSnapHeight();
      int offsetx = rcv.left % sw, offsety = rcv.top % sh;
      for (int i = 0; i <= sz.x / sw; i++) {
        for (int j = 0; j <= sz.y / sh; j++) {
          memdc.SetPixel(sw * i + offsetx, sh * j + offsety, RGB(0,0,0));
        }
      }
    }
  }

  void DoPaint(HDC hdc)
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mEditor.mId);

    //
    // Prepare.
    //

    RECT rcv;
    POINT pt, sz, ptDP;
    PrepareDraw(rcv, pt, sz, ptDP);

    //
    // Create memory dc of clip size(fit to client rect), and fill bkgnd with
    // grid pattern.
    //

    CDC memdc;
    memdc.CreateCompatibleDC(hdc);

    CBitmap membmp;
    membmp.CreateCompatibleBitmap(hdc, sz.x, sz.y);
    memdc.SelectBitmap(membmp);

    CBitmap pat;
    pat.LoadBitmap(IDB_BKBRUSH);

    CBrush br;
    br.CreatePatternBrush(pat);

    RECT rcb = {0, 0, sz.x, sz.y};
    memdc.FillRect(&rcb, br);

    //
    // Grid lines.
    //

    DoPaintGridLines(memdc, rcv);

    //
    // Draw object(s).
    //

    CPen redPen, redDotPen, pinkPen, grayPen;
    redPen.CreatePen(PS_SOLID, 3, RGB(255,0,0));
    redDotPen.CreatePen(PS_SOLID, 1, RGB(255,0,0));
    pinkPen.CreatePen(PS_DOT, 3, RGB(255,0,0));
    grayPen.CreatePen(PS_SOLID, 1, RGB(128,128,128));

    memdc.SetBkMode(TRANSPARENT);
    memdc.SelectBrush((HBRUSH)::GetStockObject(NULL_BRUSH));

    DoPaintChildObj(memdc, lvl, lvl.mObjIdx, rcv, redPen, redDotPen, pinkPen, grayPen);

    //
    // Grid dots.
    //

    DoPaintGridDots(memdc, rcv, sz);

    //
    // Blit.
    //

    ::BitBlt(hdc, pt.x, pt.y, sz.x, sz.y, memdc, 0, 0, SRCCOPY);
  }

  //
  // Impl.
  //

  void AddCurSel(int id)
  {
    for (size_t i = 0; i < mCurSel.size(); i++) {
      if (mCurSel[i] == mHot) {
        return;
      }
    }

    mCurSel.push_back(id);
  }

  bool ClickOnTexResizer()
  {
    if (!mHotResizer) {
      return false;
    }

    PrjT::LevelT& lvl = PrjT::inst().getLevel(mEditor.mId);
    PrjT::ObjectT const& o = lvl.getObj(mHot);
    CDlgSelTextureDim dlg(o.mTextureId, o.mDim.left, o.mDim.top, o.mDim.width(), o.mDim.height());
    if (IDOK == dlg.DoModal()) {
      RECT const &rcDim = dlg.mView.mDim;
      lvl.setObjDim(mHot, rcDim.left, rcDim.top, rcDim.right - rcDim.left, rcDim.bottom - rcDim.top);
      Invalidate(FALSE);
      MainT::inst().mExpView.UpdateProperty();
    }

    return true;
  }

  int GetCurSelIdx(int id) const
  {
    for (size_t i = 0; i < mCurSel.size(); i++) {
      if (mCurSel[i] == id) {
        return i;
      }
    }

    return -1;
  }

  void UpdateCurSel()
  {
    //
    // Update selection?
    //

    int FoundIdx = GetCurSelIdx(mHot);

    if (-1 != FoundIdx) {

      //
      // Change the hot selection.
      //

      mCurSel.erase(mCurSel.begin() + FoundIdx);
      mCurSel.push_back(mHot);

    } else {

      //
      // New selection. If is single selection then reset cursel.
      //

      if (0 == (::GetKeyState(VK_CONTROL) & 0x8000)) {
        mCurSel.clear();
      }

      mCurSel.push_back(mHot);
    }

    //
    // Update property view.
    //

    SelItemChange(GOOD_RESOURCE_LEVEL_OBJECT, mHot);
  }

  bool GetWindowsMessage(MSG &msg) const
  {
    if (!::GetMessage(&msg, NULL, 0, 0)) {
      ::PostQuitMessage(msg.wParam);
      return false;
    }
    return true;
  }

  void TrackSelObj()
  {
    SetCapture();

    CClientDC dc(m_hWnd);

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);

    POINT ptLast = pt;
    MainT& mf = MainT::inst();

    DrawTrackFocusRect(dc, pt, ptLast);

    while (GetCapture() == m_hWnd) {
      MSG msg;
      if (!GetWindowsMessage(msg)) {
        break;
      }

      switch (msg.message)
      {
      case WM_MOUSEMOVE:
        {
          //
          // Erase previous focus rect.
          //

          DrawTrackFocusRect(dc, pt, ptLast);

          //
          // Get current cursor pos and draw new focus rect.
          //

          GetCursorPos(&ptLast);
          ScreenToClient(&ptLast);

          RECT r = DrawTrackFocusRect(dc, pt, ptLast);

          CString s;
          s.Format(_T("(%d,%d)-(%dx%d)"), r.left, r.top, r.right - r.left, r.bottom - r.top);

          mf.mStatus.SetPaneText(2, s); // Display selection area top left and width height info.
        }
        break;

      case WM_KEYDOWN:
        if (VK_ESCAPE != msg.wParam) {
          break;
        }

        //
        // Continue.
        //

      case WM_LBUTTONUP:
      case WM_SYSKEYDOWN:
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
        {
          //
          // Draw again to erase focus rect.
          //

          DrawTrackFocusRect(dc, pt, ptLast);

          //
          // Selection check.
          //

          TrackMoveSelCheck(pt, ptLast);

          //
          // End tracking.
          //

          ReleaseCapture();
        }
        break;

      default:
        ::DispatchMessage(&msg);
        break;
      }
    }

    ReleaseCapture();
  }

  void HandleMouseDown()
  {
    if (-1 != mHot) {

      //
      // Handle mouse down with hot obj.
      //

      if (ClickOnTexResizer()) {
        return;
      }

      //
      // Update current selection list.
      //

      UpdateCurSel();

      //
      // Handle move selected objs.
      //

      DragCurSel();

    } else {

      //
      // Not hold ctrl-key, de-select all.
      //

      if (0 == (::GetKeyState(VK_CONTROL) & 0x8000)) {
        mCurSel.clear();
      }

      SelItemChange(GOOD_RESOURCE_LEVEL, mEditor.mId);

      //
      // Handle tracking select obj.
      //

      TrackSelObj();
    }

    Invalidate(FALSE);
  }

  void PrepareDraw(RECT& rcClip, POINT& ptOffset, POINT& szView, POINT& ptDPoffset) const
  {
    RECT rcClient;
    GetClientRect(&rcClient);

    ptDPoffset.x = 0, ptDPoffset.y = 0;
    int w = rcClient.right, h = rcClient.bottom;

    if (m_sizeAll.cx < rcClient.right) {
      ptDPoffset.x = (rcClient.right - m_sizeAll.cx) / 2;
      w = m_sizeAll.cx;
    }

    if (m_sizeAll.cy < rcClient.bottom) {
      ptDPoffset.y = (rcClient.bottom - m_sizeAll.cy) / 2;
      h = m_sizeAll.cy;
    }

    ptOffset.x = ptDPoffset.x, ptOffset.y = ptDPoffset.y;
    ptOffset.x += m_ptOffset.x;
    ptOffset.y += m_ptOffset.y;

    szView.x = (int)w, szView.y = (int)h;

    if (0 == ptDPoffset.x) {
      ptOffset.x = BKGND_TILE_SIZE * (ptOffset.x / BKGND_TILE_SIZE); // Align to bkgnd tile size to avoid tear.
      szView.x += BKGND_TILE_SIZE;
    }

    if (0 == ptDPoffset.y) {
      ptOffset.y = BKGND_TILE_SIZE * (ptOffset.y / BKGND_TILE_SIZE);
      szView.y += BKGND_TILE_SIZE;
    }

    ::SetRect(&rcClip, 0, 0, szView.x, szView.y);

    ::OffsetRect(&rcClip, 0 != ptDPoffset.x ? 0 : ptOffset.x, 0 != ptDPoffset.y ? 0 : ptOffset.y);
  }

  void SelItemChange(int type, int id)
  {
    //
    // Update property view.
    //

    MainT::inst().mExpView.mProp.SetProperty(id);

    //
    // Update treeview selection.
    //

    mEditor.SelTreeItem(mCurSel);

    ScrollObjToView(id);

    Invalidate(FALSE);
  }

  void TrackMoveSelCheck(POINT a, POINT b)
  {
    //
    // Prepare.
    //

    RECT rcv;
    POINT pt, sz, ptDP;
    PrepareDraw(rcv, pt, sz, ptDP);

    if (0 != ptDP.x) {
      a.x -= pt.x;
      b.x -= pt.x;
    }

    if (0 != ptDP.y) {
      a.y -= pt.y;
      b.y -= pt.y;
    }

    if (a.x > b.x) {
      std::swap(a.x, b.x);
    }

    if (a.y > b.y) {
      std::swap(a.y, b.y);
    }

    a.x += m_ptOffset.x;
    a.y += m_ptOffset.y;
    b.x += m_ptOffset.x;
    b.y += m_ptOffset.y;
    RECT rcRgn = {a.x, a.y, b.x, b.y};

    //
    // Hittest.
    //

    PrjT const& prj = PrjT::inst();
    PrjT::LevelT const& lvl = prj.getLevel(mEditor.mId);
    RECT rcm, rcm2;

    for (int i = (int)lvl.mObjIdx.size() - 1; i >= 0; i--) {

      //
      // Hittest, last draw first hit.
      //

      int id = lvl.mObjIdx[i];
      PrjT::ObjectT const& inst = lvl.getObj(id);

      RECT rc;
      GetObjDim(lvl, inst, rc);

      if (::IntersectRect(&rcm, &rc, &rcv) && ::IntersectRect(&rcm2, &rcm, &rcRgn)) {
        AddCurSel(id);
      }
    }

    if (!mCurSel.empty()) {
      Invalidate(FALSE);
      mEditor.SelTreeItem(mCurSel);
    }
  }

  void DragCurSel()
  {
    SetCapture();

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);

    //
    // Move object(s).
    //

    MainT& mf = MainT::inst();

    PrjT& prj = PrjT::inst();
    PrjT::LevelT& lvl = prj.getLevel(mEditor.mId);

    bool oldDirty = prj.mModified;

    int oldX = -1, oldY = -1;
    POINT ptHitOffset;
    int oldOffsetX, oldOffsetY;

    pt.x += m_ptOffset.x;
    pt.y += m_ptOffset.y;

    {
      PrjT::ObjectT& o = lvl.getObj(mHot);
      ptHitOffset.x = pt.x - o.mPosX, ptHitOffset.y = pt.y - o.mPosY;
    }

    bool dragCopy = false, changed = false;

    while (GetCapture() == m_hWnd) {

      MSG msg;
      if (!GetWindowsMessage(msg)) {
        break;
      }

      bool bSnap = lvl.isShowSnap() && !(::GetKeyState(VK_LCONTROL) & 0x8000);
      int sw = lvl.getSnapWidth(), sh = lvl.getSnapHeight();

      switch (msg.message)
      {
      case WM_MOUSEMOVE:
        {
          if (-1 == oldX && -1 == oldY) {
            PrjT::ObjectT const& o = lvl.getObj(mHot);
            oldX = o.mPosX, oldY = o.mPosY;
            oldOffsetX = oldOffsetY = 0;
            break;
          }

          POINT ptCur, ptOfs;
          GetCursorPos(&ptCur);
          ScreenToClient(&ptCur);
          ptCur.x += m_ptOffset.x;
          ptCur.y += m_ptOffset.y;
          ptOfs.x = ptCur.x - pt.x, ptOfs.y = ptCur.y - pt.y;
          pt = ptCur;

          int newx, newy;
          if (bSnap) {
            newx = (ptCur.x - ptHitOffset.x)/sw*sw;
            newy = (ptCur.y - ptHitOffset.y)/sh*sh;
          } else {
            PrjT::ObjectT const& o = lvl.getObj(mHot);
            newx = o.mPosX + ptOfs.x;
            newy = o.mPosY + ptOfs.y;
          }

          changed = newx != oldX || newy != oldY || dragCopy;

          int offsetX = newx - oldX;
          int offsetY = newy - oldY;

          if (changed) {
            if (!dragCopy && (::GetKeyState(VK_LMENU) & 0x8000)) { // Drag and copy with Alt-key.
              std::vector<int> sel;
              if (lvl.copyObj(mCurSel, sel)) {
                dragCopy = true;
                mCurSel = sel;
                for (size_t i = 0; i < sel.size(); i++) {
                  int id = sel[i];
                  mEditor.AddTreeItem(id);
                  if (mHot == id) {
                    mHot = id;
                    if (1 == sel.size()) {
                      SelItemChange(GOOD_RESOURCE_LEVEL_OBJECT, id);
                    }
                  }
                }
              }
            }

            for (size_t i = 0; i < mCurSel.size(); i++) {
              PrjT::ObjectT& o2 = lvl.getObj(mCurSel[i]);
              o2.setPos(o2.mPosX + offsetX - oldOffsetX, o2.mPosY + offsetY - oldOffsetY);
            }

            Invalidate(FALSE);
          }

          oldOffsetX = offsetX;
          oldOffsetY = offsetY;

          CString s;
          s.Format(_T("%d : %d (%d select)"), -offsetX, -offsetY, mCurSel.size());

          mf.mStatus.SetPaneText(2, s);
        }
        break;

      case WM_KEYDOWN:
        if (VK_ESCAPE == msg.wParam) {

          //
          // End tracking and undo changes.
          //

          if (!dragCopy) {
            PrjT::ObjectT const& o2 = lvl.getObj(mHot);
            int offsetX = oldX - o2.mPosX, offsetY = oldY - o2.mPosY;
            for (size_t i = 0; i < mCurSel.size(); i++) {
              PrjT::ObjectT& o3 = lvl.getObj(mCurSel[i]);
              o3.setPos(o3.mPosX + offsetX, o3.mPosY + offsetY);
            }
            changed = false;
          }
        } else {

          //
          // ignore other keys.
          //

          break;
        }

        //
        // Continue.
        //

      case WM_LBUTTONUP:
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
        goto end;

      case WM_SYSKEYDOWN:
        if (::GetKeyState(VK_LMENU) & 0x8000) {
          break;
        }
        goto end;

      default:
        ::DispatchMessage(&msg);
        break;
      }
    }

end:
    ReleaseCapture();

    if (changed) {

      PrjT::ObjectT& o = lvl.getObj(mHot);

      int offsetX = o.mPosX - oldX;
      int offsetY = o.mPosY - oldY;

      for (size_t i = 0; i < mCurSel.size(); i++) {
        PrjT::ObjectT& o2 = lvl.getObj(mCurSel[i]);
        o2.setPos(o2.mPosX - offsetX, o2.mPosY - offsetY);
      }

      (void)lvl.moveObj(mCurSel, offsetX, offsetY);

      mf.mExpView.UpdateProperty();   // Force update prop view.
      mf.mStatus.SetPaneText(2, _T(""));

    } else {
      prj.mModified = oldDirty;       // Undo prev modify flag.
      Invalidate(FALSE);
    }
  }
};

template<class MainT>
class CLevelEditor : public CWindowImpl<CLevelEditor<MainT> >, public CUpdateUI<CLevelEditor<MainT> >, public CIdleHandler
{
public:
  CGoodPane mEditPane;
  CToolBarCtrl mTBEdit;

  CLevelEditView<MainT,CLevelEditor> mEditView;
  CTreeViewCtrlEx mTree;

  CImageListManaged mImages;

  int mPos;
  CSplitterWindow mSplit;

  int mId;                              // Level ID.

  CLevelEditor(int id) : mEditView(*this), mPos(210), mId(id)
  {
  }

  virtual void OnFinalMessage(HWND)
  {
    //
    // Delete self when window destroy.
    //

    MainT::inst().ResetStatusBar();
    delete this;
  }

  virtual BOOL OnIdle()
  {
    //
    // Update ui states.
    //

    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mId);

    //
    // Update toolbar.
    //

    UISetCheck(ID_LEVELEDIT_MOVEITEM, lvl.isMoveTool());
    UISetCheck(ID_LEVELEDIT_REMOVE, lvl.isRemoveTool());

    UISetCheck(ID_LEVELEDIT_ADDCOLBG, lvl.isAddColorTool());
    UISetCheck(ID_LEVELEDIT_ADDTEXBG, lvl.isAddTexTool());
    UISetCheck(ID_LEVELEDIT_ADDMAPBG, lvl.isAddMapTool());
    UISetCheck(ID_LEVELEDIT_ADDSPRITE, lvl.isAddSpriteTool());
    UISetCheck(ID_LEVELEDIT_ADDDUMMY, lvl.isAddDummyTool());
    UISetCheck(ID_LEVELEDIT_ADDLVLOBJ, lvl.isAddLevelobjTool());

    bool SingleSel = 1 == mEditView.mCurSel.size();
    UIEnable(ID_LEVELEDIT_BOTTOMMOST, SingleSel && lvl.canMoveObjBottommost(mEditView.mCurSel[0]));
    UIEnable(ID_LEVELEDIT_MOVEDOWN, SingleSel && lvl.canMoveObjDown(mEditView.mCurSel[0]));
    UIEnable(ID_LEVELEDIT_MOVEUP, SingleSel && lvl.canMoveObjUp(mEditView.mCurSel[0]));
    UIEnable(ID_LEVELEDIT_TOPMOST, SingleSel && lvl.canMoveObjTopmost(mEditView.mCurSel[0]));

    bool MultiSel = 1 < mEditView.mCurSel.size();
    UIEnable(ID_LEVELEDIT_ALIGNLEFT, MultiSel);
    UIEnable(ID_LEVELEDIT_ALIGNRIGHT, MultiSel);
    UIEnable(ID_LEVELEDIT_ALIGNTOP, MultiSel);
    UIEnable(ID_LEVELEDIT_ALIGNBOTTOM, MultiSel);

    UISetCheck(ID_LEVELEDIT_GRID, lvl.isShowSnap());
    UISetCheck(ID_LEVELEDIT_LINE, lvl.isShowLine());

    UIUpdateToolBar();

    //
    // Update status bar.
    //

    MainT& mt = MainT::inst();

    CString str;
    if (0 != mEditView.m_ptOffset.x || 0 != mEditView.m_ptOffset.y) {
      str.Format(_T("SZ %dx%d LT(%d,%d)"), lvl.mWidth, lvl.mHeight, mEditView.m_ptOffset.x, mEditView.m_ptOffset.y);
    } else {
      str.Format(_T("SZ %dx%d"), lvl.mWidth, lvl.mHeight);
    }
    mt.mStatus.SetPaneText(1, str);

    if (!mEditView.mCurSel.empty()) {
      str.Format(_T("%d select"), mEditView.mCurSel.size());
      mt.mStatus.SetPaneText(2, str);
    } else {
      mt.mStatus.SetPaneText(2, _T(""));
    }

    //
    // Save splitter pos.
    //

    RECT rcClient;
    GetClientRect(&rcClient);

    mPos = rcClient.right - mSplit.GetSplitterPos();

    return FALSE;
  }

  void InitTreeItem()
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mId);

    mTree.DeleteAllItems();

    CTreeItem tiRoot = mTree.GetChildItem(NULL);
    CTreeItem ti = tiRoot.AddTail(lvl.getName().c_str(), 0); // Add level root.
    ti.SetData(mId);

    InitTreeItem(ti, lvl.mObjIdx);      // Add child items.

    tiRoot.GetChild().Expand();         // Expand level node.
  }

  void InitTreeItem(CTreeItem &tnode, const std::vector<int> &v)
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mId);

    for (size_t i = 0; i < v.size(); i++) {
      int idObj = v[i];
      PrjT::ObjectT const &o = lvl.getObj(idObj);
      CTreeItem ti = tnode.AddTail(o.getName().c_str(), o.mType + 1);
      ti.SetData(idObj);
      InitTreeItem(ti, o.mObjIdx);
    }
  }

  CTreeItem FindTreeItemById(int id) const
  {
    return FindTreeItemById(mTree.GetChildItem(NULL), id);
  }

  CTreeItem FindTreeItemById(CTreeItem &ti, int id) const
  {
    CTreeItem hItem = ti.GetChild();
    while (hItem) {
      if (id == (int)hItem.GetData()) {
        return hItem;
      }
      CTreeItem hItem2 = FindTreeItemById(hItem, id);
      if (NULL != hItem2) {
        return hItem2;
      }
      hItem = hItem.GetNextSibling();
    }
    return NULL;
  }

  void SelTreeItem(const std::vector<int> &v)
  {
    mTree.SelectItem(NULL);             // Unselect all.
    for (size_t i = 0; i < v.size(); i++) {
      int idObj = v[i];
      CTreeItem hItem = FindTreeItemById(idObj);
      if (hItem) {
        mTree.SelectItem(hItem);
      }
    }
  }

  void AddTreeItem(const std::vector<int> &v)
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mId);

    CTreeItem trLvl = mTree.GetChildItem(NULL);
    for (size_t i = 0; i < v.size(); ++i) {
      int idObj = v[i];
      PrjT::ObjectT const &o = lvl.getObj(idObj);
      trLvl.AddTail(o.getName().c_str(), o.mType + 1).SetData(idObj);
    }
  }

  void AddTreeItem(int id)
  {
    std::vector<int> v;
    v.push_back(id);
    AddTreeItem(v);
  }

  void DelTreeItem(const std::vector<int> &v)
  {
    for (size_t i = 0; i < v.size(); i++) {
      int idObj = v[i];
      CTreeItem hItem = FindTreeItemById(idObj);
      if (hItem) {
        mTree.DeleteItem(hItem);
      }
    }
  }

  void RefreshAddRemove(bool bRefresh)
  {
    mEditView.Invalidate(FALSE);
    if (bRefresh) {

      //
      // Remove all prop and add again, lvl will be the first visible prop.
      //

      InitTreeItem();
      mEditView.mCurSel.clear();
      MainT::inst().mExpView.Refresh(mId);
    } else {
      MainT::inst().mExpView.UpdateProperty();
    }
  }

  BEGIN_UPDATE_UI_MAP(CLevelEditor)
    UPDATE_ELEMENT(ID_LEVELEDIT_MOVEITEM, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_REMOVE, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ADDCOLBG, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ADDTEXBG, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ADDMAPBG, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ADDSPRITE, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ADDDUMMY, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ADDLVLOBJ, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_TOPMOST, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_MOVEUP, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_MOVEDOWN, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_BOTTOMMOST, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ALIGNLEFT, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ALIGNRIGHT, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ALIGNTOP, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_ALIGNBOTTOM, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_GRID, UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_LEVELEDIT_LINE, UPDUI_TOOLBAR)
  END_UPDATE_UI_MAP()

  BEGIN_MSG_MAP_EX(CLevelEditor)
    MSG_WM_CREATE(OnCreate)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    MSG_WM_SIZE(OnSize)
    MSG_WM_GOOD(OnGoodMessage)
    COMMAND_ID_HANDLER_EX(ID_EDIT_SELECT_ALL, OnEditSelectAll)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_ADDCOLBG, OnAddObjTool)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_ADDTEXBG, OnAddObjTool)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_ADDMAPBG, OnAddObjTool)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_ADDSPRITE, OnAddObjTool)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_ADDDUMMY, OnAddObjTool)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_ADDLVLOBJ, OnAddObjTool)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_GRID, OnToggleGrid)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_LINE, OnToggleLine)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_MOVEITEM, OnMoveTool)
    COMMAND_ID_HANDLER_EX(ID_LEVELEDIT_REMOVE, OnRemoveTool)
    COMMAND_RANGE_HANDLER_EX(ID_SNAP_8, ID_SNAP_64, OnSnapSize)
    COMMAND_RANGE_HANDLER_EX(ID_LEVELEDIT_MOVEUP, ID_LEVELEDIT_BOTTOMMOST, OnChangeItemZorder)
    COMMAND_RANGE_HANDLER_EX(ID_LEVELEDIT_ALIGNLEFT, ID_LEVELEDIT_ALIGNBOTTOM, OnAlignObject)
    COMMAND_ID_HANDLER_EX(ID_LINE_CUSTOMIZE, OnLineCustomize)
    COMMAND_ID_HANDLER_EX(ID_SNAP_CUSTOMIZE, OnSnapCustomize)
    COMMAND_ID_HANDLER_EX(ID_EDIT_GOTO, OnEditGoto)
    NOTIFY_CODE_HANDLER_EX(TBN_DROPDOWN, OnDropDown)
    NOTIFY_CODE_HANDLER_EX(NM_CLICK, OnTreeClk)
    NOTIFY_CODE_HANDLER_EX(TVN_KEYDOWN, OnTreeKeyDown)
    NOTIFY_CODE_HANDLER_EX(TVN_BEGINDRAG, OnTreeBeginDrag)
    CHAIN_MSG_MAP(CUpdateUI<CLevelEditor>)
    FORWARD_TOOLTIP_GETDISPINFO()
  END_MSG_MAP()

  //
  // Message handler.
  //

  LRESULT OnCreate(LPCREATESTRUCT)
  {
    //
    // Create splits and panes.
    //

    mEditPane.Create(m_hWnd);
    mEditPane.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);

    mSplit.Create(mEditPane);
    mEditPane.SetClient(mSplit);

    mTBEdit = GoodCreateSimpleToolbar(mEditPane, IDR_TB_LEVELEDIT);
    mTBEdit.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
    UIAddToolBar(mTBEdit);

    TBBUTTONINFO tbi;
    tbi.cbSize = sizeof(TBBUTTONINFO);
    tbi.dwMask = TBIF_STYLE;

    mTBEdit.GetButtonInfo(ID_LEVELEDIT_GRID, &tbi);
    tbi.fsStyle |= BTNS_DROPDOWN;
    mTBEdit.SetButtonInfo(ID_LEVELEDIT_GRID, &tbi);

    mTBEdit.GetButtonInfo(ID_LEVELEDIT_LINE, &tbi);
    tbi.fsStyle |= BTNS_DROPDOWN;
    mTBEdit.SetButtonInfo(ID_LEVELEDIT_LINE, &tbi);

    //
    // Create views.
    //

    mEditView.Create(mSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_STATICEDGE);
    mTree.Create(mSplit, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);

    mImages.Create(IDB_LVLOBJ, 16, 0, RGB(255,0,255));
    mTree.SetImageList(mImages, TVSIL_NORMAL);

    mSplit.SetSplitterPanes(mEditView, mTree);
    mSplit.SetActivePane(0);

    InitTreeItem();

    SetMsgHandled(FALSE);
    return 0;
  }

  bool IsUndoRedoRequireRefresh(int Cmd) const
  {
    return good::ed::GOOD_LEVELED_CMD_REMOVEOBJ == Cmd ||
           good::ed::GOOD_LEVELED_CMD_ADDOBJ == Cmd ||
           good::ed::GOOD_LEVELED_CMD_CHANGEPARENT == Cmd;
  }

  void UpdateView(int idItem)
  {
    const PrjT::LevelT& lvl = PrjT::inst().getLevel(mId);
    if (mEditView.m_sizeAll.cx != lvl.mWidth || mEditView.m_sizeAll.cy != lvl.mHeight) {
      SIZE sz = {lvl.mWidth, lvl.mHeight};
      mEditView.SetScrollSize(sz);
    }
    if (lvl.isObj(idItem)) {            // Item changed.
      PrjT::ObjectT const& o = lvl.getObj(idItem);
      CTreeItem hTree = FindTreeItemById(idItem);
      hTree.SetText(o.getName().c_str());
    }
    mEditView.Invalidate();
  }

  int OnGoodMessage(int uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (WM_GOOD_GETRESOURCEID == uMsg) {
      return mId;
    }

    PrjT::LevelT& lvl = PrjT::inst().getLevel(mId);

    switch (uMsg)
    {
    case WM_GOOD_CANUNDO:
      return lvl.canUndo();

    case WM_GOOD_UNDO:
      {
        int UndoCmd = lvl.mUndo.getCurCommand()->getId();
        if (lvl.undo()) {
          RefreshAddRemove(IsUndoRedoRequireRefresh(UndoCmd));
          if (good::ed::GOOD_LEVELED_CMD_SETSIZE == UndoCmd) {
            UpdateView(0);
          }
        }
      }
      return true;

    case WM_GOOD_CANREDO:
      return lvl.canRedo();

    case WM_GOOD_REDO:
      if (lvl.redo()) {
        int UndoCmd = lvl.mUndo.getCurCommand()->getId();
        RefreshAddRemove(IsUndoRedoRequireRefresh(UndoCmd));
        if (good::ed::GOOD_LEVELED_CMD_SETSIZE == UndoCmd) {
          UpdateView(0);
        }
      }
      return true;

    case WM_GOOD_UPDATE:
      UpdateView(wParam);
      return 0;

    case WM_GOOD_SETCURSEL:
      lvl.setToolByResId(wParam);
      return 0;
    }

    return 0;
  }

  void OnShowWindow(BOOL bShow, UINT nStatus)
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    if (bShow) {
      pLoop->AddIdleHandler(this);
    } else {
      pLoop->RemoveIdleHandler(this);
    }

    SetMsgHandled(FALSE);
  }

  void OnSize(UINT nType, CSize size)
  {
    mEditPane.SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOACTIVATE | SWP_NOZORDER);
    mSplit.SetSplitterPos(size.cx - mPos);
  }

  //
  // Command handler.
  //

  void OnAddObjTool(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::LevelT &lvl = PrjT::inst().getLevel(mId);
    if (ID_LEVELEDIT_ADDTEXBG == nID) {
      MainT::inst().mExpView.mTabView.SetActivePage(1);
    } else if (ID_LEVELEDIT_ADDMAPBG == nID) {
      MainT::inst().mExpView.mTabView.SetActivePage(3);
    } else if (ID_LEVELEDIT_ADDCOLBG == nID) {
      CColorDialog dlg(lvl.mAddCol, CC_FULLOPEN);
      if (IDOK == dlg.DoModal()) {
        lvl.setAddColorTool(dlg.GetColor());
      }
    } else if (ID_LEVELEDIT_ADDSPRITE == nID) {
      MainT::inst().mExpView.mTabView.SetActivePage(2);
    } else if (ID_LEVELEDIT_ADDDUMMY == nID) {
      lvl.setAddDummyTool();
    } else if (ID_LEVELEDIT_ADDLVLOBJ == nID) {
      CDlgLevelObjPicker dlg;
      if (IDOK == dlg.DoModal()) {
        lvl.setAddLevelObjTool(dlg.mId);
      }
    }
    mEditView.SetFocus();
  }

  void OnAlignObject(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::LevelT &lvl = PrjT::inst().getLevel(mId);
    switch (nID)
    {
    case ID_LEVELEDIT_ALIGNLEFT:
      lvl.alignLeft<ImgT>(mEditView.mCurSel);
      break;
    case ID_LEVELEDIT_ALIGNRIGHT:
      lvl.alignRight<ImgT>(mEditView.mCurSel);
      break;
    case ID_LEVELEDIT_ALIGNTOP:
      lvl.alignTop<ImgT>(mEditView.mCurSel);
      break;
    case ID_LEVELEDIT_ALIGNBOTTOM:
      lvl.alignBottom<ImgT>(mEditView.mCurSel);
      break;
    }
    mEditView.Invalidate();
  }

  void OnEditSelectAll(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mEditView.mCurSel.clear();
    mEditView.mCurSel = PrjT::inst().getLevel(mId).mObjIdx;
    mEditView.SelItemChange(GOOD_RESOURCE_LEVEL, mId);
  }

  void OnChangeItemZorder(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::LevelT &lvl = PrjT::inst().getLevel(mId);
    int id = mEditView.mCurSel[0];
    switch (nID)
    {
    case ID_LEVELEDIT_MOVEUP:
      lvl.moveObjUp(id);
      break;
    case ID_LEVELEDIT_MOVEDOWN:
      lvl.moveObjDown(id);
      break;
    case ID_LEVELEDIT_TOPMOST:
      lvl.moveObjTopmost(id);
      break;
    case ID_LEVELEDIT_BOTTOMMOST:
      lvl.moveObjBottommost(id);
      break;
    }
    mEditView.Invalidate(FALSE);
  }

  void OnEditGoto(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::LevelT const& lvl = PrjT::inst().getLevel(mId);
    if (lvl.mWidth > mEditView.m_sizeClient.cx || lvl.mHeight > mEditView.m_sizeClient.cy) {
      int cx = lvl.mWidth - mEditView.m_sizeClient.cx;
      int cy = lvl.mHeight - mEditView.m_sizeClient.cy;
      CDlgGotoView dlg(cx, cy, mEditView.m_ptOffset.x, mEditView.m_ptOffset.y);
      if (IDOK != dlg.DoModal()) {
        return;
      }
      RECT rcClient;
      mEditView.GetClientRect(&rcClient);
      OffsetRect(&rcClient, dlg.mOffsetX, dlg.mOffsetY);
      mEditView.ScrollToView(rcClient);
    }
  }

  void OnMoveTool(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::inst().getLevel(mId).setMoveTool();
  }

  void OnRemoveTool(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::inst().getLevel(mId).setRemoveTool();
  }

  void OnSnapSize(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    mEditView.mSnapSize = nID - ID_SNAP_8 + 1;
    int sz = mEditView.mSnapSize * EDITOR_SNAP_SCALE;
    PrjT::LevelT &lvl = PrjT::inst().getLevel(mId);
    if (lvl.setSnapSize(sz, sz) && lvl.isShowSnap()) {
      mEditView.Invalidate(FALSE);
    }
  }

  void OnLineCustomize(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    CDlgLevelOption<MainT> dlg(mId);
    if (IDOK == dlg.DoModal()) {
      mEditView.Invalidate(FALSE);
    }
  }

  void OnSnapCustomize(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::LevelT &lvl = PrjT::inst().getLevel(mId);
    CDlgSnapCustomize dlg(lvl.getSnapWidth(), lvl.getSnapHeight());
    if (IDOK == dlg.DoModal()) {
      mEditView.mSnapSize = ID_SNAP_CUSTOMIZE - ID_SNAP_8 + 1;
      if (lvl.setSnapSize(dlg.mWidth, dlg.mHeight) && lvl.isShowSnap()) {
        mEditView.Invalidate(FALSE);
      }
    }
  }

  void OnToggleGrid(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::inst().getLevel(mId).toggleShowSnap();
    mEditView.Invalidate(FALSE);
  }

  void OnToggleLine(UINT uNotifyCode, int nID, CWindow wndCtl)
  {
    PrjT::inst().getLevel(mId).toggleShowLine();
    mEditView.Invalidate(FALSE);
  }

  //
  // Notify handler.
  //

  LRESULT OnDropDown(LPNMHDR pnmh)
  {
    LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR)pnmh;

    CMenu m;
    m.LoadMenu(ID_LEVELEDIT_GRID == lpnmtb->iItem ? (LPCSTR)IDR_MENU_SNAP : (LPCSTR)IDR_MENU_LINE);

    CMenuHandle ms;
    ms = m.GetSubMenu(0);

    if (ID_LEVELEDIT_GRID == lpnmtb->iItem) {
      ms.CheckMenuRadioItem(ID_SNAP_8, ID_SNAP_CUSTOMIZE, ID_SNAP_8 + mEditView.mSnapSize - 1, MF_BYCOMMAND);
    }

    RECT rc;
    ::SendMessage(pnmh->hwndFrom, TB_GETRECT, lpnmtb->iItem, (LPARAM)&rc);
    ::MapWindowPoints(pnmh->hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);
    ::TrackPopupMenu(ms, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, 0, m_hWnd, NULL);

    return 0;
  }

  LRESULT OnTreeClk(LPNMHDR pnmh)
  {
    if (mTree != pnmh->hwndFrom) {
      return 0;
    }

    POINT pt;
    ::GetCursorPos(&pt);
    mTree.ScreenToClient(&pt);

    UINT flag;
    HTREEITEM sel = mTree.HitTest(pt, &flag);
    if (NULL == sel || !(flag & TVHT_ONITEM)) {
      return 0;
    }

    int idObj = (int)mTree.GetItemData(sel);
    if (idObj != mId) {
      mEditView.mCurSel.clear();
      mEditView.mCurSel.push_back(idObj);
      mEditView.Invalidate(FALSE);
      mEditView.ScrollObjToView(idObj);
      mEditView.SetFocus();
    }

    MainT::inst().mExpView.mProp.SetProperty(idObj);

    return 0;
  }

  LRESULT OnTreeKeyDown(LPNMHDR pnmh)
  {
    LPNMTVKEYDOWN ptvkd = (LPNMTVKEYDOWN)pnmh;
    mEditView.OnKeyDown(ptvkd->wVKey, 0, 0);
    mEditView.SetFocus();
    return 0;
  }

  LRESULT OnTreeBeginDrag(LPNMHDR pnmh)
  {
    LPNMTREEVIEW lpnmtv = (LPNMTREEVIEW)pnmh;

    HTREEITEM hParent = mTree.GetParentItem(lpnmtv->itemNew.hItem);
    if (NULL == hParent) {
      return 0;
    }

    RECT rcItem;
    TreeView_GetItemRect(mTree, lpnmtv->itemNew.hItem, &rcItem, TRUE);

    int offsetx = lpnmtv->ptDrag.x - rcItem.left + 18;
    int offsety = lpnmtv->ptDrag.y - rcItem.top;

    int idParent = (int)mTree.GetItemData(hParent);
    int idObj = (int)mTree.GetItemData(lpnmtv->itemNew.hItem);

    RECT rcClient;
    mTree.GetClientRect(&rcClient);

    //
    // Drag loop.
    //

    CImageListManaged img = TreeView_CreateDragImage(mTree, lpnmtv->itemNew.hItem);
    img.BeginDrag(0, 0, 0);
    img.DragEnter(mTree, lpnmtv->ptDrag.x - offsetx, lpnmtv->ptDrag.y - offsety);
    SetCapture();

    LPCTSTR cur = IDC_ARROW;

    HTREEITEM htiTarget = NULL;

    MSG msg;
    while (GetCapture() == m_hWnd) {

      if (!::GetMessage(&msg, NULL, 0, 0)) {
        ::PostQuitMessage(msg.wParam);
        break;
      }

      switch (msg.message)
      {
      case WM_MOUSEMOVE:
        {
          POINT point;
          point.x = LOWORD(msg.lParam);
          point.y = HIWORD(msg.lParam);
          ClientToScreen(&point);
          mTree.ScreenToClient(&point);
          img.DragMove(point.x - offsetx, point.y - offsety);
          img.DragShowNolock(FALSE);

          if (::PtInRect(&rcClient, point)) {
            TVHITTESTINFO tvht;
            tvht.pt.x = point.x;
            tvht.pt.y = point.y;
            if ((htiTarget = TreeView_HitTest(mTree, &tvht)) != NULL) {
              TreeView_SelectDropTarget(mTree, htiTarget);
              if (htiTarget == hParent || htiTarget == lpnmtv->itemNew.hItem) {
                cur = IDC_NO;
              } else {
                cur = IDC_ARROW;
              }
            }
          } else {
            cur = IDC_NO;
          }

          SetCursor(LoadCursor(NULL, cur));

          img.DragShowNolock(TRUE);
        }
        break;

      case WM_KEYDOWN:
        if (VK_ESCAPE != msg.wParam) {
          break;
        }

        //
        // Escape key pressed, end tracking.
        //

      case WM_LBUTTONUP:
        goto end;

      case WM_SYSKEYDOWN:
      case WM_LBUTTONDOWN:
        goto end;
      }

      ::DispatchMessage(&msg);          // Default.
    }
end:
    ReleaseCapture();

    img.EndDrag();                      // Destroy tmp image list.
    TreeView_SelectDropTarget(mTree, NULL);

    if (htiTarget != NULL && IDC_NO != cur) {
      int idTarget = (int)mTree.GetItemData(htiTarget);
      if (PrjT::inst().getLevel(mId).changeParent(idObj, idTarget)) {
        std::vector<int> v;
        v.push_back(idObj);
        InitTreeItem(CTreeItem(htiTarget, &mTree), v); // Re-create tree node of the orig item to new parent.
        TreeView_Expand(mTree, htiTarget, TVE_EXPAND);
        TreeView_DeleteItem(mTree, lpnmtv->itemNew.hItem); // Delete old item.
        mEditView.mCurSel.clear();
        mEditView.Invalidate();
      }
    }

    return 0;
  }
};

// end of EditLevel.h
