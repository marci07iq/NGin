#pragma once

#include <GL/glew.h>
#include "../GL/freeglut.h"
#include "ClientCore.h"
#include "stb_image.h"

#pragma comment(lib, "glew32.lib")

extern map<string, map<string, colorargb>> colors;

colorargb getColor(string object, string key);
colorargb getColor(string val);
colorargb getColor(xml_node<>* me, string elem, string key);

void loadColors(string filename = "html/colors.cfg");

class Canvas;

class key {
public:
  enum type {
    type_key = 0,
    type_special = 1,
    type_mouse = 2,
    type_wheel = 3
  };
  int _type;
  int _keycode;
  bool isKey();
  void fromKey(unsigned char key);
  void fromSpecial(int key);
  void fromMouse(int button);
  void fromWheel(int delta);
  string toKeyName();
  string toSpecialName();
  string toMouseName();
  string toWheelName();
  string toName();
  key(int keycode, int type);
  key(string keycode);
  key();
};

class key_location : public key {
public:
  int _mx;
  int _my;
  void setLocation(int mx, int my);
  key_location(int keycode, int type, int mx, int my);
  key_location(key k) {
    _keycode = k._keycode;
    _type = k._type;
  }
  key_location();
};


bool operator==(const key& lhs, const key& rhs);

bool operator<(const key& lhs, const key& rhs);

bool operator<=(const key& lhs, const key& rhs);

bool operator>(const key& lhs, const key& rhs);

bool operator>=(const key& lhs, const key& rhs);

class gui_event {
public:
  key_location _key;
  enum type {
    evt_none = 0,
    evt_down = 1,
    evt_pressed = 2,
    evt_up = 3
  };
  type _type;
  string toName();
  gui_event(key_location key, type type);
};

key loadKey(xml_attribute<>* me);

typedef void(*ClickCallback)();
typedef void(*CheckCallback)(bool);
typedef void(*TextInputFunc)(string);
typedef void(*ControlInputFunc)(key, int);
typedef void(*SliderInputFunc)(float);
typedef bool(*TextValidatorFunc)(string, int, unsigned char);

typedef void(*RenderManager)();
typedef void(*ResizeManager)(int x, int y);
//typedef void(*GUIEventManager)(gui_event evt, int x, int y, set<key_location>& down);
typedef void(*KeyManager)(unsigned char key, int x, int y);
typedef void(*SpecialKeyManager)(int key, int x, int y);
typedef void(*MouseClickManager)(int idk, int key, int x, int y);
typedef void(*MouseWheelManager)(int idk, int key, int x, int y);
typedef void(*MouseEntryManager)(int state);
typedef void(*MouseMoveManager)(int x, int y);

typedef int(*IRenderManager)(int ax, int ay, int bx, int by, set<key_location>& down);
typedef int(*IResizeManager)(int x, int y);
typedef int(*IGUIEventManager)(gui_event evt, int x, int y, set<key_location>& down, Canvas* me);
typedef int(*IMouseEntryManager)(int state);
typedef int(*IMouseMoveManager)(int x, int y, int ox, int oy, set<key_location>& down);

int defaultIRenderManager(int ax, int ay, int bx, int by, set<key_location>& down);
int defaultIResizeManager(int x, int y);
int defaultIGUIEventManager(gui_event evt, int x, int y, set<key_location>& down);
int defaultIMouseEntryManager(int state);
int defaultIMouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down);

struct OpenGLData {
  GLdouble model_view[16];
  GLdouble projection[16];
  GLint viewport[4];
  vec3<double> cameraEye;
};

struct WindowManagers {
  RenderManager renderManager;
  ResizeManager resizeManager;
  KeyManager keyManager;
  SpecialKeyManager specialKeyManager;
  KeyManager keyUpManager;
  SpecialKeyManager specialUpKeyManager;
  MouseEntryManager mouseEntryManager;
  MouseMoveManager mouseMoveManager;
  MouseClickManager mouseClickManager;
  MouseWheelManager mouseWheelManager;
};

struct IWindowManagers {
  IRenderManager renderManager         ;//= defaultIRenderManager;
  IResizeManager resizeManager;//= defaultIResizeManager;
  IGUIEventManager guiEventManager;// = defaultIKeyManager;
  IMouseEntryManager mouseEntryManager;//= defaultIMouseEntryManager;
  IMouseMoveManager mouseMoveManager;//= defaultIMouseMoveManager;
};

//extern map<int, Graphics::WindowData> Graphics::windows;

namespace Graphics {

  void resetViewport();

}

class LinearScale {
public:
  int absc;
  float relc;
  LinearScale();
  LinearScale(float relc);
  LinearScale(float relc, int absc);
  int getVal(int size);
};

LinearScale operator+(LinearScale& lhs, LinearScale& rhs);
LinearScale operator-(LinearScale& lhs, LinearScale& rhs);
LinearScale operator-(LinearScale& lhs);
LinearScale operator*(LinearScale& lhs, float rhs);
LinearScale operator*(float rhs, LinearScale& lhs);
LinearScale operator/(LinearScale& lhs, float rhs);
LinearScale operator*(LinearScale& lhs, float& rhs);
LinearScale operator*(float& rhs, LinearScale& lhs);
LinearScale operator/(LinearScale& lhs, float& rhs);

class LocationData {
public:
  enum Lock {
    Lock_Min,
    Lock_Max,
    Lock_Mid,
    Lock_Dist
  };
  LinearScale bottom, top, left, right;

  LocationData();
  LocationData(LinearScale lbottom, LinearScale ltop, LinearScale lleft, LinearScale lright);

  void setWidth(LinearScale to, Lock lock);
  void setHeight(LinearScale to, Lock lock);
  void setTop(LinearScale to, Lock lock);
  void setBot(LinearScale to, Lock lock);
  void setLeft(LinearScale to, Lock lock);
  void setRight(LinearScale to, Lock lock);
  void setMidLR(LinearScale to, Lock lock);
  void setMidBT(LinearScale to, Lock lock);

  int getLeft(int contWidth);
  int getRight(int contWidth);
  int getTop(int contHeigth);
  int getBot(int contHeigth);
  int getWidth(int contWidth);
  int getHeight(int contHeight);
  
};

extern LocationData fullContainer;

/// <summary>
/// Return the lock that can be used when setting
/// </summary>
/// <param name="to">The previously set parameter. Will be returned if possible. Changed to newlock</param>
/// <param name="newlock">What the new set will be. NEVER returned</param>
/// <returns>Lock that can be passed to set function</returns>
LocationData::Lock setLock(LocationData::Lock& to, LocationData::Lock newlock);

/// <summary>
/// Load linear scale from xml node attributes.
/// </summary>
/// <param name="me">XML node</param>
/// <returns>LinearScale loaded result</returns>
LinearScale loadLinear(xml_node<>* me);

/// <summary>
/// Load location data from location xml tag.
/// </summary>
/// <param name="me">The location tag</param>
/// <returns>Loaded location data</returns>
LocationData loadLocation(xml_node<>* me);

static void shapesPrintf(int row, int col, const char *fmt, ...);

void setColor(colorargb v);

void renderBitmapString(float x, float y, string text, colorargb color, bool center, int cursor = -1);

GLuint png_texture_load(string filename, int& w, int& h);

bool numericalValidator(string s, int cursor, unsigned char c);
bool floatValidator(string s, int cursor, unsigned char c);
bool textValidator(string s, int cursor, unsigned char c);