#include "GraphicsUtils.h"

map<int, key_config> keybinds; //key, display name

bool key::isKey() {
  return _type == type_key;
}
void key::fromKey(unsigned char key) {
  _keycode = key;
  _type = type_key;
}
void key::fromSpecial(int key) {
  _keycode = key;
  _type = type_special;
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
    case 9:
      return "TAB";
      break;
    case 8:
      return "BACKSPACE";
      break;
    case '\n':
    case '\r':
      return "ENTER";
      break;
    case 127:
      return "DELETE";
      break;
    case 27:
      return "ESC";
      break;
  }
}
const string key::toSpecialName() {
  switch (_keycode) {
    //Function keys
    case GLUT_KEY_SHIFT_L:
      return "LSHIFT";
      break;
    case GLUT_KEY_SHIFT_R:
      return "RSHIFT";
      break;
    case GLUT_KEY_ALT_L:
      return "LALT";
      break;
    case GLUT_KEY_ALT_R:
      return "RALT";
      break;
    case GLUT_KEY_CTRL_L:
      return "LCTRL";
      break;
    case GLUT_KEY_CTRL_R:
      return "RCTRL";
      break;

      //Toggle keys
    case GLUT_KEY_NUM_LOCK:
      return "NUMLOCK";
      break;

    case GLUT_KEY_BEGIN:
      return "BEGIN";
      break;
    case GLUT_KEY_HOME:
      return "HOME";
      break;
    case GLUT_KEY_END:
      return "END";
      break;
    case GLUT_KEY_INSERT:
      return "INSERT";
      break;
    case GLUT_KEY_DELETE:
      return "DELETE";
      break;

    case GLUT_KEY_PAGE_DOWN:
      return "PGDOWN";
      break;
    case GLUT_KEY_PAGE_UP:
      return "PGUP";
      break;
      ///Repeat

      //Arrow
    case GLUT_KEY_LEFT:
      return "LEFT";
      break;
    case GLUT_KEY_RIGHT:
      return "RIGHT";
      break;
    case GLUT_KEY_UP:
      return "UP";
      break;
    case GLUT_KEY_DOWN:
      return "DOWN";
      break;

      //F
    case GLUT_KEY_F1:
      return "F1";
      break;
    case GLUT_KEY_F2:
      return "F2";
      break;
    case GLUT_KEY_F3:
      return "F3";
      break;
    case GLUT_KEY_F4:
      return "F4";
      break;
    case GLUT_KEY_F5:
      return "F5";
      break;
    case GLUT_KEY_F6:
      return "F6";
      break;
    case GLUT_KEY_F7:
      return "F7";
      break;
    case GLUT_KEY_F8:
      return "F8";
      break;
    case GLUT_KEY_F9:
      return "F9";
      break;
    case GLUT_KEY_F10:
      return "F10";
      break;
    case GLUT_KEY_F11:
      return "F11";
      break;
    case GLUT_KEY_F12:
      return "F12";
      break;
  }
  return "ERROR" + to_string(_keycode);
}
const string key::toMouseName() {
  switch (_keycode) {
    case 0:
      return "LBUTTON";
      break;
    case 1:
      return "MBUTTON";
      break;
    case 2:
      return "RBUTTON";
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
    case type_special:
      return toSpecialName();
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

void key_config::addKey(key k) {
  if (k._type == k.type_key || k._type == k.type_special) {
    /*if (k._type == k.type_special && (
      k._keycode == GLUT_KEY_SHIFT_L || k._keycode == GLUT_KEY_SHIFT_R ||
      k._keycode == GLUT_KEY_ALT_L || k._keycode == GLUT_KEY_ALT_R ||
      k._keycode == GLUT_KEY_CTRL_L || k._keycode == GLUT_KEY_CTRL_R)) {
      _parts.push_front(k);
    } else {
      _parts.clear();*/
      _parts.push_back(k);
    //}

  }
}

bool key_config::check(set<key_location>& down, key spec, bool need) {
  bool good = _parts.size();
  bool hasSpec = false;
  for (auto&& it : _parts) {
    good &= down.count(it);
    if (it == spec) {
      hasSpec = true;
    }
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


void keybindReply(key_config nkey, int id) {
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
      keybinds[id].addKey(key(valv, valt));
    }
  }

  binds.close();
}

void saveKeybinds(string filename) {
  ofstream binds;
  binds.open(filename);

  int id = 0;
  for(auto&& it : keybinds) {
    string name = it.second.display;
    replaceChar(name, ' ', '_');

    binds << id << " " << name;
    for (auto&& itk : it.second._parts) {
      binds << " " << itk._type << " " << itk._keycode;
    }
    binds << endl;

    id++;
  }

  binds.close();
}

bool checkKey(int id, set<key_location>& down, key spec, bool need) {
  return keybinds[id].check(down, spec, need);
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

