#include "Button.h"

int Button::mouseEnter(int state) {
  bool oactive = active;
  active &= state; //if mouse leaves, deactivate.
  return oactive xor active; //rerender if changed
}
int Button::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  bool oactive = active;
  active = isIn(mx, my);
  return oactive xor active; //if state changed
}

int Button::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  if (isIn(mx, my) && evt._key._type == key::type_mouse && evt._type == gui_event::evt_pressed && evt._key._keycode == 0) { //mouse, click, left
    clickCallback(name);
    return 3;
  }
  if (triggerkey == evt._key && evt._type == evt.evt_pressed) {
    clickCallback(name);
    return 3;
  }
  return 0;
}
void Button::render(set<key_location>& down) {

  glBegin(GL_QUADS);
  if (active) {
    setColor(activeColor);
  }
  else {
    setColor(bgColor);
  }
  glVertex2d(cax, cay);
  glVertex2d(cbx, cay);
  glVertex2d(cbx, cby);
  glVertex2d(cax, cby);
  glEnd();
  
  renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, text, textColor, true);
  //shapesPrintf(0, 0, text.c_str());
}

Button::~Button() {

}