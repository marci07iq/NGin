#pragma once

#include "../Maths/helperFunctions.h"

int jumpAfterClosing(const string& data, int open);

char unescapeChar(const string& data, int& from);

string escapeChar(char c);

/// <summary>
/// Escape sensitive charates in string
/// Characters escaped: are: \" \'
/// </summary>
/// <param name="data">String to escape</param>
/// <param name="from">First character index</param>
/// <param name="to">Last character index (not included)</param>
/// <returns>Escaped string</returns>
string escape(const string& data, int from = 0, int to = -1);

string unescape(const string& data, int from = 0, int to = -1);

class ScriptRunner {
  int energyLeft;
  string error;
};

class ScriptDataBase;

class ScriptDataBool;
class ScriptDataChar;
class ScriptDataInteger;
class ScriptDataFloat;
class ScriptDataString;
class ScriptDataArray;
class ScriptDataDict;

class ScriptData {
public:
  ScriptData();
  ScriptData(ScriptDataBase* val);
  ScriptDataBase* _val;
  size_t _instances;
  void fromString(const string& s, int& from);
  string toString();
  ScriptData* CopyContent();
  ScriptDataBool* asBool();
  ScriptDataChar* asChar();
  ScriptDataInteger* asInt();
  ScriptDataFloat* asFloat();
  ScriptDataString* asStr();
  ScriptDataArray* asArray();
  ScriptDataDict* asDict();

  ScriptData* toBool();
  ScriptData* toChar();
  ScriptData* toInt();
  ScriptData* toFloat();
  ScriptData* toStr();
  ScriptData* toArray();
  ScriptData* toDict();

  ~ScriptData();
};

class ScriptDataBase {
public:
  enum VarType {
    TNULL = 0, //Nothing
    TBOOLEAN = 1,
    TCHAR = 2,
    TINTEGER = 3,
    TFLOAT = 4,
    TSTRING = 5,
    TARRAY = 6,
    TDICT = 7
  };
  virtual VarType type();
  virtual ScriptDataBase* CopyContent();
  virtual string toString();
  virtual void fromString(const string& s, int& from);

  ScriptData* _up;

  virtual ScriptData* toBool();
  virtual ScriptData* toChar();
  virtual ScriptData* toInt();
  virtual ScriptData* toFloat();
  virtual ScriptData* toStr();
  virtual ScriptData* toArray();
  virtual ScriptData* toDict();

  ScriptDataBase();
  ~ScriptDataBase();
};

class ScriptDataBool : public ScriptDataBase {
public:
  bool _bl;
  ScriptDataBool();
  ScriptDataBool(bool bl) {
    _bl = bl;
  }
  ScriptDataBool(string str) {
    int start = 0;
    fromString(str, start);
  }
  VarType type();
  ScriptDataBase* CopyContent();
  string toString();
  void fromString(const string& s, int& from);

  ScriptData* toBool();
  ScriptData* toChar();
  ScriptData* toInt();
  ScriptData* toFloat();
  ScriptData* toStr();
  ScriptData* toArray();
  ScriptData* toDict();
};
class ScriptDataChar : public ScriptDataBase {
public:
  char _chr;
  ScriptDataChar();
  ScriptDataChar(char chr);
  VarType type();
  ScriptDataBase* CopyContent();
  string toString();
  void fromString(const string& s, int& from);

  ScriptData* toBool();
  ScriptData* toChar();
  ScriptData* toInt();
  ScriptData* toFloat();
  ScriptData* toStr();
  ScriptData* toArray();
  ScriptData* toDict();

};
class ScriptDataInteger : public ScriptDataBase {
public:
  uint64_t _num;
  ScriptDataInteger();
  ScriptDataInteger(int num) {
    _num = num;
  }
  ScriptDataInteger(string str) {
    int start = 0;
    fromString(str, start);
  }
  VarType type();
  ScriptDataBase* CopyContent();
  string toString();
  void fromString(const string& s, int& from);

  ScriptData* toBool();
  ScriptData* toChar();
  ScriptData* toInt();
  ScriptData* toFloat();
  ScriptData* toStr();
  ScriptData* toArray();
  ScriptData* toDict();
};
class ScriptDataFloat : public ScriptDataBase {
public:
  double _num;
  ScriptDataFloat();
  ScriptDataFloat(double num) {
    _num = num;
  }
  ScriptDataFloat(string str) {
    int start = 0;
    fromString(str, start);
  }
  VarType type();
  ScriptDataBase* CopyContent();
  string toString();
  void fromString(const string& s, int& from);

