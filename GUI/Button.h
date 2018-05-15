#pragma once

#include "GUIElement.h"

class Button : public GUIElement{
public:
  key triggerkey;
  string text;
  ClickCallback clickCallback;
  Button(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, string ltext, key ltriggerkey, ClickCallback lclickCallback) :
  GUIElement(lname, llocation, lbg, lactive, ltextColor) {
    text = ltext;
    clickCallback = lclickCallback;
    triggerkey = ltriggerkey;
  }
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);
  ~Button();
};
