#include "Graphics.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

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

/*static void shapesPrintf(int row, int col, const char *fmt, ...)
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
}*/

int defaultIRenderManager(Canvas*, int ax, int ay, int bx, int by, set<key_location>& down) {
  return 0;
}
int defaultIResizeManager(Canvas*, int x, int y) {
  return 0;
}
int defaultIGUIEventManager(Canvas*, gui_event& evt, int x, int y, set<key_location>& down) {
  return 0;
}
int defaultIMouseEntryManager(Canvas*, int state) {
  return 0;
}
int defaultIMouseMoveManager(Canvas*, int x, int y, int ox, int oy, set<key_location>& down) {
  return 0;
}

void Graphics::resetViewport() {
  glViewport(0, 0, Graphics::current->width, Graphics::current->height);
  glScissor(0, 0, Graphics::current->width, Graphics::current->height);

  //glGetIntegerv(GL_VIEWPORT, arr);
  glDisable(GL_DEPTH_TEST);
  //glMatrixMode(GL_PROJECTION);
  //glPushMatrix();
  //glLoadIdentity();
  //glOrtho(0, Graphics::current->width,
  //  0, Graphics::current->height, -1, 1);
  //glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  //glLoadIdentity();
  //glColor3ub(0, 255, 0);
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
  //glColor3f(((v & 0x00FF0000) >> 16) / 255.0f, ((v & 0x0000FF00) >> 8) / 255.0f, ((v & 0x000000FF) >> 0) / 255.0f);
  Gll::gllColor(v);
}

void renderBitmapString(float x, float y, string text, colorargb color, bool center, int cursor) {
  if (0 <= cursor && cursor <= text.size()) {
    text.insert(cursor, 1, '|');
  }
  
  setColor(color);
  /*int length = text.length();
  if (center) {
    //glRasterPos2f(x - (9 * length) / 2, y - 5);
  }
  else {
    //glRasterPos2f(x, y);
  }*/

  Gll::gllText(text, x, y, center ? 0 : -1, 0, 16.0 / Gll::gllFontCharSize.y); //default: scale to 16 high

  //glutBitmapString(GLUT_BITMAP_9_BY_15, reinterpret_cast<const unsigned char*>(text.c_str()));
}

bool numericalValidator(Graphics::ElemHwnd e, string s, int cursor, unsigned char c) {
  size_t Fminus = s.find('-');
  return ('0' <= c && c <= '9' && (Fminus == string::npos || Fminus < cursor)) || (cursor == 0 && c == '-');
}

bool floatValidator(Graphics::ElemHwnd e, string s, int cursor, unsigned char c) {
  size_t Fminus = s.find('-');
  size_t Fdot = s.find('.');
  return ('0' <= c && c <= '9' && (Fminus == string::npos || Fminus < cursor)) || (Fdot == string::npos && c == '.' && (Fminus == string::npos || Fminus < cursor)) || (cursor == 0 && c == '-');
}

bool textValidator(Graphics::ElemHwnd e, string s, int cursor, unsigned char c) {
  return (32 <= c && c < 127);
}

pair<float, float> scrollBar(float contentHeight, float offset, float poslow, float poshigh, float minSize, float margin) {
  float poslow2 = poslow + margin;
  float poshigh2 = poshigh - margin;

  float viewportHeight = poshigh - poslow;

  float sliderSize = min(max(viewportHeight*viewportHeight / contentHeight, minSize), poshigh2 - poslow2);

  poslow2 += sliderSize / 2;
  poshigh2 -= sliderSize / 2;

  float iviewportHeight = poshigh2 - poslow2;

  float sliderMid = poslow2 + offset * iviewportHeight / (contentHeight - viewportHeight);

  return{ sliderMid - sliderSize / 2, sliderMid + sliderSize / 2 };
}

string openFileSelector(string message, list<pair<string, string>> formats) {
#ifdef _WIN32
  char filename[MAX_PATH];

  OPENFILENAME ofn;
  ZeroMemory(&filename, sizeof(filename));
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
  string formatString = "";
  for(auto&& it : formats) {
    formatString += it.first + string("\0", 1) + it.second + string("\0", 1);
  }
  ofn.lpstrFilter = formatString.c_str();
  ofn.lpstrFile = filename;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrTitle = message.c_str();
  ofn.Flags = OFN_FILEMUSTEXIST;

  //GLFW triggers event handlers if window is opened on the same thread.
  //That will crash the GUI Mutex lock.
  std::thread dialog(GetOpenFileNameA, &ofn);
  dialog.join();
  return filename;
  /*if (GetOpenFileNameA(&ofn)) {
    //std::cout << "You chose the file \"" << filename << "\"\n";
    return filename;
  } else {
    // All this stuff below is to tell you exactly how you messed up above. 
    // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
    switch (CommDlgExtendedError()) {
      case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
      case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
      case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
      case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
      case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
      case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
      case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
      case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
      case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
      case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
      case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
      case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
      case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
      case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
      case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
      default: std::cout << "You cancelled.\n";
    }
    return "";
  }*/
#else
  throw 1;
  return "";
#endif
}

