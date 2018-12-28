#pragma once

#include "../Maths/helperFunctions.h"
#include "GraphicsUtils.h"

class key {
public:
  enum type {
    type_key = 0,
    type_special = 1,
    type_mouse = 2,
    type_wheel = 3
  };
  int _type;
  int _keycode;
  bool isKey();
  void fromKey(unsigned char key);
  void fromSpecial(int key);
  void fromMouse(int button);
  void fromWheel(int delta);
  const string toKeyName();
  const string toSpecialName();
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
  key_location();
};

class key_config {
public:
  list<key> _parts;
  string display;
  string toName();
  void addKey(key k);
  bool check(set<key_location>& down, key spec = key(), bool need = false);
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

key loadKey(xml_attribute<>* me);


extern map<int, key_config> keybinds; //key, display name

void keybindReply(key_config nkey, int id);

void loadKeybinds(string filename = "html/keybinds.cfg");
void saveKeybinds(string filename = "html/keybinds.cfg");

bool checkKey(int id, set<key_location>& down, key spec = key(), bool need = false);
bool checkKey(key id, set<key_location>& down);


bool isDown(set<key_location>& down, key toTest);
bool isDown(set<key_location>& down, key_location toTest);