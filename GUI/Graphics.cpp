#include "Graphics.h"

RenderManager Graphics::defaultRenderManager = Graphics::defaultRenderManagerL;
ResizeManager Graphics::defaultResizeManager= Graphics::defaultResizeManagerL;
KeyManager Graphics::defaultKeyManager= Graphics::defaultKeyManagerL;
SpecialKeyManager Graphics::defaultSpecialKeyManager= Graphics::defaultSpecialKeyManagerL;
KeyManager Graphics::defaultKeyUpManager= Graphics::defaultKeyUpManagerL;
SpecialKeyManager Graphics::defaultSpecialKeyUpManager= Graphics::defaultSpecialKeyUpManagerL;
MouseEntryManager Graphics::defaultMouseEntryManager= Graphics::defaultMouseEntryManagerL;
MouseMoveManager Graphics::defaultMouseMoveManager= Graphics::defaultMouseMoveManagerL;
MouseClickManager Graphics::defaultMouseClickManager= Graphics::defaultMouseClickManagerL;
MouseWheelManager Graphics::defaultMouseWheelManager= Graphics::defaultMouseWheelManagerL;

Graphics::WinHwnd Graphics::CreateMainWindow(string caption, WindowManagers managers, int width, int height, bool setsize, int x, int y, bool setposition) {
  if(setsize) {
    glutInitWindowSize(width, height);
  }
  if(setposition) {
    glutInitWindowPosition(x, y);
  }

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

  glutCreateWindow(caption.c_str());

  //glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

  int id = glutGetWindow();

  return SetUpWindow(id, -1, LocationData(LinearScale(0,y), LinearScale(0, y+height), LinearScale(0, x), LinearScale(0, x+width)), managers);
}

Graphics::WinHwnd Graphics::SetUpWindow(int id, int parent, LocationData pos, WindowManagers manager) {
  colorargb bgcolor = getColor("win", "bgcolor");
  
  if(id != -1) {
    glutReshapeFunc(manager.resizeManager);
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

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    glClearColor(((bgcolor & 0xff0000) >> 16)/255.0, ((bgcolor & 0xff00) >> 8) / 255.0, ((bgcolor & 0xff) >> 0) / 255.0, 1);
   }

  GWindow* data = new GWindow();
  
  PanelHwnd panel = new Panel("", fullContainer, bgcolor);

  data->id = id;
  data->parent = parent;
  data->windowManagers = manager;
  data->location = pos;
  data->myPanel = panel;
  if (id != -1) {
    windows[id] = data;
  }
  return data;
}

int Graphics::DestroyWindow(WinHwnd id) {
  delete id->myPanel;

  windows.erase(id->id);

   glutDestroyWindow(id->id);

  delete id;
  return 0;
}

Graphics::WinHwnd Graphics::GetWinHwnd(int id) {
  return windows[id];
}

void Graphics::GWindow::getWin(float pax, float pay, float pbx, float pby) {
  ax = pax + location.getBot(pbx - pax);
  ay = pay + location.getLeft(pby - pay);
  bx = pbx + location.getTop(pbx - pax);
  by = pby + location.getRight(pby - pay);
}

void Graphics::defaultRenderManagerL() {
  netlock.lock();

  defaultRenderManagerNL();

  netlock.unlock();
}
void Graphics::defaultRenderManagerNL() {
  glutSetWindow(glutGetWindow());

  glClear(GL_COLOR_BUFFER_BIT);
  resetViewport();

  elementRenderManager(GetWinHwnd(glutGetWindow()));

  glutSwapBuffers();
}

void Graphics::defaultResizeManagerL(int x, int y) {
  netlock.lock();

  defaultResizeManagerNL(x, y);

  netlock.unlock();
}
void Graphics::defaultResizeManagerNL(int x, int y) {
  WinHwnd h = GetWinHwnd(glutGetWindow());
  int width = x;//glutGet(GLUT_WINDOW_WIDTH);
  int height = y;//glutGet(GLUT_WINDOW_HEIGHT);
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  elementResizeManager(h, width, height);
}

void Graphics::defaultKeyManagerL(unsigned char keyc, int x, int y) {
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
    if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_down), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
      glutPostRedisplay();
    }
  }
  //Always pressed
  if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_pressed), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
    glutPostRedisplay();
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
    if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_down), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
      glutPostRedisplay();
    }
  }
  //Always pressed
  if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_pressed), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
    glutPostRedisplay();
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
  if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_up), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
    glutPostRedisplay();
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
  if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_up), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
    glutPostRedisplay();
  }
}

void Graphics::defaultMouseEntryManagerL(int state) {
  netlock.lock();

  defaultMouseEntryManagerNL(state);

  netlock.unlock();
}
void Graphics::defaultMouseEntryManagerNL(int state) {
  if (1 & elementMouseEnterManager(GetWinHwnd(glutGetWindow()), state)) {
    glutPostRedisplay();
  }
}

