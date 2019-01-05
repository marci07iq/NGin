#include "Graphics.h"

//Default managers
RenderManager Graphics::defaultRenderManager = Graphics::defaultRenderManagerL;
ResizeManager Graphics::defaultResizeManager= Graphics::defaultResizeManagerL;
//KeyManager Graphics::defaultKeyManager= Graphics::defaultKeyManagerL;
//SpecialKeyManager Graphics::defaultSpecialKeyManager= Graphics::defaultSpecialKeyManagerL;
//KeyManager Graphics::defaultKeyUpManager= Graphics::defaultKeyUpManagerL;
//SpecialKeyManager Graphics::defaultSpecialKeyUpManager= Graphics::defaultSpecialKeyUpManagerL;
MouseEntryManager Graphics::defaultMouseEntryManager= Graphics::defaultMouseEntryManagerL;
MouseMoveManager Graphics::defaultMouseMoveManager= Graphics::defaultMouseMoveManagerL;
MouseClickManager Graphics::defaultMouseClickManager= Graphics::defaultMouseClickManagerL;
MouseWheelManager Graphics::defaultMouseWheelManager= Graphics::defaultMouseWheelManagerL;
WindowCloseManager Graphics::defaultWindowCloseManager = Graphics::defaultWindowCloseManagerL;
GUIEventManager Graphics::defaultGUIEventManager = Graphics::defaultGUIEventManagerL;

//Variables
Graphics::WinHwnd Graphics::current; //Set when any window specific callback is called.

map<Graphics::RawWinHwnd, Graphics::WinHwnd> Graphics::windows;
map<string, void(*)()> Graphics::funcs;
set<key_location> Graphics::keysdown;

bool Graphics::redrawFrame = false;
list<Graphics::WinHwnd> Graphics::wdeleteQueue;
list<pair<Graphics::ElemHwnd, Graphics::ElemHwnd>> Graphics::edeleteQueue;
list<Graphics::winCreationData> Graphics::wcreateQueue;

//Engine management
void Graphics::initGraphics() {
  if (!glfwInit()) {
    LOG FATAL "GLFW init failed" << endl;
    exit(1);
  }
}

void Graphics::mainLoop(bool needsWindows) {
  while (windows.size() || !needsWindows) {
    //Do delete tasks left over from event handlers (so a button can close its parent window)
    cleanQueues();

    bool redraw = redrawFrame;
    redrawFrame = false;
    for (auto&& it : windows) {
      glfwMakeContextCurrent(it.second->rawHwnd);
      if (redraw || it.second->autoRedraw) {
        current = it.second;
        current->windowManagers.renderManager();
        //cout << "DRAW" << endl;
        //glfwSwapBuffers(current->rawHwnd);
      }
      if (glfwWindowShouldClose(it.second->rawHwnd)) {
        cout << "Close" << endl;
        DestroyWindow(it.second);
      }
    }

    /* Poll for and process events */
    glfwPollEvents();
  }
}

void Graphics::requestRedraw() {
  redrawFrame = true;
}

void Graphics::cleanQueues() { //Never call from event handler. IT WILL CRASH
  current = NULL; //It should not matter at this point anyway
  while (wdeleteQueue.size()) {
    delete wdeleteQueue.front();
    wdeleteQueue.pop_front();
  }
  while (edeleteQueue.size()) {
    edeleteQueue.front().second->deleteElement(edeleteQueue.front().first);
    edeleteQueue.pop_front();
  }
  while (wcreateQueue.size()) {
    winCreationData from = wcreateQueue.front();
    wcreateQueue.pop_front();
    WinHwnd newWin = rawCreateMainWindow(from);
    if(from.onCreated) {
      from.onCreated(newWin);
    }
    rawSetUpWindow(newWin->rawHwnd, from.managers);
    if(from.onSetup) {
      from.onSetup(newWin);
    }
  }
}

void Graphics::shutdownGraphics() {
  glfwTerminate();
}

void Graphics::forceShutdown() {
  shutdownGraphics();
  exit(0);
}



