#include "Canvas.h"

int Canvas::mouseEnter(int state) {
  return managers.mouseEntryManager(state);
}

int Canvas::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return managers.mouseMoveManager(mx, my, ox, oy, down);
}

int Canvas::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return managers.guiEventManager(evt, mx, my, down, this);
}

void Canvas::render(set<key_location>& down) {
  managers.renderManager(cax, cay, cbx, cby, down);
}

Canvas::~Canvas() {

}