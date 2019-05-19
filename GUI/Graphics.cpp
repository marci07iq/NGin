#include "Graphics.h"

namespace NGin {
  namespace Graphics {
    //Default managers
    RenderManager defaultRenderManager = defaultRenderManagerL;
    ResizeManager defaultResizeManager = defaultResizeManagerL;
    //KeyManager defaultKeyManager= defaultKeyManagerL;
    //SpecialKeyManager defaultSpecialKeyManager= defaultSpecialKeyManagerL;
    //KeyManager defaultKeyUpManager= defaultKeyUpManagerL;
    //SpecialKeyManager defaultSpecialKeyUpManager= defaultSpecialKeyUpManagerL;
    MouseEntryManager defaultMouseEntryManager = defaultMouseEntryManagerL;
    MouseMoveManager defaultMouseMoveManager = defaultMouseMoveManagerL;
    MouseClickManager defaultMouseClickManager = defaultMouseClickManagerL;
    MouseWheelManager defaultMouseWheelManager = defaultMouseWheelManagerL;
    WindowCloseManager defaultWindowCloseManager = defaultWindowCloseManagerL;
    GUIEventManager defaultGUIEventManager = defaultGUIEventManagerL;

    //Variables
    WinHwnd current; //Set when any window specific callback is called.

    std::map<RawWinHwnd, WinHwnd> windows;
    std::map<string, void(*)()> funcs;
    std::set<key_location> keysdown;

    bool redrawFrame = false;
    list<WinHwnd> wdeleteQueue;
    list<winCreationData> wcreateQueue;

    //Engine management
    void initGraphics() {
      if (!glfwInit()) {
        LOG FATAL "GLFW init failed" << endl;
        exit(1);
      }
    }

    void mainLoop(bool needsWindows) {
      while (windows.size() || !needsWindows) {
        //Do delete tasks left over from event handlers (so a button can close its parent window)
        cleanQueues();

        bool redraw = redrawFrame;
        redrawFrame = false;
        for (auto&& it : windows) {
          glfwMakeContextCurrent(it.second->rawHwnd);
          current = it.second;
          if (redraw || it.second->autoRedraw) {
            current->windowManagers.renderManager();
            //cout << "F" << endl;
            //cout << "DRAW" << endl;
            //glfwSwapBuffers(current->rawHwnd);
          }
          if (glfwWindowShouldClose(it.second->rawHwnd)) {
            cout << "Close" << endl;
            DestroyWindow(it.second);
          }
        }

        /* Wait for events */
        glfwWaitEvents();
      }
    }

    void requestRedraw() {
      redrawFrame = true;
      glfwPostEmptyEvent();
    }

    void cleanQueues() { //Never call from event handler. IT WILL CRASH
      current = nullptr; //It should not matter at this point anyway
      while (wdeleteQueue.size()) {
        delete wdeleteQueue.front();
        wdeleteQueue.pop_front();
      }
      while (wcreateQueue.size()) {
        winCreationData from = wcreateQueue.front();
        wcreateQueue.pop_front();
        WinHwnd newWin = rawCreateMainWindow(from);
        if (from.onCreated) {
          from.onCreated(newWin);
        }
        rawSetUpWindow(newWin->rawHwnd, from.managers);
        if (from.onSetup) {
          from.onSetup(newWin);
        }
      }
    }

    void shutdownGraphics() {
      glfwTerminate();
    }

    void forceShutdown() {
      shutdownGraphics();
      exit(0);
    }



