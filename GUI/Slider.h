#pragma once

#include "GUIElement.h"


namespace NGin {
  namespace Graphics {

    class Slider : public GUIElement {
    private:
      void mouseAt(int x, int y);
      bool mdown = false;
    public:
      string text;
      SliderInputFunc clickCallback;
      colorargb pulledcolor;
      float minv;
      float maxv;
      float val;
      float quanta;
      int cursor = -1;
      bool textActive = false;
      Slider(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata, colorargb lpulledcolor, float lmin, float lmax, float lvalue, float lquanta, SliderInputFunc lclickCallback) :
        GUIElement(lname, llocation, lbg, lactive, ltextColor, ldata) {
        minv = lmin;
        maxv = lmax;
        val = lvalue;
        quanta = lquanta;
        clickCallback = lclickCallback;
        pulledcolor = lpulledcolor;
      }
      void setVal(float val);
      int mouseEnter(int state);
      int mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down);
      int guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down);
      void render(set<key_location>& down);
      virtual ~Slider();
    };
  }
}