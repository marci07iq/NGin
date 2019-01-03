#include "Checkbox.h"

int Checkbox::mouseEnter(int state) {
  bool oactive = active;
  active &= state; //if mouse leaves, deactivate.
  return oactive xor active; //rerender if changed
}
int Checkbox::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  bool oactive = active;
  active = isIn(mx, my);
  return oactive xor active; //if state changed
}

int Checkbox::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  if (isIn(mx, my) && evt._key._type == key::type_mouse && evt._type == gui_event::evt_pressed && evt._key._keycode == 0) { //mouse, click, left
    checked = !checked;
    checkCallback(this, checked);
    return 3;
  }
  return 0;
}
void Checkbox::render(set<key_location>& down) {

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
  
  renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, (checked ? "[X]" : "[ ]"), textColor, true);
  //shapesPrintf(0, 0, text.c_str());
}

Checkbox::~Checkbox() {

}