//Window management
void Graphics::CreateMainWindow(string caption, WindowManagers managers, int width, int height, bool setsize, int x, int y, bool setposition, int additionalFlags, WinCreateManager onCreated, WinCreateManager onSetup) {
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
Graphics::WinHwnd Graphics::rawCreateMainWindow(winCreationData from) {
  if (!from.setSize) {
    from.width = 640;
    from.height = 480;
  }
  RawWinHwnd window = glfwCreateWindow(from.width, from.height, from.caption.c_str(), NULL, Gll::initOn);
  if (window == NULL) {
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
  PanelHwnd panel = new Panel("base", fullContainer, bgcolor);

  glfwMakeContextCurrent(window);
  glClearColor(((bgcolor & 0xff0000) >> 16) / 255.0, ((bgcolor & 0xff00) >> 8) / 255.0, ((bgcolor & 0xff) >> 0) / 255.0, 1);
  if (current != NULL) {
    glfwMakeContextCurrent(current->rawHwnd);
  }
  
  data->rawHwnd = window;
  data->windowManagers = from.managers;
  data->myPanel = panel;
  windows[window] = data;

  data->rescanSize();
  //glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE | additionalFlags);

  //glutCreateWindow(caption.c_str());

  //glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

  Graphics::requestRedraw();

  return data;
}
void Graphics::rawSetUpWindow(RawWinHwnd id, WindowManagers manager) {

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

Graphics::WinHwnd Graphics::GetWinHwnd(RawWinHwnd id) {
  return windows[id];
}

int Graphics::DestroyWindow(WinHwnd id) {
  defaultWindowCloseManagerNL();

  wdeleteQueue.push_back(id);
  return 0;
}

void Graphics::GWindow::rescanSize() {
  current = this;
  glfwGetWindowSize(rawHwnd, &width, &height);
  windowManagers.resizeManager(width, height);
}

Graphics::GWindow::~GWindow() {
  delete myPanel;
  windows.erase(rawHwnd);
  glfwDestroyWindow(rawHwnd);
}

//Default manager implementations
void Graphics::defaultRenderManagerL() {
  netlock.lock();

  defaultRenderManagerNL();

  netlock.unlock();
}
void Graphics::defaultRenderManagerNL() {
  glClear(GL_COLOR_BUFFER_BIT);
  resetViewport();

  elementRenderManager();

  glfwSwapBuffers(Graphics::current->rawHwnd);
}

void Graphics::rawResizeManager(GLFWwindow * window, int width, int height) {
  glfwMakeContextCurrent(window);
  current = GetWinHwnd(window);
  current->width = width;
  current->height = height;
  current->windowManagers.resizeManager(width, height);
}

void Graphics::defaultResizeManagerL(int x, int y) {
  netlock.lock();

  defaultResizeManagerNL(x, y);

  netlock.unlock();
}
void Graphics::defaultResizeManagerNL(int x, int y) {
  int width = x;//glutGet(GLUT_WINDOW_WIDTH);
  int height = y;//glutGet(GLUT_WINDOW_HEIGHT);
  glViewport(0, 0, width, height);

  //glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();

  //glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
  elementResizeManager(width, height);

  Graphics::requestRedraw();
}

void Graphics::rawKeyManager(GLFWwindow * window, int key, int scancode, int action, int mods) {
  glfwMakeContextCurrent(window);
  current = GetWinHwnd(window);
  key_location nkey = key_location(key, key::type_key, current->oldMouseX, current->oldMouseY);
  gui_event::type gEvtType = gui_event::evt_none;
  if (action == GLFW_PRESS) { //Down
    keysdown.insert(nkey);
    gEvtType = gui_event::evt_pressed; //Send press
    current->windowManagers.guiEventManager(gui_event(nkey, gEvtType), current->oldMouseX, current->oldMouseY, keysdown);

    gEvtType = gui_event::evt_down;
  }
  if(action == GLFW_REPEAT) { //Repeat
   keysdown.insert(nkey);
   gEvtType = gui_event::evt_pressed;
  }
  if (action == GLFW_RELEASE) { //Release
    keysdown.erase(nkey);
    gEvtType = gui_event::evt_up;
  }
  current->windowManagers.guiEventManager(gui_event(nkey, gEvtType), current->oldMouseX, current->oldMouseY, keysdown);
}

void Graphics::rawCharManager(GLFWwindow * window, unsigned int codepoint) {
}

/*void Graphics::defaultKeyManagerL(unsigned char keyc, int x, int y) {
  netlock.lock();

  defaultKeyManagerNL(keyc, x, y);

  netlock.unlock();
}
void Graphics::defaultKeyManagerNL(unsigned char keyc, int x, int y) {
  key_location keyd;
  keyd.fromKey(tolower(keyc));
  keyd.setLocation(x, y);
  if (!keysdown.count(keyd)) { //If new key, down
    keysdown.insert(keyd);
    if (1 & elementGUIEventManager(Graphics::current, gui_event(keyd, gui_event::evt_down), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
      Graphics::requestRedraw();
    }
  }
  //Always pressed
  if (1 & elementGUIEventManager(Graphics::current, gui_event(keyd, gui_event::evt_pressed), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
    Graphics::requestRedraw();
  }
}

void Graphics::defaultSpecialKeyManagerL(int keyc, int x, int y) {
  netlock.lock();

  defaultSpecialKeyManagerNL(keyc, x, y);

  netlock.unlock();
}
void Graphics::defaultSpecialKeyManagerNL(int keyc, int x, int y) {
  key_location keyd;
  keyd.fromSpecial(keyc);
  keyd.setLocation(x, y);
  if (!keysdown.count(keyd)) { //If new key, down
    keysdown.insert(keyd);
    if (1 & elementGUIEventManager(Graphics::current, gui_event(keyd, gui_event::evt_down), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
      Graphics::requestRedraw();
    }
  }
  //Always pressed
  if (1 & elementGUIEventManager(Graphics::current, gui_event(keyd, gui_event::evt_pressed), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
    Graphics::requestRedraw();
  }
}

void Graphics::defaultKeyUpManagerL(unsigned char keyc, int x, int y) {
  netlock.lock();

  defaultKeyUpManagerNL(keyc,x, y);

  netlock.unlock();
}
void Graphics::defaultKeyUpManagerNL(unsigned char keyc, int x, int y) {
  key_location keyd;
  keyd.fromKey(tolower(keyc));
  keyd.setLocation(x, y);
  keysdown.erase(keyd);
  if (1 & elementGUIEventManager(Graphics::current, gui_event(keyd, gui_event::evt_up), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
    Graphics::requestRedraw();
  }
}

void Graphics::defaultSpecialKeyUpManagerL(int keyc, int x, int y) {
  netlock.lock();

  defaultSpecialKeyUpManagerNL(keyc, x, y);

  netlock.unlock();
}
void Graphics::defaultSpecialKeyUpManagerNL(int keyc, int x, int y) {
  key_location keyd;
  keyd.fromSpecial(keyc);
  keyd.setLocation(x, y);
  keysdown.erase(keyd);
  if (1 & elementGUIEventManager(Graphics::current, gui_event(keyd, gui_event::evt_up), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
    Graphics::requestRedraw();
  }
}*/

void Graphics::rawMouseEntryManager(GLFWwindow * window, int entered) {
  glfwMakeContextCurrent(window);
  current = GetWinHwnd(window);
  current->windowManagers.mouseEntryManager(entered);
}

void Graphics::defaultMouseEntryManagerL(int state) {
  netlock.lock();

  defaultMouseEntryManagerNL(state);

  netlock.unlock();
}
void Graphics::defaultMouseEntryManagerNL(int state) {
  if (1 & elementMouseEnterManager(state)) {
    requestRedraw();
  }
}

void Graphics::rawMouseMoveManager(GLFWwindow * window, double xpos, double ypos) {
  glfwMakeContextCurrent(window);
  current = GetWinHwnd(window);
  current->windowManagers.mouseMoveManager(xpos, current->height - ypos);
}

void Graphics::defaultMouseMoveManagerL(int x, int y) {
  netlock.lock();

  defaultMouseMoveManagerNL(x, y);

  netlock.unlock();
}
void Graphics::defaultMouseMoveManagerNL(int x, int y) {
  if (1 & elementMouseMoveManager(x, y)) {
    Graphics::requestRedraw();
  }
}

void Graphics::rawMouseClickCallback(GLFWwindow * window, int button, int action, int mods) {
  glfwMakeContextCurrent(window);
  current = GetWinHwnd(window);
  current->windowManagers.mouseClickManager(button, action, current->oldMouseX, current->oldMouseY);
}

void Graphics::defaultMouseClickManagerL(int button, int state, int x, int y) {
  netlock.lock();

  defaultMouseClickManagerNL(button, state, x, y);

  netlock.unlock();
}
void Graphics::defaultMouseClickManagerNL(int button, int state, int x, int y) {
  key_location keyd;
  keyd.fromMouse(button);
  keyd.setLocation(x, y);
  if (state == 1) { //Down
    if (!keysdown.count(keyd)) { //If new key, down
      keysdown.insert(keyd);
      if (1 & elementGUIEventManager(gui_event(keyd, gui_event::evt_down), x, y, keysdown)) {
        Graphics::requestRedraw();
      }
    }
    //Always pressed
    if (1 & elementGUIEventManager(gui_event(keyd, gui_event::evt_pressed), x, y, keysdown)) {
      Graphics::requestRedraw();
    }
  }
  if (state == 0) { //Up
    keysdown.erase(keyd);
    if (1 & elementGUIEventManager(gui_event(keyd, gui_event::evt_up), x, y, keysdown)) {
      Graphics::requestRedraw();
    }
  }
}

void Graphics::rawMouseWheelManager(GLFWwindow * window, double xoffset, double yoffset) {
  glfwMakeContextCurrent(window);
  current = GetWinHwnd(window);
  current->windowManagers.mouseWheelManager(0, yoffset, current->oldMouseX, current->oldMouseY);
}

void Graphics::defaultMouseWheelManagerL(int state, int delta, int x, int y) {
  netlock.lock();

  defaultMouseWheelManagerNL(state, delta, x, y);

  netlock.unlock();
}
void Graphics::defaultMouseWheelManagerNL(int state, int delta, int x, int y) {
  key_location keyd;
  keyd.fromWheel(delta);
  keyd.setLocation(x, y);
  if (1 & elementGUIEventManager(gui_event(keyd, gui_event::evt_none), x, y, keysdown)) {
    Graphics::requestRedraw();
  }
}

void Graphics::rawWindowCloseManager(GLFWwindow * window) {
  glfwMakeContextCurrent(window);
  current = GetWinHwnd(window);
  current->windowManagers.windowCloseManager();
}

void Graphics::defaultWindowCloseManagerL() {
  netlock.lock();

  defaultWindowCloseManagerNL();

  netlock.unlock();
}
void Graphics::defaultWindowCloseManagerNL() {
  
}

int Graphics::defaultGUIEventManagerL(gui_event evt, int mx, int my, set<key_location>& down) {
  netlock.lock();

  int res = defaultGUIEventManagerNL(evt, mx, my, down);
  if (res & 1) {
    requestRedraw();
  }

  netlock.unlock();

  return res;
}
int Graphics::defaultGUIEventManagerNL(gui_event evt, int mx, int my, set<key_location>& down) {
  return Graphics::elementGUIEventManager(evt, mx, my, down);
}


WindowManagers Graphics::defaultWindowManagers =
WindowManagers {
  Graphics::defaultRenderManager,
  Graphics::defaultResizeManager,
  /*Graphics::defaultKeyManager,
  Graphics::defaultSpecialKeyManager,
  Graphics::defaultKeyUpManager,
  Graphics::defaultSpecialKeyUpManager,*/
  Graphics::defaultGUIEventManager,
  Graphics::defaultMouseEntryManager,
  Graphics::defaultMouseMoveManager,
  Graphics::defaultMouseClickManager,
  Graphics::defaultMouseWheelManager,
  Graphics::defaultWindowCloseManager,
};

//Element managers
int Graphics::elementMouseEnterManager(int mstate) {
  return current->myPanel->mouseEnter(mstate);
}

int Graphics::elementMouseMoveManager(int x, int y) {
  int ret = current->myPanel->mouseMoved(x, y, current->oldMouseX, current->oldMouseY, keysdown);
  current->oldMouseX = x;
  current->oldMouseY = y;
  return ret;
}

int Graphics::elementGUIEventManager(gui_event evt, int mx, int my, set<key_location>& down) {
  return current->myPanel->guiEvent(evt, mx, my, down);
}

void Graphics::elementResizeManager(int width, int height) {
  return elementResizeManager(current->myPanel, width, height);
}

void Graphics::elementResizeManager(PanelHwnd id, int width, int height) {
  return id->getRect(width, height, 0, 0);
}

void Graphics::elementRenderManager() {
  current->myPanel->render(keysdown);
}

void Graphics::elementCloseManager() {
  Graphics::deleteElements(current);
}

//Element creation
void Graphics::setElements(PanelHwnd id, xml_node<> *data) {
  for (xml_node<> *pElem = data->first_node(); pElem; pElem = pElem->next_sibling()) {
    addElement(id, createElement(pElem));
  }
  return;
}

void Graphics::setElements(PanelHwnd id, string filename) {
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
  Graphics::requestRedraw();
}

void Graphics::setElements(TableHwnd id, xml_node<> *data) {
  for (xml_node<> *pElem = data->first_node(); pElem; pElem = pElem->next_sibling()) {
    string name = pElem->name();
    if (name == "tr") {
      addElement(id, createElement(pElem));
    }
  }
}
void Graphics::setElements(TablerowHwnd id, xml_node<> *data) {
  for (xml_node<> *pElem = data->first_node(); pElem; pElem = pElem->next_sibling()) {
    addElement(id, createElement(pElem));
  }
}

Graphics::ElemHwnd Graphics::createElement(xml_node<> *me) {
  string name = me->name();
  if (name == "location") { //Parameter to div, NOT element
    return NULL;
  }
  if (name == "button") {
    return createButton(me);
  }
  else if (name == "checkbox") {
    return createCheckbox(me);
  }
  else if (name == "label" || name == "text") {
    return createLabel(me);
  }
  else if (name == "labelbind" || name == "textbind") {
    return createLabelBind(me);
  }
  else if (name == "container") {
    return createContainer(me);
  }
  else if (name == "textinput" || name == "input") {
    return createTextInput(me);
  }
  else if (name == "panel" || name == "div") {
    return createPanel(me);
  }
  else if (name == "slider") {
    return createSlider(me);
  }
  else if (name == "config") {
    return createControl(me);
  }
  else if (name == "table") {
    return createTable(me);
  }
  else if (name == "tr") {
    return createTableRow(me);
  }
  else if (name == "image") {
    return createImage(me);
  }
  else {
    throw 1;
    return NULL;
  }
}

Graphics::ButtonHwnd Graphics::createButton(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, string text, int trigger, ClickCallback clickCallback) {
  return new Button(lname, location, bg, active, textColor, text, trigger, clickCallback);
}
Graphics::ButtonHwnd Graphics::createButton(xml_node<> *me) {
  return createButton(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "button", "bgcolor"),
    getColor(me, "button", "activecolor"),
    getColor(me, "button", "textcolor"),
    me->value(),
    me->first_attribute("trigger") ? strTo<int>(me->first_attribute("trigger")->value()) : -1,
    reinterpret_cast<ClickCallback>(funcs[me->first_attribute("callback")->value()]));
}

Graphics::CheckboxHwnd Graphics::createCheckbox(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, bool checked, CheckCallback checkCallback) {
  return new Checkbox(lname, location, bg, active, textColor, checked, checkCallback);
}
Graphics::CheckboxHwnd Graphics::createCheckbox(xml_node<> *me) {
  return createCheckbox(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "checkbox", "bgcolor"),
    getColor(me, "checkbox", "activecolor"),
    getColor(me, "checkbox", "textcolor"),
    strTo<bool>(me->value()),
    reinterpret_cast<CheckCallback>(funcs[me->first_attribute("callback")->value()]));
}

Graphics::LabelHwnd Graphics::createLabel(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, string text, int align) {
  return new Label(lname, location, bg, active, textColor, text, align);
}
Graphics::LabelHwnd Graphics::createLabel(xml_node<> *me) {
  return createLabel(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "label", "bgcolor"),
    getColor(me, "label", "activecolor"),
    getColor(me, "label", "textcolor"),
    me->value(),
    strTo<int>(me->first_attribute("align")->value()));
}

