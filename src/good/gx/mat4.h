//
// mat4.h
// 4x4 matrix transformation.
//
// Copyright (c) 2016 Waync Cheng.
// All Rights Reserved.
//
// 2016/9/14 Waync created.
//

#pragma once

#include <math.h>

namespace good {

namespace gx {

class Mat4
{
public:
  float matrix[16];

  Mat4()
  {
  }

  void swap(int i, int j)
  {
    float tmp = matrix[i];
    matrix[i] = matrix[j];
    matrix[j] = tmp;
  }

  void transpose()
  {
    swap( 1,  4);
    swap( 2,  8);
    swap( 3, 12);
    swap( 6,  9);
    swap( 7, 13);
    swap(11, 14);
  }

  void setIdentity()
  {
    matrix[ 0] = 1.0f; matrix[ 1] = 0.0f; matrix[ 2] = 0.0f; matrix[ 3] = 0.0f;
    matrix[ 4] = 0.0f; matrix[ 5] = 1.0f; matrix[ 6] = 0.0f; matrix[ 7] = 0.0f;
    matrix[ 8] = 0.0f; matrix[ 9] = 0.0f; matrix[10] = 1.0f; matrix[11] = 0.0f;
    matrix[12] = 0.0f; matrix[13] = 0.0f; matrix[14] = 0.0f; matrix[15] = 1.0f;
  }

  void postMultiply(const Mat4 &transform)
  {
    const float *m = transform.matrix;
    float ms[16];
    memcpy(ms, matrix, sizeof(ms));

    matrix[ 0] = ms[ 0] * m[ 0] + ms[ 1] * m[ 4] + ms[ 2] * m[ 8] + ms[ 3] * m[12];
    matrix[ 1] = ms[ 0] * m[ 1] + ms[ 1] * m[ 5] + ms[ 2] * m[ 9] + ms[ 3] * m[13];
    matrix[ 2] = ms[ 0] * m[ 2] + ms[ 1] * m[ 6] + ms[ 2] * m[10] + ms[ 3] * m[14];
    matrix[ 3] = ms[ 0] * m[ 3] + ms[ 1] * m[ 7] + ms[ 2] * m[11] + ms[ 3] * m[15];

    matrix[ 4] = ms[ 4] * m[ 0] + ms[ 5] * m[ 4] + ms[ 6] * m[ 8] + ms[ 7] * m[12];
    matrix[ 5] = ms[ 4] * m[ 1] + ms[ 5] * m[ 5] + ms[ 6] * m[ 9] + ms[ 7] * m[13];
    matrix[ 6] = ms[ 4] * m[ 2] + ms[ 5] * m[ 6] + ms[ 6] * m[10] + ms[ 7] * m[14];
    matrix[ 7] = ms[ 4] * m[ 3] + ms[ 5] * m[ 7] + ms[ 6] * m[11] + ms[ 7] * m[15];

    matrix[ 8] = ms[ 8] * m[ 0] + ms[ 9] * m[ 4] + ms[10] * m[ 8] + ms[11] * m[12];
    matrix[ 9] = ms[ 8] * m[ 1] + ms[ 9] * m[ 5] + ms[10] * m[ 9] + ms[11] * m[13];
    matrix[10] = ms[ 8] * m[ 2] + ms[ 9] * m[ 6] + ms[10] * m[10] + ms[11] * m[14];
    matrix[11] = ms[ 8] * m[ 3] + ms[ 9] * m[ 7] + ms[10] * m[11] + ms[11] * m[15];

    matrix[12] = ms[12] * m[ 0] + ms[13] * m[ 4] + ms[14] * m[ 8] + ms[15] * m[12];
    matrix[13] = ms[12] * m[ 1] + ms[13] * m[ 5] + ms[14] * m[ 9] + ms[15] * m[13];
    matrix[14] = ms[12] * m[ 2] + ms[13] * m[ 6] + ms[14] * m[10] + ms[15] * m[14];
    matrix[15] = ms[12] * m[ 3] + ms[13] * m[ 7] + ms[14] * m[11] + ms[15] * m[15];
  }

  void postScale(float sx, float sy, float sz)
  {
    Mat4 s;
    s.setIdentity();
    s.matrix[ 0] = sx;
    s.matrix[ 5] = sy;
    s.matrix[10] = sz;
    postMultiply(s);
  }

  void postRotate(float angle, float ax, float ay, float az)
  {
    if (0.0f != angle && 0.0f == ax && 0.0f == ay && 0.0f == az) {
      return;
    }

    float f = 1.0f / (float)sqrt(ax * ax + ay * ay + az * az); // Normalize.
    ax *= f;
    ay *= f;
    az *= f;

    float fHalfAngle = 0.5f * angle * (3.1415925f / 180.0f);
    float fSin = (float)sin(fHalfAngle);
    float qw = (float)cos(fHalfAngle);
    float qx = fSin * ax;
    float qy = fSin * ay;
    float qz = fSin * az;
    postRotateQuat(qx, qy, qz, qw);
  }

  void postRotateQuat(float qx, float qy, float qz, float qw)
  {
    if (0.0f == qx && 0.0f == qy && 0.0f == qz && 0.0f == qw) {
      return;
    }

    float f = 1.0f / (float)sqrt(qx * qx + qy * qy + qz * qz + qw * qw);
    qx *= f;
    qy *= f;
    qz *= f;
    qw *= f;

    if (0.0f == qx && 0.0f == qy && 0.0f == qz) {
      qw = 1.0f;
    }

    float xx = 2.0f * qx * qx, yy = 2.0f * qy * qy, zz = 2.0f * qz * qz;
    float xy = 2.0f * qx * qy, xz = 2.0f * qx * qz, xw = 2.0f * qx * qw;
    float yz = 2.0f * qy * qz, yw = 2.0f * qy * qw, zw = 2.0f * qz * qw;

    Mat4 r;
    r.matrix[ 0] = 1.0f - (yy + zz);
    r.matrix[ 1] = (xy - zw);
    r.matrix[ 2] = (xz + yw);
    r.matrix[ 3] = 0.0f;
    r.matrix[ 4] = (xy + zw);
    r.matrix[ 5] = 1.0f - (xx + zz);
    r.matrix[ 6] = (yz - xw);
    r.matrix[ 7] = 0.0f;
    r.matrix[ 8] = (xz - yw);
    r.matrix[ 9] = (yz + xw);
    r.matrix[10] = 1.0f - (xx + yy);
    r.matrix[11] = 0.0f;
    r.matrix[12] = 0.0f;
    r.matrix[13] = 0.0f;
    r.matrix[14] = 0.0f;
    r.matrix[15] = 1.0f;

    postMultiply(r);
  }

  void postTranslate(float tx, float ty, float tz)
  {
    Mat4 t;
    t.setIdentity();
    t.matrix[ 3] = tx;
    t.matrix[ 7] = ty;
    t.matrix[11] = tz;
    postMultiply(t);
  }

  void transform(int n, const float *v3, float *outv3) const
  {
    for (int i = 0; i < n; i++, v3 += 3, outv3 += 3) {
      outv3[0] = matrix[ 0] * v3[0] + matrix[ 1] * v3[1] + matrix[ 2] * v3[2] + matrix[ 3];
      outv3[1] = matrix[ 4] * v3[0] + matrix[ 5] * v3[1] + matrix[ 6] * v3[2] + matrix[ 7];
      outv3[2] = matrix[ 8] * v3[0] + matrix[ 9] * v3[1] + matrix[10] * v3[2] + matrix[ 11];
    }
  }
};

} // namespace gx

} // namespace good

// end of mat4.h
