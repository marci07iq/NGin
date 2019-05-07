#include "Table.h"

void TableRow::render(set<key_location>& down) {
  glEnable(GL_SCISSOR_TEST);
  glScissor(cax, cay, cbx - cax, cby - cay);

  if (bgColor > 0xffffff) { //Has alpha
    Gll::gllBegin(Gll::GLL_QUADS);
    setColor(bgColor);
    Gll::gllVertex(cax, cay);
    Gll::gllVertex(cbx, cay);
    Gll::gllVertex(cbx, cby);
    Gll::gllVertex(cax, cby);
    Gll::gllEnd();
  }


  auto it = data.begin();

  while (it != data.end()) {
    (*it)->render(down);
    ++it;
  }


  Gll::gllBegin(Gll::GLL_QUADS);
  setColor(activeColor);
  Gll::gllVertex(sab, cay + 5);
  Gll::gllVertex(sab, cay + 15);
  Gll::gllVertex(sbb, cay + 15);
  Gll::gllVertex(sbb, cay + 5);
  Gll::gllEnd();

  NGin::Graphics::resetViewport();
}

void TableRow::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
  cax = offsetX + location.getLeft(winWidth);
  cay = offsetY + location.getBot(winHeight);
  cbx = offsetX + location.getRight(winWidth);
  cby = offsetY + location.getTop(winHeight);

  getRect();
}

void TableRow::getRect() {
  if (data.size()) {
    width = 0;

    for (auto&& it : data) {
      width += it->getWidth(cbx - cax);
    }
    int minscroll = cbx - cax - width; //lower most
    if (scroll < minscroll) {
      scroll = minscroll;
    }
    int maxscroll = 0; //upper most
    if (scroll > maxscroll) {
      scroll = maxscroll;
    }

    int chx = scroll;

    for (auto&& it : data) {
      it->getRect(cbx - cax, cby - cay - 20, chx, cay + 20);
      chx = it->cbx;
    }

    pair<float, float> scrollBarPos = scrollBar(width, -scroll, cax, cbx, 10, 5);

    sab = scrollBarPos.first;
    sbb = scrollBarPos.second;
  }
}

int TableRow::mouseEnter(int state) {
  auto it = data.end();

  int bstate = 0;

  while (it != data.begin() && !(state & 2)) {
    --it;

      bstate |= (*it)->mouseEnter(state);

  }
  return bstate;
}
int TableRow::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  auto it = data.end();

  int state = 0;

  while (it != data.begin() && !(state & 2)) {
    --it;
      state |= (*it)->mouseMoved(mx, my, ox, oy, down);
    
  }
  return state;
}

void TableRow::deleteElement(shared_ptr<GUIElement> elem) {
  data.remove(static_pointer_cast<TableRow, GUIElement>(elem));
}

int TableRow::activateElement(shared_ptr<GUIElement> id) {
  auto it = data.end();

  int bstate = 0;

  while (it != data.begin()) {
    --it;
      bstate |= (*it)->activateElement(id);
    
  }
  return bstate;
}

int TableRow::guiEvent(gui_event& evt, int mx, int my, set<key_location>& down) {
  if (evt._key._type == key::type_wheel) {
    if (!evt.captured && isIn(mx, my)) {
      evt.captured = true;
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
      state |= (*it)->guiEvent(evt, mx, my, down);
    }
    return state;
  }
}

TableRow::~TableRow() {

}

shared_ptr<GUIElement> TableRow::getElementById(string id) {
  if (name == id) {
    return shared_from_this();
  }
  else {
    shared_ptr<GUIElement> res = NULL;

    auto it = data.begin();

    while (it != data.end() && res == NULL) {
        shared_ptr<GUIElement> e = (*it)->getElementById(id);

        if (e != NULL) {
          res = e;
        }

      ++it;
    }
    return res;
  }
}

void Table::render(set<key_location>& down) {
  glEnable(GL_SCISSOR_TEST);
  glScissor(cax, cay, cbx - cax, cby - cay);

  if (bgColor > 0xffffff) { //Has alpha
    Gll::gllBegin(Gll::GLL_QUADS);
    setColor(bgColor);
    Gll::gllVertex(cax, cay);
    Gll::gllVertex(cbx, cay);
    Gll::gllVertex(cbx, cby);
    Gll::gllVertex(cax, cby);
    Gll::gllEnd();
  }


  auto it = data.begin();

  while (it != data.end()) {
    (*it)->render(down);
    ++it;
  }


  Gll::gllBegin(Gll::GLL_QUADS);
  setColor(activeColor);
  Gll::gllVertex(cbx - 15, sba);
  Gll::gllVertex(cbx - 5, sba);
  Gll::gllVertex(cbx - 5, sbb);
  Gll::gllVertex(cbx - 15, sbb);
  Gll::gllEnd();

  NGin::Graphics::resetViewport();
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

    pair<float, float> scrollBarPos = scrollBar(height, scroll- minscroll, cay, cby, 10, 5);

    sba = scrollBarPos.first;
    sbb = scrollBarPos.second;
  }
}

int Table::mouseEnter(int state) {
  auto it = data.end();

  int bstate = 0;

  while (it != data.begin() && !(state & 2)) {
    --it;
      bstate |= (*it)->mouseEnter(state);
  }
  return bstate;
}

int Table::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  auto it = data.end();

  int state = 0;

  while (it != data.begin() && !(state & 2)) {
    --it;
      state |= (*it)->mouseMoved(mx, my, ox, oy, down);
  }
  return state;
}

int Table::guiEvent(gui_event& evt, int mx, int my, set<key_location>& down) {
  if (evt._key._type == key::type_wheel) {
    if (!evt.captured && isIn(mx, my)) {
      evt.captured = true;
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
        state |= (*it)->guiEvent(evt, mx, my, down);
    }
    return state;
  }
}

int Table::activateElement(shared_ptr<GUIElement> id) {
  auto it = data.end();

  int bstate = 0;

  while (it != data.begin()) {
    --it;
    bstate |= (*it)->activateElement(id);
  }
  return bstate;
}

shared_ptr<GUIElement> Table::getElementById(string id) {
  if (name == id) {
    return shared_from_this();
  }
  else {
    shared_ptr<GUIElement> res = NULL;

    auto it = data.begin();

    while (it != data.end() && res == NULL) {
        shared_ptr<GUIElement> e = (*it)->getElementById(id);

        if (e != NULL) {
          res = e;
        }

      ++it;
    }
    return res;
  }
}

void Table::deleteElement(shared_ptr<GUIElement> elem) {
  data.remove(static_pointer_cast<TableRow, GUIElement>(elem));
}

Table::~Table() {
  
}