Graphics::ImageHwnd Graphics::createImage(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, string text, int align) {
  return new Image(lname, location, bg, active, textColor, text, align);
}
Graphics::ImageHwnd Graphics::createImage(xml_node<> *me) {
  return createImage(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "image", "bgcolor"),
    getColor(me, "image", "activecolor"),
    getColor(me, "image", "textcolor"),
    me->value(),
    strTo<int>(me->first_attribute("align")->value()));
}

Graphics::TextInputHwnd Graphics::createTextInput(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, string text, TextInputFunc inputCallback, TextValidatorFunc validator) {
  return new TextInput(lname, location, bg, active, textColor, text, inputCallback, validator);
}
Graphics::TextInputHwnd Graphics::createTextInput(xml_node<> *me) {
  return createTextInput(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "input", "bgcolor"),
    getColor(me, "input", "activecolor"),
    getColor(me, "input", "textcolor"),
    me->value(),
    reinterpret_cast<TextInputFunc>(funcs[me->first_attribute("inputfunc")->value()]),
    reinterpret_cast<TextValidatorFunc>(funcs[me->first_attribute("validatorfunc")->value()]));
}

Graphics::ControlHwnd Graphics::createControl(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, key_config selected, int id, ControlInputFunc inputCallback) {
  return new ControlSetting(lname, location, bg, active, textColor, selected, id, inputCallback);
}
Graphics::ControlHwnd Graphics::createControl(xml_node<> *me) {
  return createControl(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "control", "bgcolor"),
    getColor(me, "control", "activecolor"),
    getColor(me, "control", "textcolor"),
    keybinds[strTo<int>(me->first_attribute("id")->value())],
    strTo<int>(me->first_attribute("id")->value()),
    reinterpret_cast<ControlInputFunc>(funcs[me->first_attribute("inputfunc")->value()]));
}

