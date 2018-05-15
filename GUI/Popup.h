#pragma once
#include "GuiElement.h"

/*Coordinate to_mincorner(int topleftx, int toplefty, Coordinate dimension);
Coordinate to_maxcorner(int topleftx, int toplefty, Coordinate dimension);

class Popup : public GUIElement {
public:
  GUIElement* element;
  Coordinate topleftc;
  Coordinate sizec;
  Popup(string lname, int topleftx, int toplefty, Coordinate dimension, colorargb lbg) :
    GUIElement(
    lname,
    to_mincorner(topleftx, toplefty, dimension),
    to_maxcorner(topleftx, toplefty, dimension),
    lbg, 0, 0) {
    topleftc = Coordinate(0,0,topleftx,toplefty);
    sizec = dimension;
  }
  void transformCoordinates(int &mx, int &my);
  void render(set<key_location>& down);

  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);

  void moveTo(int topleftx, int toplefty);

  void deleteElements(bool hard);

  GUIElement* getElementById(string id);
  /*bool keyPressed(unsigned char key, int mx, int my);
  bool specialPressed(int key, int mx, int my);
  bool mouseClicked(int mx, int my);
  void render(set<key_location>& down);*//*
  ~Popup();
};*/
