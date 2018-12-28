#include "Scripts.h"

/*//In:
//Root: NUMERIC number to convert
//Out:
//Root: STRING result
ScriptData* ScriptApiFunctions::num_to_str(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->CopyContent(&_args);
  res->type = ScriptDataBase::TSTRING;
  return res;
}

//In:
//Root: STRING number to convert
//Out:
//Root: NUMERIC result
ScriptData* ScriptApiFunctions::str_to_num(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->CopyContent(&_args);
  res->type = ScriptDataBase::TFLOAT;
  return res;
}

//In:
//Root: Array of STRING, concat order lexographic (std::map)
//Out:
//Root: STRING result
ScriptData* ScriptApiFunctions::str_concat(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->type = ScriptDataBase::TSTRING;
  for (auto&& it : _args._elems) {
    res->data.fromType<string>(res->data.toType<string>() + it.secondScriptDataFloat.toType<string>());
  }
  return res;
}

//In:
//Root: STRING
//Out:
//Root: NUMERIC result, string length
ScriptData* ScriptApiFunctions::str_len(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->type = ScriptDataBase::TFLOAT;
  res->data.fromType<int>(_args._data.toType<string>().length());
  return res;
}

//In:
//str STRING
//i NUMERIC
//Out:
//str STRING
ScriptData* ScriptApiFunctions::str_index(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->type = ScriptDataBase::TFLOAT;
  res->data.fromType<char>(_args._elems["str"]ScriptDataFloat.toType<string>()[_args._elems["i"]ScriptDataFloat.toType<int>()]);
  return res;
}

//In:
//a NUMERIC
//b NUMERIC
//Out:
//r NUMERIC
ScriptData* ScriptApiFunctions::num_rand(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->type = ScriptDataBase::TFLOAT;
  double a = _args._elems["a"]ScriptDataFloat.toType<double>();
  double b = _args._elems["b"]ScriptDataFloat.toType<double>();
  res->data.fromType<double>(a + (b - a)*ran1());
  return res;
}*/

int jumpAfterClosing(const string & data, int open, char end) {
  while (open < data.length() && data[open] != end) {
    if (data[open] == '\\') {
      open += 2;
    } else {
      open = jumpAfterClosing(data, open);
    }
  }
  return open;
}


int jumpAfterClosing(const string & data, int open) {
  char end = '\0';
  switch (data[open]) {
    case '(':
      end = ')';
      break;
    case '[':
      end = ']';
      break;
    case '{':
      end = '}';
      break;
    case '\"':
      end = '\"';
      break;
    case '\'':
      end = '\'';
      break;
    
  }
  ++open;
  if (end == '\0') {
    return open;
  }
  return jumpAfterClosing(data, open, end);
}

char unescapeChar(const string & data, int & from) {
  ++from;
  if (data[from - 1] == '\\') {
    ++from;

    switch (data[from - 1]) {
      case '\'':
        return '\'';
        break;
      case '\"':
        return ('\"');
        break;
      case '?':
        return ('?');
        break;
      case '\\':
        return ('\\');
        break;
      case 'a':
        return ('\a');
        break;
      case 'b':
        return ('\b');
        break;
      case 'f':
        return ('\f');
        break;
      case 'n':
        return ('\n');
        break;
      case 'r':
        return ('\r');
        break;
      case 't':
        return ('\t');
        break;
      case 'v':
        return ('\v');
        break;
      default:
        return data[from - 1];
    }
  }
  return data[from - 1];
}
string escapeChar(char c) {
  switch (c) {
    case '\'':
      return "\\\'";
      break;
    case '\"':
      return "\\\"";
      break;
    case '\?':
      return "\\?";
      break;
    case '\\':
      return "\\\\";
      break;
    case '\a':
      return "\\a";
      break;
    case '\b':
      return "\\b";
      break;
    case '\f':
      return "\\f";
      break;
    case '\n':
      return "\\n";
      break;
    case '\r':
      return "\\r";
      break;
    case '\t':
      return "\\t";
      break;
    case '\v':
      return "\\v";
      break;
    default:
      return string(1, c);
  }
}

/// <summary>
/// Escape sensitive charates in string
/// Characters escaped: are: \" \'
/// </summary>
/// <param name="data">String to escape</param>
/// <param name="from">First character index</param>
/// <param name="to">Last character index (not included)</param>
/// <returns>Escaped string</returns>
string escape(const string & data, int from, int to) {
  if (to < 0) {
    to = data.size() + 1 + to;
  }
  string res;
  while (from < to) {
    res += escapeChar(data[from]);
    ++from;
  }

  return res;
}
string unescape(const string & data, int from, int to) {
  if (to < 0) {
    to = data.size() + 1 + to;
  }

  string res;
  while (from < to) {
    res += unescapeChar(data, from);
  }
  return res;
}

ScriptData::ScriptData() {
  _instances = 1;
  _val = NULL;
}
ScriptData::ScriptData(ScriptDataBase * val) {
  _val = val;
  _instances = 1;
  val->_up = this;
}
void ScriptData::fromString(const string& s, int& from) {
  while (trimChar(s[from], Trim_Space)) {
    ++from;
  }
  ScriptDataBase* res = NULL;
  switch (s[from]) {
    case '\'':
      res = new ScriptDataChar();
      res->fromString(s, from);
      break;
    case '\"':
      res = new ScriptDataString();
      res->fromString(s, from);
      break;
    case '[':
      res = new ScriptDataArray();
      res->fromString(s, from);
      break;
    case '{':
      res = new ScriptDataDict();
      res->fromString(s, from);
      break;
    case 't':
    case 'T':
    case 'f':
    case 'F':
      res = new ScriptDataBool();
      res->fromString(s, from);
      break;
    default:
      int sfrom = from;

      bool p = false;
      while (sfrom < s.length() && (('0' <= s[sfrom] && s[sfrom] <= '9') || s[sfrom] == '-' || s[sfrom] == 'e' || s[sfrom] == '.')) {
        if (s[sfrom] == '.') {
          p = true;
        }
        ++sfrom;
      }

      if (p) {
        res = new ScriptDataFloat();
        res->fromString(s, from);
      } else {
        res = new ScriptDataInteger();
        res->fromString(s, from);
      }
  }
  if (res != NULL) {
    if (_val) {
      delete _val;
    }
    _val = res;
    res->_up = this;
  }
}
string ScriptData::toString() {
  if (_val) {
    return _val->toString();
  }
  return "";
}
ScriptData * ScriptData::CopyContent() {
  return new ScriptData(_val->CopyContent());
}
ScriptDataBool * ScriptData::asBool() {
  return (ScriptDataBool*)_val;
}
ScriptDataChar * ScriptData::asChar() {
  return (ScriptDataChar*)_val;
}
ScriptDataInteger * ScriptData::asInt() {
  return (ScriptDataInteger*)_val;
}
ScriptDataFloat * ScriptData::asFloat() {
  return (ScriptDataFloat*)_val;
}
ScriptDataString * ScriptData::asStr() {
  return (ScriptDataString*)_val;
}
ScriptDataArray * ScriptData::asArray() {
  return (ScriptDataArray*)_val;
}
ScriptDataDict * ScriptData::asDict() {
  return (ScriptDataDict*)_val;
}
ScriptData * ScriptData::toBool() {
  return _val->toBool();
}
ScriptData * ScriptData::toChar() {
  return _val->toChar();
}
ScriptData * ScriptData::toInt() {
  return _val->toInt();
}
ScriptData * ScriptData::toFloat() {
  return _val->toFloat();
}
ScriptData * ScriptData::toStr() {
  return _val->toStr();
}
ScriptData * ScriptData::toArray() {
  return _val->toArray();
}
ScriptData * ScriptData::toDict() {
  return _val->toDict();
}
ScriptData::~ScriptData() {
  if(_val != NULL) {
    delete _val;
  }
}

ScriptData* CopyPtr(ScriptData* _in) {
  if (_in != NULL) {
    _in->_instances += 1;
  }
  return _in;
}
bool DeletePtr(ScriptData* _in) {
  if (_in != NULL) {
    _in->_instances -= 1;
    if (_in->_instances == 0) {
      delete _in;
      _in = NULL;
      return true;
    }
  }
  _in = NULL;
  return false;
}

ScriptDataBase::ScriptDataBase() {
}
ScriptDataBase::VarType ScriptDataBase::type() {
  throw 1;
  return TNULL;
}
ScriptDataBase * ScriptDataBase::CopyContent() {
  return NULL;
}
string ScriptDataBase::toString() {
  throw 1;
  return "null";
}
void ScriptDataBase::fromString(const string & s, int& from) { //Try to decode from string

}
ScriptData * ScriptDataBase::toBool() {
  return NULL;
}
ScriptData * ScriptDataBase::toChar() {
  return NULL;
}
ScriptData * ScriptDataBase::toInt() {
  return NULL;
}
ScriptData * ScriptDataBase::toFloat() {
  return NULL;
}
ScriptData * ScriptDataBase::toStr() {
  return NULL;
}
ScriptData * ScriptDataBase::toArray() {
  return NULL;
}
ScriptData * ScriptDataBase::toDict() {
  return NULL;
}
ScriptDataBase::~ScriptDataBase() {

}