Graphics::CanvasHwnd Graphics::createCanvas(string lname, LocationData location, IWindowManagers managers) {
  return new Canvas(lname, location, managers);
}

Graphics::PlotHwnd Graphics::createPlot(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor) {
  return new Plot(lname, location, bg, active, textColor);
}

Graphics::PanelHwnd Graphics::createPanel(string lname, LocationData location, colorargb bg) {
  return new Panel(lname, location, bg);
}
Graphics::PanelHwnd Graphics::createPanel(xml_node<> *me) {
  PanelHwnd p = createPanel(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "panel", "bgcolor"));

  setElements(p, me);

  return p;
}

Graphics::TableHwnd Graphics::createTable(string lname, LocationData location, colorargb bg, colorargb active) {
  return new Table(lname, location, bg, active);
}
Graphics::TableHwnd Graphics::createTable(xml_node<> *me) {
  TableHwnd p = createTable(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "table", "bgcolor"),
    getColor(me, "table", "activecolor"));

  setElements(p, me);

  return p;
}

Graphics::TablerowHwnd Graphics::createTableRow(string lname, LocationData location, colorargb bg, colorargb active) {
  return new TableRow(lname, location, bg, active);
}
Graphics::TablerowHwnd Graphics::createTableRow(xml_node<> *me) {
  TablerowHwnd p = createTableRow(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "tablerow", "bgcolor"),
    getColor(me, "tablerow", "activecolor"));

  setElements(p, me);

  return p;
}