    //Window management
    void CreateMainWindow(string caption, WindowManagers managers, int width, int height, bool setsize, int x, int y, bool setposition, int additionalFlags, WinCreateManager onCreated, WinCreateManager onSetup) {
      winCreationData from;
      from.caption = caption;
      from.managers = managers;
      from.width = width;
      from.height = height;
      from.setSize = setsize;
      from.x = x;
      from.y = y;
      from.setPosition = setposition;
      from.onCreated = onCreated;
      from.onSetup = onSetup;
      wcreateQueue.push_back(from);
    }
    WinHwnd rawCreateMainWindow(winCreationData from) {
      if (!from.setSize) {
        from.width = 640;
        from.height = 480;
      }
      RawWinHwnd window = glfwCreateWindow(from.width, from.height, from.caption.c_str(), nullptr, Gll::initOn);
      if (window == nullptr) {
        cout << "FAILED TO CREATE WINDOW" << endl;
      }
      //glfwMakeContextCurrent(window);

      if (from.setPosition) {
        //glutInitWindowPosition(x, y);
        glfwSetWindowPos(window, from.x, from.y);
        //LOG LERROR "Setting window position is not currently supported. Sorry" << endl;
      }

      GWindow* data = new GWindow();

      colorargb bgcolor = getColor("win", "bgcolor");
      PanelHwnd panel = createGUI_T<Panel>("base", fullContainer, bgcolor, nullptr);

      glfwMakeContextCurrent(window);
      glClearColor(((bgcolor & 0xff0000) >> 16) / 255.0, ((bgcolor & 0xff00) >> 8) / 255.0, ((bgcolor & 0xff) >> 0) / 255.0, 1);

      data->rawHwnd = window;
      data->windowManagers = from.managers;
      data->myPanel = panel;
      windows[window] = data;
      current = data;

      data->rescanSize();


      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE | additionalFlags);

      //glutCreateWindow(caption.c_str());

      //glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

      requestRedraw();

      return data;
    }
    void rawSetUpWindow(RawWinHwnd id, WindowManagers manager) {

      /*glutReshapeFunc(manager.resizeManager);
      glutDisplayFunc(manager.renderManager);

      glutKeyboardFunc(manager.keyManager);
      glutSpecialFunc(manager.specialKeyManager);
      glutKeyboardUpFunc(manager.keyUpManager);
      glutSpecialUpFunc(manager.specialUpKeyManager);
      glutEntryFunc(manager.mouseEntryManager);
      glutMotionFunc(manager.mouseMoveManager);
      glutPassiveMotionFunc(manager.mouseMoveManager);
      glutMouseFunc(manager.mouseClickManager);
      glutMouseWheelFunc(manager.mouseWheelManager);
      glutCloseFunc(manager.windowCloseManager);

      glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);*/

      glfwSetWindowSizeCallback(id, rawResizeManager);
      glfwSetCharCallback(id, rawCharManager);
      glfwSetKeyCallback(id, rawKeyManager);
      glfwSetMouseButtonCallback(id, rawMouseClickCallback);
      glfwSetCursorEnterCallback(id, rawMouseEntryManager);
      glfwSetCursorPosCallback(id, rawMouseMoveManager);
      glfwSetScrollCallback(id, rawMouseWheelManager);
      glfwSetWindowCloseCallback(id, rawWindowCloseManager);

      ///data->
    }

    WinHwnd GetWinHwnd(RawWinHwnd id) {
      return windows[id];
    }

    int DestroyWindow(WinHwnd id) {
      defaultWindowCloseManagerNL();

      wdeleteQueue.push_back(id);
      return 0;
    }

    void GWindow::rescanSize() {
      current = this;
      glfwGetWindowSize(rawHwnd, &width, &height);
      windowManagers.resizeManager(width, height);
    }

    GWindow::~GWindow() {
      windows.erase(rawHwnd);
      glfwDestroyWindow(rawHwnd);
    }

    //Default manager implementations
    void defaultRenderManagerL() {
      netlock.lock();

      defaultRenderManagerNL();

      netlock.unlock();
    }
    void defaultRenderManagerNL() {
      glClear(GL_COLOR_BUFFER_BIT);
      resetViewport();

      elementRenderManager();

      glfwSwapBuffers(current->rawHwnd);
    }

    void rawResizeManager(GLFWwindow* window, int width, int height) {
      glfwMakeContextCurrent(window);
      current = GetWinHwnd(window);
      current->width = width;
      current->height = height;
      current->windowManagers.resizeManager(width, height);
    }

