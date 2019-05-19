#include "Button.h"

namespace NGin {
  namespace Graphics {
    Button::Button(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata, string ltext, int ltriggerId, ClickCallback lclickCallback) :
      GUIElement(lname, llocation, lbg, lactive, ltextColor, ldata) {
      text = ltext;
      clickCallback = lclickCallback;
      triggerId = ltriggerId;
    }

    int Button::mouseEnter(int state) {
      bool oactive = active;
      active &= state; //if mouse leaves, deactivate.
      return oactive xor active; //rerender if changed
    }
    int Button::mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down) {
      bool oactive = active;
      active = isIn(mx, my);
      return oactive xor active; //if state changed
    }

    int Button::guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down) {
      if (!evt.captured && isIn(mx, my) && evt._key._type == key::type_mouse && evt._type == gui_event::evt_pressed && evt._key._keycode == 0) { //mouse, click, left
        evt.captured = true;
        clickCallback(shared_from_this());
        return 1;
      }
      if (!evt.captured && evt._key._type == key::type_key && evt._type == gui_event::evt_down) {
        if (triggerId != -1 && checkKey(triggerId, down, evt._key, true)) {
          evt.captured = true;
          clickCallback(shared_from_this());
          return 1;
        }
      }
      return 0;
    }
    void Button::render(set<key_location>& down) {

      Gll::gllBegin(Gll::GLL_QUADS);
      if (active || stuck) {
        setColor(activeColor);
      } else {
        setColor(bgColor);
      }
      Gll::gllVertex(cax, cay);
      Gll::gllVertex(cbx, cay);
      Gll::gllVertex(cbx, cby);
      Gll::gllVertex(cax, cby);
      Gll::gllEnd();

      renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, text, textColor, true);
      //shapesPrintf(0, 0, text.c_str());
    }

    Button::~Button() {

    }

    IconButton::IconButton(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata, string ltext, int ltriggerId, ClickCallback lclickCallback, string icon, string ilfFilepath) :
      Button(lname, llocation, lbg, lactive, ltextColor, ldata, ltext, ltriggerId, lclickCallback) {
      ic = getIcon(icon, ilfFilepath);
    }

    void IconButton::render(set<key_location>& down) {

      if (active || stuck) {
        setColor(activeColor);
      } else {
        setColor(bgColor);
      }
      Gll::gllBegin(Gll::GLL_QUADS);

      Gll::gllVertex(cax, cay);
      Gll::gllVertex(cbx, cay);
      Gll::gllVertex(cbx, cby);
      Gll::gllVertex(cax, cby);
      Gll::gllEnd();

      setColor(textColor);


      if (ic == NULL) {
        renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, text, textColor, true);
      } else {
        Gll::gllIcon(ic, cax, cay, cbx, cby);
      }
      //shapesPrintf(0, 0, text.c_str());
    }
  }
}