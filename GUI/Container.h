#pragma once

#include "GUIElement.h"

class Container : public GUIElement {
public:
  GUIElement* element;
  int mxo;
  int myo;
  bool mid = false;
  Container(string lname, LocationData llocation, colorargb lbg) :
    GUIElement(lname, llocation, lbg, 0, 0) {
  }
  void transformCoordinates(int &mx, int &my);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);

  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);

  void deleteElements(bool hard);

  GUIElement* getElementById(string id);
  /*bool keyPressed(unsigned char key, int mx, int my);
  bool specialPressed(int key, int mx, int my);
  bool mouseClicked(int mx, int my);
  void render(set<key_location>& down);*/
  ~Container();

};
