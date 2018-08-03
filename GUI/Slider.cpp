#include "Slider.h"

void Slider::setVal(float nval) {
  if (quanta != 0) {
    val = nval + quanta / 2.0f - fmod(nval + quanta / 2.0f, quanta);
  }
  val = min(max(val, minv), maxv);
  clickCallback(val);
  cursor = -1;
}
void Slider::mouseAt(int x, int y) {
  float nval = minv + (maxv - minv) * float(x - cax) / float(cbx - cax - 1);
  setVal(nval);
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
        if(textActive) {
          setVal(strTo<float>(text));
          textActive = false;
          cursor = -1;
        }
      }
      return 1;
    }
  }

  bool otextActive = textActive;

  if (!otextActive && isIn(mx, my) && (evt._key._type == key::type_special || evt._key._type == key::type_key) && evt._type == gui_event::evt_pressed) {
    textActive = true;
    cursor = text.size();
  }
  if(textActive) {
    if (evt._key._type == key::type_special && evt._type == gui_event::evt_pressed) {
      if (evt._key._keycode == GLUT_KEY_LEFT) {
        cursor = max(0, cursor - 1);
        return 1;
      }
      if (evt._key._keycode == GLUT_KEY_RIGHT) {
        cursor = min(int(text.size()), cursor + 1);
        return 1;
      }
    }
    if (evt._key._type == key::type_key && evt._type == gui_event::evt_pressed) {
      if (evt._key._keycode == '\b' && text.length() && cursor > 0) {
        text.erase(cursor - 1, 1);
        cursor--;
        return 1;
      }
      if (evt._key._keycode == 127 && text.length() && cursor < text.size()) {
        text.erase(cursor, 1);
        return 1;
      }
      if (evt._key._keycode == '\n' || evt._key._keycode == '\r') {
        textActive = false;
        cursor = -1;
        setVal(strTo<float>(text));
        return 3;
      }
      if (floatValidator(text, cursor, evt._key._keycode)) {
        text.insert(cursor, 1, evt._key._keycode);
        cursor++;
        return 1;
      }
    }
  }

  return (oactive xor active) | (otextActive xor textActive); //rerender if changed
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

  if(textActive) {
    renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, text, textColor, true, cursor);
  } else {
    text = to_string(val);
    renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, text, textColor, true);
  }
  //shapesPrintf(0, 0, text.c_str());
}

Slider::~Slider() {

}