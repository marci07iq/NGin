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
  typedef GLFWwindow* RawWinHwnd; //type passed to underlying graphics lib


  class GWindow {
  public:
    //LocationData location;
    int oldMouseX, oldMouseY;
    int width, height;
    void rescanSize();
    RawWinHwnd rawHwnd;
    WindowManagers windowManagers;
    Panel* myPanel;
    bool autoRedraw = false; //So other thread redraws can update
    //void getWin(float pax, float pay, float pbx, float pby);
    ~GWindow();
  };

  void defaultRenderManagerL();
  void defaultRenderManagerNL();
  extern RenderManager defaultRenderManager;

  void rawResizeManager(GLFWwindow* window, int width, int height);
  void defaultResizeManagerL(int x, int y);
  void defaultResizeManagerNL(int x, int y);
  extern ResizeManager defaultResizeManager;

  void rawKeyManager(GLFWwindow* window, int key, int scancode, int action, int mods); //Handle unmodified hardware key events
  void rawCharManager(GLFWwindow* window, unsigned int codepoint); //Unicode compatible character handling (only hardware keys are saved in down)
  /*void defaultKeyManagerL(unsigned char key, int x, int y);
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
  extern SpecialKeyManager defaultSpecialKeyUpManager;*/

  void rawMouseEntryManager(GLFWwindow* window, int entered);
  void defaultMouseEntryManagerL(int state);
  void defaultMouseEntryManagerNL(int state);
  extern MouseEntryManager defaultMouseEntryManager;

  void rawMouseMoveManager(GLFWwindow* window, double xpos, double ypos);
  void defaultMouseMoveManagerL(int x, int y);
  void defaultMouseMoveManagerNL(int x, int y);
  extern MouseMoveManager defaultMouseMoveManager;

  void rawMouseClickCallback(GLFWwindow* window, int button, int action, int mods);
  void defaultMouseClickManagerL(int button, int state, int x, int y);
  void defaultMouseClickManagerNL(int button, int state, int x, int y);
  extern MouseClickManager defaultMouseClickManager;

  void rawMouseWheelManager(GLFWwindow* window, double xoffset, double yoffset);
  void defaultMouseWheelManagerL(int keys, int delta, int x, int y);
  void defaultMouseWheelManagerNL(int keys, int delta, int x, int y);
  extern MouseWheelManager defaultMouseWheelManager;

  void rawWindowCloseManager(GLFWwindow* window);
  void defaultWindowCloseManagerL();
  void defaultWindowCloseManagerNL();
  extern WindowCloseManager defaultWindowCloseManager;

  int defaultGUIEventManagerL(gui_event& evt, int x, int y, set<key_location>& down);
  int defaultGUIEventManagerNL(gui_event& evt, int x, int y, set<key_location>& down);
  extern GUIEventManager defaultGUIEventManager;

  typedef GUIElement* ElemHwnd;
  typedef Panel* PanelHwnd;
  typedef Container* ContainerHwnd;
  typedef Button* ButtonHwnd;
  typedef IconButton* IconButtonHwnd;
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
  
  extern WinHwnd current; //Set when any window specific callback is called.
  extern int oldMouseX, oldMouseY;

  extern WindowManagers defaultWindowManagers;
  extern map<RawWinHwnd, WinHwnd> windows;

  extern map<string, void(*)()> funcs;
  extern set<key_location> keysdown;

  extern bool redrawFrame;
  extern list<WinHwnd> wdeleteQueue;
  extern list<pair<pair<ElemHwnd, ElemHwnd>,bool>> edeleteQueue;

  void initGraphics();

  void requestRedraw();

  void cleanQueues();

  void mainLoop(bool needsWindows = true);

  void shutdownGraphics();

  void forceShutdown();

  typedef void(*WinCreateManager)(Graphics::WinHwnd);
  struct winCreationData {
    string caption = "";
    WindowManagers managers = defaultWindowManagers;
    int width = 100;
    int height = 100;
    bool setSize = false;
    int x = 0;
    int y = 0;
    bool setPosition = false;
    WinCreateManager onCreated = NULL; //called after createWindow and registring
    WinCreateManager onSetup = NULL; //called after event handlers, color, etc set.
  };
  extern list<winCreationData> wcreateQueue;
  void CreateMainWindow(string caption = "", WindowManagers managers = defaultWindowManagers, int width = 100, int height = 100, bool setSize = false, int x = 0, int y = 0, bool setPosition = false, int additionalFlags = 0, WinCreateManager onCreated = NULL, WinCreateManager onSetup = NULL);
  
  WinHwnd rawCreateMainWindow(winCreationData from); //Do not directy call from callback
  void rawSetUpWindow(RawWinHwnd id, WindowManagers manager); //Do not directy call from callback

  int DestroyWindow(WinHwnd id);

  WinHwnd GetWinHwnd(RawWinHwnd id);

  int elementMouseEnterManager(int mstate);
  int elementMouseMoveManager(int x, int y);

  int elementGUIEventManager(gui_event evt, int mx, int my, set<key_location>& down);

  void elementResizeManager(int width, int height);
  void elementResizeManager(PanelHwnd id, int width, int height);

  void elementRenderManager();

  void elementCloseManager();


  template <typename T> void setName(string name, T func) {
    funcs[name] = reinterpret_cast<void(*)()>(func);
  }


  ButtonHwnd createButton(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data, string text, int trigger, ClickCallback clickCallback);
  ButtonHwnd createButton(xml_node<> *me);

  IconButtonHwnd createIconButton(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data, string text, int trigger, ClickCallback clickCallback, string icon, string ilfFilepath);
  IconButtonHwnd createIconButton(xml_node<> *me);

  CheckboxHwnd createCheckbox(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data, bool checked, CheckCallback checkCallback);
  CheckboxHwnd createCheckbox(xml_node<> *me);

  LabelHwnd createLabel(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data, string text, int align);
  LabelHwnd createLabel(xml_node<> *me);

  ImageHwnd createImage(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data, string text, int align);
  ImageHwnd createImage(xml_node<> *me);

  TextInputHwnd createTextInput(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data, string text, TextInputFunc inputCallback, TextValidatorFunc validator);
  TextInputHwnd createTextInput(xml_node<> *me);

  ControlHwnd createControl(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data, key_config selected, int id, ControlInputFunc inputCallback);
  ControlHwnd createControl(xml_node<> *me);

  CanvasHwnd createCanvas(string lname, LocationData location, IWindowManagers managers, void* ldata);

  PlotHwnd createPlot(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data);

  PanelHwnd createPanel(string lname, LocationData location, colorargb bg, void* data);
  PanelHwnd createPanel(xml_node<> *me);

  TableHwnd createTable(string lname, LocationData location, colorargb bg, colorargb active, void* data);
  TableHwnd createTable(xml_node<> *me);

  TablerowHwnd createTableRow(string lname, LocationData location, colorargb bg, colorargb active, void* data);
  TablerowHwnd createTableRow(xml_node<> *me);

  ContainerHwnd createContainer(string lname, LocationData location, colorargb bg, void* data);
  ContainerHwnd createContainer(xml_node<> *me);

  SliderHwnd createSlider(string name, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data, colorargb pulledcolor, float min, float max, float value, float quanta, SliderInputFunc clickCallback);
  SliderHwnd createSlider(xml_node<> *me);

  LabelBindHwnd createLabelBind(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, void* data, int center);
  LabelBindHwnd createLabelBind(xml_node<> *me);

  ElemHwnd addElement(WinHwnd id, ElemHwnd elem);
  ElemHwnd addElement(PanelHwnd id, ElemHwnd elem);
  ElemHwnd addElement(TableHwnd id, ElemHwnd elem);
  ElemHwnd addElement(TablerowHwnd id, ElemHwnd elem);

  ElemHwnd createElement(xml_node<> *me);

  void deleteElement(ElemHwnd elemId, ElemHwnd elemFrom, bool hard = true);

  void deleteElements(PanelHwnd id, bool hard = true);
  void deleteElements(TableHwnd id, bool hard = true);
  void deleteElements(TablerowHwnd id, bool hard = true);
  
  void deleteElements(WinHwnd winId, bool hard = true);

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