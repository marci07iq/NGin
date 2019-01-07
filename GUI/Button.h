#pragma once

#include "GUIElement.h"

class Button : public GUIElement{
public:
  int triggerId;
  string text;
  ClickCallback clickCallback;
  void* data;
  Button(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, string ltext, int ltriggerId, ClickCallback lclickCallback, void* ldata) :
  GUIElement(lname, llocation, lbg, lactive, ltextColor) {
    text = ltext;
    clickCallback = lclickCallback;
    triggerId = ltriggerId;
    data = ldata;
  }
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  virtual void render(set<key_location>& down);
  ~Button();
};

class IconButton : public Button {
public:
  Icon* ic;
  IconButton(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, string ltext, int ltriggerId, ClickCallback lclickCallback, void* ldata, string icon, string ilfFilepath) :
    Button(lname, llocation, lbg, lactive, ltextColor, ltext, ltriggerId, lclickCallback, ldata) {
    ic = getIcon(icon, ilfFilepath);
  }
  void render(set<key_location>& down);
};
