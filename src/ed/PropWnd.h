
//
// PropWnd.h
// Good editor, custom property view item impl.
//
// Copyright (c) 2008 Waync Cheng.
// All Rights Reserved.
//
// 2008/08/16 Waync created.
//

#pragma once

template<class T, class TBase>
class CPropertyDotDotDotWindow :
  public CWindowImpl<CPropertyDotDotDotWindow<T, TBase>, TBase>
{
public:

  CButton m_wndButton;
  IProperty* m_prop;

  virtual void OnFinalMessage(HWND /*hWnd*/)
  {
    delete this;
  }

  BEGIN_MSG_MAP(CPropertyDotDotDotWindow)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
    COMMAND_CODE_HANDLER(BN_CLICKED, OnButtonClicked)
  END_MSG_MAP()

  LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
  {
    LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
    RECT rcClient;
    GetClientRect(&rcClient);
    int cy = rcClient.bottom - rcClient.top;
    SetFont(GetParent().GetFont());

    //
    // Create button.
    //

    RECT rcButton = {rcClient.right - cy, rcClient.top - 1, rcClient.right, rcClient.bottom};

    m_wndButton.Create(m_hWnd, &rcButton, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON | BS_OWNERDRAW);
    ATLASSERT(m_wndButton.IsWindow());
    m_wndButton.SetFont(GetFont());

    return lRes;
  }

  //
  // Owner draw button.
  //

  LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
  {
    LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
    if( m_wndButton != lpdis->hwndItem ) return 0;
    CDCHandle dc(lpdis->hDC);

    //
    // Paint as ellipsis button.
    //

    dc.FillRect(&lpdis->rcItem, (HBRUSH)(COLOR_BTNFACE + 1));
    ::InflateRect(&lpdis->rcItem, -1, -1);
    dc.DrawEdge(&lpdis->rcItem, (lpdis->itemState & ODS_SELECTED) ? BDR_SUNKENOUTER : BDR_RAISEDOUTER, BF_RECT);
    dc.SetBkMode(TRANSPARENT);
    LPCTSTR pstrEllipsis = _T("...");
    ::InflateRect(&lpdis->rcItem, 1, 1);
    dc.DrawText(pstrEllipsis, ::lstrlen(pstrEllipsis), &lpdis->rcItem, DT_CENTER | DT_EDITCONTROL | DT_SINGLELINE | DT_VCENTER);

    return 0;
  }

  LRESULT OnButtonClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  {
    ATLASSERT(m_prop);

    //
    // Call Property class' implementation of BROWSE action.
    //

    if (m_prop->Activate(PACT_BROWSE, (LPARAM)((T*)this))) {
      Invalidate();
    }

    return 0;
  }
};

class CPropertyColorPickerWindow :
  public CPropertyDotDotDotWindow<CPropertyColorPickerWindow, CStatic>
{
public:

  COLORREF m_clrBack;

  typedef CPropertyDotDotDotWindow<CPropertyColorPickerWindow,CStatic> BaseClass;

  BEGIN_MSG_MAP(CPropertyColorPickerWindow)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    CHAIN_MSG_MAP(BaseClass)
  END_MSG_MAP()

  LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
  {
    CPaintDC dc(m_hWnd);
    RECT rc;
    GetClientRect(&rc);
    rc.right -= rc.bottom - rc.top;
    dc.FillRect(&rc, (HBRUSH)(COLOR_WINDOW+1));
    ::InflateRect(&rc, -2, 0);
    rc.top ++;
    dc.DrawFocusRect(&rc);
    ::InflateRect(&rc, -1, -1);
    dc.FillSolidRect(&rc, m_clrBack & 0xFFFFFF);
    return 0;
  }
};

class CPropertyColorPickerItem : public CPropertyItem
{
public:
  enum {
    s_cxIndent = 2,
    s_cyIndent = 1,
    KIND = 100
  };

  COLORREF m_clrBack;
  IResourceProperty* mProp;

  CPropertyColorPickerItem(LPCTSTR pstrName, COLORREF color, IResourceProperty* prop)
    : CPropertyItem(pstrName, 0), m_clrBack(color), mProp(prop)
  {
  }

  BYTE GetKind() const
  {
    return KIND;
  }

  HWND CreateInplaceControl(HWND hWnd, const RECT& rc)
  {
    CPropertyColorPickerWindow* win = new CPropertyColorPickerWindow();
    ATLASSERT(win);
    RECT rcWin = rc;
    win->m_prop = this;
    win->m_clrBack = m_clrBack;
    win->Create(hWnd, rcWin);
    ATLASSERT(win->IsWindow());
    return *win;
  }

  BOOL SetValue(const VARIANT& value)
  {
    m_clrBack = value.lVal;
    return TRUE;
  }

  BOOL SetValue(HWND /*hWnd*/)
  {
    return TRUE;
  }

  BOOL Activate(UINT action, LPARAM lParam)
  {
    switch (action)
    {
    case PACT_BROWSE:
    case PACT_DBLCLICK:
      {
        CColorDialog cc(GetBkColor(), CC_FULLOPEN);
        if (IDOK != cc.DoModal()) {
          return FALSE;
        }

        if (mProp->SetColor(cc.GetColor())) {
          m_clrBack = cc.GetColor();
          ((CPropertyColorPickerWindow*)lParam)->m_clrBack = cc.GetColor();
          NMPROPERTYITEM nmh = {m_hWndOwner, ::GetDlgCtrlID(m_hWndOwner), PIN_BROWSE, this};
          ::SendMessage(::GetParent(m_hWndOwner), WM_NOTIFY, nmh.hdr.idFrom, (LPARAM)&nmh);
        }
      }
      break;
    }

    return TRUE;
  }

  BOOL GetDisplayValue(LPTSTR /*pstr*/, UINT /*cchMax*/) const
  {
    return FALSE;
  }

  UINT GetDisplayValueLength() const
  {
    return 0;
  }

  void DrawValue(PROPERTYDRAWINFO& di)
  {
    RECT rc = di.rcItem;
    CDCHandle dc(di.hDC);

    dc.FillRect(&rc, (HBRUSH)(COLOR_WINDOW+1));
    ::InflateRect(&rc, -2, -2);
    rc.top ++;
    dc.DrawFocusRect(&rc);
    ::InflateRect(&rc, -1, -1);
    dc.FillSolidRect(&rc, m_clrBack & 0xFFFFFF);
  }

  COLORREF GetBkColor() const
  {
    return m_clrBack & 0xFFFFFF;
  }

  COLORREF SetBkColor(COLORREF clrBack)
  {
    COLORREF clrOld = m_clrBack;
    m_clrBack = clrBack;
    return clrOld;
  }
};

HPROPERTY PropCreateColorPicker(LPCTSTR pstrName, COLORREF color, IResourceProperty* prop)
{
  return new CPropertyColorPickerItem(pstrName, color, prop);
}

// end of PropWnd.h
