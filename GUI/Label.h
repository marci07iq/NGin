#pragma once

#include "GUIElement.h"


namespace NGin {
  namespace Graphics {

    class Label : public GUIElement {
    public:
      string text;
      int align;
      Label(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata, string ltext, int lalign) :
        GUIElement(lname, llocation, lbg, lactive, ltextColor, ldata) {
        text = ltext;
        align = lalign;
      }
      void render(set<key_location>& down);
      virtual ~Label();
    };
  }
}