ScriptDataBool::ScriptDataBool() {
  _bl = false;
}
ScriptDataBase::VarType ScriptDataBool::type() {
  return TBOOLEAN;
}
ScriptDataBase * ScriptDataBool::CopyContent() {
  ScriptDataBool* res = new ScriptDataBool();
  res->_bl = _bl;
  return res;
}
string ScriptDataBool::toString() {
  return _bl ? "true" : "false";
}
void ScriptDataBool::fromString(const string & s, int& from) { //Try to decode from string
  _bl = false;
  if (s.substr(from, 4) == "true") {
    _bl = true;
    from += 4;
    return;
  }
  if (s.substr(from, 4) == "True") {
    _bl = true;
    from += 4;
    return;
  }
  if (s.substr(from, 5) == "false") {
    _bl = true;
    from += 5;
    return;
  }
  if (s.substr(from, 5) == "False") {
    _bl = true;
    from += 5;
    return;
  }
  _bl = false;
  ++from;
}
ScriptData * ScriptDataBool::toBool() {
  return CopyPtr(_up);
}
ScriptData * ScriptDataBool::toChar() {
  return new ScriptData(new ScriptDataChar(_bl));
}
ScriptData * ScriptDataBool::toInt() {
  return new ScriptData(new ScriptDataInteger(_bl));
}
ScriptData * ScriptDataBool::toFloat() {
  return new ScriptData(new ScriptDataFloat(_bl));
}
ScriptData * ScriptDataBool::toStr() {
  return new ScriptData(new ScriptDataString(toString()));
}
ScriptData * ScriptDataBool::toArray() {
  return NULL;
}
ScriptData * ScriptDataBool::toDict() {
  return NULL;
}

ScriptDataChar::ScriptDataChar() {
}
ScriptDataChar::ScriptDataChar(char chr) {
  _chr = chr;
}
ScriptDataBase::VarType ScriptDataChar::type() {
  return TCHAR;
}
ScriptDataBase * ScriptDataChar::CopyContent() {
  ScriptDataChar* res = new ScriptDataChar();
  res->_chr = _chr;
  return res;
}
string ScriptDataChar::toString() {
  return "\'" + escapeChar(_chr) + "\'";
}
void ScriptDataChar::fromString(const string & s, int& from) { //Try to decode from string
  ++from;
  _chr = unescapeChar(s, from);
  while (s[from] != '\'') {
    ++from;
  }
  ++from;
}
ScriptData * ScriptDataChar::toBool() {
  return new ScriptData(new ScriptDataBool(_chr));
}
ScriptData * ScriptDataChar::toChar() {
  return CopyPtr(_up);
}
ScriptData * ScriptDataChar::toInt() {
  return new ScriptData(new ScriptDataInteger(_chr));
}
ScriptData * ScriptDataChar::toFloat() {
  return new ScriptData(new ScriptDataFloat(_chr));
}
ScriptData * ScriptDataChar::toStr() {
  return new ScriptData(new ScriptDataString(toString()));
}
ScriptData * ScriptDataChar::toArray() {
  return NULL;
}
ScriptData * ScriptDataChar::toDict() {
  return NULL;
}

ScriptDataInteger::ScriptDataInteger() {
}
ScriptDataBase::VarType ScriptDataInteger::type() {
  return TINTEGER;
}
ScriptDataBase * ScriptDataInteger::CopyContent() {
  ScriptDataInteger* res = new ScriptDataInteger();
  res->_num = _num;
  return res;
}
string ScriptDataInteger::toString() {
  return to_string(_num);
}
void ScriptDataInteger::fromString(const string & s, int& from) { //Try to decode from string
  _num = 0;
  int sign = 1;
  if (s[from] == '-') {
    sign = -1;
    ++from;
  } else {
    if (s[from] == '+') {
      sign = 1;
      ++from;
    }
  }
  while ('0' <= s[from] && s[from] <= '9') {
    _num = 10*_num + s[from] - '0';
    ++from;
  }
  _num *= sign;

    int _exp = 0;
  if (s[from] == 'e') {
    ++from;
    while ('0' <= s[from] && s[from] <= '9') {
      _exp = 10 * _exp + s[from] - '0';
      ++from;
    }
  }
  for (int i = 0; i < _exp; i++) {
    _num *= 10;
  }
}
ScriptData * ScriptDataInteger::toBool() {
  return new ScriptData(new ScriptDataBool(_num));
}
ScriptData * ScriptDataInteger::toChar() {
  return new ScriptData(new ScriptDataChar(_num));
}
ScriptData * ScriptDataInteger::toInt() {
  return CopyPtr(_up);
}
ScriptData * ScriptDataInteger::toFloat() {
  return new ScriptData(new ScriptDataFloat(_num));
}
ScriptData * ScriptDataInteger::toStr() {
  return new ScriptData(new ScriptDataString(toString()));
}
ScriptData * ScriptDataInteger::toArray() {
  return NULL;
}
ScriptData * ScriptDataInteger::toDict() {
  return NULL;
}

ScriptDataFloat::ScriptDataFloat() {
}
ScriptDataBase::VarType ScriptDataFloat::type() {
  return TFLOAT;
}
ScriptDataBase * ScriptDataFloat::CopyContent() {
  ScriptDataFloat* res = new ScriptDataFloat();
  res->_num = _num;
  return res;
}
string ScriptDataFloat::toString() {
  return to_string(_num);
}
void ScriptDataFloat::fromString(const string & s, int& from) { //Try to decode from string
  _num = 0;
  int sign = 1;
  double multi = 1;
  if (s[from] == '-') {
    sign = -1;
    ++from;
  } else {
    if (s[from] == '+') {
      sign = 1;
      ++from;
    }
  }
  while ('0' <= s[from] && s[from] <= '9') {
    _num = 10 * _num + s[from] - '0';
    ++from;
  }

  if (s[from] == '.') {
    ++from;
    while ('0' <= s[from] && s[from] <= '9') {
      _num = 10 * _num + s[from] - '0';
      multi *= 10;
      ++from;
    }
  }

  _num = _num / multi;

  _num *= sign;

  if (s[from] == 'e') {
    int _exp = 0;
    int expsign = 1;
    ++from;
    
    if (s[from] == '-') {
      expsign = -1;
      ++from;
    } else {
      if (s[from] == '+') {
        expsign = 1;
        ++from;
      }
    }
    while ('0' <= s[from] && s[from] <= '9') {
      _exp = 10 * _exp + s[from] - '0';
      ++from;
    }
    _exp *= expsign;

    _num *= pow(10, _exp);
  }
  
}
ScriptData * ScriptDataFloat::toBool() {
  return new ScriptData(new ScriptDataBool(_num));
}
ScriptData * ScriptDataFloat::toChar() {
  return new ScriptData(new ScriptDataChar(_num));
}
ScriptData * ScriptDataFloat::toInt() {
  return new ScriptData(new ScriptDataInteger(_num));
}
ScriptData * ScriptDataFloat::toFloat() {
  return CopyPtr(_up);
}
ScriptData * ScriptDataFloat::toStr() {
  return new ScriptData(new ScriptDataString(toString()));
}
ScriptData * ScriptDataFloat::toArray() {
  return NULL;
}
ScriptData * ScriptDataFloat::toDict() {
  return NULL;
}

ScriptDataString::ScriptDataString() {
}
ScriptDataBase::VarType ScriptDataString::type() {
  return TSTRING;
}
ScriptDataBase * ScriptDataString::CopyContent() {
  ScriptDataString* res = new ScriptDataString();
  res->_str = _str;
  return res;
}
string ScriptDataString::toString() {
  return "\"" + escape(_str) + "\"";
}
void ScriptDataString::fromString(const string & s, int& from) { //Try to decode from string
  int close = jumpAfterClosing(s, from);
  _str = unescape(s, from + 1, close);
  from = close + 1;
}
ScriptData * ScriptDataString::toBool() {
  return new ScriptData(new ScriptDataBool(_str));
}
ScriptData * ScriptDataString::toChar() {
  return new ScriptData(new ScriptDataChar(_str[0]));
}
ScriptData * ScriptDataString::toInt() {
  return new ScriptData(new ScriptDataInteger(_str));
}
ScriptData * ScriptDataString::toFloat() {
  return new ScriptData(new ScriptDataFloat(_str));
}
ScriptData * ScriptDataString::toStr() {
  return CopyPtr(_up);
}
ScriptData * ScriptDataString::toArray() {
  ScriptDataArray* a = new ScriptDataArray();
  for (auto&& it : _str) {
    a->_data.push_back(new ScriptData(new ScriptDataChar(it)));
  }
  return new ScriptData(a);
}
ScriptData * ScriptDataString::toDict() {
  ScriptDataDict* a = new ScriptDataDict();
  int ai = 0;
  for (auto&& it : _str) {
    a->_elems[to_string(ai)] = new ScriptData(new ScriptDataChar(it));
    ++ai;
  }
  return new ScriptData(a);

}

