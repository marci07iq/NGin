#pragma once

#include "GUIElement.h"

class ControlSetting : public GUIElement {
public:
  key_config selected;
  int id;
  ControlInputFunc input;
  ControlSetting(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, key_config lselected, int lid, ControlInputFunc linput) :
    GUIElement(lname, llocation, lbg, lactive, ltextColor) {
    selected = lselected;
    input = linput;
    id = lid;
  }
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);
  ~ControlSetting();
};
