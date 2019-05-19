#include "Panel.h"


namespace NGin {
  namespace Graphics {

    void Panel::transformCoordinates(int& mx, int& my) {
      /*mx -= cax;
      my -= cay;*/
    }
    int Panel::mouseEnter(int state) {
      auto it = elements.end();

      int bstate = 0;

      while (it != elements.begin() && !(state & 2)) {
        --it;
        bstate |= (*it)->mouseEnter(state);
      }
      return bstate;
    }
    int Panel::mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down) {
      transformCoordinates(mx, my);

      auto it = elements.end();

      int state = 0;

      while (it != elements.begin() && !(state & 2)) {
        --it;
        state |= (*it)->mouseMoved(mx, my, ox, oy, down);
      }
      return state;
    }
    int Panel::guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down) {
      transformCoordinates(mx, my);

      auto it = elements.end();

      int bstate = 0;

      while (it != elements.begin() && !(bstate & 2)) {
        --it;
        bstate |= (*it)->guiEvent(evt, mx, my, down);
      }
      if (bgColor > 0xffffff && isIn(mx, my) && evt._key._type == key::type_mouse && (evt._type == evt.evt_pressed || evt._type == evt.evt_down)) { //Has alpha
        evt.captured = true;
      }
      return bstate;
    }
    void Panel::render(set<key_location>& down) {
      if (bgColor > 0xffffff) { //Has alpha
        Gll::gllBegin(Gll::GLL_QUADS);
        setColor(bgColor);
        Gll::gllVertex(cax, cay);
        Gll::gllVertex(cbx, cay);
        Gll::gllVertex(cbx, cby);
        Gll::gllVertex(cax, cby);
        Gll::gllEnd();
      }

      auto it = elements.begin();

      while (it != elements.end()) {
        //glPushMatrix();
        //glViewport(cax, cay, cbx - cax, cby - cay);
        //glScissor((*it)->cax, (*it)->cay, (*it)->cbx - (*it)->cax, (*it)->cby - (*it)->cay);
        (*it)->render(down);
        //glPopMatrix();
        ++it;
      }
    }

    void Panel::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
      cax = offsetX + location.getLeft(winWidth);
      cay = offsetY + location.getBot(winHeight);
      cbx = offsetX + location.getRight(winWidth);
      cby = offsetY + location.getTop(winHeight);

      getRect();
    }

    void Panel::getRect() {
      auto it = elements.begin();

      while (it != elements.end()) {
        (*it)->getRect(cbx - cax, cby - cay, cax, cay);
        ++it;
      }
    }

    shared_ptr<GUIElement> Panel::getElementById(string id) {
      if (name == id) {
        return shared_from_this();
      } else {
        shared_ptr<GUIElement> res = NULL;

        auto it = elements.begin();

        while (it != elements.end() && res == NULL) {
          shared_ptr<GUIElement> e = (*it)->getElementById(id);

          if (e != NULL) {
            res = e;
          }

          ++it;
        }
        return res;
      }
    }

    int Panel::activateElement(shared_ptr<GUIElement> id) {
      auto it = elements.end();

      int bstate = 0;

      while (it != elements.begin()) {
        --it;
        bstate |= (*it)->activateElement(id);

      }
      return bstate;
    }

    void Panel::deleteElement(shared_ptr<GUIElement> elem, bool hard) {
      elements.remove(elem);
    }

    Panel::~Panel() {

    }
  }
}