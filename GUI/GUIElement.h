#pragma once

#include "GraphicsUtils.h"


namespace NGin {
  namespace Graphics {

    class GUIElement : public enable_shared_from_this<GUIElement> {
    public:
      string name;
      int cax, cay, cbx, cby; //Not for long term storage.
      LocationData location;

      colorargb bgColor;
      colorargb activeColor;
      colorargb textColor;

      bool active;
      void* data;

      bool toDelete = false;

      GUIElement(string name, LocationData llocation, colorargb lbg, colorargb lactive, colorargb ltextColor, void* ldata);
      virtual bool isIn(int mx, int my);

      //Calculate and set new size
      virtual void getRect(int winWidth, int winHeight, int offsetX, int offsetY);

      //Update componenets
      virtual void getRect();

      //Calculate new values (dont update)
      virtual int getWidth(int winWidth);
      virtual int getHeight(int winHeight);

      //Calculate current values
      virtual int getWidth();
      virtual int getHeight();

      virtual int mouseEnter(int state);
      virtual int mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down);
      virtual int guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down);
      virtual void render(set<key_location>& down);
      virtual shared_ptr<GUIElement> getElementById(string id);
      virtual int activateElement(shared_ptr<GUIElement> id);

      virtual void deleteElement(shared_ptr<GUIElement> elem, bool hard);
      void deleteElement(shared_ptr<GUIElement> elem);
      virtual ~GUIElement();
    };
  }
}