string saveFileSelector(string message, list<pair<string, string>> formats, string ext) {
#ifdef _WIN32
  char filename[MAX_PATH];

  OPENFILENAME ofn;
  ZeroMemory(&filename, sizeof(filename));
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
  string formatString = "";
  for (auto&& it : formats) {
    formatString += it.first + string("\0", 1) + it.second + string("\0", 1);
  }
  ofn.lpstrFilter = formatString.c_str();
  ofn.lpstrFile = filename;
  ofn.lpstrDefExt = ext.c_str();
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrTitle = message.c_str();
  ofn.Flags = OFN_EXTENSIONDIFFERENT;

  //GLFW triggers event handles if window is opened on the same thread.
  //That will crash the GUI Mutex lock.
  std::thread dialog(GetSaveFileNameA, &ofn);
  dialog.join();
  return filename;
  /*if (GetOpenFileNameA(&ofn)) {
  //std::cout << "You chose the file \"" << filename << "\"\n";
  return filename;
  } else {
  // All this stuff below is to tell you exactly how you messed up above.
  // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
  switch (CommDlgExtendedError()) {
  case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
  case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
  case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
  case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
  case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
  case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
  case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
  case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
  case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
  case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
  case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
  case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
  case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
  case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
  case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
  default: std::cout << "You cancelled.\n";
  }
  return "";
  }*/
#else
  throw 1;
  return "";
#endif
}

void insertColor(float* arr, size_t to, colorargb col, fVec3 light) {
  arr[to] = ((col >> 16) & 255) / 255.0 * light.x;
  arr[to + 1] = ((col >> 8) & 255) / 255.0 * light.y;
  arr[to + 2] = ((col >> 0) & 255) / 255.0 * light.z;
  arr[to + 3] = ((col >> 24) & 255) / 255.0;
}

void insertVector(float* arr, size_t to, fVec3 vec) {
  arr[to] = vec.x;
  arr[to + 1] = vec.y;
  arr[to + 2] = vec.z;
}


Gll::gllModes Gll::_mode;
vector<pair<fVec2, colorargb>> Gll::_pts;
colorargb Gll::_col;

Graphics::RawWinHwnd Gll::initOn = NULL;

Shader Gll::gllBaseS;
Shader Gll::gllTextS;

Texture Gll::gllFontMap;
iVec2 Gll::gllFontCharSize;
iVec2 Gll::gllFontCharCount;

void Gll::gllInit(string base) {
  gllBaseS.create(base + "LegDraw");
  gllTextS.create(base + "Text");
  gllFontMap = Texture(base + "ascii.png");
  initOn = (Graphics::current) ? Graphics::current->rawHwnd : NULL;
  gllFontCharCount = {16,16};
  gllFontCharSize = gllFontMap._raw->size / gllFontCharCount;
}

void Gll::gllBegin(gllModes m) {
  _mode = m;
}

void Gll::gllColor(colorargb col) {
  _col = col;
}

void Gll::gllVertex(double x, double y) {
  gllVertex({x, y});//, 0);
}
void Gll::gllVertex(fVec2 pt) {
  //gllVertex(pt.x, pt.y, 0);
  _pts.push_back({ pt, _col });
}

/*void Gll::gllVertex(double x, double y, double z) {
  gllVertex({x, y, z});
}
void Gll::gllVertex(fVec3 pt) {
  _pts.push_back({pt, _col})
}*/

