#pragma once

#include "GUIElement.h"


namespace NGin {
  namespace Graphics {

    class Image : public GUIElement {
    public:
      GLuint texture;
      string filename;
      int align;
      int w, h;
      Image(string lname, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata, string lfilename, int lalign) :
        GUIElement(lname, llocation, lbg, lactive, ltextColor, ldata) {
        filename = lfilename;
        texture = 0;
        align = lalign;
      }
      void render(set<key_location>& down);
      virtual ~Image() {
        glDeleteTextures(1, &texture);
      }
    };
  }
}