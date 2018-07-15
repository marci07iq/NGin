#include "GUIElement.h"

GUIElement::GUIElement(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor) {
  name = lname;
  location = llocation;
  bgColor = lbg;
  activeColor = lactive;
  textColor = ltextColor;
  active = false;
  toDelete = false;
}

void GUIElement::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
  cax = offsetX + location.getLeft(winWidth);
  cay = offsetY + location.getBot(winHeight);
  cbx = offsetX + location.getRight(winWidth);
  cby = offsetY + location.getTop(winHeight);

  getRect();
}

void GUIElement::getRect() {

}

bool GUIElement::isIn(int mx, int my) {
  //getRect(winWidth, winHeight);
  if ((cax  <= mx) && (mx <= cbx ) && (cay <= my) && (my <= cby)) {
    return true;
  }
  return false;
}

int GUIElement::mouseEnter(int state) {
  return 0;
}

int GUIElement::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}

int GUIElement::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}

void GUIElement::render(set<key_location>& down) {

}

GUIElement* GUIElement::getElementById(string id) {
  if (name == id) {
    return this;
  }
  else {
    return NULL;
  }
}

int GUIElement::getWidth() {
  return cbx - cax;
}
int GUIElement::getWidth(int winWidth) {
  return location.getWidth(winWidth);
}
int GUIElement::getHeight() {
  return cby - cay;
}
int GUIElement::getHeight(int winHeight) {
  return location.getHeight(winHeight);
}

int GUIElement::activateElement(GUIElement* id) {
  bool oactive = active;
  active = (this == id);
  return oactive ^ active;
}

GUIElement::~GUIElement() {

}