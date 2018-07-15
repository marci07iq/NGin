#include "Panel.h"

void Panel::transformCoordinates(int &mx, int &my) {
  /*mx -= cax;
  my -= cay;*/
}
int Panel::mouseEnter(int state) {
  auto it = elements.end();

  int bstate = 0;

  while (it != elements.begin() && !(state & 2)) {
    --it;
    if (!(*it)->toDelete) {
      bstate |= (*it)->mouseEnter(state);
    }
  }
  return bstate;
}
int Panel::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  transformCoordinates(mx, my);

  auto it = elements.end();

  int state = 0;

  while (it != elements.begin() && !(state & 2)) {
    --it;
    if (!(*it)->toDelete) {
      state |= (*it)->mouseMoved(mx, my, ox, oy, down);
    }
  }
  return state;
}
int Panel::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  transformCoordinates(mx, my);

  auto it = elements.end();

  int bstate = 0;

  while (it != elements.begin() && !(bstate & 2)) {
    --it;
    if (!(*it)->toDelete) {
      bstate |= (*it)->guiEvent(evt, mx, my, down);
    }
  }
  return bstate;
}
void Panel::render(set<key_location>& down) {
  auto it = elements.begin();

  while (it != elements.end()) {
    if ((*it)->toDelete) {
      delete *it;
      auto it2 = it;
      ++it;
      elements.erase(it2);
    } else {
      //glPushMatrix();
      //glViewport(cax, cay, cbx - cax, cby - cay);
      //glScissor((*it)->cax, (*it)->cay, (*it)->cbx - (*it)->cax, (*it)->cby - (*it)->cay);
      (*it)->render(down);
      //glPopMatrix();
      ++it;
    }
    
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

GUIElement* Panel::getElementById(string id) {
  if (name == id) {
    return this;
  }
  else {
    GUIElement* res = NULL;

    auto it = elements.begin();

    while (it != elements.end() && res == NULL) {
      if (!(*it)->toDelete) {
        GUIElement* e = (*it)->getElementById(id);

        if (e != NULL) {
          res = e;
        }
      }

      ++it;
    }
    return res;
  }
}

int Panel::activateElement(GUIElement* id) {
  auto it = elements.end();

  int bstate = 0;

  while (it != elements.begin()) {
    --it;
    if (!(*it)->toDelete) {
      bstate |= (*it)->activateElement(id);
    }
  }
  return bstate;
}

Panel::~Panel() {
  while (elements.size()) {
    if (elements.front() != NULL) {
      delete elements.front();
      elements.pop_front();
    }
  }
}