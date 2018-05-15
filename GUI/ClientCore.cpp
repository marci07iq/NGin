#include "GraphicsUtils.h"

vector<pair<key, string>> keybinds; //key, display name

void keybindReply(key nkey, int id) {
  if (id < keybinds.size()) {
    keybinds[id].first = nkey;
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

  while (binds >> id >> display >> valt >> valv) {
    replaceChar(display, '_', ' ');

    if (keybinds.size() <= id) {
      keybinds.resize(id + 1);
    }

    keybinds[id] = {key(valv, valt), display};
  }

  binds.close();
}

void saveKeybinds(string filename) {
  ofstream binds;
  binds.open(filename);

  int id = 0;
  for(auto&& it : keybinds) {
    string name = it.second;
    replaceChar(name, ' ', '_');

    binds << id << " " << name << " " << it.first._type << " " << it.first._keycode END;

    id++;
  }

  binds.close();
}

bool checkKey(int id, key toTest) {
  return keybinds[id].first == toTest;
}
bool checkKey(int id, key_location toTest) {
  return keybinds[id].first == toTest;
}

bool isDown(set<key_location>& down, int toTestId) {
  return isDown(down, keybinds[toTestId].first);
}
bool isDown(set<key_location>& down, key toTest) {
  return isDown(down, key_location(toTest));
}
bool isDown(set<key_location>& down, key_location toTest) {
  return down.count(toTest) > 0;
}