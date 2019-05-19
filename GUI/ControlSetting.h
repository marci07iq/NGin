#pragma once

#include "GUIElement.h"

namespace NGin {
  namespace Graphics {
    class ControlSetting : public GUIElement {
    public:
      key_config selected;
      int id;
      ControlInputFunc input;
      ControlSetting(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata, key_config lselected, int lid, ControlInputFunc linput) :
        GUIElement(lname, llocation, lbg, lactive, ltextColor, ldata) {
        selected = lselected;
        input = linput;
        id = lid;
      }
      int guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down);
      void render(set<key_location>& down);
      virtual ~ControlSetting();
    };
  }
}