void Graphics::defaultMouseMoveManagerL(int x, int y) {
  netlock.lock();

  defaultMouseMoveManagerNL(x, y);

  netlock.unlock();
}
void Graphics::defaultMouseMoveManagerNL(int x, int y) {
  if (1 & elementMouseMoveManager(GetWinHwnd(glutGetWindow()), x, glutGet(GLUT_WINDOW_HEIGHT) - y)) {
    glutPostRedisplay();
  }
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
  if (state == 0) { //Down
    if (!keysdown.count(keyd)) { //If new key, down
      keysdown.insert(keyd);
      if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_down), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
        glutPostRedisplay();
      }
    }
    //Always pressed
    if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_pressed), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
      glutPostRedisplay();
    }
  }
  if (state == 1) { //Up
    keysdown.erase(keyd);
    if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_up), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
      glutPostRedisplay();
    }
  }
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
  if (1 & elementGUIEventManager(GetWinHwnd(glutGetWindow()), gui_event(keyd, gui_event::evt_none), x, glutGet(GLUT_WINDOW_HEIGHT) - y, keysdown)) {
    glutPostRedisplay();
  }
}

WindowManagers Graphics::defaultWindowManagers =
WindowManagers {
  Graphics::defaultRenderManager,
  Graphics::defaultResizeManager,
  Graphics::defaultKeyManager,
  Graphics::defaultSpecialKeyManager,
  Graphics::defaultKeyUpManager,
  Graphics::defaultSpecialKeyUpManager,
  Graphics::defaultMouseEntryManager,
  Graphics::defaultMouseMoveManager,
  Graphics::defaultMouseClickManager,
  Graphics::defaultMouseWheelManager,
};
map<int, Graphics::GWindow*> Graphics::windows;
map<string, void(*)()> Graphics::funcs;
set<key_location> Graphics::keysdown;

int Graphics::elementMouseEnterManager(WinHwnd id, int mstate) {
  return id->myPanel->mouseEnter(mstate);
}

int Graphics::elementMouseMoveManager(WinHwnd id, int x, int y) {
  int ret = id->myPanel->mouseMoved(x, y, id->ox, id->oy, keysdown);
  id->ox = x;
  id->oy = y;
  return ret;
}

int Graphics::elementGUIEventManager(WinHwnd id, gui_event evt, int mx, int my, set<key_location>& down) {
  return id->myPanel->guiEvent(evt, mx, my, down);
}

void Graphics::elementResizeManager(WinHwnd id, int width, int height) {
  return elementResizeManager(id->myPanel, width, height);
}

void Graphics::elementResizeManager(PanelHwnd id, int width, int height) {
  return id->getRect(width, height, 0, 0);
}

void Graphics::elementRenderManager(WinHwnd id) {
  id->myPanel->render(keysdown);
}

Graphics::ButtonHwnd Graphics::createButton(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, string text, key trigger, ClickCallback clickCallback) {
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
    loadKey(me->first_attribute("trigger")),
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

Graphics::ControlHwnd Graphics::createControl(string lname, LocationData location, colorargb bg, colorargb active, colorargb textColor, key selected, int id, ControlInputFunc inputCallback) {
  return new ControlSetting(lname, location, bg, active, textColor, selected, id, inputCallback);
}
Graphics::ControlHwnd Graphics::createControl(xml_node<> *me) {
  return createControl(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "control", "bgcolor"),
    getColor(me, "control", "activecolor"),
    getColor(me, "control", "textcolor"),
    key(me->value()),
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

Graphics::TablerowHwnd Graphics::createTableRow(string lname, LocationData location, colorargb bg) {
  return new TableRow(lname, location, bg);
}
Graphics::TablerowHwnd Graphics::createTableRow(xml_node<> *me) {
  TablerowHwnd p = createTableRow(
    me->first_attribute("id")->value(),
    loadLocation(me->first_node("location")),
    getColor(me, "tablerow", "bgcolor"));

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

void Graphics::deleteElements(WinHwnd id) {
  deleteElements(id->myPanel);
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
  return elem;
}
Graphics::ElemHwnd Graphics::addElement(TableHwnd id, TablerowHwnd elem) {
  id->data.push_back(elem);
  id->getRect();
  return elem;
}
Graphics::ElemHwnd Graphics::addElement(TablerowHwnd id, ElemHwnd elem) {
  id->data.push_back(elem);
  id->getRect();
  return elem;
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
  else if (name == "image") {
    return createImage(me);
  }
  else {
    throw 1;
    return NULL;
  }
}

void Graphics::deleteElement(ElemHwnd elem) {
  elem->toDelete = true;
}

void Graphics::deleteElements(PanelHwnd id) {
  for (auto&& it : id->elements) {
    it->toDelete = true;
  }
}
void Graphics::deleteElements(TableHwnd id) {
  for (auto&& it : id->data) {
    it->toDelete = true;
  }
}
void Graphics::deleteElements(TablerowHwnd id) {
  for (auto&& it : id->data) {
    it->toDelete = true;
  }
}

void Graphics::setElements(PanelHwnd id, xml_node<> *data) {
  for (xml_node<> *pElem = data->first_node(); pElem; pElem = pElem->next_sibling())
  {
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

  defaultResizeManagerNL(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  glutPostRedisplay();
}

void Graphics::setElements(TableHwnd id, xml_node<> *data) {
  for (xml_node<> *pElem = data->first_node(); pElem; pElem = pElem->next_sibling())
  {
    string name = pElem->name();
    if (name == "tr") {
      addElement(id, createTableRow(pElem));
    }
  }
}
void Graphics::setElements(TablerowHwnd id, xml_node<> *data) {
  for (xml_node<> *pElem = data->first_node(); pElem; pElem = pElem->next_sibling())
  {
    addElement(id, createElement(pElem));
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
    glutPostRedisplay();
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
