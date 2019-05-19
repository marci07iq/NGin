#include "Transpose.h"

void Transform::setIdentity() {
  matrix.setIdentity();
}

void Transform::scale(fVec3 scale) {
  Matrix4f with;

  with.at(0, 0) = scale.x;
  with.at(1, 1) = scale.y;
  with.at(2, 2) = scale.z;
  with.at(3, 3) = 1;
  matrix = with*matrix;
}
void Transform::translate(fVec3 translate) {
  Matrix4f with;
  with.setIdentity();

  with.at(0, 3) = translate.x;
  with.at(1, 3) = translate.y;
  with.at(2, 3) = translate.z;

  matrix = with*matrix;
}
void Transform::createLook(mVec3 from, sVec3 dir) {
  polar_vec3 p;
  p.fromCartesian(dir);

  float sinp = sin(p.phi);
  float cosp = cos(p.phi);
  float sint = sin(p.theta);
  float cost = cos(p.theta);

  setIdentity();
  translate(-from);
  Matrix4f with;
  float withf[16] = {
    sinp, -cosp, 0, 0,
    -cosp*cost, -sinp*cost, sint, 0,
    -cosp * sint, -sinp*sint, -cost, 0,
    0, 0, 0, 1 };
  for(int i = 0; i < 16; i++) {
    with._vals[i] = withf[i];
  }
  matrix = with*matrix;
}

void Transform::createLook(mVec3 from, sVec3 dir, sVec3 up) {
  dir.norm();
  up.norm();
  sVec3 right = crs(dir, up).norm();

  translate(-from);
  Matrix4f with_d;
  float with_df[16] = {
    right.x, right.y, right.z, 0,
    up.x, up.y, up.z, 0,
    -dir.x, -dir.y, -dir.z, 0,
    0,       0,    0,     1
  };
  with_d.set(with_df);

  matrix = with_d * matrix;
}

void Transform::project(float fov, float aspectRatio, float zFar, float zNear) {
  float yScale = 1 / tan(fov / 2);
  float xScale = yScale / aspectRatio;
  Matrix4f with;
  float withf[16] = {
    xScale, 0, 0, 0,
    0, yScale, 0, 0,
    0, 0, -(zFar + zNear) / (zFar - zNear), -2 * zNear*zFar / (zFar - zNear),
    0, 0, -1, 0 };
  for (int i = 0; i < 16; i++) {
    with._vals[i] = withf[i];
  }
  matrix = with * matrix;
}

// set the OpenGL orthographic projection matrix
void Transform::ortho(float left, float right, float top, float bottom, float zFar, float zNear) {
  // set OpenGL perspective projection matrix
  Matrix4f with;
  float withf[16] = {
    2 / (right - left),0,0,-(right + left) / (right - left),
    0,2 / (top - bottom),0,-(top + bottom) / (top - bottom),
    0,0,-2 / (zFar - zNear),-(zFar + zNear) / (zFar - zNear),
    0,0,0,1
  };
  for (int i = 0; i < 16; i++) {
    with._vals[i] = withf[i];
  }
  matrix = with*matrix;
}

void Transform::read(float * to) {
  for (int i = 0; i < 16; i++) {
    to[i] = matrix.at(i % 4, i / 4);
  }
}
void Transform::read(double * to) {
  for (int i = 0; i < 16; i++) {
    to[i] = matrix.at(i % 4, i / 4);
  }
}

void Transform::transpose() {
  matrix.transpose();
}

void Transform::applyTransform(Transform& other) {
  matrix = other.matrix * matrix;
}

Transform::Transform() {
  setIdentity();
}
