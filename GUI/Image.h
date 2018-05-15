#pragma once

#include "GUIElement.h"

class Image : public GUIElement {
public:
  GLuint texture;
  string filename;
  int align;
  int w, h;
  Image(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, string lfilename, int lalign) :
    GUIElement(lname, llocation, lbg, lactive, ltextColor) {
    filename = lfilename;
    texture = 0;
    align = lalign;
  }
  void render(set<key_location>& down);
  ~Image() {
    glDeleteTextures(1, &texture);
  }
};
