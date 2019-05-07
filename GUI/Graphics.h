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

namespace NGin {
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
      shared_ptr<Panel> myPanel;
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

    typedef shared_ptr<GUIElement> ElemHwnd;
    typedef shared_ptr<Panel> PanelHwnd;
    typedef shared_ptr<Container> ContainerHwnd;
    typedef shared_ptr<Button> ButtonHwnd;
    typedef shared_ptr<IconButton> IconButtonHwnd;
    typedef shared_ptr<TextInput> TextInputHwnd;
    typedef shared_ptr<Label> LabelHwnd;
    typedef shared_ptr<LabelBind> LabelBindHwnd;
    typedef shared_ptr<Canvas> CanvasHwnd;
    typedef shared_ptr<Plot> PlotHwnd;
    typedef shared_ptr<Slider> SliderHwnd;
    typedef shared_ptr<Checkbox> CheckboxHwnd;
    typedef shared_ptr<ControlSetting> ControlHwnd;
    typedef shared_ptr<Table> TableHwnd;
    typedef shared_ptr<TableRow> TablerowHwnd;
    typedef shared_ptr<Image> ImageHwnd;
    typedef GWindow* WinHwnd;

    extern WinHwnd current; //Set when any window specific callback is called.
    extern int oldMouseX, oldMouseY;

    extern WindowManagers defaultWindowManagers;
    extern map<RawWinHwnd, WinHwnd> windows;

    extern map<string, void(*)()> funcs;
    extern set<key_location> keysdown;

    extern bool redrawFrame;
    extern list<WinHwnd> wdeleteQueue;
    extern list<pair<pair<ElemHwnd, ElemHwnd>, bool>> edeleteQueue;

    void initGraphics();

    void requestRedraw();

    void cleanQueues();

    void mainLoop(bool needsWindows = true);

    void shutdownGraphics();

    void forceShutdown();

    typedef void(*WinCreateManager)(NGin::Graphics::WinHwnd);
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

    template <typename T, typename... ARG>
    shared_ptr<T> createGUI_T(ARG... args) {
      return make_shared<T>(args...);
    }

    ButtonHwnd createButton(xml_node<>* me);

    IconButtonHwnd createIconButton(xml_node<>* me);

    CheckboxHwnd createCheckbox(xml_node<>* me);

    LabelHwnd createLabel(xml_node<>* me);

    ImageHwnd createImage(xml_node<>* me);

    TextInputHwnd createTextInput(xml_node<>* me);

    ControlHwnd createControl(xml_node<>* me);

    PanelHwnd createPanel(xml_node<>* me);

    TableHwnd createTable(xml_node<>* me);

    TablerowHwnd createTableRow(xml_node<>* me);

    ContainerHwnd createContainer(xml_node<>* me);

    SliderHwnd createSlider(xml_node<>* me);

    LabelBindHwnd createLabelBind(xml_node<>* me);

    ElemHwnd addElement(WinHwnd id, ElemHwnd elem);
    ElemHwnd addElement(PanelHwnd id, ElemHwnd elem);
    ElemHwnd addElement(TableHwnd id, ElemHwnd elem);
    ElemHwnd addElement(TablerowHwnd id, ElemHwnd elem);

    ElemHwnd createElement(xml_node<>* me);

    void deleteElement(ElemHwnd elemId, ElemHwnd elemFrom);

    void deleteElements(PanelHwnd id);
    void deleteElements(TableHwnd id);
    void deleteElements(TablerowHwnd id);

    void deleteElements(WinHwnd winId);

    void setElements(PanelHwnd id, xml_node<>* data);
    void setElements(TableHwnd id, xml_node<>* data);
    void setElements(TablerowHwnd id, xml_node<>* data);

    void setElements(PanelHwnd id, string filename);

    ElemHwnd getElementById(PanelHwnd pId, string id);
    ElemHwnd getElementById(WinHwnd winId, string id);
    ElemHwnd getElementById(string id);

    void activateElement(PanelHwnd pId, ElemHwnd id);
    void activateElement(WinHwnd winId, ElemHwnd id);
    void activateElement(ElemHwnd id);
  }
}