#pragma once

#include "GUIElement.h"

namespace NGin {
  namespace Graphics {
    class Canvas : public GUIElement {
    public:
      IWindowManagers managers;
      Canvas(string lname, LocationData llocation, IWindowManagers lmanagers, void* ldata) :
        GUIElement(lname, llocation, 0, 0, 0, ldata) {
        managers = lmanagers;
      }
      virtual int mouseEnter(int state);
      virtual int mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down);
      virtual int guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down);
      virtual void render(set<key_location>& down);
      virtual ~Canvas();
    };
  }
}