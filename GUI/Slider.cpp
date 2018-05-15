#include "Slider.h"

void Slider::mouseAt(int x, int y) {
  val = minv + (maxv - minv) * float(x - cax) / float(cbx - cax - 1);
  val = val + quanta / 2.0f - fmod(val + quanta/2.0f, quanta);
  val = min(max(val, minv),maxv);
  clickCallback(val);
}

int Slider::mouseEnter(int state) {
  return 0;
}
int Slider::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  bool oactive = active;
  if (active && mdown) {
    mouseAt(mx, my);
  }

  return mdown | (oactive xor active); //if state changed
}

int Slider::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  bool oactive = active;

  if (evt._key._type == key::type_mouse) {
    if (evt._key._keycode == 0) {
      if (evt._type == gui_event::evt_up) {
        mdown = false;
        active = false;
      }
      if(isIn(mx, my)) {
        if (evt._type == gui_event::evt_down) {
          mdown = true;
          active = true;
          mouseAt(mx, my);
        }
      }
      else {
        mdown = false;
        active = false;
      }
      return 1;
    }
  }
  return oactive xor active; //rerender if changed
}

void Slider::render(set<key_location>& down) {

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

  //inner bar
  glBegin(GL_QUADS);
  if (active) {
    setColor(pulledcolor);
  }
  else {
    setColor(activeColor);
  }
  glVertex2d(cax, cay);
  glVertex2d(cax + (cbx-cax)*float(val-minv)/float(maxv-minv), cay);
  glVertex2d(cax + (cbx - cax)*float(val - minv) / float(maxv - minv), cby);
  glVertex2d(cax, cby);
  glEnd();

  renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, to_string(val), textColor, true);
  //shapesPrintf(0, 0, text.c_str());
}

Slider::~Slider() {

}