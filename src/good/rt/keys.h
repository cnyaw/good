
//
// keys.h
// Good key code defines.
//
// Copyright (c) 2020 Waync Cheng.
// All Rights Reserved.
//
// 2020/1/14 Waync created.
//

#pragma once

enum GOOD_KEYS
{
  GOOD_KEYS_UP          = 1,
  GOOD_KEYS_DOWN        = 1 << 1,
  GOOD_KEYS_LEFT        = 1 << 2,
  GOOD_KEYS_RIGHT       = 1 << 3,
  GOOD_KEYS_RETURN      = 1 << 4,
  GOOD_KEYS_BTN_A       = 1 << 5,
  GOOD_KEYS_BTN_B       = 1 << 6,
  GOOD_KEYS_LBUTTON     = 1 << 7,
  GOOD_KEYS_RBUTTON     = 1 << 8,
  GOOD_KEYS_ESCAPE      = 1 << 9,
  GOOD_KEYS_ANY         = 0xffffffff
};

#ifdef WIN32
int GetWin32ToGoodKeyStates()
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
#endif

// end of keys.h
