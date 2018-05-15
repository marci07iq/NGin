#pragma once

#include "GUIElement.h"

class Label : public GUIElement {
public:
  string text;
  int align;
  Label(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, string ltext, int lalign) :
    GUIElement(lname, llocation, lbg, lactive, ltextColor) {
    text = ltext;
    align = lalign;
  }
  void render(set<key_location>& down);
  ~Label();
};
