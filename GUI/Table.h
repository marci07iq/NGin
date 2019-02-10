#pragma once

#include "GUIElement.h"
//#include "LabelBind.h"

class TableRow : public GUIElement {
public:
  list<GUIElement*> data;

  int width;

  float scroll;
  int sab, sbb; //scroll bar left - right

  TableRow(string lname, LocationData llocation, colorargb lbg, colorargb lactive, void* ldata) :
    GUIElement(lname, llocation, lbg, lactive, 0, ldata) {
  }
  void render(set<key_location>& down);
  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
  void getRect();
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event& evt, int mx, int my, set<key_location>& down);

  GUIElement* getElementById(string id);

  int activateElement(GUIElement* id);
  void deleteElement(GUIElement* elem, bool hard);

  ~TableRow();
};

class Table : public GUIElement {
public:
  list<GUIElement*> data;
  int height;

  float scroll;
  int sba, sbb; //scroll bar top - bottom

  Table(string lname, LocationData llocation, colorargb lbg, colorargb lactive, void* ldata) :
    GUIElement(lname, llocation, lbg, lactive, 0, ldata) {
  }
  void render(set<key_location>& down);
  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
  void getRect();
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event& evt, int mx, int my, set<key_location>& down);

  GUIElement* getElementById(string id);

  int activateElement(GUIElement* id);
  void deleteElement(GUIElement* elem, bool hard);

  ~Table();
};