ScriptDataArray::ScriptDataArray() {
}
ScriptDataBase::VarType ScriptDataArray::type() {
  return TARRAY;
}
ScriptDataBase * ScriptDataArray::CopyContent() {
  ScriptDataArray* res = new ScriptDataArray();
  for (auto&& it : _data) {
    res->_data.push_back(new ScriptData(it->_val->CopyContent()));
  }
  return res;
}
string ScriptDataArray::toString() {
  string res = "[";
  string append = "";
  for (auto&& it : _data) {
    res += append + it->toString();
    append = ",";
  }
  res.push_back(']');
  return res;
}
void ScriptDataArray::fromString(const string & s, int& from) { //Try to decode from string
  ++from;
  while (trimChar(s[from], Trim_Space)) {
    ++from;
  }
  while(s[from] != ']') {

    ScriptData * data = new ScriptData();
    data->fromString(s, from);
    _data.push_back(data);
    while (trimChar(s[from], Trim_Space)) {
      ++from;
    }
    if (s[from] == ',') {
      ++from;
      while (trimChar(s[from], Trim_Space)) {
        ++from;
      }
    }
  }
  ++from;
}
ScriptData * ScriptDataArray::toBool() {
  return NULL;
}
ScriptData * ScriptDataArray::toChar() {
  return NULL;
}
ScriptData * ScriptDataArray::toInt() {
  return NULL;
}
ScriptData * ScriptDataArray::toFloat() {
  return NULL;
}
ScriptData * ScriptDataArray::toStr() {
  return new ScriptData(new ScriptDataString(toString()));
}
ScriptData * ScriptDataArray::toArray() {
  return CopyPtr(_up);
}
ScriptData * ScriptDataArray::toDict() {
  ScriptDataDict* a = new ScriptDataDict();
  int ai = 0;
  for (auto&& it : _data) {
    a->_elems[to_string(ai)] = CopyPtr(it);
    ++ai;
  }
  return new ScriptData(a);

}

ScriptDataDict::ScriptDataDict() {
}
ScriptDataBase::VarType ScriptDataDict::type() {
  return TDICT;
}
ScriptDataBase * ScriptDataDict::CopyContent() {
  ScriptDataDict* res = new ScriptDataDict();
  for (auto&& it : _elems) {
    res->_elems[it.first] = new ScriptData(it.second->_val->CopyContent());
  }
  return res;
}
string ScriptDataDict::toString() {
  string res = "{";
  string append = "";
  for (auto&& it : _elems) {
    res += append + "\"" + escape(it.first) + "\" : " + it.second->toString();
    append = ",";
  }
  res.push_back('}');
  return res;
}
void ScriptDataDict::fromString(const string & s, int& from) { //Try to decode from string
  ++from;
  while (trimChar(s[from], Trim_Space)) {
    ++from;
  }
  while (s[from] != '}') {
    int close = jumpAfterClosing(s, from);
    string key = unescape(s, from + 1, close);
    from = close + 1;

    while (trimChar(s[from], Trim_Space)) {
      ++from;
    }

    ++from;

    while (trimChar(s[from], Trim_Space)) {
      ++from;
    }

    ScriptData * data = new ScriptData();
    data->fromString(s, from);
    _elems.insert({key,data});
    while (trimChar(s[from], Trim_Space)) {
      ++from;
    }
    if (s[from] == ',') {
      ++from;
      while (trimChar(s[from], Trim_Space)) {
        ++from;
      }
    }
  }
  ++from;
}
ScriptData * ScriptDataDict::toBool() {
  return NULL;
}
ScriptData * ScriptDataDict::toChar() {
  return NULL;
}
ScriptData * ScriptDataDict::toInt() {
  return NULL;
}
ScriptData * ScriptDataDict::toFloat() {
  return NULL;
}
ScriptData * ScriptDataDict::toStr() {
  return new ScriptData(new ScriptDataString(toString()));
}
ScriptData * ScriptDataDict::toArray() {
  ScriptDataArray* a = new ScriptDataArray();
  int ai = 0;
  for (auto&& it : _elems) {
    a->_data.push_back(CopyPtr(it.second));
    ++ai;
  }
  return new ScriptData(a);
}
ScriptData * ScriptDataDict::toDict() {
  return CopyPtr(_up);
}

map<string, APICall> apiMap;

ScriptData* ScriptInstruction::run(ScriptData& _args) {
  throw 1;
  return NULL;
}
void ScriptInstruction::load(xml_node<> *data) {
  throw 1;
}
void ScriptInstruction::save(xml_node<> *data, xml_document<> &doc) {
  throw 1;
}

ScriptData* ScriptIIfElse::run(ScriptData& _args) {
  ScriptData* condition = _condition->run(_args);
  ScriptData* conditionb = condition->toBool();
  DeletePtr(condition);
  if (conditionb->asBool()->_bl) {
    DeletePtr(conditionb);
    return _then->run(_args);
  }
  else {
    DeletePtr(conditionb);
    return _else->run(_args);
  }
  return NULL;
}
void ScriptIIfElse::load(xml_node<> *data) {
  xml_node<> *pElem = data->first_node("cond");
  ScriptIBlock* condition = new ScriptIBlock;
  if (pElem) {
    condition->load(pElem);
  }
  else {
    ScriptIConstant* _false = new ScriptIConstant();
    _false->_val = "false";//new ScriptDataBool(false);
    condition->_instructions.push_back(_false);
  }

  if (_condition != NULL) {
    delete _condition;
  }
  _condition = condition;

  pElem = data->first_node("then");
  ScriptIBlock* then = new ScriptIBlock();
  if (pElem) {
    then->load(pElem);
  }
  
  if (_then != NULL) {
    delete _then;
  }
  _then = then;

  pElem = data->first_node("else");
  ScriptIBlock* elsen = new ScriptIBlock();
  if (pElem) {
    elsen->load(pElem);
  }

  if (_else != NULL) {
    delete _else;
  }
  _else = elsen;
}
void ScriptIIfElse::save(xml_node<> *data, xml_document<> &doc) {
  xml_node<>*  me = doc.allocate_node(node_element, "ifelse");

  xml_node<>*  cond = doc.allocate_node(node_element, "cond");
  _condition->save(cond, doc);
  me->append_node(cond);
  xml_node<>*  then = doc.allocate_node(node_element, "then");
  _then->save(cond, doc);
  me->append_node(then);
  xml_node<>*  elss = doc.allocate_node(node_element, "else");
  _else->save(cond, doc);
  me->append_node(elss);

  data->append_node(me);
}
ScriptIIfElse::~ScriptIIfElse() {
  delete _then;
  delete _else;
  delete _condition;
}

ScriptData* ScriptILoop::run(ScriptData& _args) {
  ScriptData* condition = _condition->run(_args);
  ScriptData* conditionb = condition->toBool();
  DeletePtr(condition);
  while (conditionb->asBool()->_bl) {
    ScriptData* insres = _code->run(_args);
    if (insres != NULL) {
      DeletePtr(conditionb);
      return insres;
    }
    DeletePtr(conditionb);
    condition = _condition->run(_args);
    conditionb = condition->toBool();
    DeletePtr(condition);
  }
  DeletePtr(conditionb);
  return NULL;
}
void ScriptILoop::load(xml_node<> *data) {
  xml_node<> *pElem = data->first_node("cond");
  ScriptIBlock* condition = new ScriptIBlock;
  if (pElem) {
    condition->load(pElem);
  }
  else {
    ScriptIConstant* _false = new ScriptIConstant();
    _false->_val = "false";//new ScriptDataBool(false);
    condition->_instructions.push_back(_false);
  }

  if (_condition != NULL) {
    delete _condition;
  }
  _condition = condition;

  pElem = data->first_node("then");
  ScriptIBlock* then = new ScriptIBlock();
  if (pElem) {
    then->load(pElem);
  }

  if (_code != NULL) {
    delete _code;
  }
  _code = then;
}
void ScriptILoop::save(xml_node<> *data, xml_document<> &doc) {
  xml_node<>*  me = doc.allocate_node(node_element, "loop");

  xml_node<>*  cond = doc.allocate_node(node_element, "cond");
  _condition->save(cond, doc);
  me->append_node(cond);
  xml_node<>*  then = doc.allocate_node(node_element, "then");
  _code->save(then, doc);
  me->append_node(then);

  data->append_node(me);
}
ScriptILoop::~ScriptILoop() {
  delete _code;
  delete _condition;
}

