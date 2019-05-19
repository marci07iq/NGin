#include "Container.h"

namespace NGin {
  namespace Graphics {
    void Container::transformCoordinates(int& mx, int& my) {
      /*mx -= cax;
      my -= cay;*/
    }
    int Container::mouseEnter(int state) {
      int bstate = 0;

      if (element != NULL) {
        bstate = element->mouseEnter(state);
      }
      return bstate;
    }
    int Container::mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down) {

      int bstate = 0;
      if (mid) {
        mxo -= mx - ox;
        myo -= my - oy;
        if (element != NULL) {
          element->getRect(cbx - cax, cby - cay, cax - mxo, cay - myo);
        }
        bstate = 1;
      } else {
        if (element != NULL) {
          bstate = element->mouseMoved(mx, my, ox, oy, down);
        }
      }

      return bstate;
    }
    int Container::guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down) {
      int bstate = 0;
      if (evt._key._type == evt._key.type_mouse && evt._key._keycode == GLFW_MOUSE_BUTTON_RIGHT) {
        if (evt._type == evt.evt_down) {
          mid = true;
        }
        if (evt._type == evt.evt_up) {
          mid = false;
        }
      } else {
        if (element != NULL) {
          bstate = element->guiEvent(evt, mx, my, down);
        }
        return bstate;
      }
    }
    void Container::render(set<key_location>& down) {
      if (element != NULL) {
        element->render(down);
      }
    }

    void Container::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
      cax = offsetX + location.getLeft(winWidth);
      cay = offsetY + location.getBot(winHeight);
      cbx = offsetX + location.getRight(winWidth);
      cby = offsetY + location.getTop(winHeight);

      if (element != NULL) {
        element->getRect(cbx - cax, cby - cay, cax - mxo, cay - myo);
      }
    }

    shared_ptr<GUIElement> Container::getElementById(string id) {
      if (name == id) {
        return shared_from_this();
      } else {
        shared_ptr<GUIElement> res = NULL;
        if (element != NULL) {
          shared_ptr<GUIElement> e = element->getElementById(id);
        }
        return res;
      }
    }

    void Container::deleteElement(shared_ptr<GUIElement> elem, bool hard) {
      if (element != elem) {
        throw 1;
      } else {
        element.reset();
      }
    }

    Container::~Container() {

    }
  }
}