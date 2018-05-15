#include "GraphicsUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

map<string, map<string, colorargb>> colors; //type, elem

colorargb getColor(string object, string key) {
  auto it = colors[key];
  if (it.count(object)) {
    return it[object];
  }
  if (it.count("*")) {
    return it["*"];
  }
  LOG WARN GRAPHICS "[Colors]\tUndefined color key " << object << " " << key END;
  return 0;
}
colorargb getColor(string val) {
  if (val[0] == '#') {
    return hexToInt(val);
  }
  else {
    vector<string> tokens = tokenize(val, '_');
    return colors[tokens[0]][tokens[1]];
  }
}
colorargb getColor(xml_node<>* me, string elem, string key) {
  xml_attribute<>* att = me->first_attribute(key.c_str());
  if (att) {
    return getColor(att->value());
  }
  return getColor(elem, key);
}

void loadColors(string filename) {
  ifstream cols(filename);
  string type;
  string elem;
  string col;
  while (cols.good()) {
    cols >> elem >> type >> col;
    colors[type][elem] = getColor(col);
  }
}


bool key::isKey() {
  return _type == type_key;
}
void key::fromKey(unsigned char key) {
  _keycode = key;
  _type = type_key;
}
void key::fromSpecial(int key) {
  _keycode = key;
  _type = type_special;
}
void key::fromMouse(int button) {
  _keycode = button;
  _type = type_mouse;
}
void key::fromWheel(int delta) {
  _keycode = delta;
  _type = type_wheel;
}
string key::toKeyName() {
  if (33 <= _keycode && _keycode <= 96) { //puncutation, uppercase
    return string(1, char(_keycode));
  }
  if (97 <= _keycode && _keycode <= 122) { //lowercase prints as upper
    return string(1, char(_keycode - 'a' + 'A'));
  }
  if (123 <= _keycode && _keycode <= 126) { //special
    return string(1, char(_keycode));
  }
  switch (_keycode) {
  case ' ':
    return "SPACE";
    break;
  case 9:
    return "TAB";
    break;
  case 8:
    return "BACKSPACE";
    break;
  case '\n':
  case '\r':
    return "ENTER";
    break;
  case 127:
    return "DELETE";
    break;
  case 27:
    return "ESC";
    break;
  }
}
string key::toSpecialName() {
  switch (_keycode) {
    //Function keys
  case GLUT_KEY_SHIFT_L:
    return "LSHIFT";
    break;
  case GLUT_KEY_SHIFT_R:
    return "RSHIFT";
    break;
  case GLUT_KEY_ALT_L:
    return "LALT";
    break;
  case GLUT_KEY_ALT_R:
    return "RALT";
    break;
  case GLUT_KEY_CTRL_L:
    return "LCTRL";
    break;
  case GLUT_KEY_CTRL_R:
    return "RCTRL";
    break;

    //Toggle keys
  case GLUT_KEY_NUM_LOCK:
    return "NUMLOCK";
    break;

  case GLUT_KEY_BEGIN:
    return "BEGIN";
    break;
  case GLUT_KEY_HOME:
    return "HOME";
    break;
  case GLUT_KEY_END:
    return "END";
    break;
  case GLUT_KEY_INSERT:
    return "INSERT";
    break;
  case GLUT_KEY_DELETE:
    return "DELETE";
    break;

  case GLUT_KEY_PAGE_DOWN:
    return "PGDOWN";
    break;
  case GLUT_KEY_PAGE_UP:
    return "PGUP";
    break;
    ///Repeat

    //Arrow
  case GLUT_KEY_LEFT:
    return "LEFT";
    break;
  case GLUT_KEY_RIGHT:
    return "RIGHT";
    break;
  case GLUT_KEY_UP:
    return "UP";
    break;
  case GLUT_KEY_DOWN:
    return "DOWN";
    break;

    //F
  case GLUT_KEY_F1:
    return "F1";
    break;
  case GLUT_KEY_F2:
    return "F2";
    break;
  case GLUT_KEY_F3:
    return "F3";
    break;
  case GLUT_KEY_F4:
    return "F4";
    break;
  case GLUT_KEY_F5:
    return "F5";
    break;
  case GLUT_KEY_F6:
    return "F6";
    break;
  case GLUT_KEY_F7:
    return "F7";
    break;
  case GLUT_KEY_F8:
    return "F8";
    break;
  case GLUT_KEY_F9:
    return "F9";
    break;
  case GLUT_KEY_F10:
    return "F10";
    break;
  case GLUT_KEY_F11:
    return "F11";
    break;
  case GLUT_KEY_F12:
    return "F12";
    break;
  }
  return "ERROR" + to_string(_keycode);
}
string key::toMouseName() {
  switch (_keycode) {
  case 0:
    return "LBUTTON";
    break;
  case 1:
    return "MBUTTON";
    break;
  case 2:
    return "RBUTTON";
    break;
  }
}
string key::toWheelName() {
  return to_string(_keycode);
}
string key::toName() {
  switch (_type) {
  case type_key:
    return toKeyName();
    break;
  case type_special:
    return toSpecialName();
    break;
  case type_mouse:
    return toMouseName();
    break;
  case type_wheel:
    return toWheelName();
    break;
  }
  return "T ERROR" + to_string(_type);
}
key::key(int keycode, int type) {
  _keycode = keycode;
  _type = type;
}
key::key(string keycode) {
  vector<string> spliced = tokenize(keycode, ' ');
  _keycode = strTo<int>(spliced[0]);
  _type = strTo<int>(spliced[1]);
}
key::key() {
  _keycode = 0;
  _type = type_key;
}