  ScriptData* toBool();
  ScriptData* toChar();
  ScriptData* toInt();
  ScriptData* toFloat();
  ScriptData* toStr();
  ScriptData* toArray();
  ScriptData* toDict();
};
class ScriptDataString : public ScriptDataBase {
public:
  string _str;
  ScriptDataString();
  ScriptDataString(string s) {
    _str = s;
  }
  VarType type();
  ScriptDataBase* CopyContent();
  string toString();
  void fromString(const string& s, int& from);

  ScriptData* toBool();
  ScriptData* toChar();
  ScriptData* toInt();
  ScriptData* toFloat();
  ScriptData* toStr();
  ScriptData* toArray();
  ScriptData* toDict();
};
class ScriptDataArray : public ScriptDataBase {
public:
  vector<ScriptData*> _data;
  ScriptDataArray();
  VarType type();
  ScriptDataBase* CopyContent();
  string toString();
  void fromString(const string& s, int& from);

  ScriptData* toBool();
  ScriptData* toChar();
  ScriptData* toInt();
  ScriptData* toFloat();
  ScriptData* toStr();
  ScriptData* toArray();
  ScriptData* toDict();
};
class ScriptDataDict : public ScriptDataBase {
public:
  map<string, ScriptData*> _elems;
  ScriptDataDict();
  VarType type();
  ScriptDataBase* CopyContent();
  string toString();
  void fromString(const string& s, int& from);

  ScriptData* toBool();
  ScriptData* toChar();
  ScriptData* toInt();
  ScriptData* toFloat();
  ScriptData* toStr();
  ScriptData* toArray();
  ScriptData* toDict();
};

/*namespace ScriptApiFunctions {
  //In:
  //Root: NUMERIC number to convert
  //Out:
  //Root: STRING result
  ScriptData* num_to_str(ScriptData& _args);

  //In:
  //Root: STRING number to convert
  //Out:
  //Root: NUMERIC result
  ScriptData* str_to_num(ScriptData& _args);

  //In:
  //Root: Array of STRING, concat order lexographic (std::map)
  //Out:
  //Root: STRING result
  ScriptData* str_concat(ScriptData& _args);

  //In:
  //Root: STRING
  //Out:
  //Root: NUMERIC result, string length
  ScriptData* str_len(ScriptData& _args);

  //In:
  //str STRING
  //i NUMERIC
  //Out:
  //str STRING
  ScriptData* str_index(ScriptData& _args);

  //In:
  //a NUMERIC
  //b NUMERIC
  //Out:
  //r NUMERIC
  ScriptData* num_rand(ScriptData& _args);
}*/

ScriptData* CopyPtr(ScriptData* _in);
bool DeletePtr(ScriptData* _in);

typedef ScriptData*(*APICall)(ScriptData&);

extern map<string, APICall> apiMap;

class ScriptIBlock;
class ScriptInstruction;
class gui_event;
class key_location;
class GUIElement;

#ifdef SCRIPT_GUI
class ScriptGUI : public GUIElement {
public:
  colorargb bgcolor_even;
  colorargb bgcolor_odd;
  ScriptIBlock* code;

  bool editor = true;
  ScriptInstruction* dragging = NULL;
  
  int cox, coy;
  iVec2 dragOffset, dragPos;
  bool mid = false;


  ScriptGUI(string name, LocationData llocation, colorargb lbgcolorodd, colorargb lactivecolor, colorargb ltextcolor, colorargb lbgcoloreven);

  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);
  void getRect();
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event& evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);
};

class ScriptGUIBase {
public:
  int cax, cay, cbx, cby;

  bool insertingIn = false;

  bool isIn(int mx, int my);
  virtual void getRect(int lcax, int lcay);
  virtual int mouseEnter(int state);
  virtual int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  virtual int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  virtual void renderBg(ScriptGUI* base, int depth);
  virtual void render(ScriptGUI* base, int depth);
};
#endif

