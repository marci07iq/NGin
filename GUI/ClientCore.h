#pragma once

#include "../Maths/FlowSystem.h"

class key;
class key_location;

extern vector<pair<key, string>> keybinds; //key, display name

void keybindReply(key nkey, int id);

void loadKeybinds(string filename = "html/keybinds.cfg");
void saveKeybinds(string filename = "html/keybinds.cfg");

bool checkKey(int id, key toTest);
bool checkKey(int id, key_location toTest);

bool isDown(set<key_location>& down, int toTestId);
bool isDown(set<key_location>& down, key toTest);
bool isDown(set<key_location>& down, key_location toTest);