ScriptData* ScriptIAssign::run(ScriptData& _args) {
  ScriptData* val = _val->run(_args);
  ScriptData* to = _to->run(_args);
  if (to->_val) {
    delete to->_val;
  }
  to->_val = val->_val->CopyContent();
  to->_val->_up = to;
  DeletePtr(to);
  DeletePtr(val);

  return NULL;
}
void ScriptIAssign::load(xml_node<> *data) {
  xml_node<>* pElem = data->first_node("to");
  ScriptIBlock* to = new ScriptIBlock();
  if (pElem) {
    to->load(pElem);
  }

  if (_to != NULL) {
    delete _to;
  }
  _to = to;

  pElem = data->first_node("val");
  ScriptIBlock* val = new ScriptIBlock();
  if (pElem) {
    val->load(pElem);
  }

  if (_val != NULL) {
    delete _val;
  }
  _val = val;
}
void ScriptIAssign::save(xml_node<> *data, xml_document<> &doc) {
  xml_node<>*  me = doc.allocate_node(node_element, "assign");

  xml_node<>*  to = doc.allocate_node(node_element, "to");
  _to->save(to, doc);
  me->append_node(to);
  xml_node<>*  val = doc.allocate_node(node_element, "val");
  _val->save(val, doc);
  me->append_node(val);

  data->append_node(me);
}
ScriptIAssign::~ScriptIAssign() {
  delete _val;
  delete _to;
}

/*ScriptData* ScriptICopy::run(ScriptData& _args) {
  if (_args._elems.count(_to)) {
    DeletePtr(_args._elems[_to]);
  }
  _args._elems[_to]->CopyContent(_args._elems[_from]);

  return NULL;
}
void ScriptICopy::load(xml_node<> *data) {
  _to = data->first_attribute("to")->value();
  _from = data->first_attribute("from")->value();
}
ScriptICopy::~ScriptICopy() {
}*/

ScriptData* ScriptIConstant::run(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  int start = 0;
  res->fromString(_val, start);
  return res;
}
void ScriptIConstant::load(xml_node<> *data) {
  /*if (_val != NULL) {
    delete _val;
  }
  _val = new ScriptDataBase();
  int start = 0;*/
  /*string s = data->value();
  _val->fromString(s, start);
  #ifdef SCRIPT_GUI
  _enteredVal = s;
  #endif*/
  _val = data->value();;
}
void ScriptIConstant::save(xml_node<> *data, xml_document<> &doc) {
  xml_node<>*  me = doc.allocate_node(node_element, "math", _val.c_str());

  data->append_node(me);
}
ScriptIConstant::~ScriptIConstant() {
  //delete _val;
}

ScriptData* ScriptIMath::run(ScriptData& _args) {
  ScriptData* res1 = _arg1->run(_args);
  ScriptData* res1f = res1->toFloat();
  DeletePtr(res1);

  ScriptData* res2 = NULL;
  ScriptData* res2f = NULL;
  if (_oper < 16) {
    res2 = _arg2->run(_args);
    res2f = res2->toFloat();
    DeletePtr(res2);
  }
  ScriptData* s = NULL;
  switch (_oper) {
  case PLUS:
    s = new ScriptData(new ScriptDataFloat(res1f->asFloat()->_num + res2f->asFloat()->_num));
    break;
  case MINUS:
    s = new ScriptData(new ScriptDataFloat(res1f->asFloat()->_num - res2f->asFloat()->_num));
    break;
  case MULTI:
    s = new ScriptData(new ScriptDataFloat(res1f->asFloat()->_num * res2f->asFloat()->_num));
    break;
  case DIV:
    s = new ScriptData(new ScriptDataFloat(res1f->asFloat()->_num / res2f->asFloat()->_num));
    break;
  case POW:
    s = new ScriptData(new ScriptDataFloat(pow(res1f->asFloat()->_num , res2f->asFloat()->_num)));
    break;
  case LOGAB:
    s = new ScriptData(new ScriptDataFloat(log(res1f->asFloat()->_num) / log(res2f->asFloat()->_num)));
    break;
  case MOD:
    s = new ScriptData(new ScriptDataFloat(fmodf(res1f->asFloat()->_num, res2f->asFloat()->_num)));
    break;
  case ATAN2:
    s = new ScriptData(new ScriptDataFloat(atan2(res1f->asFloat()->_num, res2f->asFloat()->_num)));
    break;
  case L:
    s = new ScriptData(new ScriptDataBool(res1f->asFloat()->_num < res2f->asFloat()->_num));
    break;
  case LEQ:
    s = new ScriptData(new ScriptDataBool(res1f->asFloat()->_num <= res2f->asFloat()->_num));
    break;
  case EQ:
    s = new ScriptData(new ScriptDataBool(res1f->asFloat()->_num == res2f->asFloat()->_num));
    break;
  case NEQ:
    s = new ScriptData(new ScriptDataBool(res1f->asFloat()->_num != res2f->asFloat()->_num));
    break;
  case GEQ:
    s = new ScriptData(new ScriptDataBool(res1f->asFloat()->_num >= res2f->asFloat()->_num));
    break;
  case G:
    s = new ScriptData(new ScriptDataBool(res1f->asFloat()->_num > res2f->asFloat()->_num));
    break;
  case SQRT:
    s = new ScriptData(new ScriptDataFloat(sqrt(res1f->asFloat()->_num)));
    break;
  case SIN:
    s = new ScriptData(new ScriptDataFloat(sin(res1f->asFloat()->_num)));
    break;
  case COS:
    s = new ScriptData(new ScriptDataFloat(cos(res1f->asFloat()->_num)));
    break;
  case TAN:
    s = new ScriptData(new ScriptDataFloat(tan(res1f->asFloat()->_num)));
    break;
  case ASIN:
    s = new ScriptData(new ScriptDataFloat(asin(res1f->asFloat()->_num)));
    break;
  case ACOS:
    s = new ScriptData(new ScriptDataFloat(acos(res1f->asFloat()->_num)));
    break;
  case ATAN:
    s = new ScriptData(new ScriptDataFloat(atan(res1f->asFloat()->_num)));
    break;
  }
  DeletePtr(res1f);
  if(res2f != NULL) {
    DeletePtr(res2f);
  }
  return s;
}
void ScriptIMath::load(xml_node<> *data) {
  xml_attribute<> *opern = data->first_attribute("oper");
  if(opern) {
    string oper = opern->value();
    if(oper == "+") {
      _oper = PLUS;
    }
    if (oper == "-") {
      _oper = MINUS;
    }
    if (oper == "*") {
      _oper = MULTI;
    }
    if (oper == "/") {
      _oper = DIV;
    }
    if (oper == "^") {
      _oper = POW;
    }
    if (oper == "log") {
      _oper = LOGAB;
    }
    if (oper == "%") {
      _oper = MOD;
    }
    if (oper == "atan2") {
      _oper = ATAN2;
    }
    if (oper == "l") {
      _oper = L;
    }
    if (oper == "lew") {
      _oper = LEQ;
    }
    if (oper == "eq") {
      _oper = EQ;
    }
    if (oper == "neq") {
      _oper = NEQ;
    }
    if (oper == "geq") {
      _oper = GEQ;
    }
    if (oper == "g") {
      _oper = G;
    }
    if (oper == "sqrt") {
      _oper = SQRT;
    }
    if (oper == "sin") {
      _oper = SIN;
    }
    if (oper == "cos") {
      _oper = COS;
    }
    if (oper == "tan") {
      _oper = TAN;
    }
    if (oper == "asin") {
      _oper = ASIN;
    }
    if (oper == "acos") {
      _oper = ACOS;
    }
    if (oper == "atan") {
      _oper = ATAN;
    }
  }

  xml_node<>* pElem = data->first_node("lhs");
  ScriptIBlock* lhs = new ScriptIBlock();
  if (pElem) {
    lhs->load(pElem);
  }

  if (_arg1 != NULL) {
    delete _arg1;
  }
  _arg1 = lhs;

  pElem = data->first_node("rhs");
  ScriptIBlock* rhs = new ScriptIBlock();
  if (pElem) {
    rhs->load(pElem);
  }

  if (_arg2 != NULL) {
    delete _arg2;
  }
  _arg2 = rhs;
}
void ScriptIMath::save(xml_node<> *data, xml_document<> &doc) {
  xml_node<>*  me = doc.allocate_node(node_element, "math");

  xml_attribute<> *oper = doc.allocate_attribute("oper", "google.com");

  xml_node<>*  lhs = doc.allocate_node(node_element, "lhs");
  _arg1->save(lhs, doc);
  me->append_node(lhs);
  xml_node<>*  rhs = doc.allocate_node(node_element, "rhs");
  _arg2->save(rhs, doc);
  me->append_node(rhs);

  data->append_node(me);
}
ScriptIMath::~ScriptIMath() {
  delete _arg1;
  delete _arg2;
}

