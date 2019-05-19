#pragma once

#include "GUIElement.h"


namespace NGin {
  namespace Graphics {

    class Panel : public GUIElement {
    public:
      list<shared_ptr<GUIElement>> elements;
      Panel(string lname, LocationData llocation, colorargb lbg, void* ldata) :
        GUIElement(lname, llocation, lbg, 0, 0, ldata) {
      }
      void transformCoordinates(int& mx, int& my);
      int mouseEnter(int state);
      int mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down);
      int guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down);
      void render(set<key_location>& down);

      void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
      void getRect();

      int activateElement(shared_ptr<GUIElement> id);
      void deleteElement(shared_ptr<GUIElement> elem, bool hard);

      shared_ptr<GUIElement> getElementById(string id);
      /*bool keyPressed(unsigned char key, int mx, int my);
      bool specialPressed(int key, int mx, int my);
      bool mouseClicked(int mx, int my);
      void render(set<key_location>& down);*/
      virtual ~Panel();
    };
  }
}