#include "Graphics.h"

map<int, key_config> keybinds; //key, display name

bool key::isKey() {
  return _type == type_key;
}
void key::fromKey(int key) {
  _keycode = key;
  _type = type_key;
}
void key::fromChar(int c) {
  _keycode = c;
  _type = type_char;
}
void key::fromMouse(int button) {
  _keycode = button;
  _type = type_mouse;
}
void key::fromWheel(int delta) {
  _keycode = delta;
  _type = type_wheel;
}
const string key::toKeyName() {
  if (33 <= _keycode && _keycode <= 96) { //puncutation, uppercase
    return string(1, char(_keycode));
  }
  if (97 <= _keycode && _keycode <= 122) { //lowercase prints as upper
    return string(1, char(_keycode - 'a' + 'A'));
  }
  if (123 <= _keycode && _keycode <= 126) { //special
    return string(1, char(_keycode));
  }
  switch (_keycode) {
    case ' ':
      return "SPACE";
      break;
    case GLFW_KEY_TAB:
      return "TAB";
      break;
    case GLFW_KEY_BACKSPACE:
      return "BACKSPACE";
      break;
    case '\n':
    case '\r':
    case GLFW_KEY_ENTER:
      return "ENTER";
      break;
    case 127:
      return "DELETE";
      break;
    case 27:
      return "ESC";
      break;

      //Function keys
    case GLFW_KEY_LEFT_SHIFT:
      return "LSHIFT";
      break;
    case GLFW_KEY_RIGHT_SHIFT:
      return "RSHIFT";
      break;
    case GLFW_KEY_LEFT_ALT:
      return "LALT";
      break;
    case GLFW_KEY_RIGHT_ALT:
      return "RALT";
      break;
    case GLFW_KEY_LEFT_CONTROL:
      return "LCTRL";
      break;
    case GLFW_KEY_RIGHT_CONTROL:
      return "RCTRL";
      break;

      //Toggle keys
    case GLFW_KEY_NUM_LOCK:
      return "NUMLOCK";
      break;
    case GLFW_KEY_CAPS_LOCK:
      return "CAPSLOCK";
      break;
    case GLFW_KEY_SCROLL_LOCK:
      return "SCRLOCK";
      break;

    //Special block
    case GLFW_KEY_PRINT_SCREEN:
      return "PRT SCR";
      break;
    case GLFW_KEY_PAUSE:
      return "PAUSE";
      break;

    case GLFW_KEY_HOME:
      return "HOME";
      break;
    case GLFW_KEY_END:
      return "END";
      break;
    case GLFW_KEY_INSERT:
      return "INSERT";
      break;
    case GLFW_KEY_DELETE:
      return "DELETE";
      break;

    case GLFW_KEY_PAGE_DOWN:
      return "PGDOWN";
      break;
    case GLFW_KEY_PAGE_UP:
      return "PGUP";
      break;

      //Arrow
    case GLFW_KEY_LEFT:
      return "LEFT";
      break;
    case GLFW_KEY_RIGHT:
      return "RIGHT";
      break;
    case GLFW_KEY_UP:
      return "UP";
      break;
    case GLFW_KEY_DOWN:
      return "DOWN";
      break;

      //F
    case GLFW_KEY_F1:
      return "F1";
      break;
    case GLFW_KEY_F2:
      return "F2";
      break;
    case GLFW_KEY_F3:
      return "F3";
      break;
    case GLFW_KEY_F4:
      return "F4";
      break;
    case GLFW_KEY_F5:
      return "F5";
      break;
    case GLFW_KEY_F6:
      return "F6";
      break;
    case GLFW_KEY_F7:
      return "F7";
      break;
    case GLFW_KEY_F8:
      return "F8";
      break;
    case GLFW_KEY_F9:
      return "F9";
      break;
    case GLFW_KEY_F10:
      return "F10";
      break;
    case GLFW_KEY_F11:
      return "F11";
      break;
    case GLFW_KEY_F12:
      return "F12";
      break;

      //Numpad
    case GLFW_KEY_KP_0:
      return "NUM0";
      break;
    case GLFW_KEY_KP_1:
      return "NUM1";
      break;
    case GLFW_KEY_KP_2:
      return "NUM2";
      break;
    case GLFW_KEY_KP_3:
      return "NUM3";
      break;
    case GLFW_KEY_KP_4:
      return "NUM4";
      break;
    case GLFW_KEY_KP_5:
      return "NUM5";
      break;
    case GLFW_KEY_KP_6:
      return "NUM6";
      break;
    case GLFW_KEY_KP_7:
      return "NUM7";
      break;
    case GLFW_KEY_KP_8:
      return "NUM8";
      break;
    case GLFW_KEY_KP_9:
      return "NUM9";
      break;
    case GLFW_KEY_KP_ADD:
      return "NUM ADD";
      break;
    case GLFW_KEY_KP_SUBTRACT:
      return "NUM SUB";
      break;
    case GLFW_KEY_KP_MULTIPLY:
      return "NUM MULT";
      break;
    case GLFW_KEY_KP_DIVIDE:
      return "NUM DIV";
      break;
    case GLFW_KEY_KP_DECIMAL:
      return "NUM DEC";
      break;
    case GLFW_KEY_KP_ENTER:
      return "NUM ENT";
      break;
    case GLFW_KEY_KP_EQUAL:
      return "NUM EQ";
      break;
  }
  return "ERROR" + to_string(_keycode);
}
const string key::toCharName() {
  if (_keycode < 256) {
    return string(1, char(_keycode));
  }
  return "ERROR" + _keycode;
}
const string key::toMouseName() {
  switch (_keycode) {
    case GLFW_MOUSE_BUTTON_LEFT:
      return "LBUTTON";
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      return "MBUTTON";
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      return "RBUTTON";
      break;
    default:
      return "?BUTTON" + _keycode;
      break;
  }
}
const string key::toWheelName() {
  return to_string(_keycode);
}
const string key::toName() {
  switch (_type) {
    case type_key:
      return toKeyName();
      break;
    case type_char:
      return toCharName();
      break;
    case type_mouse:
      return toMouseName();
      break;
    case type_wheel:
      return toWheelName();
      break;
  }
  return "T ERROR" + to_string(_type);
}
key::key(int keycode, int type) {
  _keycode = keycode;
  _type = type;
}
key::key(string keycode) {
  vector<string> spliced = tokenize(keycode, ' ');
  _keycode = strTo<int>(spliced[0]);
  _type = strTo<int>(spliced[1]);
}
key::key() {
  _keycode = 0;
  _type = type_key;
}

