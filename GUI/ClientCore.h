#pragma once

#include "../Maths/helperFunctions.h"
//#include "GraphicsUtils.h"

class key {
public:
  enum type {
    type_key = 0,
    type_char = 1,
    type_mouse = 2,
    type_wheel = 3
  };
  int _type;
  int _keycode;
  bool isKey();
  void fromKey(int key);
  void fromChar(int c);
  void fromMouse(int button);
  void fromWheel(int delta);
  const string toKeyName();
  const string toCharName();
  const string toMouseName();
  const string toWheelName();
  const string toName();
  key(int keycode, int type);
  key(string keycode);
  key();
};

class key_location : public key {
public:
  int _mx;
  int _my;
  void setLocation(int mx, int my);
  key_location(int keycode, int type, int mx, int my);
  key_location(key k) {
    _keycode = k._keycode;
    _type = k._type;
  }
  key_location(const key_location& other) {
    _mx = other._mx;
    _my = other._my;
    _keycode = other._keycode;
    _type = other._type;
  }
  key_location();
};

bool operator==(const key& lhs, const key& rhs);

bool operator<(const key& lhs, const key& rhs);

bool operator<=(const key& lhs, const key& rhs);

bool operator>(const key& lhs, const key& rhs);

bool operator>=(const key& lhs, const key& rhs);

class gui_event {
public:
  key_location _key;
  enum type {
    evt_none = 0,
    evt_down = 1,
    evt_pressed = 2,
    evt_up = 3
  };
  type _type;
  string toName();
  gui_event(key_location key, type type);
};

class key_config {
public:
  list<key> _parts;
  key trigger;
  string display;
  bool resetOnNext;
  string toName();
  void addKey(key k, gui_event::type type);
  bool check(set<key_location>& down, key special = key(), bool need = false);
};


key loadKey(xml_attribute<>* me);


extern map<int, key_config> keybinds; //key, display name

class GUIElement;
namespace Graphics {
  typedef GUIElement* ElemHwnd;
}

void keybindReply(Graphics::ElemHwnd e, key_config nkey);

void loadKeybinds(string filename = "html/keybinds.cfg");
void saveKeybinds(string filename = "html/keybinds.cfg");

bool checkKey(int id, set<key_location>& down, key special = key(), bool need = false);
bool checkKey(key id, set<key_location>& down);


bool isDown(set<key_location>& down, key toTest);
bool isDown(set<key_location>& down, key_location toTest);