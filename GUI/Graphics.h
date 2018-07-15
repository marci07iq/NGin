#pragma once

#include "Button.h"
#include "Checkbox.h"
#include "TextInput.h"
#include "Label.h"
#include "LabelBind.h"
#include "Panel.h"
#include "Canvas.h"
#include "Plot.h"
#include "Slider.h"
#include "Container.h"
#include "Table.h"
#include "Image.h"
#include "ControlSetting.h"
#include "Popup.h"

namespace Graphics {
  void defaultRenderManagerL();
  void defaultRenderManagerNL();
  extern RenderManager defaultRenderManager;

  void defaultResizeManagerL(int x, int y);
  void defaultResizeManagerNL(int x, int y);
  extern ResizeManager defaultResizeManager;

  void defaultKeyManagerL(unsigned char key, int x, int y);
  void defaultKeyManagerNL(unsigned char key, int x, int y);
  extern KeyManager defaultKeyManager;

  void defaultSpecialKeyManagerL(int key, int x, int y);
  void defaultSpecialKeyManagerNL(int key, int x, int y);
  extern SpecialKeyManager defaultSpecialKeyManager;

  void defaultKeyUpManagerL(unsigned char key, int x, int y);
  void defaultKeyUpManagerNL(unsigned char key, int x, int y);
  extern KeyManager defaultKeyUpManager;

  void defaultSpecialKeyUpManagerL(int key, int x, int y);
  void defaultSpecialKeyUpManagerNL(int key, int x, int y);
  extern SpecialKeyManager defaultSpecialKeyUpManager;

  void defaultMouseEntryManagerL(int state);
  void defaultMouseEntryManagerNL(int state);
  extern MouseEntryManager defaultMouseEntryManager;

  void defaultMouseMoveManagerL(int x, int y);
  void defaultMouseMoveManagerNL(int x, int y);
  extern MouseMoveManager defaultMouseMoveManager;

  void defaultMouseClickManagerL(int button, int state, int x, int y);
  void defaultMouseClickManagerNL(int button, int state, int x, int y);
  extern MouseClickManager defaultMouseClickManager;

  void defaultMouseWheelManagerL(int keys, int delta, int x, int y);
  void defaultMouseWheelManagerNL(int keys, int delta, int x, int y);
  extern MouseWheelManager defaultMouseWheelManager;


  class GWindow {
  public:
    LocationData location;
    float ax, ay, bx, by;
    int ox, oy;
    int id;
    WindowManagers windowManagers;
    int parent;
    Panel* myPanel;
    void getWin(float pax, float pay, float pbx, float pby);
  };

  typedef GUIElement* ElemHwnd;
  typedef Panel* PanelHwnd;
  typedef Container* ContainerHwnd;
  typedef Button* ButtonHwnd;
  typedef TextInput* TextInputHwnd;
  typedef Label* LabelHwnd;
  typedef LabelBind* LabelBindHwnd;
  typedef Canvas* CanvasHwnd;
  typedef Plot* PlotHwnd;
  typedef Slider* SliderHwnd;
  typedef Checkbox* CheckboxHwnd;
  typedef ControlSetting* ControlHwnd;
  typedef Table* TableHwnd;
  typedef TableRow* TablerowHwnd;
  typedef Image* ImageHwnd;
  typedef GWindow* WinHwnd;

  extern WindowManagers defaultWindowManagers;
  extern map<int, WinHwnd> windows;

  extern map<string, void(*)()> funcs;
  extern set<key_location> keysdown;

  WinHwnd CreateMainWindow(string caption = "", WindowManagers managers = defaultWindowManagers, int width = 100, int height = 100, bool setSize = false, int x = 0, int y = 0, bool setPosition = false);

  WinHwnd SetUpWindow(int id, int parent, LocationData pos, WindowManagers manager);

  int DestroyWindow(WinHwnd id);

  WinHwnd GetWinHwnd(int id);

  int elementMouseEnterManager(WinHwnd id, int mstate);

  int elementMouseMoveManager(WinHwnd id, int x, int y);

  int elementGUIEventManager(WinHwnd id, gui_event evt, int mx, int my, set<key_location>& down);

  void elementResizeManager(WinHwnd id, int width, int height);

  void elementResizeManager(PanelHwnd id, int width, int height);

  void elementRenderManager(WinHwnd id);

  template <typename T> void setName(string name, T func) {
    funcs[name] = reinterpret_cast<void(*)()>(func);
  }

  ButtonHwnd createButton(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, string text, key trigger, ClickCallback clickCallback);
  ButtonHwnd createButton(xml_node<> *me);

  CheckboxHwnd createCheckbox(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, bool checked, CheckCallback checkCallback);
  CheckboxHwnd createCheckbox(xml_node<> *me);
  
  LabelHwnd createLabel(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, string text, int center);
  LabelHwnd createLabel(xml_node<> *me);

  ImageHwnd createImage(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, string text, int align);
  ImageHwnd createImage(xml_node<> *me);

  TextInputHwnd createTextInput(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, string text, TextInputFunc inputCallback, TextValidatorFunc validator = *textValidator);
  TextInputHwnd createTextInput(xml_node<> *me);

  ControlHwnd createControl(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, key selected, int id, ControlInputFunc inputCallback);
  ControlHwnd createControl(xml_node<> *me);

  CanvasHwnd createCanvas(string lname, LocationData location, IWindowManagers managers);

  PlotHwnd createPlot(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor);

  PanelHwnd createPanel(string lname, LocationData location, colorargb bg);
  PanelHwnd createPanel(xml_node<> *me);

  ContainerHwnd createContainer(string lname, LocationData location, colorargb bg);
  ContainerHwnd createContainer(xml_node<> *me);

  SliderHwnd createSlider(string name, LocationData location, colorargb bg, colorargb active, colorargb textColor, colorargb pulledcolor, float min, float max, float value, float quanta, SliderInputFunc clickCallback);
  SliderHwnd createSlider(xml_node<> *me);

  LabelBindHwnd createLabelBind(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, int center);
  LabelBindHwnd createLabelBind(xml_node<> *me);

  TableHwnd createTable(string lname, LocationData location, colorargb bg, colorargb active);
  TableHwnd createTable( xml_node<> *me);

  TablerowHwnd createTableRow(string lname, LocationData location, colorargb bg);
  TablerowHwnd createTableRow(xml_node<> *me);

  ElemHwnd addElement(WinHwnd id, ElemHwnd elem);
  ElemHwnd addElement(PanelHwnd id, ElemHwnd elem);
  ElemHwnd addElement(TableHwnd id, TablerowHwnd elem);
  ElemHwnd addElement(TablerowHwnd id, ElemHwnd elem);

  ElemHwnd createElement(xml_node<> *me);

  void deleteElement(ElemHwnd elemId);

  void deleteElements(PanelHwnd id);
  void deleteElements(TableHwnd id);
  void deleteElements(TablerowHwnd id);
  
  void deleteElements(WinHwnd winId);

  void setElements(PanelHwnd id, xml_node<> *data);
  void setElements(TableHwnd id, xml_node<> *data);
  void setElements(TablerowHwnd id, xml_node<> *data);

  void setElements(PanelHwnd id, string filename);

  ElemHwnd getElementById(PanelHwnd pId, string id);
  ElemHwnd getElementById(WinHwnd winId, string id);
  ElemHwnd getElementById(string id);

  void activateElement(PanelHwnd pId, ElemHwnd id);
  void activateElement(WinHwnd winId, ElemHwnd id);
  void activateElement(ElemHwnd id);
}