ScriptData* ScriptILogic::run(ScriptData& _args) {
  ScriptData* res1 = _arg1->run(_args);
  ScriptData* res1f = res1->toBool();
  DeletePtr(res1);

  ScriptData* res2 = NULL;
  ScriptData* res2f = NULL;
  if (_oper < 16) {
    res2 = _arg2->run(_args);
    res2f = res2->toBool();
    DeletePtr(res2);
  }
  ScriptData* s = NULL;
  switch (_oper) {
  case EQ:
    s = new ScriptData(new ScriptDataBool(res1f->asBool()->_bl == res2f->asBool()->_bl));
    break;
  case NEQ:
    s = new ScriptData(new ScriptDataBool(res1f->asBool()->_bl != res2f->asBool()->_bl));
    break;
  case AND:
    s = new ScriptData(new ScriptDataBool(res1f->asBool()->_bl && res2f->asBool()->_bl));
    break;
  case NAND:
    s = new ScriptData(new ScriptDataBool(!(res1f->asBool()->_bl && res2f->asBool()->_bl)));
    break;
  case OR:
    s = new ScriptData(new ScriptDataBool(res1f->asBool()->_bl || res2f->asBool()->_bl));
    break;
  case NOR:
    s = new ScriptData(new ScriptDataBool(!(res1f->asBool()->_bl || res2f->asBool()->_bl)));
    break;
  case XOR:
    s = new ScriptData(new ScriptDataBool(res1f->asBool()->_bl ^ res2f->asBool()->_bl));
    break;
  case NXOR:
    s = new ScriptData(new ScriptDataBool(!(res1f->asBool()->_bl ^ res2f->asBool()->_bl)));
    break;
  case NOT:
    s = new ScriptData(new ScriptDataBool(!res1f->asBool()->_bl));
    break;
  }
  DeletePtr(res1f);
  if (res2f != NULL) {
    DeletePtr(res2f);
  }
  return s;
}
void ScriptILogic::load(xml_node<> *data) {
  xml_attribute<> *opern = data->first_attribute("oper");
  if (opern) {
    string oper = opern->value();
    if (oper == "eq") {
      _oper = EQ;
    }
    if (oper == "neq") {
      _oper = NEQ;
    }
    if (oper == "and") {
      _oper = AND;
    }
    if (oper == "nand") {
      _oper = NAND;
    }
    if (oper == "or") {
      _oper = OR;
    }
    if (oper == "nor") {
      _oper = NOR;
    }
    if (oper == "xor") {
      _oper = XOR;
    }
    if (oper == "nxor") {
      _oper = NXOR;
    }
    if (oper == "not") {
      _oper = NOT;
    }
  }

  xml_node<>* pElem = data->first_node("lhs");
  ScriptIBlock* lhs = new ScriptIBlock();
  if (pElem) {
    lhs->load(pElem);
  }

  if (_arg1 != NULL) {
    delete _arg1;
  }
  _arg1 = lhs;

  pElem = data->first_node("rhs");
  ScriptIBlock* rhs = new ScriptIBlock();
  if (pElem) {
    rhs->load(pElem);
  }

  if (_arg2 != NULL) {
    delete _arg2;
  }
  _arg2 = rhs;
}
void ScriptILogic::save(xml_node<> *data, xml_document<> &doc) {
  xml_node<>*  me = doc.allocate_node(node_element, "logic");

  xml_node<>*  lhs = doc.allocate_node(node_element, "lhs");
  _arg1->save(lhs, doc);
  me->append_node(lhs);
  xml_node<>*  rhs = doc.allocate_node(node_element, "rhs");
  _arg2->save(rhs, doc);
  me->append_node(rhs);

  data->append_node(me);
}
ScriptILogic::~ScriptILogic() {
  delete _arg1;
  delete _arg2;
}

ScriptData* ScriptIVariable::run(ScriptData& _args) {
  ScriptData* res;
  if (_args.asDict()->_elems.count(_arg) == 0) {
    _args.asDict()->_elems[_arg] = new ScriptData();
  }
  res = CopyPtr(_args.asDict()->_elems[_arg]);
  return res;
}
void ScriptIVariable::load(xml_node<> *data) {
  _arg = data->first_attribute("var")->value();
}
void ScriptIVariable::save(xml_node<> *data, xml_document<> &doc) {
  throw 1;
}
ScriptIVariable::~ScriptIVariable() {
}

ScriptData* ScriptIIndex::run(ScriptData& _args) {
  ScriptData* ind = _ind->run(_args);
  ScriptData* arg = _arg->run(_args);
  string index = ind->toString();
  ScriptData* res = NULL;
  if (arg->_val->type() == ScriptDataBase::TARRAY) {
    ScriptData* indi = ind->toInt();
    DeletePtr(ind);
    int indexi = indi->asInt()->_num;
    DeletePtr(indi);
    if(arg->asArray()->_data.size()) {
      res = CopyPtr(arg->asArray()->_data[indexi]);
    }
  }
  if (arg->_val->type() == ScriptDataBase::TDICT) {
    ScriptData* inds = ind->toStr();
    DeletePtr(ind);
    string indexs = inds->asStr()->_str;
    DeletePtr(inds);
    res = CopyPtr(arg->asDict()->_elems[indexs]);
    if (res == NULL) {
      res = new ScriptData();
    }
  }
  DeletePtr(ind);
  DeletePtr(arg);
  return res;
}
void ScriptIIndex::load(xml_node<> *data) {
  xml_node<>* pElem = data->first_node("arg");
  ScriptIBlock* arg = new ScriptIBlock();
  if (pElem) {
    arg->load(pElem);
  }

  if (_arg != NULL) {
    delete _arg;
  }
  _arg = arg;

  pElem = data->first_node("ind");
  ScriptIBlock* ind = new ScriptIBlock();
  if (pElem) {
    ind->load(pElem);
  }

  if (_ind != NULL) {
    delete _ind;
  }
  _ind = ind;
}
void ScriptIIndex::save(xml_node<> *data, xml_document<> &doc) {
  throw 1;
}
ScriptIIndex::~ScriptIIndex() {
  delete _arg;
  delete _ind;
}

ScriptData* ScriptIFunctionCall::run(ScriptData& _args) {
  ScriptData _nargs(new ScriptDataDict());
  for (auto&& it : _arguments) {
    ScriptData* _nval;
    _nval = it.second->run(_args);
    _nargs.asDict()->_elems.insert({ it.first, _nval });
  }
  ScriptData* res = _function->run(_nargs);
  return res;
}
void ScriptIFunctionCall::load(xml_node<> *data) {
  xml_node<> *pElem = data->first_node("func");
  ScriptIBlock* function = new ScriptIBlock();
  if (pElem) {
    function->load(pElem);
  }

  if (_function != NULL) {
    delete _function;
  }
  _function = function;

  for (xml_node<> *pElem = data->first_node("arg"); pElem; pElem = pElem->next_sibling("arg")) {
    ScriptIBlock* b = new ScriptIBlock();
    b->load(pElem);
    _arguments.push_back({pElem->first_attribute("name")->value(), b});
  }
}
void ScriptIFunctionCall::save(xml_node<> *data, xml_document<> &doc) {
  throw 1;
}
ScriptIFunctionCall::~ScriptIFunctionCall() {
  delete _function;
  while (_arguments.size()) {
    delete _arguments.begin()->second;
    _arguments.erase(_arguments.begin());
  }
}

void ScriptIAPICall::load(xml_node<> *data) {
  _func = apiMap[data->first_attribute("name")->value()];

  for (xml_node<> *pElem = data->first_node("arg"); pElem; pElem = pElem->next_sibling("arg")) {
    ScriptIBlock* b = new ScriptIBlock();
    b->load(pElem);
    _arguments.push_back({ pElem->first_attribute("name")->value(), b });
  }
}
void ScriptIAPICall::save(xml_node<> *data, xml_document<> &doc) {
  throw 1;
}
ScriptData* ScriptIAPICall::run(ScriptData& _args) {
  ScriptData _nargs(new ScriptDataDict());
  for (auto&& it : _arguments) {
    ScriptData* _nval;
    _nval = it.second->run(_args);
    _nargs.asDict()->_elems.insert({ it.first, _nval });
  }
  ScriptData* res = _func(_nargs);
  return res;
}