void key_location::setLocation(int mx, int my) {
  _mx = mx;
  _my = my;
}
key_location::key_location(int keycode, int type, int mx, int my) : key(keycode, type), _mx(mx), _my(my) {

}
key_location::key_location() {

}

string gui_event::toName() {
  switch (_type) {
  case evt_none:
    return _key.toName();
    break;
  case evt_down:
    return _key.toName() + " DOWN";
    break;
  case evt_pressed:
    return _key.toName() + " PRESS";
    break;
  case evt_up:
    return _key.toName() + " UP";
    break;
  }
}
gui_event::gui_event(key_location key, type type) {
  _key = key;
  _type = type;
}

key loadKey(xml_attribute<>* me) {
  if (me) {
    stringstream ss;
    ss << me->value();
    int valt, valv;
    ss >> valt >> valv;
    return key(valv, valt);
  } else {
    return key(key::type_key,0);
  }
}

bool operator==(const key& lhs, const key& rhs) {
  return (lhs._type == rhs._type) & (lhs._keycode == rhs._keycode);
}

bool operator<(const key& lhs, const key& rhs) {
  if (lhs._type < rhs._type) {
    return true;
  }
  if ((lhs._type == rhs._type) && (lhs._keycode < rhs._keycode)) {
    return true;
  }
  return false;
}

bool operator<=(const key& lhs, const key& rhs) {
  if (lhs._type <= rhs._type) {
    return true;
  }
  if ((lhs._type == rhs._type) && (lhs._keycode <= rhs._keycode)) {
    return true;
  }
  return false;
}

bool operator>(const key& lhs, const key& rhs) {
  if (lhs._type > rhs._type) {
    return true;
  }
  if ((lhs._type == rhs._type) && (lhs._keycode > rhs._keycode)) {
    return true;
  }
  return false;
}

bool operator>=(const key& lhs, const key& rhs) {
  if (lhs._type >= rhs._type) {
    return true;
  }
  if ((lhs._type == rhs._type) && (lhs._keycode >= rhs._keycode)) {
    return true;
  }
  return false;
}

