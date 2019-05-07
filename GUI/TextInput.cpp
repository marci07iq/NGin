#include "TextInput.h"

int TextInput::guiEvent(gui_event& evt, int mx, int my, set<key_location>& down) {
  if (evt._key._type == key::type_mouse) {
    bool oactive = active;
    if (evt._type == gui_event::evt_pressed && evt._key._keycode == 0) { //click, left
      active = isIn(mx, my);
      if (active) {
        evt.captured = true;
      }
    }
    
    if (oactive xor active) {
      if (active) {
        cursor = text.size();
      }
      if (!active) {
        cursor = -1;
        input(shared_from_this(), text);
      }
    }
    return (oactive xor active);
  }

  if (active) {
    if(!evt.captured && evt._key._type == key::type_key && evt._type == gui_event::evt_pressed) {
      evt.captured = true;
      if (evt._key._keycode == GLFW_KEY_LEFT) {
        cursor = max(0, cursor - 1);
        return 1;
      }
      if (evt._key._keycode == GLFW_KEY_RIGHT) {
        cursor = min(int(text.size()), cursor + 1);
        return 1;
      }
      if (evt._key._keycode == GLFW_KEY_BACKSPACE && text.length() && cursor > 0) {
        text.erase(cursor - 1, 1);
        input(shared_from_this(), text);
        cursor--;
        return 1;
      }
      if (evt._key._keycode == GLFW_KEY_DELETE && text.length() && cursor < text.size()) {
        text.erase(cursor, 1);
        input(shared_from_this(), text);
        return 1;
      }
      if (evt._key._keycode == GLFW_KEY_ENTER || evt._key._keycode == GLFW_KEY_KP_ENTER) {
        input(shared_from_this(), text);
        active = false;
        cursor = -1;
        return 1;
      }
    }
    if (!evt.captured && evt._key._type == key::type_char && evt._type == gui_event::evt_pressed) {
      evt.captured = true;
      if (validator(shared_from_this(), text, cursor, evt._key._keycode)) {
        text.insert(cursor, 1, evt._key._keycode);
        input(shared_from_this(), text);
        cursor++;
        return 3;
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

int TextInput::activateElement(shared_ptr<GUIElement> id) {
  bool oactive = active;
  active = (shared_from_this() == id);
  if (active) {
    cursor = text.size();
  } else {
    cursor = -1;
  }
  return oactive ^ active;
}

TextInput::~TextInput() {

}