Graphics::ContainerHwnd Graphics::createContainer(string lname, LocationData location, colorargb bg) {
  return new Container(lname, location, bg);
}
Graphics::ContainerHwnd Graphics::createContainer(xml_node<> *me) {
  ContainerHwnd p = createContainer(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "container", "bgcolor"));

  return p;
}

Graphics::SliderHwnd Graphics::createSlider(string name, LocationData location, colorargb bg, colorargb active, colorargb textColor, colorargb pulledcolor, float min, float max, float value, float quanta, SliderInputFunc clickCallback) {
return new Slider(name, location, bg, active, textColor, pulledcolor, min, max, value, quanta, clickCallback);
}
Graphics::SliderHwnd Graphics::createSlider(xml_node<> *me) {
  SliderHwnd p = createSlider(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "slider", "bgcolor"),
    getColor(me, "slider", "activecolor"),
    getColor(me, "slider", "textcolor"),
    getColor(me, "slider", "dragcolor"),
    strTo<float>(me->first_attribute("minvalue")->value()),
    strTo<float>(me->first_attribute("maxvalue")->value()),
    strTo<float>(me->first_attribute("startvalue")->value()),
    strTo<float>(me->first_attribute("quanta")->value()),
    reinterpret_cast<SliderInputFunc>(funcs[me->first_attribute("callback")->value()])
  );
  return p;
}

