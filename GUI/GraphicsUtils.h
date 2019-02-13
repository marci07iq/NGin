#pragma once

#include "Shader.h"

extern map<string, map<string, colorargb>> colors;

colorargb getColor(string object, string key);
colorargb getColor(string val);
colorargb getColor(xml_node<>* me, string elem, string key);

void loadColors(string filename = "html/colors.cfg");

class key_config;
class key_location;
class gui_event;

class Canvas;
class GUIElement;
namespace Graphics {
  class GWindow;
  typedef GUIElement* ElemHwnd;
  typedef GLFWwindow* RawWinHwnd;
}

typedef void(*ClickCallback)(Graphics::ElemHwnd, void*);
typedef void(*CheckCallback)(Graphics::ElemHwnd, void*, bool&);
typedef void(*TextInputFunc)(Graphics::ElemHwnd, void*, string&);
typedef void(*ControlInputFunc)(Graphics::ElemHwnd, void*, key_config&);
typedef void(*SliderInputFunc)(Graphics::ElemHwnd, void*, float&);
typedef bool(*TextValidatorFunc)(Graphics::ElemHwnd, string, int, unsigned char);

typedef void(*RenderManager)();
typedef void(*ResizeManager)(int x, int y);
typedef int(*GUIEventManager)(gui_event& evt, int x, int y, set<key_location>& down);
/*typedef void(*KeyManager)(unsigned char key, int x, int y);
typedef void(*SpecialKeyManager)(int key, int x, int y);*/
typedef void(*MouseClickManager)(int idk, int key, int x, int y);
typedef void(*MouseWheelManager)(int idk, int key, int x, int y);
typedef void(*MouseEntryManager)(int state);
typedef void(*MouseMoveManager)(int x, int y);
typedef void(*WindowCloseManager)();

typedef int(*IRenderManager)(Canvas*, int ax, int ay, int bx, int by, set<key_location>& down);
typedef int(*IResizeManager)(Canvas*, int x, int y);
typedef int(*IGUIEventManager)(Canvas*, gui_event& evt, int x, int y, set<key_location>& down);
typedef int(*IMouseEntryManager)(Canvas*, int state);
typedef int(*IMouseMoveManager)(Canvas*, int x, int y, int ox, int oy, set<key_location>& down);

int defaultIRenderManager(Canvas*, int ax, int ay, int bx, int by, set<key_location>& down);
int defaultIResizeManager(Canvas*, int x, int y);
int defaultIGUIEventManager(Canvas*, gui_event& evt, int x, int y, set<key_location>& down);
int defaultIMouseEntryManager(Canvas*, int state);
int defaultIMouseMoveManager(Canvas*, int x, int y, int ox, int oy, set<key_location>& down);

struct OpenGLData {
  GLdouble model_view[16];
  GLdouble projection[16];
  GLint viewport[4];
  vec3<double> cameraEye;
};

struct WindowManagers {
  RenderManager renderManager;
  ResizeManager resizeManager;
  GUIEventManager guiEventManager;
  /*KeyManager keyManager;
  SpecialKeyManager specialKeyManager;
  KeyManager keyUpManager;
  SpecialKeyManager specialUpKeyManager;*/
  MouseEntryManager mouseEntryManager;
  MouseMoveManager mouseMoveManager;
  MouseClickManager mouseClickManager;
  MouseWheelManager mouseWheelManager;
  WindowCloseManager windowCloseManager;
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

//static void shapesPrintf(int row, int col, const char *fmt, ...);

void setColor(colorargb v);

void renderBitmapString(float x, float y, string text, colorargb color, bool center, int cursor = -1);

bool numericalValidator(Graphics::ElemHwnd e, string s, int cursor, unsigned char c);
bool floatValidator(Graphics::ElemHwnd e, string s, int cursor, unsigned char c);
bool textValidator(Graphics::ElemHwnd e, string s, int cursor, unsigned char c);

void insertColor(float* arr, size_t to, colorargb col, fVec3 light = { 1,1,1 });
void insertVector(float* arr, size_t to, fVec3 vec);

namespace Gll {
  enum gllModes {
    GLL_POLY,
    GLL_QUADS,
    //GLL_LINES,
    //GLL_LINES,
    //GLL_LINE_STRIP
  };

  extern gllModes _mode;
  extern vector<pair<fVec2, colorargb>> _pts;
  extern colorargb _col;

  extern Shader gllBaseS;
  //extern Shader gllLinesS;
  extern Shader gllTextS;

  extern Texture gllFontMap;
  extern iVec2 gllFontCharSize;
  extern iVec2 gllFontCharCount;

  extern Graphics::RawWinHwnd initOn;

  void gllInit(string base);

  void gllBegin(gllModes m);

  void gllVertex(fVec2 pt);
  void gllVertex(double x, double y);

  /*void gllVertex(fVec3 pt);
  void gllVertex(double x, double y, double z);*/


  void gllColor(colorargb col);

  void gllText(string s, int x, int y, int xAlign = -1, int yAlign = -1, float scale = 1); //-1: left, 0:center, 1:right

  void gllEnd();

  void gllIcon(Icon* ic, int cax, int cay, int cbx, int cby);
}

pair<float, float> scrollBar(float contentHeight, float offset, float poslow, float poshigh, float minSize, float margin);

string openFileSelector(string message, list<pair<string, string>> formats = {});

string saveFileSelector(string message, list<pair<string, string>> formats = {}, string ext = "");
