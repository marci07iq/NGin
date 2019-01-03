#include "TextInput.h"

int TextInput::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  if (evt._key._type == key::type_mouse) {
    bool oactive = active;
    if (evt._type == gui_event::evt_pressed && evt._key._keycode == 0) { //click, left
      active = isIn(mx, my);
    }
    if (oactive xor active) {
      if (active) {
        cursor = text.size();
      }
      if (!active) {
        cursor = -1;
        input(this, text);
      }
    }
    return oactive xor active;
  }

  if (active) {
    if(evt._key._type == key::type_special && evt._type == gui_event::evt_pressed) {
      if (evt._key._keycode == GLFW_KEY_LEFT) {
        cursor = max(0, cursor - 1);
        return 1;
      }
      if (evt._key._keycode == GLFW_KEY_RIGHT) {
        cursor = min(int(text.size()), cursor + 1);
        return 1;
      }
    }
    if (evt._key._type == key::type_key && evt._type == gui_event::evt_pressed) {
      if (evt._key._keycode == '\b' && text.length() && cursor > 0) {
        text.erase(cursor - 1, 1);
        input(this, text);
        cursor--;
        return 1;
      }
      if (evt._key._keycode == 127 && text.length() && cursor < text.size()) {
        text.erase(cursor, 1);
        input(this, text);
        return 1;
      }
      if (evt._key._keycode == '\n' || evt._key._keycode == '\r') {
        input(this, text);
        return 3;
      }
      if (validator(this, text, cursor, evt._key._keycode)) {
        text.insert(cursor, 1, evt._key._keycode);
        input(this, text);
        cursor++;
        return 1;
      }
    }
  }

  return 0;
}

void TextInput::render(set<key_location>& down) {
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

  renderBitmapString((cax+cbx) / 2.0f, (cay + cby) / 2.0f, text, textColor, true, cursor);
  //shapesPrintf(0, 0, text.c_str());
}

int TextInput::activateElement(GUIElement* id) {
  bool oactive = active;
  active = (this == id);
  if (active) {
    cursor = text.size();
  } else {
    cursor = -1;
  }
  return oactive ^ active;
}

TextInput::~TextInput() {

}