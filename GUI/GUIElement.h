#pragma once

#include "GraphicsUtils.h"

class GUIElement {
public:
  string name;
  int cax, cay, cbx, cby; //Not for long term storage.
  LocationData location;
  colorargb bgColor; //#aarrggbb
  colorargb activeColor; //#aarrggbb
  colorargb textColor;
  bool active;
  bool toDelete;

  GUIElement(string name, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor);
  virtual bool isIn(int mx, int my);
  virtual void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
  virtual void getRect();
  virtual int mouseEnter(int state);
  virtual int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  virtual int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  virtual void render(set<key_location>& down);
  virtual int getWidth();
  virtual int getWidth(int winWidth);
  virtual int getHeight();
  virtual int getHeight(int winHeight);
  virtual GUIElement* getElementById(string id);
  ~GUIElement();
};