void Gll::gllEnd() {
  int size = 0;
  float* raw = NULL;
  float* col = NULL;
  switch (_mode) {
    case GLL_POLY:
      size = 3 * (_pts.size() - 2);
      raw = new float[size * 2];
      col = new float[size * 4];
      for (int i = 0; i < _pts.size() - 2; i++) {
        raw[6 * i + 0] = _pts[0].first.x;
        raw[6 * i + 1] = _pts[0].first.y;
        insertColor(col, 12*i, _pts[0].second);

        raw[6 * i + 2] = _pts[i + 1].first.x;
        raw[6 * i + 3] = _pts[i + 1].first.y;
        insertColor(col, 12 * i + 4, _pts[i+1].second);

        raw[6 * i + 4] = _pts[i + 2].first.x;
        raw[6 * i + 5] = _pts[i + 2].first.y;
        insertColor(col, 12 * i + 8, _pts[i+2].second);
      }
      break;

    case GLL_QUADS:
      size = 6 * (_pts.size() / 4);
      raw = new float[size * 2];
      col = new float[size * 4];
      for (int i = 0; i < _pts.size() / 4; i++) {
        raw[12 * i + 0] = _pts[4 * i + 0].first.x;
        raw[12 * i + 1] = _pts[4 * i + 0].first.y;
        insertColor(col, 24 * i + 0, _pts[4 * i + 0].second);

        raw[12 * i + 2] = _pts[4 * i + 1].first.x;
        raw[12 * i + 3] = _pts[4 * i + 1].first.y;
        insertColor(col, 24 * i + 4, _pts[4 * i + 1].second);

        raw[12 * i + 4] = _pts[4 * i + 2].first.x;
        raw[12 * i + 5] = _pts[4 * i + 2].first.y;
        insertColor(col, 24 * i + 8, _pts[4 * i + 2].second);

        raw[12 * i + 6] = _pts[4 * i + 2].first.x;
        raw[12 * i + 7] = _pts[4 * i + 2].first.y;
        insertColor(col, 24 * i + 12, _pts[4 * i + 2].second);

        raw[12 * i + 8] = _pts[4 * i + 3].first.x;
        raw[12 * i + 9] = _pts[4 * i + 3].first.y;
        insertColor(col, 24 * i + 16, _pts[4 * i + 3].second);

        raw[12 * i + 10] = _pts[4 * i + 0].first.x;
        raw[12 * i + 11] = _pts[4 * i + 0].first.y;
        insertColor(col, 24 * i + 20, _pts[4 * i + 0].second);
      }
      break;

    /*case GLL_LINES:d
      size = 2 * (_pts.size() / 2);
      raw = new float[size * 2];
      col = new float[size * 4];
      break;*/
  }

  GLuint vbo_pos, vbo_col, vao;

  glGenBuffers(1, &vbo_pos);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
  glBufferData(GL_ARRAY_BUFFER, size * 2 * sizeof(float), raw, GL_STATIC_DRAW);

  glGenBuffers(1, &vbo_col);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
  glBufferData(GL_ARRAY_BUFFER, size * 4 * sizeof(float), col, GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

  delete raw;
  delete col;
  _pts.clear();

  gllBaseS.bind();

  GLint loc = glGetUniformLocation(gllBaseS._pID, "scale");
  if (loc != -1) {
    //cout << Graphics::current->width << " " <<  << endl;
    glUniform2f(loc,
      Graphics::current->width / 2.0,
      Graphics::current->height / 2.0);
  } else {
    cout << "scale not found!" << endl;
  }

  glDisable(GL_DEPTH_TEST);
  
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, size);
  glEnable(GL_DEPTH_TEST);

  gllBaseS.unbind();

  glDeleteBuffers(1, &vbo_pos);
  glDeleteBuffers(1, &vbo_col);
  glDeleteVertexArrays(1, &vao);
}

