#pragma once

#include "Compress.h"
#include "NetworkData.h"


//#ifdef WIN32
#include <winsock2.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
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

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class Network;

class NetworkBinder;

typedef bool(*RecivePacketFunc)(DataElement *Data, int Id, Network* connection, NetworkBinder* binder);


//void defaultRecivePacketFunc(unsigned char *Data, int Id, int DataLen);

class Network {
public:
  Network(RecivePacketFunc recivePacketFunc) {
    RecivePacket = recivePacketFunc;
  }
  ~Network() {

  }

  int SendData(char* Data, int Id, int DataLen);
  int SendData(unsigned char* Data, int Id, int DataLen);
  template<typename T> int SendData(T& data, int Id) {
    unsigned char* c;
    int l;
    serialize(data, &c, l);
    return SendData(c, Id, l);
  }
  int SendData(DataElement* data, int Id);
  
  int ReciveData();
  
  thread ReciveLoopThread;
  
  void Loop();
  
  bool Running = true;
  int error = 0;
  bool cleaned = true;
  
  RecivePacketFunc RecivePacket;
  string _port;

  NetworkBinder* _binder;
protected:
  WSADATA wsaData;
  SOCKET ClientSocket = INVALID_SOCKET;
  struct addrinfo hints;
};

class NetworkS : public Network {
public:
  NetworkS(string port, RecivePacketFunc recivePacketFunc);
  ~NetworkS();
private:
  SOCKET ListenSocket = INVALID_SOCKET;
};

class NetworkC : public Network {
public:
  NetworkC(string IP, string port, RecivePacketFunc recivePacketFunc);
  ~NetworkC();
private:
  string _IP;
};

class NetworkBinder {
public:
  Network* connection;
  virtual bool recivePacket(DataElement* Data, int Id, Network* connection) {
    return true;
  }
};