ScriptData* ScriptIBlock::run(ScriptData& _args) {
  ScriptData* res = NULL;
  for (auto&& it : _instructions) {
    res = it->run(_args);
    if (res != NULL) {
      break;
    }
  }
  return res;
}
void ScriptIBlock::load(xml_node<> *data) {
  while (_instructions.size()) {
    delete *(_instructions.begin());
    _instructions.erase(_instructions.begin());
  }
  for (xml_node<> *pElem = data->first_node(); pElem; pElem = pElem->next_sibling()) {
    string name = pElem->name();
    if (name == "ifelse") {
      ScriptInstruction* nins = new ScriptIIfElse();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    if (name == "loop") {
      ScriptInstruction* nins = new ScriptILoop();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    if (name == "assign") {
      ScriptInstruction* nins = new ScriptIAssign();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    /*if (name == "copy") {
      ScriptInstruction* nins = new ScriptICopy();
      nins->load(pElem);
      _instructions.push_back(nins);
    }*/
    if (name == "constant") {
      ScriptInstruction* nins = new ScriptIConstant();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    if (name == "math") {
      ScriptInstruction* nins = new ScriptIMath();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    if (name == "logic") {
      ScriptInstruction* nins = new ScriptILogic();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    if (name == "var") {
      ScriptInstruction* nins = new ScriptIVariable();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    if (name == "index") {
      ScriptInstruction* nins = new ScriptIIndex();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    if (name == "func") {
      ScriptInstruction* nins = new ScriptIFunctionCall();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    if (name == "api") {
      ScriptInstruction* nins = new ScriptIAPICall();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
    if (name == "block") {
      ScriptInstruction* nins = new ScriptIBlock();
      nins->load(pElem);
      _instructions.push_back(nins);
    }
  }
}
void ScriptIBlock::save(xml_node<> *data, xml_document<> &doc) {
  throw 1;
}
ScriptIBlock::~ScriptIBlock() {
  while (_instructions.size()) {
    delete *(_instructions.begin());
    _instructions.erase(_instructions.begin());
  }
}

#ifdef SCRIPT_GUI

ScriptGUI::ScriptGUI(string name, LocationData llocation, colorargb lbgcolorodd, colorargb lactivecolor, colorargb ltextcolor, colorargb lbgcoloreven) : GUIElement(name, llocation, lbgcolorodd, lactivecolor, ltextcolor) {
  bgcolor_even = lbgcoloreven;
  bgcolor_odd = lbgcolorodd;
}

void ScriptGUI::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
  cax = offsetX + location.getLeft(winWidth);
  cay = offsetY + location.getBot(winHeight);
  cbx = offsetX + location.getRight(winWidth);
  cby = offsetY + location.getTop(winHeight);

  getRect();
}
void ScriptGUI::getRect() {
  code->getRect(cax - cox, cby - coy);
}
int ScriptGUI::mouseEnter(int state) {
  return code->mouseEnter(state);
}
int ScriptGUI::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  dragPos = {mx, my};

  if (mid) {
    cox -= mx - ox;
    coy -= my - oy;
    if (code != NULL) {
      code->getRect(cax - cox, cby - coy);
      return 1;
    }
  }

  return code->mouseMoved(mx, my, ox, oy, down) | (dragging != NULL);
}
int ScriptGUI::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  if (evt._key._type == evt._key.type_mouse && evt._key._keycode == GLUT_RIGHT_BUTTON) {
    if (evt._type == evt.evt_down) {
      mid = true;
      return 2;
    }
    if (evt._type == evt.evt_up) {
      mid = false;
      return 2;
    }
  }
  
  if (cax <= mx && mx < cax + 100 && editor && dragging == NULL && evt._type == evt.evt_down && evt._key._type == evt._key.type_mouse && evt._key._keycode == 0) { //LDown
    switch ((my - cay) / 20) {
      case 0:
      {
        ScriptIIfElse        * scriptIIfElse = new ScriptIIfElse();
        scriptIIfElse->_condition = new ScriptIBlock();
        scriptIIfElse->_then = new ScriptIBlock();
        scriptIIfElse->_else = new ScriptIBlock();
        dragging = scriptIIfElse;
      }
      break;
      case 1:
      {
        ScriptILoop          * scriptILoop = new ScriptILoop();
        scriptILoop->_condition = new ScriptIBlock();
        scriptILoop->_code = new ScriptIBlock();
        dragging = scriptILoop;
      }
      break;
      case 2:
      {
        ScriptIAssign        * scriptIAssign = new ScriptIAssign();
        scriptIAssign->_to = new ScriptIBlock();
        scriptIAssign->_val = new ScriptIBlock();
        dragging = scriptIAssign;
      }
      break;
      case 3:
      {
        ScriptIConstant      * scriptIConstant = new ScriptIConstant();
        dragging = scriptIConstant;
      }
      break;
      case 4:
      {
        ScriptIMath          * scriptIMath = new ScriptIMath();
        scriptIMath->_arg1 = new ScriptIBlock();
        scriptIMath->_arg2 = new ScriptIBlock();
        scriptIMath->_oper = ScriptIMath::PLUS;
        dragging = scriptIMath;
      }
      break;
      case 5:
      {
        ScriptILogic         * scriptILogic = new ScriptILogic();
        scriptILogic->_arg1 = new ScriptIBlock();
        scriptILogic->_arg2 = new ScriptIBlock();
        scriptILogic->_oper = ScriptILogic::NOT;
        dragging = scriptILogic;
        break;
      }
      case 6:
      {
        ScriptIVariable      * scriptIVariable = new ScriptIVariable();
        dragging = scriptIVariable;
      }
      break;
      case 7:
      {
        ScriptIIndex         * scriptIIndex = new ScriptIIndex();
        scriptIIndex->_arg = new ScriptIBlock();
        scriptIIndex->_ind = new ScriptIBlock();
        dragging = scriptIIndex;
      }
      break;/*
      case 8:
      ScriptIFunctionCall  * scriptIFunctionCall = new ScriptIFunctionCall();
      dragging = scriptIFunctionCall;
      break;
      case 9:
      ScriptIAPICall       * scriptIAPICall = new ScriptIAPICall();
      dragging = scriptIAPICall;
      break;
      case 10:
      ScriptIBlock         * scriptIBlock = new ScriptIBlock();
      dragging = scriptIBlock;
      break;*/
    }
    if (dragging) {
      dragging->getRect(mx, my);
      dragOffset = {0, dragging->cby - dragging->cay};
      return 3;
    }
  }
  if (cax <= mx && mx < cax + 100 && editor && dragging != NULL && evt._type == evt.evt_up && evt._key._type == evt._key.type_mouse && evt._key._keycode == 0) { //LDown
    if ((my - cay) / 20 == 8) {
      delete dragging;
      dragging = false;
      return 3;
    }
  }
  if (cax <= mx && mx < cax + 100 && editor && dragging == NULL && evt._type == evt.evt_down && evt._key._type == evt._key.type_mouse && evt._key._keycode == 0) { //LDown
    if ((my - cay) / 20 == 9) {
      ScriptData* res = code->run(ScriptData(new ScriptDataDict()));
      if(res != NULL) {
        cout << res->toString() << endl;
        DeletePtr(res);
      }
      return 3;
    }
  }
  return code->guiEvent(this, evt, mx, my, down);
}
void ScriptGUI::render(set<key_location>& down) {
  glScissor(cax, cay, cbx - cax, cby - cay);
  glEnable(GL_SCISSOR_TEST);
  code->render(this, 0);
  glDisable(GL_SCISSOR_TEST);
  Graphics::resetViewport();

  if (dragging) {
    dragging->getRect(dragPos.x + dragOffset.x, dragPos.y + dragOffset.y);
    dragging->render(this, 0);
  }
  if(editor) {
    for(int i = 0; i <= 7; i++) {
      setColor((i%2) ? 0xff006060: 0xff004040);
      glBegin(GL_QUADS);
      glVertex2d(cax + 0  ,cay+ i*20);
      glVertex2d(cax + 100,cay+ i*20);
      glVertex2d(cax + 100,cay+ i*20+20);
      glVertex2d(cax + 0  ,cay+ i*20+20);
      glEnd();
    }
    renderBitmapString(cax + 10, cay +  10 -5, "If", 0xffffffff, false);
    renderBitmapString(cax + 10, cay +  30 -5, "Loop", 0xffffffff, false);
    renderBitmapString(cax + 10, cay +  50 -5, "Assign", 0xffffffff, false);
    renderBitmapString(cax + 10, cay +  70 -5, "Constant", 0xffffffff, false);
    renderBitmapString(cax + 10, cay +  90 -5, "Math", 0xffffffff, false);
    renderBitmapString(cax + 10, cay + 110 -5, "Logic", 0xffffffff, false);
    renderBitmapString(cax + 10, cay + 130 -5, "Var", 0xffffffff, false);
    renderBitmapString(cax + 10, cay + 150 -5, "Index", 0xffffffff, false);
    setColor(0xffff0000);
    glBegin(GL_QUADS);
    glVertex2d(cax + 0  , cay+160);
    glVertex2d(cax + 100, cay+160);
    glVertex2d(cax + 100, cay+180);
    glVertex2d(cax + 0  , cay+  180);
    glEnd();
    renderBitmapString(cax + 10, cay + 165, "Delete", 0xffffffff, false);
    setColor(0xff00ff00);
    glBegin(GL_QUADS);
    glVertex2d(cax + 0, cay + 180);
    glVertex2d(cax + 100, cay + 180);
    glVertex2d(cax + 100, cay + 200);
    glVertex2d(cax + 0, cay + 200);
    glEnd();
    renderBitmapString(cax + 10, cay + 185, "RUN", 0xffffffff, false);
  }
}

bool ScriptGUIBase::isIn(int mx, int my) {
  return (cax <= mx && mx <= cbx) && (cay <= my && my <= cby);
}
void ScriptGUIBase::getRect(int lcax, int lcby) {
}
int ScriptGUIBase::mouseEnter(int state) {
  return 0;
}
int ScriptGUIBase::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptGUIBase::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
void ScriptGUIBase::render(ScriptGUI* base, int depth) {
}

void ScriptInstruction  ::getRect(int offsetX, int offsetY) {

}
void ScriptIIfElse      ::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cby = offsetY;

  cbx = cax + 5;
  cay = cby - 20;

  if (_condition != NULL) {
    _condition->getRect(cax + 5, cay);
    cay = _condition->cay - 5;
    cbx = max(cbx, _condition->cbx + 5);
  }
    cty = cay;
    cay = cay - 15;
  if (_then != NULL) {
    _then->getRect(cax + 5, cay);
    cay = _then->cay - 5;
    cbx = max(cbx, _then->cbx + 5);
  }
    cey = cay;
    cay = cay - 15;
  if (_else != NULL) {
    _else->getRect(cax + 5, cay);
    cay = _else->cay - 5;
    cbx = max(cbx, _else->cbx + 5);
  }
}
void ScriptILoop        ::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cby = offsetY;

  cbx = cax + 5;
  cay = cby - 20;

    if (_condition != NULL) {
      _condition->getRect(cax + 5, cay);
      cay = _condition->cay - 5;
      cbx = max(cbx, _condition->cbx + 5);
  }
      cdy = cay;
      cay = cay - 15;
  if (_code != NULL) {
    _code->getRect(cax + 5, cay);
    cay = _code->cay - 5;
    cbx = max(cbx, _code->cbx + 5);
  }
}
void ScriptIAssign      ::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cby = offsetY;

  cbx = cax + 5;
  cay = cby - 5;

  if (_to != NULL) {
    _to->getRect(cbx, cby - 5);
    cbx = _to->cbx + 5;
    cbx = cbx + 15;
    cex = cbx;
    cay = min(cay, _to->cay - 5);
  }
  if (_val != NULL) {
    _val->getRect(cbx, cby - 5);
    cbx = _val->cbx + 5;
    cay = min(cay, _val->cay - 5);
  }
}
/*void ScriptICopy::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cay = offsetY;
  if (_from != NULL) {
    _from->getRect(cax + 5, cay + 5);
    cax = _from->cbx + 5;
    cby = max(cby, _from->cby + 5);
  }
  if (_to != NULL) {
    _to->getRect(cax + 5, cay + 5);
    cax = _to->cbx + 5;
    cby = max(cby, _to->cby + 5);
  }
}*/
void ScriptIConstant    ::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cby = offsetY;
  cay = cby - 15;
  cbx = cax + 9 + 9 * _val.length();
}
void ScriptIMath        ::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cby = offsetY;

  cbx = cax + 5;
  cay = cby - 5;

  if (_arg1 != NULL) {
    _arg1->getRect(cbx, cby - 5);
    cbx = _arg1->cbx + 5;
    cbx = cbx + 15;
    cox = cbx;
    cay = min(cay, _arg1->cay - 5);
  }
  if (_arg2 != NULL) {
    _arg2->getRect(cbx, cby - 5);
    cbx = _arg2->cbx + 5;
    cay = min(cay, _arg2->cay - 5);
  }
}
void ScriptILogic       ::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cby = offsetY;

  cbx = cax + 5;
  cay = cby - 5;

  if(_oper >= 16) {
    cbx = cbx + 10;
  }
  if (_arg1 != NULL) {
    _arg1->getRect(cbx, cby - 5);
    cbx = _arg1->cbx + 5;
    cay = min(cay, _arg1->cay - 5);
  }
  if(_oper < 16) {
    cbx = cbx + 15;
    cox = cbx;
    if (_arg2 != NULL) {
      _arg2->getRect(cbx, cby - 5);
      cbx = _arg2->cbx + 5;
      cay = min(cay, _arg2->cay - 5);
    }
  }
}
void ScriptIVariable    ::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cby = offsetY;
  cay = cby - 15;
  cbx = cax + 9 + 9 * _arg.length();
}
void ScriptIIndex       ::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cby = offsetY;

  cbx = cax + 5;
  cay = cby - 5;

  if (_arg != NULL) {
    _arg->getRect(cbx, cby - 5);
    cbx = _arg->cbx + 5;
    cix = cbx;
    cbx = cbx + 10;
    cay = min(cay, _arg->cay - 5);
  }
  if (_ind != NULL) {
    _ind->getRect(cbx, cby - 5);
    cbx = _ind->cbx + 5;
    cay = min(cay, _ind->cay - 5);
  }
  cbx += 10;
}
void ScriptIFunctionCall::getRect(int offsetX, int offsetY) {
  //getRect();
}
void ScriptIAPICall     ::getRect(int offsetX, int offsetY) {
  //getRect();
}
void ScriptIBlock       ::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cby = offsetY;

  cbx = cax + 15;
  cay = cby - 5;

  auto it = _instructions.begin();

  while(it != _instructions.end()) {
    (*it)->getRect(cax + 5, cay);
    cay = (*it)->cay - 5;
    cbx = max(cbx, (*it)->cbx + 5);
    ++it;
  }
  cay = min(cay, cby - 15);
}