void Gll::gllText(string s, int x, int y, int xAlign, int yAlign, float scale) {
  float width = s.size() * scale * gllFontCharSize.x;
  float height = scale * gllFontCharSize.y;
  
  float tx = (x - (xAlign + 1) * 0.5 * width) * 2 / Graphics::current->width - 1;
  float ty = (y - (yAlign + 1) * 0.5 * height) * 2 / Graphics::current->height - 1;

  float cx = scale * gllFontCharSize.x / Graphics::current->width * 2;
  float cy = scale * gllFontCharSize.y / Graphics::current->height * 2;

  int sz = 6 * s.size();

  float* poss = new float[sz * 2];
  float* texs = new float[sz * 2];

  for (int i = 0; i < s.size(); i++) {
    poss[12 * i + 0] = tx + i * cx;
    poss[12 * i + 1] = ty;
    poss[12 * i + 2] = tx + i * cx;
    poss[12 * i + 3] = ty + cy;
    poss[12 * i + 4] = tx + i * cx + cx;
    poss[12 * i + 5] = ty + cy;

    poss[12 * i + 6] = tx + i * cx + cx;
    poss[12 * i + 7] = ty + cy;
    poss[12 * i + 8] = tx + i * cx + cx;
    poss[12 * i + 9] = ty;
    poss[12 * i + 10] = tx + i * cx;
    poss[12 * i + 11] = ty;

    iVec2 charPos;
    charPos.x = s[i] % gllFontCharCount.x;
    charPos.y = s[i] / gllFontCharCount.y;

    fVec2 charLow = {charPos.x * 1.0 / gllFontCharCount.x, charPos.y * 1.0 / gllFontCharCount.y};
    fVec2 charHigh = { (charPos.x+1) * 1.0 / gllFontCharCount.x, (charPos.y+1) * 1.0 / gllFontCharCount.y };

    texs[12 * i + 0] = charLow.x;
    texs[12 * i + 1] = charHigh.y;
    texs[12 * i + 2] = charLow.x;
    texs[12 * i + 3] = charLow.y;
    texs[12 * i + 4] = charHigh.x;
    texs[12 * i + 5] = charLow.y;

    texs[12 * i + 6] = charHigh.x;
    texs[12 * i + 7] = charLow.y;
    texs[12 * i + 8] = charHigh.x;
    texs[12 * i + 9] = charHigh.y;
    texs[12 * i + 10] = charLow.x;
    texs[12 * i + 11] = charHigh.y;
  }

  GLuint vbo_p, vbo_l, vao;

  glGenBuffers(1, &vbo_p);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_p);
  glBufferData(GL_ARRAY_BUFFER, sz * 2 * sizeof(float), poss, GL_STATIC_DRAW);
  
  glGenBuffers(1, &vbo_l);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_l);
  glBufferData(GL_ARRAY_BUFFER, sz * 2 * sizeof(float), texs, GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_p);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_l);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  gllTextS.bind();

  GLint loc = glGetUniformLocation(gllTextS._pID, "color");
  if (loc != -1) {
    glUniform4f(loc,
      ((_col >> 16) & 0xff) / 255.0,
      ((_col >> 8) & 0xff) / 255.0,
      ((_col >> 0) & 0xff) / 255.0,
      ((_col >> 24) & 0xff) / 255.0);
      //1);
  } else {
    cout << "color not found!" << endl;
  }

  loc = glGetUniformLocation(gllTextS._pID, "font");
  if (loc != -1) {
   
    gllFontMap.bind(loc, 0);

  } else {
    cout << "font not found!" << endl;
  }

  glDisable(GL_DEPTH_TEST);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, sz);
  glEnable(GL_DEPTH_TEST);

  gllTextS.unbind();

  glDeleteBuffers(1, &vbo_p);
  glDeleteBuffers(1, &vbo_l);
  glDeleteVertexArrays(1, &vao);

  delete texs, poss;
}

void Gll::gllIcon(Icon* ic, int cax, int cay, int cbx, int cby) {

  float* poss = new float[12];
  float* texs = new float[12];

  fVec2 ca = fVec2(cax, cay) / fVec2(Graphics::current->width, Graphics::current->height) * 2.0 - 1;
  fVec2 cb = fVec2(cbx, cby) / fVec2(Graphics::current->width, Graphics::current->height) * 2.0 - 1;

  poss[0] = ca.x;
  poss[1] = ca.y;
  poss[2] = ca.x;
  poss[3] = cb.y;
  poss[4] = cb.x;
  poss[5] = cb.y;

  poss[6] = cb.x;
  poss[7] = cb.y;
  poss[8] = cb.x;
  poss[9] = ca.y;
  poss[10] = ca.x;
  poss[11] = ca.y;

  texs[0] = ic->_tl.x;
  texs[1] = ic->_br.y;
  texs[2] = ic->_tl.x;
  texs[3] = ic->_tl.y;
  texs[4] = ic->_br.x;
  texs[5] = ic->_tl.y;

  texs[6] = ic->_br.x;
  texs[7] = ic->_tl.y;
  texs[8] = ic->_br.x;
  texs[9] = ic->_br.y;
  texs[10] = ic->_tl.x;
  texs[11] = ic->_br.y;

  GLuint vbo_p, vbo_l, vao;

  glGenBuffers(1, &vbo_p);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_p);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), poss, GL_STATIC_DRAW);

  glGenBuffers(1, &vbo_l);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_l);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), texs, GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_p);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_l);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  gllTextS.bind();

  GLint loc = glGetUniformLocation(gllTextS._pID, "color");
  if (loc != -1) {
    glUniform4f(loc,
      ((_col >> 16) & 0xff) / 255.0,
      ((_col >> 8) & 0xff) / 255.0,
      ((_col >> 0) & 0xff) / 255.0,
      ((_col >> 24) & 0xff) / 255.0);
      //1);
  } else {
    cout << "color not found!" << endl;
  }

  loc = glGetUniformLocation(gllTextS._pID, "font");
  if (loc != -1) {

    ic->_from->_dataSrc.bind(loc, 0);

  } else {
    cout << "font not found!" << endl;
  }

  glDisable(GL_DEPTH_TEST);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_DEPTH_TEST);

  gllTextS.unbind();

  glDeleteBuffers(1, &vbo_p);
  glDeleteBuffers(1, &vbo_l);
  glDeleteVertexArrays(1, &vao);

  delete texs, poss;
}