class ScriptInstruction
#ifdef SCRIPT_GUI
: public ScriptGUIBase
#endif
{
public:
  ScriptInstruction() {

  }
  virtual ScriptData* run(ScriptData& _args);
  virtual void load(xml_node<> *data);
  virtual void save(xml_node<> *data, xml_document<> &doc);
  virtual ~ScriptInstruction() {
    //throw 1;
  }
#ifdef SCRIPT_GUI

  virtual void getRect(int offsetX, int offsetY);
  virtual int mouseEnter(int state);
  virtual int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  virtual int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  virtual void render(ScriptGUI* base, int depth);
#endif
};

class ScriptIIfElse : public ScriptInstruction {
public:
  ScriptInstruction* _then;
  ScriptInstruction* _else;
  ScriptInstruction* _condition;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  ~ScriptIIfElse();
#ifdef SCRIPT_GUI
  int cty;//Then, else text coord
  int cey;
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

class ScriptILoop : public ScriptInstruction {
public:
  ScriptInstruction* _code;
  ScriptInstruction* _condition;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  int cdy = 0;
  ~ScriptILoop();
#ifdef SCRIPT_GUI
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

class ScriptIAssign : public ScriptInstruction {
public:
  ScriptInstruction* _to;
  ScriptInstruction* _val;
  ScriptData* run(ScriptData& _args);
  int cex; //Equation
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  ~ScriptIAssign();
#ifdef SCRIPT_GUI
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

/*class ScriptICopy : public ScriptInstruction {
public:
  string _to;
  string _from;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  ~ScriptICopy();
#ifdef SCRIPT_GUI
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};*/

class ScriptIConstant : public ScriptInstruction {
public:
  //ScriptDataBase* _val;
  string _val;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  ~ScriptIConstant();
#ifdef SCRIPT_GUI
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

class ScriptIMath : public ScriptInstruction {
public:
  ScriptInstruction* _arg1;
  ScriptInstruction* _arg2;
  enum Operation {
    PLUS = 1,
    MINUS = 2,
    MULTI = 3,
    DIV = 4,
    POW = 5,
    LOGAB = 6,
    MOD = 7,
    ATAN2 = 8,
    L = 9,
    LEQ = 10,
    EQ = 11,
    NEQ = 12,
    GEQ = 13,
    G = 14,
    SQRT = 16,
    SIN = 17,
    COS = 18,
    TAN = 19,
    ASIN = 20,
    ACOS = 21,
    ATAN = 22
  };
  int _oper;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  ~ScriptIMath();
#ifdef SCRIPT_GUI
  int cox; //Operator sign (for binary)
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

class ScriptILogic : public ScriptInstruction {
public:
  ScriptInstruction* _arg1;
  ScriptInstruction* _arg2;
  enum Operation {
    EQ = 1,
    NEQ = 2,
    AND = 3,
    NAND = 4,
    OR = 5,
    NOR = 6,
    XOR = 7,
    NXOR = 8,
    NOT = 16
  };
  Operation _oper;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  ~ScriptILogic();
#ifdef SCRIPT_GUI
  int cox; //Operator sign (for binary)
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

class ScriptIVariable : public ScriptInstruction {
public:
  string _arg;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  ~ScriptIVariable();
#ifdef SCRIPT_GUI
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

class ScriptIIndex : public ScriptInstruction {
public:
  ScriptInstruction* _arg;
  ScriptInstruction* _ind;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  ~ScriptIIndex();
#ifdef SCRIPT_GUI
  int cix;
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

class ScriptIFunctionCall : public ScriptInstruction {
public:
  list<pair<string, ScriptInstruction*>> _arguments;
  ScriptInstruction* _function;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  ~ScriptIFunctionCall();
#ifdef SCRIPT_GUI
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

class ScriptIAPICall : public ScriptInstruction {
public:
  APICall _func;
  list<pair<string, ScriptInstruction*>> _arguments;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
#ifdef SCRIPT_GUI
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};

class ScriptIBlock : public ScriptInstruction {
public:
  list<ScriptInstruction*> _instructions;
  ScriptData* run(ScriptData& _args);
  void load(xml_node<> *data);
  void save(xml_node<> *data, xml_document<> &doc);
  ~ScriptIBlock();
#ifdef SCRIPT_GUI
  void getRect(int offsetX, int offsetY);
  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down);
  void render(ScriptGUI* base, int depth);
#endif
};