static void shapesPrintf(int row, int col, const char *fmt, ...)
{
  static char buf[256];
  int viewport[4];
  void *font = GLUT_BITMAP_9_BY_15;
  va_list args;

  va_start(args, fmt);
#if defined(WIN32) && !defined(__CYGWIN__)
  (void)_vsnprintf_s(buf, sizeof(buf), fmt, args);
#else
  (void)vsnprintf_s(buf, sizeof(buf), fmt, args);
#endif
  va_end(args);

  glGetIntegerv(GL_VIEWPORT, viewport);

  glPushMatrix();
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glOrtho(0, viewport[2], 0, viewport[3], -1, 1);

  glRasterPos2i
    (
      glutBitmapWidth(font, ' ') * col,
      -glutBitmapHeight(font) * (row + 2) + viewport[3]
      );
  glutBitmapString(font, (unsigned char*)buf);

  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

int defaultIRenderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  return 0;
}
int defaultIResizeManager(int x, int y) {
  return 0;
}
int defaultIGUIEventManager(gui_event evt, int x, int y, set<key_location>& down) {
  return 0;
}
int defaultIMouseEntryManager(int state) {
  return 0;
}
int defaultIMouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down) {
  return 0;
}

void Graphics::resetViewport() {
  glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  glScissor(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

  //glGetIntegerv(GL_VIEWPORT, arr);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  //glPushMatrix();
  glLoadIdentity();
  glOrtho(0, glutGet(GLUT_WINDOW_WIDTH),
    0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1);
  glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  glLoadIdentity();
  glColor3ub(0, 255, 0);
}

LinearScale::LinearScale() {
  absc = 0;
  relc = 0;
}
LinearScale::LinearScale(float lrelc) {
  relc = lrelc;
  absc = 0;
}
LinearScale::LinearScale(float lrelc, int labsc) {
  absc = labsc;
  relc = lrelc;
}
int LinearScale::getVal(int size) {
  return size*relc + absc;
}

LinearScale operator+(LinearScale& lhs, LinearScale& rhs) {
  return LinearScale(lhs.relc + rhs.relc, lhs.absc + rhs.absc);
}
LinearScale operator-(LinearScale& lhs, LinearScale& rhs) {
  return LinearScale(lhs.relc - rhs.relc, lhs.absc - rhs.absc);
}
LinearScale operator-(LinearScale& lhs) {
  return LinearScale(-lhs.relc, -lhs.absc);
}
LinearScale operator*(LinearScale& lhs, float& rhs) {
  return LinearScale(lhs.relc * rhs, lhs.absc * rhs);
}
LinearScale operator*(float& rhs, LinearScale& lhs) {
  return LinearScale(lhs.relc * rhs, lhs.absc * rhs);
}
LinearScale operator/(LinearScale& lhs, float& rhs) {
  return LinearScale(lhs.relc / rhs, lhs.absc / rhs);
}
LinearScale operator*(LinearScale& lhs, float rhs) {
  return LinearScale(lhs.relc * rhs, lhs.absc * rhs);
}
LinearScale operator*(float rhs, LinearScale& lhs) {
  return LinearScale(lhs.relc * rhs, lhs.absc * rhs);
}
LinearScale operator/(LinearScale& lhs, float rhs) {
  return LinearScale(lhs.relc / rhs, lhs.absc / rhs);
}


LocationData::LocationData() {

}
LocationData::LocationData(LinearScale lbottom, LinearScale ltop, LinearScale lleft, LinearScale lright) :
  bottom(lbottom),
  top(ltop),
  left(lleft),
  right(lright)
{

}


void LocationData::setWidth(LinearScale to, Lock lock) {
  LinearScale mid = (left+right)/2;
  switch (lock) {
    case Lock_Min:
      right = left + to;
      break;
    case Lock_Max:
      left = right - to;
      break;
    case Lock_Mid:
      left = mid - to/2;
      right = mid + to/2;
      break;
    case Lock_Dist:
      throw 1;
      break;
  }
}
void LocationData::setHeight(LinearScale to, Lock lock) {
  LinearScale mid = (bottom + top) / 2;
  switch (lock) {
    case Lock_Min:
      top = bottom + to;
      break;
    case Lock_Max:
      bottom = top - to;
      break;
    case Lock_Mid:
      bottom = mid - to / 2;
      top = mid + to / 2;
      break;
    case Lock_Dist:
      throw 1;
      break;
  }
}
void LocationData::setTop(LinearScale to, Lock lock) {
  LinearScale mid2 = bottom + top;
  LinearScale dist = top - bottom;
  switch (lock) {
    case Lock_Min:
      top = to;
      break;
    case Lock_Max:
      throw 1;
      break;
    case Lock_Mid:
      top = to;
      bottom = mid2 - to;
      break;
    case Lock_Dist:
      top = to;
      bottom = to - dist;
      break;
  }
}
void LocationData::setBot(LinearScale to, Lock lock) {
  LinearScale mid2 = bottom + top;
  LinearScale dist = top - bottom;
  switch (lock) {
    case Lock_Min:
      throw 1;
      break;
    case Lock_Max:
      bottom = to;
      break;
    case Lock_Mid:
      bottom = to;
      top = mid2 - to;
      break;
    case Lock_Dist:
      bottom = to;
      top = to + dist;
      break;
  }
}
void LocationData::setLeft(LinearScale to, Lock lock) {
  LinearScale mid2 = left + top;
  LinearScale dist = right - left;
  switch (lock) {
    case Lock_Min:
      throw 1;
      break;
    case Lock_Max:
      left = to;
      break;
    case Lock_Mid:
      left = to;
      right = mid2 - to;
      break;
    case Lock_Dist:
      left = to;
      right = to + dist;
      break;
  }
}
void LocationData::setRight(LinearScale to, Lock lock) {
  LinearScale mid2 = left + right;
  LinearScale dist = right - left;
  switch (lock) {
    case Lock_Min:
      right = to;
      break;
    case Lock_Max:
      throw 1;
      break;
    case Lock_Mid:
      right = to;
      left = mid2 - to;
      break;
    case Lock_Dist:
      right = to;
      left = to - dist;
      break;
  }
}
void LocationData::setMidLR(LinearScale to, Lock lock) {
  switch (lock) {
    case Lock_Min:
      right = 2*to - left;
      break;
    case Lock_Max:
      left = 2 * to - right;
      break;
    case Lock_Mid:
      throw 1;
      break;
    case Lock_Dist:
      LinearScale dist = right - left;
      left = to - dist / 2;
      right = to + dist/2;
  }
}
void LocationData::setMidBT(LinearScale to, Lock lock) {
  switch (lock) {
    case Lock_Min:
      top = 2 * to - bottom;
      break;
    case Lock_Max:
      bottom = 2 * to - top;
      break;
    case Lock_Mid:
      throw 1;
      break;
    case Lock_Dist:
      LinearScale dist = top - bottom;
      bottom = to - dist / 2;
      top = to + dist / 2;
  }
}

int LocationData::getLeft(int contWidth) {
  return left.getVal(contWidth);
}
int LocationData::getRight(int contWidth) {
  return right.getVal(contWidth);
}
int LocationData::getTop(int contHeigth) {
  return top.getVal(contHeigth);
}
int LocationData::getBot(int contHeigth) {
  return bottom.getVal(contHeigth);
}
int LocationData::getWidth(int contWidth) {
  return getRight(contWidth) - getLeft(contWidth);
}
int LocationData::getHeight(int contHeight) {
  return getTop(contHeight) - getBot(contHeight);
}

LocationData fullContainer(LinearScale(0), LinearScale(1), LinearScale(0), LinearScale(1));

LocationData::Lock setLock(LocationData::Lock& to, LocationData::Lock newlock) {
  if (to == newlock) {
    if (newlock == LocationData::Lock_Min) {
      return LocationData::Lock_Max;
    } else {
      return LocationData::Lock_Min;
    }
  } else {
    LocationData::Lock res = to;
    to = newlock;
    return res;
  }
}

LinearScale loadLinear(xml_node<>* me) {
  return LinearScale(strTo<float>(me->first_attribute("rel")->value()), strTo<int>(me->first_attribute("abs")->value()));
}

LocationData loadLocation(xml_node<>* me) {
  LocationData res;
  LocationData::Lock lastx = LocationData::Lock_Min;
  LocationData::Lock lasty = LocationData::Lock_Min;

  for(xml_node<>* it = me->first_node(); it; it = it->next_sibling()) {
    string s = it->name();
    if (s == "bot") {
      res.setBot(loadLinear(it), setLock(lasty, LocationData::Lock_Min));
    }
    if (s == "top") {
      res.setTop(loadLinear(it), setLock(lasty, LocationData::Lock_Max));
    }
    if (s == "left") {
      res.setLeft(loadLinear(it), setLock(lastx, LocationData::Lock_Min));
    }
    if (s == "right") {
      res.setRight(loadLinear(it), setLock(lastx, LocationData::Lock_Max));
    }
    if (s == "width") {
      res.setWidth(loadLinear(it), setLock(lastx, LocationData::Lock_Dist));
    }
    if (s == "height") {
      res.setHeight(loadLinear(it), setLock(lasty, LocationData::Lock_Dist));
    }
    if (s == "midlr") {
      res.setMidLR(loadLinear(it), setLock(lastx, LocationData::Lock_Mid));
    }
    if (s == "midbt") {
      res.setMidBT(loadLinear(it), setLock(lasty, LocationData::Lock_Mid));
    }
  }
  return res;
}

void setColor(colorargb v) {
  glColor3f(((v & 0x00FF0000) >> 16) / 255.0f, ((v & 0x0000FF00) >> 8) / 255.0f, ((v & 0x000000FF) >> 0) / 255.0f);
}

void renderBitmapString(float x, float y, string text, colorargb color, bool center, int cursor) {
  if (0 <= cursor && cursor <= text.size()) {
    text.insert(cursor, 1, '|');
  }
  
  setColor(color);
  int length = text.length();
  if (center) {
    glRasterPos2f(x - (9 * length) / 2, y - 5);
  }
  else {
    glRasterPos2f(x, y);
  }
  glutBitmapString(GLUT_BITMAP_9_BY_15, reinterpret_cast<const unsigned char*>(text.c_str()));
}

GLuint png_texture_load(string filename, int& w, int& h) {
  int comp;
  unsigned char* image = stbi_load(trimStr(filename).c_str(), &w, &h, &comp, STBI_rgb_alpha);

  if (image == nullptr)
    throw(std::string("Failed to load texture"));

  GLuint m_texture;

  glGenTextures(1, &m_texture);

  glBindTexture(GL_TEXTURE_2D, m_texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  if (comp == 3)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  else if (comp == 4)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);

  return m_texture;
}

bool numericalValidator(string s, int cursor, unsigned char c) {
  size_t Fminus = s.find('-');
  return ('0' <= c && c <= '9' && (Fminus == string::npos || Fminus < cursor)) || (cursor == 0 && c == '-');
}

bool floatValidator(string s, int cursor, unsigned char c) {
  size_t Fminus = s.find('-');
  size_t Fdot = s.find('.');
  return ('0' <= c && c <= '9' && (Fminus == string::npos || Fminus < cursor)) || (Fdot == string::npos && c == '.' && (Fminus == string::npos || Fminus < cursor)) || (cursor == 0 && c == '-');
}

bool textValidator(string s, int cursor, unsigned char c) {
  return (32 <= c && c < 127);
}