    void defaultResizeManagerL(int x, int y) {
      netlock.lock();

      defaultResizeManagerNL(x, y);

      netlock.unlock();
    }
    void defaultResizeManagerNL(int x, int y) {
      int width = x;//glutGet(GLUT_WINDOW_WIDTH);
      int height = y;//glutGet(GLUT_WINDOW_HEIGHT);
      glViewport(0, 0, width, height);

      //glMatrixMode(GL_PROJECTION);
      //glLoadIdentity();

      //glMatrixMode(GL_MODELVIEW);
      //glLoadIdentity();
      elementResizeManager(width, height);

      requestRedraw();
    }

    void rawKeyManager(GLFWwindow* window, int key, int scancode, int action, int mods) {
      glfwMakeContextCurrent(window);
      current = GetWinHwnd(window);
      key_location nkey = key_location(key, key::type_key, current->oldMouseX, current->oldMouseY);
      gui_event::type gEvtType = gui_event::evt_none;

      //cout << key << " " << scancode << endl;

      if (action == GLFW_PRESS) { //Down
        keysdown.insert(nkey);
        gEvtType = gui_event::evt_down; //Send press

        gui_event temp_evt = gui_event(nkey, gEvtType);
        current->windowManagers.guiEventManager(temp_evt, current->oldMouseX, current->oldMouseY, keysdown);

        gEvtType = gui_event::evt_pressed;
      }
      if (action == GLFW_REPEAT) { //Repeat
        keysdown.insert(nkey);
        gEvtType = gui_event::evt_pressed;
      }
      if (action == GLFW_RELEASE) { //Release
        keysdown.erase(nkey);
        gEvtType = gui_event::evt_up;
      }
      gui_event temp_evt = gui_event(nkey, gEvtType);
      current->windowManagers.guiEventManager(temp_evt, current->oldMouseX, current->oldMouseY, keysdown);
    }

    void rawCharManager(GLFWwindow* window, unsigned int codepoint) {
      glfwMakeContextCurrent(window);
      current = GetWinHwnd(window);
      key_location nkey = key_location(codepoint, key::type_char, current->oldMouseX, current->oldMouseY);
      gui_event::type gEvtType = gui_event::evt_pressed; //Send press

      gui_event temp_evt = gui_event(nkey, gEvtType);
      current->windowManagers.guiEventManager(temp_evt, current->oldMouseX, current->oldMouseY, keysdown);
    }

    /*void defaultKeyManagerL(unsigned char keyc, int x, int y) {
      netlock.lock();

      defaultKeyManagerNL(keyc, x, y);

      netlock.unlock();
    }
    void defaultKeyManagerNL(unsigned char keyc, int x, int y) {
      key_location keyd;
      keyd.fromKey(tolower(keyc));
      keyd.setLocation(x, y);
      if (!keysdown.count(keyd)) { //If new key, down
        keysdown.insert(keyd);
        if (1 & elementGUIEventManager(current, gui_event(keyd, gui_event::evt_down), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
          requestRedraw();
        }
      }
      //Always pressed
      if (1 & elementGUIEventManager(current, gui_event(keyd, gui_event::evt_pressed), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
        requestRedraw();
      }
    }

    void defaultSpecialKeyManagerL(int keyc, int x, int y) {
      netlock.lock();

      defaultSpecialKeyManagerNL(keyc, x, y);

      netlock.unlock();
    }
    void defaultSpecialKeyManagerNL(int keyc, int x, int y) {
      key_location keyd;
      keyd.fromSpecial(keyc);
      keyd.setLocation(x, y);
      if (!keysdown.count(keyd)) { //If new key, down
        keysdown.insert(keyd);
        if (1 & elementGUIEventManager(current, gui_event(keyd, gui_event::evt_down), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
          requestRedraw();
        }
      }
      //Always pressed
      if (1 & elementGUIEventManager(current, gui_event(keyd, gui_event::evt_pressed), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
        requestRedraw();
      }
    }

    void defaultKeyUpManagerL(unsigned char keyc, int x, int y) {
      netlock.lock();

      defaultKeyUpManagerNL(keyc,x, y);

      netlock.unlock();
    }
    void defaultKeyUpManagerNL(unsigned char keyc, int x, int y) {
      key_location keyd;
      keyd.fromKey(tolower(keyc));
      keyd.setLocation(x, y);
      keysdown.erase(keyd);
      if (1 & elementGUIEventManager(current, gui_event(keyd, gui_event::evt_up), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
        requestRedraw();
      }
    }

    void defaultSpecialKeyUpManagerL(int keyc, int x, int y) {
      netlock.lock();

      defaultSpecialKeyUpManagerNL(keyc, x, y);

      netlock.unlock();
    }
    void defaultSpecialKeyUpManagerNL(int keyc, int x, int y) {
      key_location keyd;
      keyd.fromSpecial(keyc);
      keyd.setLocation(x, y);
      keysdown.erase(keyd);
      if (1 & elementGUIEventManager(current, gui_event(keyd, gui_event::evt_up), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
        requestRedraw();
      }
    }*/

