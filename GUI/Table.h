#pragma once

#include "GUIElement.h"
//#include "LabelBind.h"

class TableRow : public GUIElement {
public:
  list<GUIElement*> data;
  TableRow(string lname, LocationData llocation, colorargb lbg) :
    GUIElement(lname, llocation, lbg, 0, 0) {
  }
  void render(set<key_location>& down);
  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
  void getRect();
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);

  GUIElement* getElementById(string id);

  int activateElement(GUIElement* id);
  void deleteElement(GUIElement* elem);

  ~TableRow();
};

class Table : public GUIElement {
public:
  list<TableRow*> data;
  //float chy;
  int sba, sbb; //scroll bar top - bottom
  int height;
  float scroll;
  Table(string lname, LocationData llocation, colorargb lbg, colorargb lactive) :
    GUIElement(lname, llocation, lbg, lactive, 0) {
  }
  void render(set<key_location>& down);
  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
  void getRect();
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);

  GUIElement* getElementById(string id);

  int activateElement(GUIElement* id);
  void deleteElement(GUIElement* elem);

  ~Table();
};