void ScriptGUIBase    ::renderBg(ScriptGUI* base, int depth) {
  if (depth % 2) {
    setColor(base->bgcolor_odd);
  }
  else {
    setColor(base->bgcolor_even);
  }
  glBegin(GL_QUADS);
  glVertex2d(cax, cay);
  glVertex2d(cbx, cay);
  glVertex2d(cbx, cby);
  glVertex2d(cax, cby);
  glEnd();
}
void ScriptInstruction  ::render(ScriptGUI* base, int depth) {
  throw 1;
}
void ScriptIIfElse      ::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  renderBitmapString(cax + 5, cby - 15 , "IF", base->textColor, false);
  _condition->render(base, depth + 1);
  renderBitmapString(cax + 5, cty - 10, "THEN", base->textColor, false);
  _then->render(base, depth + 1);
  renderBitmapString(cax + 5, cey - 10, "ELSE", base->textColor, false);
  _else->render(base, depth + 1);
}
void ScriptILoop        ::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  renderBitmapString(cax + 5, cby - 15, "WHILE", base->textColor, false);
  _condition->render(base, depth + 1);
  renderBitmapString(cax + 5, cdy - 10, "DO", base->textColor, false);
  _code->render(base, depth + 1);
}
void ScriptIAssign      ::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  _val->render(base, depth + 1);
  renderBitmapString(cex - 10, (cay+cby)/2, "=", base->textColor, true);
  _to->render(base, depth + 1);
}
void ScriptIConstant    ::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, _val, base->textColor, true);
}
void ScriptIMath        ::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  string operb = "";
  string operm = "";
  switch (_oper) {
    case Operation::PLUS:
      operm = '+';
      break;
    case Operation::MINUS:
      operm = '-';
      break;
    case Operation::MULTI:
      operm = '*';
      break;
    case Operation::DIV:
      operm = '/';
      break;
    case Operation::POW:
      operm = '^';
      break;
    case Operation::LOGAB:
      operm = '_';
      operb = "LG";
      break;
    case Operation::MOD:
      operm = '%';
      break;
    case Operation::ATAN2:
      operb = "ATAN2";
      operm = ",";
      break;
    case Operation::L:
      operm = '<';
      break;
    case Operation::LEQ:
      operm = "<=";
      break;
    case Operation::EQ:
      operm = "==";
      break;
    case Operation::NEQ:
      operm = "!=";
      break;
    case Operation::GEQ:
      operm = ">=";
      break;
    case Operation::G:
      operm = '>';
      break;
    case Operation::SQRT:
      operb = "Rt";
      break;
    case Operation::SIN:
      operb = 'S';
      break;
    case Operation::COS:
      operb = 'C';
      break;
    case Operation::TAN:
      operb = 'T';
      break;
    case Operation::ASIN:
      operb = "AS";
      break;
    case Operation::ACOS:
      operb = "AC";
      break;
    case Operation::ATAN:
      operb = "AT";
      break;
  }

  renderBitmapString(cax, (cay + cby) / 2, operb, base->textColor, true);
  _arg1->render(base, depth + 1);
  if(_arg2 != NULL && _oper < 16) {
    renderBitmapString(cox - 10, (cay + cby) / 2, operm, base->textColor, true);
    _arg2->render(base, depth + 1);
  }
}
void ScriptILogic       ::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  string operb = "";
  string operm = "";
  switch (_oper) {
    case Operation::AND:
      operm = "&";
      break;
    case Operation::NAND:
      operm = "!&";
      break;
    case Operation::OR:
      operm = "|";
      break;
    case Operation::NOR:
      operm = "!|";
      break;
    case Operation::XOR:
      operm = "^";
      break;
    case Operation::NXOR:
      operm = "!^";
      break;
    case Operation::NOT:
      operb = "!";
      break;
  }

  renderBitmapString(cax + 5, (cay + cby) / 2, operb, base->textColor, true);
  _arg1->render(base, depth + 1);
  if (_arg2 != NULL && _oper < 16) {
    renderBitmapString(cox - 10, (cay + cby) / 2, operm, base->textColor, true);
    _arg2->render(base, depth + 1);
  }
}
void ScriptIVariable    ::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, _arg, base->textColor, true);

}
void ScriptIIndex       ::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  _arg->render(base, depth + 1);
  _ind->render(base, depth + 1);

  glLineWidth(1);
  setColor(0xffffffff);

  glBegin(GL_LINE_STRIP);
  glVertex2d(cix + 5, cby - 5);
  glVertex2d(cix, cby - 5);
  glVertex2d(cix, cay + 5);
  glVertex2d(cix + 5, cay + 5);
  glEnd();
  glBegin(GL_LINE_STRIP);
  glVertex2d(cbx - 10, cby - 5);
  glVertex2d(cbx - 5, cby - 5);
  glVertex2d(cbx - 5, cay + 5);
  glVertex2d(cbx - 10, cay + 5);
  glEnd();
}
void ScriptIFunctionCall::render(ScriptGUI* base, int depth) {

}
void ScriptIAPICall     ::render(ScriptGUI* base, int depth) {

}
void ScriptIBlock       ::render(ScriptGUI* base, int depth) {
  if (base->dragging) {
    setColor(base->activeColor);
 
    glBegin(GL_QUADS);
    glVertex2d(cax, cay);
    glVertex2d(cbx, cay);
    glVertex2d(cbx, cby);
    glVertex2d(cax, cby);
    glEnd();
  } else {
    renderBg(base, depth);
  }
  for (auto&& it : _instructions) {
    it->render(base, depth + 1);
  }
}