    void rawMouseEntryManager(GLFWwindow* window, int entered) {
      glfwMakeContextCurrent(window);
      current = GetWinHwnd(window);
      current->windowManagers.mouseEntryManager(entered);
    }

    void defaultMouseEntryManagerL(int state) {
      netlock.lock();

      defaultMouseEntryManagerNL(state);

      netlock.unlock();
    }
    void defaultMouseEntryManagerNL(int state) {
      if (1 & elementMouseEnterManager(state)) {
        requestRedraw();
      }
    }

    void rawMouseMoveManager(GLFWwindow* window, double xpos, double ypos) {
      glfwMakeContextCurrent(window);
      current = GetWinHwnd(window);
      current->windowManagers.mouseMoveManager(xpos, current->height - ypos);
    }

    void defaultMouseMoveManagerL(int x, int y) {
      netlock.lock();

      defaultMouseMoveManagerNL(x, y);

      netlock.unlock();
    }
    void defaultMouseMoveManagerNL(int x, int y) {
      if (1 & elementMouseMoveManager(x, y)) {
        requestRedraw();
      }
    }

    void rawMouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
      glfwMakeContextCurrent(window);
      current = GetWinHwnd(window);
      current->windowManagers.mouseClickManager(button, action, current->oldMouseX, current->oldMouseY);
    }

    void defaultMouseClickManagerL(int button, int state, int x, int y) {
      netlock.lock();

      defaultMouseClickManagerNL(button, state, x, y);

      netlock.unlock();
    }
    void defaultMouseClickManagerNL(int button, int state, int x, int y) {
      key_location keyd;
      keyd.fromMouse(button);
      keyd.setLocation(x, y);
      bool captured = false;
      if (state == 1) { //Down
        if (!keysdown.count(keyd)) { //If new key, down
          keysdown.insert(keyd);
          gui_event newDownEvt = gui_event(keyd, gui_event::evt_down);
          if (1 & elementGUIEventManager(newDownEvt, x, y, keysdown)) {
            requestRedraw();
          }
          captured |= newDownEvt.captured;
        }
        //Always pressed
        gui_event newPressEvt = gui_event(keyd, gui_event::evt_pressed);
        newPressEvt.captured = captured;
        if (1 & elementGUIEventManager(newPressEvt, x, y, keysdown)) {
          requestRedraw();
        }
      }
      if (state == 0) { //Up
        keysdown.erase(keyd);
        if (1 & elementGUIEventManager(gui_event(keyd, gui_event::evt_up), x, y, keysdown)) {
          requestRedraw();
        }
      }
    }

    void rawMouseWheelManager(GLFWwindow* window, double xoffset, double yoffset) {
      glfwMakeContextCurrent(window);
      current = GetWinHwnd(window);
      current->windowManagers.mouseWheelManager(0, yoffset, current->oldMouseX, current->oldMouseY);
    }

    void defaultMouseWheelManagerL(int state, int delta, int x, int y) {
      netlock.lock();

      defaultMouseWheelManagerNL(state, delta, x, y);

      netlock.unlock();
    }
    void defaultMouseWheelManagerNL(int state, int delta, int x, int y) {
      key_location keyd;
      keyd.fromWheel(delta);
      keyd.setLocation(x, y);
      if (1 & elementGUIEventManager(gui_event(keyd, gui_event::evt_none), x, y, keysdown)) {
        requestRedraw();
      }
    }

    void rawWindowCloseManager(GLFWwindow* window) {
      glfwMakeContextCurrent(window);
      current = GetWinHwnd(window);
      current->windowManagers.windowCloseManager();
    }

    void defaultWindowCloseManagerL() {
      netlock.lock();

      defaultWindowCloseManagerNL();

      netlock.unlock();
    }
    void defaultWindowCloseManagerNL() {

    }

    int defaultGUIEventManagerL(gui_event& evt, int mx, int my, std::set<key_location>& down) {
      netlock.lock();

      int res = defaultGUIEventManagerNL(evt, mx, my, down);
      if (res & 1) {
        requestRedraw();
      }

      netlock.unlock();

      return res;
    }
    int defaultGUIEventManagerNL(gui_event& evt, int mx, int my, std::set<key_location>& down) {
      return elementGUIEventManager(evt, mx, my, down);
    }


    WindowManagers defaultWindowManagers =
      WindowManagers{
        defaultRenderManager,
        defaultResizeManager,
        /*defaultKeyManager,
        defaultSpecialKeyManager,
        defaultKeyUpManager,
        defaultSpecialKeyUpManager,*/
        defaultGUIEventManager,
        defaultMouseEntryManager,
        defaultMouseMoveManager,
        defaultMouseClickManager,
        defaultMouseWheelManager,
        defaultWindowCloseManager,
    };

    //Element managers
    int elementMouseEnterManager(int mstate) {
      return current->myPanel->mouseEnter(mstate);
    }

    int elementMouseMoveManager(int x, int y) {
      int ret = current->myPanel->mouseMoved(x, y, current->oldMouseX, current->oldMouseY, keysdown);
      current->oldMouseX = x;
      current->oldMouseY = y;
      return ret;
    }

    int elementGUIEventManager(gui_event evt, int mx, int my, std::set<key_location>& down) {
      return current->myPanel->guiEvent(evt, mx, my, down);
    }

    void elementResizeManager(int width, int height) {
      return elementResizeManager(current->myPanel, width, height);
    }

    void elementResizeManager(PanelHwnd id, int width, int height) {
      return id->getRect(width, height, 0, 0);
    }

    void elementRenderManager() {
      current->myPanel->render(keysdown);
    }

    void elementCloseManager() {
      deleteElements(current);
    }

    //Element creation
    void setElements(PanelHwnd id, xml_node<>* data) {
      for (xml_node<>* pElem = data->first_node(); pElem; pElem = pElem->next_sibling()) {
        addElement(id, createElement(pElem));
      }
      return;
    }

    void setElements(PanelHwnd id, string filename) {
      xml_document<> doc;

      std::ifstream file(filename);
      std::stringstream buffer;
      buffer << file.rdbuf();
      file.close();
      std::string content(buffer.str());
      doc.parse<0>(&content[0]);

      deleteElements(id);

      setElements(id, doc.first_node("body"));

      //int width, height;
      //glfwGetWindowSize(current->rawHwnd, &width, &height);

      //defaultResizeManagerNL(width, height);
      requestRedraw();
    }

    void setElements(TableHwnd id, xml_node<>* data) {
      for (xml_node<>* pElem = data->first_node(); pElem; pElem = pElem->next_sibling()) {
        addElement(id, createElement(pElem));
      }
    }
    void setElements(TablerowHwnd id, xml_node<>* data) {
      for (xml_node<>* pElem = data->first_node(); pElem; pElem = pElem->next_sibling()) {
        addElement(id, createElement(pElem));
      }
    }

    ElemHwnd createElement(xml_node<>* me) {
      string name = me->name();
      if (name == "location") { //Parameter to div, NOT element
        return nullptr;
      }
      if (name == "button") {
        return createButton(me);
      }
      if (name == "ibutton") {
        return createIconButton(me);
      } else if (name == "checkbox") {
        return createCheckbox(me);
      } else if (name == "label" || name == "text") {
        return createLabel(me);
      } else if (name == "labelbind" || name == "textbind") {
        return createLabelBind(me);
      } else if (name == "container") {
        return createContainer(me);
      } else if (name == "textinput" || name == "input") {
        return createTextInput(me);
      } else if (name == "panel" || name == "div") {
        return createPanel(me);
      } else if (name == "slider") {
        return createSlider(me);
      } else if (name == "config") {
        return createControl(me);
      } else if (name == "table") {
        return createTable(me);
      } else if (name == "tr") {
        return createTableRow(me);
      } else if (name == "image") {
        return createImage(me);
      } else {
        throw 1;
        return nullptr;
      }
    }

    ButtonHwnd createButton(xml_node<>* me) {
      return createGUI_T<Button>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "button", "bgcolor"),
        getColor(me, "button", "activecolor"),
        getColor(me, "button", "textcolor"),
        nullptr,
        me->value(),
        me->first_attribute("trigger") ? strTo<int>(me->first_attribute("trigger")->value()) : -1,
        reinterpret_cast<ClickCallback>(funcs[me->first_attribute("callback")->value()]));
    }

    IconButtonHwnd createIconButton(xml_node<>* me) {
      return createGUI_T<IconButton>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "button", "bgcolor"),
        getColor(me, "button", "activecolor"),
        getColor(me, "button", "textcolor"),
        nullptr,
        me->value(),
        me->first_attribute("trigger") ? strTo<int>(me->first_attribute("trigger")->value()) : -1,
        reinterpret_cast<ClickCallback>(funcs[me->first_attribute("callback")->value()]),
        me->first_attribute("icon")->value(),
        me->first_attribute("ilf") ? me->first_attribute("ilf")->value() : "");
    }

    CheckboxHwnd createCheckbox(xml_node<>* me) {
      return createGUI_T<Checkbox>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "checkbox", "bgcolor"),
        getColor(me, "checkbox", "activecolor"),
        getColor(me, "checkbox", "textcolor"),
        nullptr,
        strTo<bool>(me->value()),
        reinterpret_cast<CheckCallback>(funcs[me->first_attribute("callback")->value()]));
    }

    LabelHwnd createLabel(xml_node<>* me) {
      return createGUI_T<Label>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "label", "bgcolor"),
        getColor(me, "label", "activecolor"),
        getColor(me, "label", "textcolor"),
        nullptr,
        me->value(),
        strTo<int>(me->first_attribute("align")->value()));
    }

    ImageHwnd createImage(xml_node<>* me) {
      return createGUI_T<Image>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "image", "bgcolor"),
        getColor(me, "image", "activecolor"),
        getColor(me, "image", "textcolor"),
        nullptr,
        me->value(),
        strTo<int>(me->first_attribute("align")->value()));
    }

    TextInputHwnd createTextInput(xml_node<>* me) {
      return createGUI_T<TextInput>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "input", "bgcolor"),
        getColor(me, "input", "activecolor"),
        getColor(me, "input", "textcolor"),
        nullptr,
        me->value(),
        reinterpret_cast<TextInputFunc>(funcs[me->first_attribute("inputfunc")->value()]),
        reinterpret_cast<TextValidatorFunc>(funcs[me->first_attribute("validatorfunc")->value()]));
    }

    ControlHwnd createControl(xml_node<>* me) {
      return createGUI_T<ControlSetting>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "control", "bgcolor"),
        getColor(me, "control", "activecolor"),
        getColor(me, "control", "textcolor"),
        nullptr,
        keybinds[strTo<int>(me->first_attribute("id")->value())],
        strTo<int>(me->first_attribute("id")->value()),
        reinterpret_cast<ControlInputFunc>(funcs[me->first_attribute("inputfunc")->value()]));
    }

    PanelHwnd createPanel(xml_node<>* me) {
      PanelHwnd p = createGUI_T < Panel>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "panel", "bgcolor"),
        nullptr);

      setElements(p, me);

      return p;
    }

    TableHwnd createTable(xml_node<>* me) {
      TableHwnd p = createGUI_T < Table>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "table", "bgcolor"),
        getColor(me, "table", "activecolor"),
        nullptr);

      setElements(p, me);

      return p;
    }

    TablerowHwnd createTableRow(xml_node<>* me) {
      TablerowHwnd p = createGUI_T < TableRow>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "tablerow", "bgcolor"),
        getColor(me, "tablerow", "activecolor"),
        nullptr);

      setElements(p, me);

      return p;
    }

    ContainerHwnd createContainer(xml_node<>* me) {
      ContainerHwnd p = createGUI_T < Container>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "container", "bgcolor"),
        nullptr);

      return p;
    }

    SliderHwnd createSlider(xml_node<>* me) {
      SliderHwnd p = createGUI_T<Slider>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "slider", "bgcolor"),
        getColor(me, "slider", "activecolor"),
        getColor(me, "slider", "textcolor"),
        nullptr,
        getColor(me, "slider", "dragcolor"),
        strTo<float>(me->first_attribute("minvalue")->value()),
        strTo<float>(me->first_attribute("maxvalue")->value()),
        strTo<float>(me->first_attribute("startvalue")->value()),
        strTo<float>(me->first_attribute("quanta")->value()),
        reinterpret_cast<SliderInputFunc>(funcs[me->first_attribute("callback")->value()])
        );
      return p;
    }

    LabelBindHwnd createLabelBind(xml_node<>* me) {
      return createGUI_T<LabelBind>(
        me->first_attribute("id")->value(),
        loadLocation(me->first_node("location")),
        getColor(me, "lablebind", "bgcolor"),
        getColor(me, "lablebind", "activecolor"),
        getColor(me, "lablebind", "textcolor"),
        nullptr,
        strTo<int>(me->first_attribute("align")->value()));
    }

    ElemHwnd addElement(WinHwnd id, ElemHwnd elem) {
      return addElement(id->myPanel, elem);
    }
    ElemHwnd addElement(PanelHwnd id, ElemHwnd elem) {
      if (elem == nullptr) {
        return nullptr;
      }
      id->elements.push_back(elem);
      id->getRect();
      requestRedraw();
      return elem;
    }
    ElemHwnd addElement(TableHwnd id, ElemHwnd elem) {
      if (elem == nullptr) {
        return nullptr;
      }
      id->data.push_back(elem);
      id->getRect();
      return elem;
    }
    ElemHwnd addElement(TablerowHwnd id, ElemHwnd elem) {
      if (elem == nullptr) {
        return nullptr;
      }
      id->data.push_back(elem);
      id->getRect();
      return elem;
    }

    void deleteElements(WinHwnd id) {
      deleteElements(id->myPanel);
    }
    void deleteElement(ElemHwnd elem, ElemHwnd from) {
      from->deleteElement(elem);
    }

    void deleteElements(PanelHwnd id) {
      for (auto&& it : id->elements) {
        deleteElement(it, id);
      }
    }
    void deleteElements(TableHwnd id) {
      for (auto&& it : id->data) {
        deleteElement(it, id);
      }
    }
    void deleteElements(TablerowHwnd id) {
      for (auto&& it : id->data) {
        deleteElement(it, id);
      }
    }

    ElemHwnd getElementById(PanelHwnd pId, string id) {
      return pId->getElementById(id);
    }
    ElemHwnd getElementById(WinHwnd winId, string id) {
      return getElementById(winId->myPanel, id);
    }
    ElemHwnd getElementById(string id) {
      auto it = windows.begin();

      ElemHwnd res = nullptr;

      while (it != windows.end() && res == nullptr) {

        ElemHwnd e = getElementById(it->second, id);

        if (e != nullptr) {
          res = e;
        }

        ++it;
      }

      return res;
    }

    void activateElement(PanelHwnd pId, ElemHwnd id) {
      if (pId->activateElement(id)) {
        requestRedraw();
      }
    }
    void activateElement(WinHwnd winId, ElemHwnd id) {
      return activateElement(winId->myPanel, id);
    }
    void activateElement(ElemHwnd id) {
      auto it = windows.begin();

      while (it != windows.end()) {
        activateElement(it->second, id);
        ++it;
      }
    }
  }
}