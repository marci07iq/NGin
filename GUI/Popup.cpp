#include "Popup.h"

/*Coordinate to_mincorner(int topleftx, int toplefty, Coordinate dimension) {
  return Coordinate(0, - dimension.rely, topleftx, toplefty - dimension.y);
}
Coordinate to_maxcorner(int topleftx, int toplefty, Coordinate dimension) {
  return Coordinate(dimension.relx, 0, topleftx + dimension.x, toplefty);
}

void Popup::transformCoordinates(int &mx, int &my) {
}
void Popup::render(set<key_location>& down) {
  if (element->toDelete) {
    delete element;
  } else {
    element->render();
  }
}

void Popup::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
  cax = offsetX + mincorner.GetX(winWidth);
  cay = offsetY + mincorner.GetY(winHeight);
  cbx = offsetX + maxcorner.GetX(winWidth);
  cby = offsetY + maxcorner.GetY(winHeight);

  if (element != NULL) {
    element->getRect(cbx - cax, cby - cay, cax, cay);
  }
}

void Popup::moveTo(int topleftx, int toplefty) {
  maxcorner = to_maxcorner(topleftx, toplefty, sizec);
  mincorner = to_mincorner(topleftx, toplefty, sizec);
 
  int offsetx = topleftx - topleftc.x;
  int offsety = toplefty - topleftc.y;

  topleftc = Coordinate(0,0,topleftx,toplefty);

  cax += offsetx;
  cbx += offsetx;
  cay += offsety;
  cby += offsety;
}

void Popup::deleteElements(bool hard) {
  if (element != NULL) {
    element->toDelete = true;
    if (hard) {
      delete element;
    }
  }
}

GUIElement* Popup::getElementById(string id) {
  if (name == id) {
    return this;
  } else {
    GUIElement* res = NULL;
    if (element != NULL) {
      GUIElement* e = element->getElementById(id);
    }
    return res;
  }
}

Popup::~Popup() {
  if (element != NULL) {
    delete element;
  }
}*/