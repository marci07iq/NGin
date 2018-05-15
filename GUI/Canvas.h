#pragma once

#include "GUIElement.h"

class Canvas : public GUIElement {
public:
  IWindowManagers managers;
  Canvas(string lname, LocationData llocation, IWindowManagers lmanagers) :
    GUIElement(lname, llocation, 0, 0, 0) {
    managers = lmanagers;
  }
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);
  ~Canvas();
};