bool operator==(const key& lhs, const key& rhs) {
  return (lhs._type == rhs._type) & (lhs._keycode == rhs._keycode);
}

bool operator<(const key& lhs, const key& rhs) {
  if (lhs._type < rhs._type) {
    return true;
  }
  if ((lhs._type == rhs._type) && (lhs._keycode < rhs._keycode)) {
    return true;
  }
  return false;
}

bool operator<=(const key& lhs, const key& rhs) {
  if (lhs._type <= rhs._type) {
    return true;
  }
  if ((lhs._type == rhs._type) && (lhs._keycode <= rhs._keycode)) {
    return true;
  }
  return false;
}

bool operator>(const key& lhs, const key& rhs) {
  if (lhs._type > rhs._type) {
    return true;
  }
  if ((lhs._type == rhs._type) && (lhs._keycode > rhs._keycode)) {
    return true;
  }
  return false;
}

bool operator>=(const key& lhs, const key& rhs) {
  if (lhs._type >= rhs._type) {
    return true;
  }
  if ((lhs._type == rhs._type) && (lhs._keycode >= rhs._keycode)) {
    return true;
  }
  return false;
}

void key_location::setLocation(int mx, int my) {
  _mx = mx;
  _my = my;
}
key_location::key_location(int keycode, int type, int mx, int my) : key(keycode, type), _mx(mx), _my(my) {

}
key_location::key_location() {

}

