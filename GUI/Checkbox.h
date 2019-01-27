#pragma once

#include "GUIElement.h"

class Checkbox : public GUIElement{
public:
  bool checked;
  CheckCallback checkCallback;
  void* data;
  Checkbox(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, bool lchecked, CheckCallback lcheckCallback, void* ldata) :
  GUIElement(lname, llocation, lbg, lactive, ltextColor) {
    checked = lchecked;
    checkCallback = lcheckCallback;
    data = ldata;
  }
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);
  ~Checkbox();
};
