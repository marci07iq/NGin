#include "Image.h"

void Image::render(set<key_location>& down) {
  if (texture == 0) {
    texture = png_texture_load(filename.c_str(), w, h);
  }
  int rax, rbx, ray, rby;

  switch (align) {
  case 0: //Fill
    rax = cax;
    rbx = cbx;
    ray = cay;
    rby = cby;
    break;
  case 1: //Enclose, lock aspect
    rax = max(cax, (cax + cbx - ((cby - cay)*w) / h) / 2);
    rbx = min(cbx, (cax + cbx + ((cby - cay)*w) / h) / 2);
    ray = max(cay, (cay + cby - ((cbx - cax)*h) / w) / 2);
    rby = min(cby, (cay + cby + ((cbx - cax)*h) / w) / 2);
    break;
  case 2: //Center, lock size
    rax = (cax + cbx - w) / 2;
    rbx = (cax + cbx + w) / 2;
    ray = (cay + cby - h) / 2;
    rby = (cay + cby + h) / 2;
    break;
  }
  setColor(0xffffffff);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  Gll::gllBegin(Gll::GLL_QUADS);
  glTexCoord2f(0, 1);
  Gll::gllVertex(rax, ray);
  glTexCoord2f(1, 1);
  Gll::gllVertex(rbx, ray);
  glTexCoord2f(1, 0);
  Gll::gllVertex(rbx, rby);
  glTexCoord2f(0, 0);
  Gll::gllVertex(rax, rby);
  Gll::gllEnd();
}