Graphics::LabelBindHwnd Graphics::createLabelBind(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, int center) {
  return new LabelBind(lname, location, bg, active, textColor, center);
}
Graphics::LabelBindHwnd Graphics::createLabelBind(xml_node<> *me) {
  return createLabelBind(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "lablebind", "bgcolor"),
    getColor(me, "lablebind", "activecolor"),
    getColor(me, "lablebind", "textcolor"),
    strTo<int>(me->first_attribute("align")->value()));
}

Graphics::ElemHwnd Graphics::addElement(WinHwnd id, ElemHwnd elem) {
  return addElement(id->myPanel, elem);
}
Graphics::ElemHwnd Graphics::addElement(PanelHwnd id, ElemHwnd elem) {
  if (elem == NULL) {
    return NULL;
  }
  id->elements.push_back(elem);
  id->getRect();
  requestRedraw();
  return elem;
}
Graphics::ElemHwnd Graphics::addElement(TableHwnd id, ElemHwnd elem) {
  if (elem == NULL) {
    return NULL;
  }
  id->data.push_back(elem);
  id->getRect();
  return elem;
}
Graphics::ElemHwnd Graphics::addElement(TablerowHwnd id, ElemHwnd elem) {
  if (elem == NULL) {
    return NULL;
  }
  id->data.push_back(elem);
  id->getRect();
  return elem;
}

