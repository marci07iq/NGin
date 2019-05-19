#include "GUIElement.h"

namespace NGin {
  namespace Graphics {


    GUIElement::GUIElement(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata) {
      name = lname;
      location = llocation;
      bgColor = lbg;
      activeColor = lactive;
      textColor = ltextColor;
      active = false;
      data = ldata;
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
      if ((cax <= mx) && (mx < cbx) && (cay <= my) && (my < cby)) {
        return true;
      }
      return false;
    }

    int GUIElement::mouseEnter(int state) {
      return 0;
    }

    int GUIElement::mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down) {
      return 0;
    }

    int GUIElement::guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down) {
      return 0;
    }

    void GUIElement::render(set<key_location>& down) {

    }

    shared_ptr<GUIElement> GUIElement::getElementById(string id) {
      if (name == id) {
        return shared_from_this();
      } else {
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

    int GUIElement::activateElement(shared_ptr<GUIElement> id) {
      bool oactive = active;
      active = (shared_from_this() == id);
      return oactive ^ active;
    }

    void GUIElement::deleteElement(shared_ptr<GUIElement> elem) {
    }

    GUIElement::~GUIElement() {

    }

  }
}