#pragma once

#include "Compress.h"
#include "NetworkData.h"

//#ifdef WIN32
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
/*#else
//If these lines fail, set complile mode to 32 bit (x86), windows. If it still fails, define WIN32 on compiler level.
#include <sys/types.h>
#include <sys/socket.h>
#endif*/

//#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//#endif

using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class NetworkC;
class NetworkS;

class NetBinder;

typedef bool(*RecivePacketFuncS)(DataElement *Data, int Id, NetworkS* thisptr, NetBinder* ship);
typedef bool(*RecivePacketFuncC)(DataElement *Data, int Id, NetworkC* thisptr, NetBinder* ship);


//void defaultRecivePacketFunc(unsigned char *Data, int Id, int DataLen);

class NetworkS {
public:
  NetworkS();
  NetworkS(string port, RecivePacketFuncS recivePacketFunc);
  ~NetworkS();
  int error = 0;
  bool cleaned = true;
  string _port;
  void Loop();
  int SendData(char *Data, int Id, int DataLen);
  int SendData(unsigned char *Data, int Id, int DataLen);
  template<typename T> int SendData(T& data, int Id) {
    unsigned char* c;
    int l;
    serialize(data, &c, l);
    return SendData(c, Id, l);
  }
  int SendData(DataElement* data, int Id);
  int ReciveData();
  thread ReciveLoopThread;
  bool Running = true;
  RecivePacketFuncS RecivePacket;
  NetBinder* ConnectedBinder;
private:
  WSADATA wsaData;
  SOCKET ListenSocket = INVALID_SOCKET;
  SOCKET ClientSocket = INVALID_SOCKET;
  struct addrinfo hints;
};

class NetworkC {
public:
  NetworkC();
  NetworkC(string IP, string port, RecivePacketFuncC recivePacketFunc);
  ~NetworkC();
  int error = 0;
  bool cleaned = true;
  string _IP = "";
  string _port;
  void Loop();
  int SendData(char *Data, int Id, int DataLen);
  int SendData(unsigned char *Data, int Id, int DataLen);
  template<typename T> int SendData(T& data, int Id) {
    stringstream ss;
    ss << data;
    std::string datas = ss.str();

    char* id_c = new char[datas.size() + 1];
    strcpy_s(id_c, datas.size(), datas.c_str());

    int val = SendData(id_c, Id, datas.length());

    delete[] id_c;
    return val;
  }
  int SendData(DataElement* data, int Id);
  int ReciveData();
  bool Running = true;
  thread ReciveLoopThread;
  NetBinder* ConnectedBinder;
private:
  WSADATA wsaData;
  SOCKET ConnectSocket = INVALID_SOCKET;
  struct addrinfo hints;
  RecivePacketFuncC RecivePacket;
};

/*
void concat(vector<pair<unsigned char*, int> > in, unsigned char** C, int &lenC, bool destroy = true);

void split(unsigned char* data, int dataLen, vector<pair<unsigned char*, int> > &split, bool destroy = true);*/