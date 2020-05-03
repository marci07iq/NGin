#pragma once

#include "GUIElement.h"
//#include "LabelBind.h"


namespace NGin {
  namespace Graphics {

    class TableRow : public GUIElement {
    public:
      list<shared_ptr<GUIElement>> data;

      int width;

      float scroll;
      int sab, sbb; //scroll bar left - right

      TableRow(string lname, LocationData llocation, colorargb lbg, colorargb lactive, void* ldata) :
        GUIElement(lname, llocation, lbg, lactive, 0, ldata) {
      }
      void render(set<key_location>& down);
      void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
      void getRect();
      int mouseEnter(int state);
      int mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down);
      int guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down);

      shared_ptr<GUIElement> getElementById(string id);

      int activateElement(shared_ptr<GUIElement> id);
      void deleteElement(shared_ptr<GUIElement> elem, bool hard);

      virtual ~TableRow();
    };

    class Table : public GUIElement {
    public:
      list<shared_ptr<GUIElement>> data;
      int height;

      float scroll;
      int sba, sbb; //scroll bar top - bottom

      Table(string lname, LocationData llocation, colorargb lbg, colorargb lactive, void* ldata) :
        GUIElement(lname, llocation, lbg, lactive, 0, ldata) {
      }
      void render(set<key_location>& down);
      void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
      void getRect();
      int mouseEnter(int state);
      int mouseMoved(int mx, int my, int ox, int oy, std::set<key_location>& down);
      int guiEvent(gui_event& evt, int mx, int my, std::set<key_location>& down);

      shared_ptr<GUIElement> getElementById(string id);

      int activateElement(shared_ptr<GUIElement> id);
      void deleteElement(shared_ptr<GUIElement> elem, bool hard);

      virtual ~Table();
    };
  }
}