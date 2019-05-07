#pragma once

#include "GUIElement.h"

class Container : public GUIElement {
public:
  shared_ptr<GUIElement> element;
  int mxo;
  int myo;
  bool mid = false;
  Container(string lname, LocationData llocation, colorargb lbg, void* ldata) :
    GUIElement(lname, llocation, lbg, 0, 0, ldata) {
  }
  void transformCoordinates(int &mx, int &my);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event& evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);

  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);

  void deleteElement(shared_ptr<GUIElement> elem, bool hard);

  shared_ptr<GUIElement> getElementById(string id);
  /*bool keyPressed(unsigned char key, int mx, int my);
  bool specialPressed(int key, int mx, int my);
  bool mouseClicked(int mx, int my);
  void render(set<key_location>& down);*/
  virtual ~Container();

};
