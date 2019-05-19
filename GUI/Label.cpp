#include "Label.h"


namespace NGin {
  namespace Graphics {

    void Label::render(set<key_location>& down) {
      renderBitmapString((cax + ((align) ? cbx : cax)) / 2, (cay + cby) / 2, text, textColor, align);
      //shapesPrintf(0, 0, text.c_str());
    }

    Label::~Label() {

    }
  }
}