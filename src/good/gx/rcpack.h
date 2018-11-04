
//
// rcpack.h
// Rect packing.
//
// Copyright (c) 2013 Waync Cheng.
// All Rights Reserved.
//
// 2013/08/25 Waync created.
//

#pragma once

#include "swGeometry.h"

namespace good {

namespace gx {

struct RectPackNode
{
  sw2::IntRect bound, img;
  RectPackNode *left, *right;

  RectPackNode(int l, int r, int w, int h) : left(0), right(0)
  {
    sw2::IntRect IR(l, r, w, h), ER(0,0,0,0);
    bound = IR;
    img = ER;
  }

  void free()
  {
    if (left) {
      left->free();
    }

    if (right) {
      right->free();
    }

    delete this;
  }

  bool add(sw2::IntRect &r)
  {
    if (left && left->add(r)) {
      return true;
    }

    if (right && right->add(r)) {
      return true;
    }

    if (0 != left || !img.empty()) {
      return false;
    }

    int bw = bound.right, bh = bound.bottom;
    int rw = r.right, rh = r.bottom;

    if (rw > bw || rh > bh) {
      return false;
    }

    if (rw == bw && rh == bh) {
      r.offset(bound.left, bound.top);
      img = r;
      return true;
    }

    if (bw - rw > bh - rh) {
      left = new RectPackNode(bound.left, bound.top, rw, bh);
      right = new RectPackNode(bound.left + rw, bound.top, bw - rw, bh);
    } else {
      left = new RectPackNode(bound.left, bound.top, bw, rh);
      right = new RectPackNode(bound.left, bound.top + rh, bw, bh - rh);
    }

    return left->add(r);
  }

  int size(const RectPackNode *pRect) const
  {
    if (0 == pRect) {
      return 0;
    }

    int nImg = 0;
    if (!pRect->img.empty()) {
      nImg = 1;
    }

    if (pRect->left) {
      nImg += size(pRect->left);
    }

    if (pRect->right) {
      nImg += size(pRect->right);
    }

    return nImg;
  }
};

} // namespace gx

} // namespace good

// end of rpack.h
