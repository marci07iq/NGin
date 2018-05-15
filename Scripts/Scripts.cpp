#include "Scripts.h"

/*//In:
//Root: NUMERIC number to convert
//Out:
//Root: STRING result
ScriptData* ScriptApiFunctions::num_to_str(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->CopyContent(&_args);
  res->type = ScriptData::TSTRING;
  return res;
}

//In:
//Root: STRING number to convert
//Out:
//Root: NUMERIC result
ScriptData* ScriptApiFunctions::str_to_num(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->CopyContent(&_args);
  res->type = ScriptData::TNUMERIC;
  return res;
}

//In:
//Root: Array of STRING, concat order lexographic (std::map)
//Out:
//Root: STRING result
ScriptData* ScriptApiFunctions::str_concat(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->type = ScriptData::TSTRING;
  for (auto&& it : _args._elems) {
    res->_data.fromType<string>(res->_data.toType<string>() + it.second->_data.toType<string>());
  }
  return res;
}

//In:
//Root: STRING
//Out:
//Root: NUMERIC result, string length
ScriptData* ScriptApiFunctions::str_len(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->type = ScriptData::TNUMERIC;
  res->_data.fromType<int>(_args._data.toType<string>().length());
  return res;
}

//In:
//str STRING
//i NUMERIC
//Out:
//str STRING
ScriptData* ScriptApiFunctions::str_index(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->type = ScriptData::TNUMERIC;
  res->_data.fromType<char>(_args._elems["str"]->_data.toType<string>()[_args._elems["i"]->_data.toType<int>()]);
  return res;
}

//In:
//a NUMERIC
//b NUMERIC
//Out:
//r NUMERIC
ScriptData* ScriptApiFunctions::num_rand(ScriptData& _args) {
  ScriptData* res = new ScriptData();
  res->type = ScriptData::TNUMERIC;
  double a = _args._elems["a"]->_data.toType<double>();
  double b = _args._elems["b"]->_data.toType<double>();
  res->_data.fromType<double>(a + (b - a)*ran1());
  return res;
}*/

ScriptData::ScriptData() {
  _instances = 1;
}
/*ScriptData* ScriptData::copy() {
  ScriptData* nval = new ScriptData();
  nval->_instances = 1;
  nval->type = type;
  nval->_data._len = _data._len;
  nval->_data._data = new unsigned char[_data._len];
  for (int i = 0; i < _data._len; i++) {
    nval->_data._data[i] = _data._data[i];
  }
  for (auto&& it : _elems) {
    nval->_elems.insert({ it.first, it.second->copy()});
  }
  return nval;
}*/
void ScriptData::CopyContent(ScriptData* _from) {
  if (_data != NULL) {
    delete _data;
    _data = NULL;
  }
  switch (_from->_data->type())
  {
  case ScriptDataBase::TNULL:
    _data = NULL;
    break;
  case ScriptDataBase::TNUMERIC:
    _data = new ScriptDataNumber(((ScriptDataNumber*)_from->_data)->_num);
    break;
  case ScriptDataBase::TSTRING:
    _data = new ScriptDataString(((ScriptDataString*)_from->_data)->_str);
    break;
  case ScriptDataBase::TCHAR:
    _data = new ScriptDataChar(((ScriptDataChar*)_from->_data)->_chr);
    break;
  case ScriptDataBase::TBOOLEAN:
    _data = new ScriptDataBool(((ScriptDataBool*)_from->_data)->_bl);
    break;
  case ScriptDataBase::TARRAY:
    _data = NULL;
    break;
  case ScriptDataBase::TVECTOR:
    _data = new ScriptDataVector(((ScriptDataVector*)_from->_data)->_vec);
    break;
  default:
    throw 1;
    _data = NULL;
    break;
  }
  while (_elems.size()) {
    DeletePtr(_elems.begin()->second);
    _elems.erase(_elems.begin());
  }
  for (auto&& it : _from->_elems) {
    ScriptData* nval = new ScriptData();
    nval->CopyContent(it.second);
    _elems.insert({ it.first, nval });
  }
}
ScriptData::~ScriptData() {
  delete _data;
  if (_instances != 0) {
    LOG LERROR SCRIPTS "Script data instances " << _instances << " when deleted" END;
  }
  while (_elems.size()) {
    DeletePtr(_elems.begin()->second);
    _elems.erase(_elems.begin());
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
      return true;
    }
  }
  return false;
}
map<string, APICall> apiMap;


ScriptData* ScriptInstruction::run(ScriptData& _args) {
  throw 1;
  return NULL;
}
void ScriptInstruction::load(xml_node<> *data) {
  throw 1;
}

