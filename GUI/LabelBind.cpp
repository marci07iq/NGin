#include "LabelBind.h"


namespace NGin {
  namespace Graphics {

    void LabelBind::render(set<key_location>& down) {
      if (text != NULL) {
        renderBitmapString((cax + ((align) ? cbx : cax)) / 2, (cay + cby) / 2, text->tostr(), textColor, align);
      } else {
        LOG LERROR GRAPHICS name << " missing text!" END;
      }
    }

    LabelBind::~LabelBind() {
      delete text;
    }
  }
}