void Graphics::deleteElements(WinHwnd id) {
  deleteElements(id->myPanel);
}
void Graphics::deleteElement(ElemHwnd elem, ElemHwnd from) {
  edeleteQueue.push_back({elem, from});
}

void Graphics::deleteElements(PanelHwnd id) {
  for (auto&& it : id->elements) {
    deleteElement(it, id);
  }
}
void Graphics::deleteElements(TableHwnd id) {
  for (auto&& it : id->data) {
    deleteElement(it, id);
  }
}
void Graphics::deleteElements(TablerowHwnd id) {
  for (auto&& it : id->data) {
    deleteElement(it, id);
  }
}

Graphics::ElemHwnd Graphics::getElementById(Graphics::PanelHwnd pId, string id) {
  return pId->getElementById(id);
}
Graphics::ElemHwnd Graphics::getElementById(Graphics::WinHwnd winId, string id) {
  return getElementById(winId->myPanel, id);
}
Graphics::ElemHwnd Graphics::getElementById(string id) {
  auto it = windows.begin();

  ElemHwnd res = NULL;

  while (it != windows.end() && res == NULL) {

    ElemHwnd e = getElementById(it->second, id);

    if (e != NULL) {
      res = e;
    }

    ++it;
  }

  return res;
}

void Graphics::activateElement(PanelHwnd pId, ElemHwnd id) {
  if(pId->activateElement(id)) {
    Graphics::requestRedraw();
  }
}
void Graphics::activateElement(WinHwnd winId, ElemHwnd id) {
  return activateElement(winId->myPanel, id);
}
void Graphics::activateElement(ElemHwnd id) {
  auto it = windows.begin();

  while (it != windows.end()) {
    activateElement(it->second, id);
    ++it;
  }
}
