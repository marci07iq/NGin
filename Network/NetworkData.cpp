#include "NetworkData.h"

void saveToFile(DataElement * data, string fileName) {
  int len = data->getLen();
  unsigned char* res = new unsigned char[len];
  int s = 0;

  data->fill(res, s);

  ofstream out(fileName);
  out.write(reinterpret_cast<char*>(res), len);
  out.close();

  delete[len] res;
  delete data;
}

int loadFromFile(DataElement * data, string fileName) {
  ifstream in(fileName);
  std::string contents((std::istreambuf_iterator<char>(in)),
    std::istreambuf_iterator<char>());
  in.close();
  if (contents.length()) {
    int s = 0;
    data->empty(reinterpret_cast<const unsigned char*>(contents.c_str()), s);
  }
  return contents.length();
}
