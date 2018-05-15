#pragma once

#include "GUIElement.h"

class LabelBind : public GUIElement {
public:
  TextBindCore* text;
  int align;
  ClickCallback clickCallback;
  LabelBind(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltext, int lalign) :
    GUIElement(lname, llocation, lbg, lactive, ltext) {
    align = lalign;
  }
  void render(set<key_location>& down);
  ~LabelBind();
};
