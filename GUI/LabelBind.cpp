#include "LabelBind.h"

void LabelBind::render(set<key_location>& down) {
  //if(text != NULL) {
    renderBitmapString((cax + ((align) ? cbx : cax)) / 2, (cay + cby) / 2, text->tostr(), textColor, align);
  //}
}

LabelBind::~LabelBind() {
  delete text;
}