ScriptData* ScriptIIfElse::run(ScriptData& _args) {
  ScriptData* condition = _condition->run(_args);
  if (condition->_data->type() == ScriptDataBase::TBOOLEAN) {
    if (((ScriptDataBool*)condition->_data)->_bl) {
      DeletePtr(condition);
      return _then->run(_args);
    }
    else {
      DeletePtr(condition);
      return _else->run(_args);
    }
  }
  throw 1;
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
    _false->_val->_data = new ScriptDataBool(false);
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
ScriptIIfElse::~ScriptIIfElse() {
  delete _then;
  delete _else;
  delete _condition;
}

ScriptData* ScriptILoop::run(ScriptData& _args) {
  ScriptData* condition = _condition->run(_args);
  while (condition->_data->type() == ScriptDataBase::TBOOLEAN && ((ScriptDataBool*)condition->_data)->_bl) {
    ScriptData* insres = _code->run(_args);
    if (insres != NULL) {
      DeletePtr(condition);
      return insres;
    }
    DeletePtr(condition);
    condition = _condition->run(_args);
  }
  DeletePtr(condition);
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
    _false->_val->_data = new ScriptDataBool(false);
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
ScriptILoop::~ScriptILoop() {
  delete _code;
  delete _condition;
}

ScriptData* ScriptIAssign::run(ScriptData& _args) {
  ScriptData* val = _val->run(_args);
  ScriptData* to = _to->run(_args);
  to->CopyContent(val);
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
  res->CopyContent(_val);
  return res;
}
void ScriptIConstant::load(xml_node<> *data) {
  if (_val != NULL) {
    delete _val;
  }
  _val = new ScriptData();
  string type = data->first_attribute("type")->value();
  if (type == "num") {
    _val->_data = new ScriptDataNumber(strTo<double>(data->value()));
  }
  if (type == "bool") {
    _val->_data = new ScriptDataBool(strTo<bool>(data->value()));
  }
  if (type == "str") {
    _val->_data = new ScriptDataString(strTo<string>(data->value()));
  }
  if (type == "char") {
    _val->_data = new ScriptDataChar(strTo<char>(data->value()));
  }
}
ScriptIConstant::~ScriptIConstant() {
  DeletePtr(_val);
}

ScriptData* ScriptIMath::run(ScriptData& _args) {
  ScriptData* res1 = _arg1->run(_args);
  if (res1->_data->type() != ScriptDataBase::TNUMERIC) {
    throw 1;
    return NULL;
  }
  ScriptData* res2 = NULL;
  if (_oper < 16) {
    res2 = _arg2->run(_args);
    if (res2->_data->type() != ScriptDataBase::TNUMERIC) {
      throw 1;
      return NULL;
    }
  }
  ScriptData* s = new ScriptData();
  switch (_oper) {
  case PLUS:
    s->_data = new ScriptDataNumber(((ScriptDataNumber*)(res1->_data))->_num + ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case MINUS:
    s->_data = new ScriptDataNumber(((ScriptDataNumber*)(res1->_data))->_num - ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case MULTI:
    s->_data = new ScriptDataNumber(((ScriptDataNumber*)(res1->_data))->_num * ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case DIV:
    s->_data = new ScriptDataNumber(((ScriptDataNumber*)(res1->_data))->_num / ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case POW:
    s->_data = new ScriptDataNumber(pow(((ScriptDataNumber*)(res1->_data))->_num , ((ScriptDataNumber*)(res2->_data))->_num));
    break;
  case LOGAB:
    s->_data = new ScriptDataNumber(log(((ScriptDataNumber*)(res1->_data))->_num) + log(((ScriptDataNumber*)(res2->_data))->_num));
    break;
  case MOD:
    s->_data = new ScriptDataNumber(modulo(((ScriptDataNumber*)(res1->_data))->_num, ((ScriptDataNumber*)(res2->_data))->_num));
    break;
  case ATAN2:
    s->_data = new ScriptDataNumber(atan2(((ScriptDataNumber*)(res1->_data))->_num, ((ScriptDataNumber*)(res2->_data))->_num));
    break;
  case L:
    s->_data = new ScriptDataBool(((ScriptDataNumber*)(res1->_data))->_num < ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case LEQ:
    s->_data = new ScriptDataBool(((ScriptDataNumber*)(res1->_data))->_num <= ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case EQ:
    s->_data = new ScriptDataBool(((ScriptDataNumber*)(res1->_data))->_num == ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case NEQ:
    s->_data = new ScriptDataBool(((ScriptDataNumber*)(res1->_data))->_num != ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case GEQ:
    s->_data = new ScriptDataBool(((ScriptDataNumber*)(res1->_data))->_num >= ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case G:
    s->_data = new ScriptDataBool(((ScriptDataNumber*)(res1->_data))->_num > ((ScriptDataNumber*)(res2->_data))->_num);
    break;
  case SQRT:
    s->_data = new ScriptDataNumber(sqrt(((ScriptDataNumber*)(res1->_data))->_num));
    break;
  case SIN:
    s->_data = new ScriptDataNumber(sin(((ScriptDataNumber*)(res1->_data))->_num));
    break;
  case COS:
    s->_data = new ScriptDataNumber(cos(((ScriptDataNumber*)(res1->_data))->_num));
    break;
  case TAN:
    s->_data = new ScriptDataNumber(tan(((ScriptDataNumber*)(res1->_data))->_num));
    break;
  case ASIN:
    s->_data = new ScriptDataNumber(asin(((ScriptDataNumber*)(res1->_data))->_num));
    break;
  case ACOS:
    s->_data = new ScriptDataNumber(acos(((ScriptDataNumber*)(res1->_data))->_num));
    break;
  case ATAN:
    s->_data = new ScriptDataNumber(atan(((ScriptDataNumber*)(res1->_data))->_num));
    break;
  }
  DeletePtr(res1);
  DeletePtr(res2);
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
ScriptIMath::~ScriptIMath() {
  delete _arg1;
  delete _arg2;
}

ScriptData* ScriptILogic::run(ScriptData& _args) {
  ScriptData* res1 = _arg1->run(_args);
  if (res1->_data->type() != ScriptDataBase::TBOOLEAN) {
    throw 1;
    return NULL;
  }
  ScriptData* res2 = NULL;
  if (_oper < 16) {
    res2 = _arg2->run(_args);
    if (res2->_data->type() != ScriptDataBase::TBOOLEAN) {
      throw 1;
      return NULL;
    }
  }
  ScriptData* s = new ScriptData();
  switch (_oper) {
  case EQ:
    s->_data = new ScriptDataBool(((ScriptDataBool*)(res1->_data))->_bl == ((ScriptDataBool*)(res2->_data))->_bl);
    break;
  case NEQ:
    s->_data = new ScriptDataBool(((ScriptDataBool*)(res1->_data))->_bl != ((ScriptDataBool*)(res2->_data))->_bl);
    break;
  case AND:
    s->_data = new ScriptDataBool(((ScriptDataBool*)(res1->_data))->_bl & ((ScriptDataBool*)(res2->_data))->_bl);
    break;
  case NAND:
    s->_data = new ScriptDataBool(!(((ScriptDataBool*)(res1->_data))->_bl & ((ScriptDataBool*)(res2->_data))->_bl));
    break;
  case OR:
    s->_data = new ScriptDataBool(((ScriptDataBool*)(res1->_data))->_bl | ((ScriptDataBool*)(res2->_data))->_bl);
    break;
  case NOR:
    s->_data = new ScriptDataBool(!(((ScriptDataBool*)(res1->_data))->_bl | ((ScriptDataBool*)(res2->_data))->_bl));
    break;
  case XOR:
    s->_data = new ScriptDataBool(((ScriptDataBool*)(res1->_data))->_bl ^ ((ScriptDataBool*)(res2->_data))->_bl);
    break;
  case NXOR:
    s->_data = new ScriptDataBool(!(((ScriptDataBool*)(res1->_data))->_bl ^ ((ScriptDataBool*)(res2->_data))->_bl));
    break;
  case NOT:
    s->_data = new ScriptDataBool(!((ScriptDataBool*)(res1->_data))->_bl);
    break;
  }
  DeletePtr(res1);
  DeletePtr(res2);
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
ScriptILogic::~ScriptILogic() {
  delete _arg1;
  delete _arg2;
}

ScriptData* ScriptIVariable::run(ScriptData& _args) {
  ScriptData* res;
  if (_args._elems.count(_arg) == 0) {
    _args._elems[_arg] = new ScriptData();
  }
  res = CopyPtr(_args._elems[_arg]);
  return res;
}
void ScriptIVariable::load(xml_node<> *data) {
  _arg = data->first_attribute("var")->value();
}
ScriptIVariable::~ScriptIVariable() {
}

ScriptData* ScriptIIndex::run(ScriptData& _args) {
  ScriptData* ind = _ind->run(_args);
  ScriptData* arg = _arg->run(_args);
  string index = ind->_data->getString();
  ScriptData* res;
  if (arg->_elems.count(index) == 0) {
    arg->_elems[index] = new ScriptData();
  }
  res = CopyPtr(arg->_elems[index]);
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
ScriptIIndex::~ScriptIIndex() {
  delete _arg;
  delete _ind;
}

ScriptData* ScriptIFunctionCall::run(ScriptData& _args) {
  ScriptData _nargs;
  for (auto&& it : _arguments) {
    ScriptData* _nval;
    _nval = it.second->run(_args);
    _nargs._elems.insert({ it.first, _nval });
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
ScriptData* ScriptIAPICall::run(ScriptData& _args) {
  ScriptData _nargs;
  for (auto&& it : _arguments) {
    ScriptData* _nval;
    _nval = it.second->run(_args);
    _nargs._elems.insert({ it.first, _nval });
  }
  return _func(_nargs);
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
ScriptIBlock::~ScriptIBlock() {
  while (_instructions.size()) {
    delete *(_instructions.begin());
    _instructions.erase(_instructions.begin());
  }
}

#ifdef SCRIPT_GUI

void ScriptGUI::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
  cax = offsetX + location.getLeft(winWidth);
  cay = offsetY + location.getBot(winHeight);
  cbx = offsetX + location.getRight(winWidth);
  cby = offsetY + location.getTop(winHeight);

  getRect(offsetX, offsetY);
}
void ScriptGUI::getRect(int offsetX, int offsetY) {
  code->getRect(offsetX, offsetY);
}
int ScriptGUI::mouseEnter(int state) {
  return code->mouseEnter(state);
}
int ScriptGUI::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return code->mouseMoved(mx, my);
}
int ScriptGUI::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return code->guiEvent(evt, mx, my, down);
}
void ScriptGUI::render(set<key_location>& down) {
  code->render(this, 1);
}

void ScriptGUIBase::getRect(int lcax, int lcby) {
}
int ScriptGUIBase::mouseEnter(int state) {
  return 0;
}
int ScriptGUIBase::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int ScriptGUIBase::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
void ScriptGUIBase::render(ScriptGUI* base, int depth) {
}

void ScriptInstruction::getRect(int offsetX, int offsetY) {

}
void ScriptIIfElse::getRect(int offsetX, int offsetY) {
  cax = cbx = offsetX;
  cay = cby = offsetY;
  if (_else != NULL) {
    _else->getRect(cax + 5, cby + 5);
    cby = _else->cby + 15;
    cey = cby;
    cbx = max(cbx, _else->cbx + 5);
  }
  if (_then != NULL) {
    _then->getRect(cax + 5, cby + 5);
    cby = _then->cby + 15;
    cty = cby;
    cbx = max(cbx, _then->cbx + 5);
  }
  if (_condition != NULL) {
    _condition->getRect(cax + 5, cby + 5);
    cby = _condition->cby + 20;
    cbx = max(cbx, _condition->cbx + 5);
  }
}
void ScriptILoop::getRect(int offsetX, int offsetY) {
  cax = cbx = offsetX;
  cay = cby = offsetY;
    if (_code != NULL) {
    _code->getRect(cax + 5, cby + 5);
    cby = _code->cby + 5;
    cbx = max(cbx, _code->cbx + 5);
  }
  if (_condition != NULL) {
    _condition->getRect(cax + 5, cby + 5);
    cby = _condition->cby + 5;
    cbx = max(cbx, _condition->cbx + 5);
  }

}
void ScriptIAssign::getRect(int offsetX, int offsetY) {
  cax = cbx = offsetX;
  cay = cby = offsetY;
  if (_to != NULL) {
    _to->getRect(cbx + 5, cay + 5);
    cbx = _to->cbx + 20;
    cex = cbx;
    cby = max(cby, _to->cby + 5);
  }
  if (_val != NULL) {
    _val->getRect(cbx + 5, cay + 5);
    cbx = _val->cbx + 5;
    cby = max(cby, _val->cby + 5);
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
void ScriptIConstant::getRect(int offsetX, int offsetY) {
  cax = offsetX;
  cay = offsetY;
  cbx = cax + 100;
  cby = cay + 15;
}
void ScriptIMath::getRect(int offsetX, int offsetY) {
  cax = cbx = offsetX;
  cay = cby = offsetY;
  if (_arg1 != NULL) {
    _arg1->getRect(cbx + 5, cay + 5);
    cbx = _arg1->cbx + 20;
    cox = cbx;
    cby = max(cby, _arg1->cby + 5);
  }
  if (_arg2 != NULL) {
    _arg2->getRect(cbx + 5, cay + 5);
    cbx = _arg2->cbx + 5;
    cby = max(cby, _arg2->cby + 5);
  }
}
void ScriptILogic::getRect(int offsetX, int offsetY) {
  cax = cbx = offsetX;
  cay = cby = offsetY;
  if (_arg1 != NULL) {
    _arg1->getRect(cbx + 5, cay + 5);
    cbx = _arg1->cbx + 20;
    cox = cbx;
    cby = max(cby, _arg1->cby + 5);
  }
  if (_arg2 != NULL) {
    _arg2->getRect(cbx + 5, cay + 5);
    cbx = _arg2->cbx + 5;
    cby = max(cby, _arg2->cby + 5);
  }
}
void ScriptIVariable::getRect(int offsetX, int offsetY) {
  cax = cbx = offsetX;
  cay = cby = offsetY;
  cbx = cax + 100;
  cby = cay + 15;
}
void ScriptIIndex::getRect(int offsetX, int offsetY) {
  cax = cbx = offsetX;
  cay = cby = offsetY;
  if (_arg != NULL) {
    _arg->getRect(cbx + 5, cay + 5);
    cbx = _arg->cbx + 5;
    cby = max(cby, _arg->cby + 5);
  }
  if (_ind != NULL) {
    _ind->getRect(cbx + 5, cay + 5);
    cbx = _ind->cbx + 5;
    cby = max(cby, _ind->cby + 5);
  }
}
void ScriptIFunctionCall::getRect(int offsetX, int offsetY) {
  //getRect();
}
void ScriptIAPICall::getRect(int offsetX, int offsetY) {
  //getRect();
}
void ScriptIBlock::getRect(int offsetX, int offsetY) {
  //getRect();
  cax = cbx = offsetX;
  cay = cby = offsetY;
  auto it = _instructions.end();

  while(it != _instructions.begin()) {
    --it;
    (*it)->getRect(cax + 5, cby + 5);
    cby = (*it)->cby;
    cbx = max(cbx, (*it)->cbx + 5);
  }
  cby = cby + 5;
}

void ScriptInstruction::render(ScriptGUI* base, int depth) {
  throw 1;
}
void ScriptGUIBase::renderBg(ScriptGUI* base, int depth) {
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
void ScriptIIfElse::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  renderBitmapString(cax + 5, cby - 15 , "IF", base->textColor, false);
  _condition->render(base, depth + 1);
  renderBitmapString(cax + 5, cty - 10, "THEN", base->textColor, false);
  _then->render(base, depth + 1);
  renderBitmapString(cax + 5, cey - 10, "ELSE", base->textColor, false);
  _else->render(base, depth + 1);
}
void ScriptILoop::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  _condition->render(base, depth + 1);
  _code->render(base, depth + 1);
}
void ScriptIAssign::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  _val->render(base, depth + 1);
  renderBitmapString(cex - 10, (cay+cby)/2, "=", base->textColor, true);
  _to->render(base, depth + 1);
}
void ScriptIConstant::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, _val->_data->getString(), base->textColor, true);
}
void ScriptIMath::render(ScriptGUI* base, int depth) {
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
      operm = '!=';
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

  renderBitmapString(cax, (cay + cby) / 2, operb, base->textColor, false);
  _arg1->render(base, depth + 1);
  if(_arg2 != NULL) {
    renderBitmapString(cox - 10, (cay + cby) / 2, operm, base->textColor, true);
    _arg2->render(base, depth + 1);
  }
}
void ScriptILogic::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  _arg1->render(base, depth + 1);
  if (_arg2 != NULL) {
    _arg2->render(base, depth + 1);
  }
}
void ScriptIVariable::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  renderBitmapString((cax + cbx) / 2.0f, (cay + cby) / 2.0f, _arg, base->textColor, true);

}
void ScriptIIndex::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

  _arg->render(base, depth + 1);
  _ind->render(base, depth + 1);
}
void ScriptIFunctionCall::render(ScriptGUI* base, int depth) {

}
void ScriptIAPICall::render(ScriptGUI* base, int depth) {

}
void ScriptIBlock::render(ScriptGUI* base, int depth) {
  renderBg(base, depth);

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

int ScriptInstruction::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptIIfElse::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptILoop::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptIAssign::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptIConstant::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptIMath::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptILogic::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptIVariable::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptIIndex::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptIFunctionCall::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptIAPICall::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
int ScriptIBlock::guiEvent(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
#endif
