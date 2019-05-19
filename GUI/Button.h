#pragma once

#include "GUIElement.h"

namespace NGin {
  namespace Graphics {
    class Button : public GUIElement {
    public:
      int triggerId;
      string text;
      ClickCallback clickCallback;
      bool stuck = false;
      Button(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata, string ltext, int ltriggerId, ClickCallback lclickCallback);
      int mouseEnter(int state);
      int mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down);
      int guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down);
      virtual void render(set<key_location>& down);
      ~Button();
    };

    class IconButton : public Button {
    public:
      Icon* ic;
      IconButton(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata, string ltext, int ltriggerId, ClickCallback lclickCallback, string icon, string ilfFilepath);
      void render(set<key_location>& down);
    };
  }
}