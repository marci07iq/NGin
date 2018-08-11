#include "helperFunctions.h"
#include <cmath>
#include <cstdlib>

using namespace std;

template <typename T>
Zero<T>::Zero() { val = T(); }
template <typename T>
T Zero<T>::ret() {
  return val;
}

Zero<int>        intZero;
Zero<float>    floatZero;
Zero<double>  doubleZero;
Zero<string>  stringZero;

void replaceChar(string& str, char ch1, char ch2) {
  for (int i = 0; i < str.length(); ++i) {
    if (str[i] == ch1)
      str[i] = ch2;
  }
}

void solve2(float a, float b, float c, float sol[2], int& numOfSol)
{
  if (a == 0)
  {
    if (b == 0) {
      numOfSol = 0;
    }
    else {
      sol[0] = -c/b;
    }
  }
  float disc = pow(b,2)-4*a*c;
  if (disc > 0)
  {
    numOfSol = 2;
    sol[0] = (-b+sqrt(disc))/(2*a);
    sol[1] = (-b-sqrt(disc))/(2*a);
  } else if (disc == 0)
  {
    numOfSol = 1;
    sol[0] = -b/(2*a);
  } else
  {
    numOfSol = 0;
  }
}

void terminalClear()
{
  #ifdef _WIN32
    system("cls"); //when i cross-compile and run it in wine, this does not work fsr
  #else
    system("clear"); //(on my terminal, i can scroll back)
  #endif
}

void waitForEnter()
{
  #ifdef _WIN32
    //todo: cin.ignore and cin.peek
    string inp;
    getline(cin, inp); //
    getline(cin, inp); //ugly
  #else
    system("read"); //ugly, inelegant, slow, not portable
  #endif
}

vector<string> tokenize(string str, char split)
{
  vector<string> tokens;
  string::iterator begin = str.begin();
  string::iterator it = str.begin();

  while (it != str.end()) {
    if (*it == split) {
      tokens.push_back(string(begin, it));
      begin = ++it;
    } else {
      ++it;
    }
  }
  tokens.push_back(string(begin, it));

  return tokens;
}

int hexToInt(string& str, int s) {
  if (s >= str.size()) {
    return 0;
  }
  else {
    int cval = 0;
    if ('0' <= str[str.size() - s - 1] && str[str.size() - s - 1] <= '9') {
      cval = str[str.size() - s - 1] - '0';
    }
    if ('A' <= str[str.size() - s - 1] && str[str.size() - s - 1] <= 'F') {
      cval = str[str.size() - s - 1] - 'A' + 10;
    }
    if ('a' <= str[str.size() - s - 1] && str[str.size() - s - 1] <= 'f') {
      cval = str[str.size() - s - 1] - 'a' + 10;
    }
    return 16 * hexToInt(str, s + 1) + cval;
  }
}

int hexToInt(string str) {
  return hexToInt(str, 0);
}

void serialize(string& s, unsigned char** data, int& dataLen) {
  *data = new unsigned char[s.size()];
  for (int i = 0; i < s.size(); i++) {
    (*data)[i] = s[i];
  }
  dataLen = s.size();
}

string deserializes(unsigned char* data, int dataLen) {
  string s(dataLen, ' ');
  for (int i = 0; i < s.size(); i++) {
    s[i] = data[i];
  }
  //delete data;
  return s;
}
int    deserializei(unsigned char* data, int dataLen) {
  return strTo<int>(deserializes(data, dataLen));
}
float  deserializef(unsigned char* data, int dataLen) {
  return strTo<float>(deserializes(data, dataLen));
}
double deserialized(unsigned char* data, int dataLen) {
  return strTo<double>(deserializes(data, dataLen));
}

double ran1(long int nseed) {
  //random -seed expected
  int j;
  long k;
  static long iy = 0;
  static long iv[NTAB];
  static long int idum = 0;
  /*long iy = 0;
  long iv[NTAB];*/
  double temp;
  if (nseed != 0) {
    idum = nseed;
  }
  if (idum <= 0 || !iy) {
    if (-idum < 1) {
      idum = 1;
    }
    else {
      idum = -idum;
    }
    for (j = NTAB + 7; j >= 0; j--) {
      k = idum / IQ;
      idum = IA * (idum - k * IQ) - IR * k;
      if (idum < 0) {
        idum += IM;
      }
      if (j < NTAB) {
        iv[j] = idum;
      }
    }
    iy = iv[0];
  }
  k = idum / IQ;
  idum = IA * (idum - k * IQ) - IR * k;
  if (idum < 0) {
    idum += IM;
  }
  j = iy / NDIV;
  iy = iv[j];
  iv[j] = idum;
  if ((temp = AM * iy) > RNMX) {
    return RNMX;
  }
  else {
    return temp;
  }
}

bool validateTickType(float minLogValDist, float maxLogValDist, float tickType, float &mid) {
  mid = ceil(minLogValDist / tickType);
  bool b = (mid <= (maxLogValDist / tickType));
  mid *= tickType;
  return b;
}

float getValPerTick(float valPerPixel, float minPixelDist, float maxPixelDist) {
  float minValDist = minPixelDist * valPerPixel;
  float maxValDist = maxPixelDist * valPerPixel;

  int logScale = pow(10, floor(log10(maxValDist)));
  float minLogValDist = minValDist / logScale;
  float maxLogValDist = maxValDist / logScale;

  float res;

  if (validateTickType(minLogValDist, maxLogValDist, 1, res)) {
    return res * logScale;
  }
  if (validateTickType(minLogValDist, maxLogValDist, 0.5, res)) {
    return res * logScale;
  }
  if (validateTickType(minLogValDist, maxLogValDist, 0.2, res)) {
    return res * logScale;
  }
  if (validateTickType(minLogValDist, maxLogValDist, 0.25, res)) {
    return res * logScale;
  }
  if (validateTickType(minLogValDist, maxLogValDist, 0.4, res)) {
    return res * logScale;
  }
  return (minPixelDist + maxPixelDist) / 2;
}

uint64_t mix(uint64_t a, uint64_t b) {
  return (a << 32) + b;
}

uint32_t low(uint64_t a) {
  return a & 0xffffffff;
}

uint32_t high(uint64_t a) {
  return a >> 32;
}

string randomHexString(size_t length) {
  string s(length, '-');
  for (size_t i = 0; i < length; i++) {
    int r = (int)(16 * ran1());
    if (r < 10) {
      s[i] = '0'+r;
    }
    else {
      s[i] = 'A'+r-10;
    }
  }
  return s;
}

bool trimChar(const char& c, int flags) {
  if (c == '\n' && (flags & Trim_NewLine)) {
    return true;
  }
  if (c == '\r' && (flags & Trim_CarrRet)) {
    return true;
  }
  if (c == ' ' && (flags & Trim_Space)) {
    return true;
  }
  if (c < 32 && (flags & Trim_Control)) {
    return true;
  }
  if (c == 127 && (flags & Trim_Control)) {
    return true;
  }
  return false;
}

string trimStr(string& in, int flags) {
  string res;
  for (auto&& it : in) {
    if (!trimChar(it, flags)) {
      res += it;
    }
  }
  return res;
}

mutex netlock; //Locking for the network thread(s)