string key_config::toName() {
  string res;
  for (auto&& it : _parts) {
    if (res.size()) {
      res += " + ";
    }
    res += it.toName();
  }
  return res;
}

void key_config::addKey(key k, gui_event::type type) {
  if (type == gui_event::evt_up) {
    resetOnNext = true;
  }
  if (type == gui_event::evt_down) {
    if (resetOnNext) {
      resetOnNext = false;
      _parts.clear();
      trigger = key();
    }
    if(
      k._keycode == GLFW_KEY_LEFT_SHIFT || k._keycode == GLFW_KEY_RIGHT_SHIFT ||
      k._keycode == GLFW_KEY_LEFT_ALT || k._keycode ==   GLFW_KEY_RIGHT_ALT ||
      k._keycode == GLFW_KEY_LEFT_CONTROL || k._keycode ==  GLFW_KEY_RIGHT_CONTROL
      ) {
      _parts.push_back(k);
    } else {
      _parts.push_back(k);
      trigger = k;
      resetOnNext = true;
    }
  }
}

bool key_config::check(set<key_location>& down, key k, bool need) {
  bool hasSpec = false;
  if (k == trigger && down.size() == _parts.size()) {
    hasSpec = true;
  }
  bool good = _parts.size();
  for (auto&& it : _parts) {
    good &= down.count(it);
  }
  return good && (!need || hasSpec);
}

string gui_event::toName() {
  switch (_type) {
    case evt_none:
      return _key.toName();
      break;
    case evt_down:
      return _key.toName() + " DOWN";
      break;
    case evt_pressed:
      return _key.toName() + " PRESS";
      break;
    case evt_up:
      return _key.toName() + " UP";
      break;
  }
}
gui_event::gui_event(key_location key, type type) {
  _key = key;
  _type = type;
}

key loadKey(xml_attribute<>* me) {
  if (me) {
    stringstream ss;
    ss << me->value();
    int valt, valv;
    ss >> valt >> valv;
    return key(valv, valt);
  } else {
    return key(key::type_key, 0);
  }
}


void keybindReply(Graphics::ElemHwnd e, key_config nkey) {
  int id = ((Graphics::ControlHwnd)e)->id;
  if (keybinds.count(id)) {
    keybinds[id] = nkey;
  }
}

void loadKeybinds(string filename) {
  ifstream binds;
  binds.open(filename);

  keybinds.clear();

  int id;
  string display;
  int valt;
  int valv;
  int n;

  while (binds >> id >> display >> n) {
    replaceChar(display, '_', ' ');

    keybinds[id].display = display;

    for (int i = 0; i < n; i++) {
      binds >> valt >> valv;
      keybinds[id].addKey(key(valv, valt), gui_event::evt_down);
    }
  }

  binds.close();
}

void saveKeybinds(string filename) {
  ofstream binds;
  binds.open(filename);

  for(auto&& it : keybinds) {
    string name = it.second.display;
    replaceChar(name, ' ', '_');

    binds << it.first << " " << name << " " << it.second._parts.size();
    for (auto&& itk : it.second._parts) {
      binds << " " << itk._type << " " << itk._keycode;
    }
    binds << endl;

  }

  binds.close();
}

bool checkKey(int id, set<key_location>& down, key k, bool need) {
  return keybinds[id].check(down, k, need);
}

bool checkKey(key id, set<key_location>& down) {
  return down.count(id);
}

bool isDown(set<key_location>& down, key toTest) {
  return isDown(down, key_location(toTest));
}
bool isDown(set<key_location>& down, key_location toTest) {
  return down.count(toTest) > 0;
}