int ScriptInstruction::mouseEnter(int state) {
  return 0;
}
int ScriptIIfElse::mouseEnter(int state) {
  return 0;
}
int ScriptILoop::mouseEnter(int state) {
  return 0;
}
int ScriptIAssign::mouseEnter(int state) {
  return 0;
}
int ScriptIConstant::mouseEnter(int state) {
  return 0;
}
int ScriptIMath::mouseEnter(int state) {
  return 0;
}
int ScriptILogic::mouseEnter(int state) {
  return 0;
}
int ScriptIVariable::mouseEnter(int state) {
  return 0;
}
int ScriptIIndex::mouseEnter(int state) {
  return 0;
}
int ScriptIFunctionCall::mouseEnter(int state) {
  return 0;
}
int ScriptIAPICall::mouseEnter(int state) {
  return 0;
}
int ScriptIBlock::mouseEnter(int state) {
  return 0;
}

int ScriptInstruction::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptIIfElse::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptILoop::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptIAssign::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptIConstant::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptIMath::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptILogic::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptIVariable::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptIIndex::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptIFunctionCall::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptIAPICall::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptIBlock::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}

int ScriptInstruction  ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  return 0;
}
int ScriptIIfElse      ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  int res = 0;
  if (_condition) {
    res |= _condition->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  if (_then) {
    res |= _then->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  if (_else) {
    res |= _else->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  return res;
}
int ScriptILoop        ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  int res = 0;
  if (_condition) {
    res |= _condition->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  if (_code) {
    res |= _code->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  return res;
}
int ScriptIAssign      ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  int res = 0;
  if (_to) {
    res |= _to->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  if (_val) {
    res |= _val->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  return res;
}
int ScriptIConstant    ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  if (evt._type == evt.evt_pressed && evt._key._type == evt._key.type_key) {
    if (evt._key._keycode == 8 && _val.size()) {
      _val.pop_back();
      base->getRect();
      return 3;
    } else {
      if (textValidator(_val, _val.size(), evt._key._keycode)) {
        if (down.find(key_location(112, key::type_special, 0, 0)) != down.end()) { //LShift
          evt._key._keycode = toupper(evt._key._keycode);
        }
        _val.push_back(evt._key._keycode);
        base->getRect();
        return 3;
      }
    }
  }
}
int ScriptIMath        ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  int res = 0;
  if (_arg1) {
    res |= _arg1->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  if (_arg2) {
    res |= _arg2->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  if (evt._type == evt.evt_pressed && evt._key._type == evt._key.type_key) {
    if(evt._key._keycode == '+') {
      ++_oper;
      if (_oper == 15) {
        _oper = 16;
      }
      if (_oper == 23) {
        _oper = 1;
      }
      return 3;
    }
    if (evt._key._keycode == '-') {
      --_oper;
      if (_oper == 15) {
        _oper = 14;
      }
      if (_oper == 0) {
        _oper = 22;
      }
      return 3;
    }
  }
  return res;
}
int ScriptILogic       ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  int res = 0;
  if (_arg1) {
    res |= _arg1->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  if (_arg2) {
    res |= _arg2->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  return res;
}
int ScriptIVariable    ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  if (evt._type == evt.evt_pressed && evt._key._type == evt._key.type_key) {
    if (evt._key._keycode == 8 && _arg.size()) {
      _arg.pop_back();
      base->getRect();
      return 3;
    } else {
      if (textValidator(_arg, _arg.size(), evt._key._keycode)) {
        if(down.find(key_location(112, key::type_special, 0, 0)) != down.end()) { //LShift
          evt._key._keycode = toupper(evt._key._keycode);
        }
        _arg.push_back(evt._key._keycode);
        base->getRect();
        return 3;
      }
    }
  }
  return 0;
}
int ScriptIIndex       ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  int res = 0;
  if (_arg) {
    res |= _arg->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  if (_ind) {
    res |= _ind->guiEvent(base, evt, mx, my, down);
    if (res & 2) {
      return res;
    }
  }
  return res;
}
int ScriptIFunctionCall::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  return 0;
}
int ScriptIAPICall     ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    return 0;
  }
  return 0;
}
int ScriptIBlock       ::guiEvent(ScriptGUI* base, gui_event evt, int mx, int my, set<key_location>& down) {
  if (!isIn(mx, my)) {
    insertingIn = false;
    return 0;
  }
  insertingIn = true;
  int res = 0;
  auto it = _instructions.begin();
  while(it != _instructions.end()) {
    if((*it)->isIn(mx, my)) {
      insertingIn = false;
      res |=(*it)->guiEvent(base, evt, mx, my, down);
      if (res & 2) {
        return res;
      }
      ++it;
      if (evt._key._type == evt._key.type_mouse && evt._type == evt.evt_down && evt._key._keycode == 0 && base->editor && base->dragging == NULL) { //LDOWN
        auto it2 = it;
        --it2;
        base->dragging = *it2;
        base->dragOffset = { 0, base->dragging->cby - base->dragging->cay };
        _instructions.erase(it2);
        base->getRect();
        return 3;
      }
    } else {
      ++it;
    }
  }

  if (evt._key._type == evt._key.type_mouse && evt._type == evt.evt_up && evt._key._keycode == 0 && base->editor && base->dragging != NULL) { //LDOWN
    auto it = _instructions.begin();
    while(it != _instructions.end() && my < (*it)->cby) {
      ++it;
    }
    _instructions.insert(it, base->dragging);
    base->dragging = NULL;
    base->getRect();
    return 3;
  }

  return res;
}
#endif
