#include "Canvas.h"

namespace NGin {
  namespace Graphics {
    int Canvas::mouseEnter(int state) {
      return managers.mouseEntryManager(static_pointer_cast<Canvas, GUIElement>(shared_from_this()), state);
    }

    int Canvas::mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down) {
      return managers.mouseMoveManager(static_pointer_cast<Canvas, GUIElement>(shared_from_this()), mx, my, ox, oy, down);
    }

    int Canvas::guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down) {
      return managers.guiEventManager(static_pointer_cast<Canvas, GUIElement>(shared_from_this()), evt, mx, my, down);
    }

    void Canvas::render(set<key_location>& down) {
      managers.renderManager(static_pointer_cast<Canvas, GUIElement>(shared_from_this()), cax, cay, cbx, cby, down);
    }

    Canvas::~Canvas() {

    }
  }
}