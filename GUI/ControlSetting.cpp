#include "ControlSetting.h"

int ControlSetting::guiEvent(gui_event& evt, int mx, int my, set<key_location>& down) {
  bool oactive = active;
  if(evt._key._type == key::type_mouse) { //mouse
    if(!evt.captured && evt._type == gui_event::evt_pressed && evt._key._keycode == 0) { //click, left
      active = isIn(mx, my);
      if(active) {
        evt.captured = true;
        selected._parts.clear();
        input(this, data, selected);
      }
    }
    return oactive xor active;
  }
  if (!evt.captured && active && (evt._key._type == key::type_key)) { //keyboard
    selected.addKey(evt._key, evt._type);
    input(this, data, selected);
    evt.captured = true;
    return 1;
  }
  return 0;
}

void ControlSetting::render(set<key_location>& down) {
  Gll::gllBegin(Gll::GLL_QUADS);
  if (active) {
    setColor(activeColor);
  }
  else {
    setColor(bgColor);
  }
  Gll::gllVertex(cax, cay);
  Gll::gllVertex(cbx, cay);
  Gll::gllVertex(cbx, cby);
  Gll::gllVertex(cax, cby);
  Gll::gllEnd();

  renderBitmapString((cax+cbx) / 2.0f, (cay + cby) / 2.0f, selected.toName(), textColor, true);
  //shapesPrintf(0, 0, text.c_str());
}

ControlSetting::~ControlSetting() {

}