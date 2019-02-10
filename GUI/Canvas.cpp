#include "Canvas.h"

int Canvas::mouseEnter(int state) {
  return managers.mouseEntryManager(this, state);
}

int Canvas::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return managers.mouseMoveManager(this, mx, my, ox, oy, down);
}

int Canvas::guiEvent(gui_event& evt, int mx, int my, set<key_location>& down) {
  return managers.guiEventManager(this, evt, mx, my, down);
}

void Canvas::render(set<key_location>& down) {
  managers.renderManager(this, cax, cay, cbx, cby, down);
}

Canvas::~Canvas() {

}