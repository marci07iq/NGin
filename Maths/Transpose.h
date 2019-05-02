#pragma once

#include "Point.h"

class Transform {
public:
  Matrix4<float> matrix;

  void setIdentity();
  void scale(fVec3 scale);
  void translate(fVec3 translate);

  void createLook(mVec3 from, sVec3 dir);
  void createLook(mVec3 from, sVec3 dir, sVec3 up);
  void project(float fov, float aspectRatio, float zFar, float zNear);
  // set the OpenGL orthographic projection matrix
  void ortho(float left, float right, float top, float bottom, float zFar, float zNear);

  void read(float* to);
  void read(double* to);
  void transpose();

  void applyTransform(Transform& other);

  Transform();
};