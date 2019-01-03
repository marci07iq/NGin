#pragma once

#include "GUIElement.h"

class Panel : public GUIElement {
public:
  list<GUIElement*> elements; 
  Panel(string lname, LocationData llocation, colorargb lbg) :
    GUIElement(lname, llocation, lbg, 0, 0) {
  }
  void transformCoordinates(int &mx, int &my);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);

  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
  void getRect();

  int activateElement(GUIElement* id);
  void deleteElement(GUIElement* elem);

  GUIElement* getElementById(string id);
  /*bool keyPressed(unsigned char key, int mx, int my);
  bool specialPressed(int key, int mx, int my);
  bool mouseClicked(int mx, int my);
  void render(set<key_location>& down);*/
  ~Panel();
};
