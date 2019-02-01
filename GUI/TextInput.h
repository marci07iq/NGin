#pragma once

#include "GUIElement.h"

class TextInput : public GUIElement {
public:
  string text;
  int cursor;
  TextValidatorFunc validator;
  TextInputFunc input;
  void* data;
  TextInput(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata, string ltext, TextInputFunc linput, TextValidatorFunc lvalidator = *textValidator) :
    GUIElement(lname, llocation, lbg, lactive, ltextColor, ldata) {
    text = ltext;
    input = linput;
    validator = lvalidator;
    cursor = -1;
    data = ldata;
  }
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);
  int activateElement(GUIElement* id);

  ~TextInput();
};
