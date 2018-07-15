#include "Table.h"

void TableRow::render(set<key_location>& down) {
  auto it = data.begin();

  while (it != data.end()) {
    if ((*it)->toDelete) {
      delete *it;
      auto it2 = it;
      ++it;
      data.erase(it2);
    }
    else {
      //glPushMatrix();
      //glViewport(cax, cay, cbx - cax, cby - cay);
      //glScissor((*it)->cax, (*it)->cay, (*it)->cbx - (*it)->cax, (*it)->cby - (*it)->cay);
      (*it)->render(down);
      //glPopMatrix();
      ++it;
    }

  }
}

void TableRow::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
  cax = offsetX + location.getLeft(winWidth);
  cay = offsetY + location.getBot(winHeight);
  cbx = offsetX + location.getRight(winWidth);
  cby = offsetY + location.getTop(winHeight);

  getRect();
}

void TableRow::getRect() {
  int chx = cax;

  auto it = data.begin();

  while (it != data.end()) {
    (*it)->getRect(cbx - cax, cby - cay, chx, cay);
    //chx = (*it)->cbx;
    ++it;
  }
}

int TableRow::mouseEnter(int state) {
  auto it = data.end();

  int bstate = 0;

  while (it != data.begin() && !(state & 2)) {
    --it;
    if (!(*it)->toDelete) {
      bstate |= (*it)->mouseEnter(state);
    }
  }
  return bstate;
}
int TableRow::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  auto it = data.end();

  int state = 0;

  while (it != data.begin() && !(state & 2)) {
    --it;
    if (!(*it)->toDelete) {
      state |= (*it)->mouseMoved(mx, my, ox, oy, down);
    }
  }
  return state;
}

int TableRow::activateElement(GUIElement* id) {
  auto it = data.end();

  int bstate = 0;

  while (it != data.begin()) {
    --it;
    if (!(*it)->toDelete) {
      bstate |= (*it)->activateElement(id);
    }
  }
  return bstate;
}

int TableRow::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  auto it = data.end();

  int state = 0;

  while (it != data.begin() && !(state & 2)) {
    --it;
    if (!(*it)->toDelete) {
      state |= (*it)->guiEvent(evt, mx, my, down);
    }
  }
  return state;
}

TableRow::~TableRow() {
  while (data.size()) {
    if (data.front() != NULL) {
      delete data.front();
      data.pop_front();
    }
  }
}

GUIElement* TableRow::getElementById(string id) {
  if (name == id) {
    return this;
  }
  else {
    GUIElement* res = NULL;

    auto it = data.begin();

    while (it != data.end() && res == NULL) {
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

void Table::render(set<key_location>& down) {
  glEnable(GL_SCISSOR_TEST);
  glScissor(cax, cay, cbx - cax, cby - cay);

  auto it = data.begin();

  while (it != data.end()) {
    if ((*it)->toDelete) {
      delete *it;
      auto it2 = it;
      ++it;
      data.erase(it2);
    }
    else {
      //glPushMatrix();
      //glViewport(cax, cay, cbx - cax, cby - cay);
      //glScissor((*it)->cax, (*it)->cay, (*it)->cbx - (*it)->cax, (*it)->cby - (*it)->cay);
      (*it)->render(down);
      //glPopMatrix();
      ++it;
    }

  }

  glBegin(GL_QUADS);
  setColor(activeColor);
  glVertex2d(cbx - 15, sba);
  glVertex2d(cbx - 5, sba);
  glVertex2d(cbx - 5, sbb);
  glVertex2d(cbx - 15, sbb);
  glEnd();

  Graphics::resetViewport();
}

void Table::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
  cax = offsetX + location.getLeft(winWidth);
  cay = offsetY + location.getBot(winHeight);
  cbx = offsetX + location.getRight(winWidth);
  cby = offsetY + location.getTop(winHeight);

  getRect();
}

void Table::getRect() {
  if(data.size()) {
    

    height = 0;

    for (auto&& it : data) {
      height += it->getHeight(cby - cay);
    }
    int minscroll = cby - cay - height; //lower most
    if (scroll < minscroll) {
      scroll = minscroll;
    }
    int maxscroll = 0; //upper most
    if (scroll > maxscroll) {
      scroll = maxscroll;
    }
    
    int chy = cby - scroll;

    for (auto&& it : data) {
      it->getRect(cbx - cax - 20, cby - cay, cax, chy - it->getHeight(cby - cay));
      chy = it->cay;
    }

    if(maxscroll != minscroll) {
      sbb = cby - 5 + ((cby - cay - 10) * scroll)/(height);
      sba = cby - 5 + ((cby - cay - 10) * (cay - cby + scroll)) / height;
    }
    else {
      sba = (cay + cby) / 2;
      sbb = (cay + cby) / 2;
    }

    if (sbb - sba < 10) {
      int sbc = (sba + sbb) / 2;
      sba = sbc - 5;
      sbb = sbc + 5;
    }
  }
}

int Table::mouseEnter(int state) {
  auto it = data.end();

  int bstate = 0;

  while (it != data.begin() && !(state & 2)) {
    --it;
    if (!(*it)->toDelete) {
      bstate |= (*it)->mouseEnter(state);
    }
  }
  return bstate;
}

int Table::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  auto it = data.end();

  int state = 0;

  while (it != data.begin() && !(state & 2)) {
    --it;
    if (!(*it)->toDelete) {
      state |= (*it)->mouseMoved(mx, my, ox, oy, down);
    }
  }
  return state;
}

int Table::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  if (evt._key._type == key::type_wheel) {
    if (isIn(mx, my)) {
      scroll += 30 * evt._key._keycode;
      getRect();
      return 1;
    }
    return 0;
  } else {
    auto it = data.end();

    int state = 0;

    while (it != data.begin() && !(state & 2)) {
      --it;
      if (!(*it)->toDelete) {
        state |= (*it)->guiEvent(evt, mx, my, down);
      }
    }
    return state;
  }
}

int Table::activateElement(GUIElement* id) {
  auto it = data.end();

  int bstate = 0;

  while (it != data.begin()) {
    --it;
    if (!(*it)->toDelete) {
      bstate |= (*it)->activateElement(id);
    }
  }
  return bstate;
}

GUIElement* Table::getElementById(string id) {
  if (name == id) {
    return this;
  }
  else {
    GUIElement* res = NULL;

    auto it = data.begin();

    while (it != data.end() && res == NULL) {
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

Table::~Table() {
  while (data.size()) {
    if (data.front() != NULL) {
      delete data.front();
      data.pop_front();
    }
  }
}