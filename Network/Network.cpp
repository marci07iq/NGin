#include "Network.h"

enum NetworkErrorCodes {
  NetworkErrorCodeInitalize,
  NetworkErrorCodeResolveServerAddress,
  NetworkErrorCodeResolveClientAddress,
  NetworkErrorCodeCreateListenSocket,
  NetworkErrorCodeCreateServerSocket,
  NetworkErrorCodeCreateClientSocket,
  NetworkErrorCodeConnectServer,
  NetworkErrorCodeConnectClient,
  NetworkErrorCodeServerSendData,
  NetworkErrorCodeServerReciveData,
  NetworkErrorCodeClientSendData,
  NetworkErrorCodeClientReciveData,
  NetworkErrorCodeBindListenSocket,
  NetworkErrorCodeServerListen,
  NetworkErrorCodeServerAccept,
};


void NetworkError(int ID, int& set, int WSAError) {
  set = ID;
  LOG LERROR NETWORK "Error code: " << to_string(ID) END;
}

NetworkS::NetworkS() {
}
NetworkS::NetworkS(string port, RecivePacketFuncS recivePacketFunc) {
  _port = port;
  RecivePacket = recivePacketFunc;
  struct addrinfo *result = NULL;
  int iResult;
  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  cleaned = false;
  if (iResult != 0) {
    NetworkError(NetworkErrorCodeInitalize, error, iResult);
    return;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the server address and port
  iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
  if (iResult != 0) {
    NetworkError(NetworkErrorCodeResolveServerAddress, error, iResult);
    cleaned = true;

    WSACleanup();
    return;
  }

  // Create a SOCKET for connecting to server
  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) {
    NetworkError(NetworkErrorCodeCreateListenSocket, error, 0);
    freeaddrinfo(result);
    cleaned = true;
    WSACleanup();
    return;
  }

  // Setup the TCP listening socket
  iResult = ::bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    NetworkError(NetworkErrorCodeBindListenSocket, error, 0);
    freeaddrinfo(result);
    closesocket(ListenSocket);
    cleaned = true;
    WSACleanup();
    return;
  }

  freeaddrinfo(result);

  //LISTEN

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    NetworkError(NetworkErrorCodeServerListen, error, 0);
    closesocket(ListenSocket);
    cleaned = true;
    WSACleanup();
    return;
  }

  // Accept a client socket
  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
    NetworkError(NetworkErrorCodeServerAccept, error, 0);
    closesocket(ListenSocket);
    cleaned = true;
    WSACleanup();
    return;
  }

  // No longer need server socket
  closesocket(ListenSocket);

  ReciveLoopThread = thread(&NetworkS::Loop, this);

}
NetworkS::~NetworkS() {
  Running = false;
  // shutdown the connection since we're done
  shutdown(ClientSocket, SD_SEND);
  // cleanup
  closesocket(ClientSocket);
  ReciveLoopThread.join();
  if(!cleaned) {
  cleaned = true;
    WSACleanup();
  }
}

void NetworkS::Loop()
{
  int iResult;
  // Receive until the peer shuts down the connection
  do {
    iResult = ReciveData();
    if (iResult <= 0) {
      if (iResult == 0) {
        cout << "Socket Closed" << endl;
        closesocket(ClientSocket);
        cleaned = true;
        WSACleanup();
      }
      else {
        NetworkError(NetworkErrorCodeServerReciveData, error, 0);
        closesocket(ClientSocket);
        cleaned = true;
        WSACleanup();
        return;
      }
    }
  } while (iResult > 0 && Running);
  Running = false;
}

int NetworkS::SendData(char *Data, int Id, int DataLen) {
  char* SendRaw;
  SendRaw = new char[DataLen + 2 * PACKET_HEADER_LEN];
  for (int i = 0; i < DataLen; i++) {
    SendRaw[i + 2 * PACKET_HEADER_LEN] = Data[i];
  }

  Packet_Header_Convertor conv;
  conv.i = DataLen;
  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    SendRaw[i] = conv.chararr.chars[i];
  }

  conv.i = Id;
  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    SendRaw[i + PACKET_HEADER_LEN] = conv.chararr.chars[i];
  }

  int iSendResult = send(ClientSocket, SendRaw, DataLen + 2 * PACKET_HEADER_LEN, 0);

  delete[] SendRaw;
  delete[] Data;

  if (iSendResult != DataLen + 2 * PACKET_HEADER_LEN) {
    NetworkError(NetworkErrorCodeServerSendData, error, WSAGetLastError());
    closesocket(ClientSocket);
    WSACleanup();
    return -1;
  }
  return iSendResult;
}

int NetworkS::SendData(DataElement* Data, int Id) {
  int len = Data->getLen();
  unsigned char* datac = new unsigned char[len];
  int start = 0;
  Data->fill(datac, start);
  if (start != len) {
    throw 1;
  }
  delete Data;
  return SendData(datac, Id, len);
}

int NetworkS::SendData(unsigned char *Data, int Id, int DataLen) {
  return SendData(reinterpret_cast<char*>(Data), Id, DataLen);
}

int NetworkS::ReciveData() {
  char len[PACKET_HEADER_LEN];
  Packet_Header_Convertor conv;
  PACKET_HEADER_TYPE dlen;
  PACKET_HEADER_TYPE pid;
  //DATA LENGTH
  int iRes = recv(ClientSocket, len, PACKET_HEADER_LEN, 0);
  if (iRes != PACKET_HEADER_LEN) {
    return 0;
  }


  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    conv.chararr.chars[i] = len[i];
  }

  dlen = conv.i;
  if (dlen == 0) {
    return 0;
  }

  //PACKET ID
  iRes = recv(ClientSocket, len, PACKET_HEADER_LEN, 0);
  if (iRes != PACKET_HEADER_LEN) {
    return 0;
  }

  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    conv.chararr.chars[i] = len[i];
  }

  pid = conv.i;

  //DATA

  netlock.lock();
  char* data;
  data = new char[dlen];
  iRes = 0;
  while (iRes != dlen) {
    iRes += recv(ClientSocket, data + iRes, dlen - iRes, 0);
  }

  DataElement* datae = new DataElement();
  int start = 0;
  datae->empty(reinterpret_cast<unsigned char*>(data), start);
  if (start != dlen) {
    throw 1;
  }

  bool t = false;

  if (!datae->verify()) {
    LOG LERROR "COULD NOT DECODE PACKET" END;
    start = 0;
    delete datae;
    __debugbreak();
    datae->empty(reinterpret_cast<unsigned char*>(data), start);
    //try to re-decode (to check in debugger)
  } else {
    t = RecivePacket(datae, pid, this, ConnectedBinder);
  }

  delete[] data;
  delete datae;

  netlock.unlock();

  if (t) {
    return 0;
  }

  return dlen;
}

NetworkC::NetworkC() {
}
NetworkC::NetworkC(string IP, string port, RecivePacketFuncC recivePacketFunc) {
  _port = port;
  _IP = IP;
  RecivePacket = recivePacketFunc;
  struct addrinfo *result, *ptr;
  result = NULL;
  ptr = NULL;
  int iResult;
  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  cleaned = false;
  if (iResult != 0) {
    NetworkError(NetworkErrorCodeInitalize, error, 0);
    return;
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  iResult = getaddrinfo(IP.c_str(), port.c_str(), &hints, &result);
  if (iResult != 0) {
    NetworkError(NetworkErrorCodeResolveServerAddress, error, 0);
    cleaned = true;
    WSACleanup();
    return;
  }

  // Attempt to connect to an address until one succeeds
  for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
      ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
      NetworkError(NetworkErrorCodeCreateClientSocket, error, 0);
      cleaned = true;
      WSACleanup();
      return;
    }

    // Connect to server.
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
      closesocket(ConnectSocket);
      ConnectSocket = INVALID_SOCKET;
      continue;
    }
    break;
  }

  freeaddrinfo(result);

  if (ConnectSocket == INVALID_SOCKET) {
    NetworkError(NetworkErrorCodeConnectServer, error, 0);
    cleaned = true;
    WSACleanup();
    return;
  }

  ReciveLoopThread = thread(&NetworkC::Loop, this);

}
NetworkC::~NetworkC() {
  Running = false;
  if (ConnectSocket != SOCKET_ERROR) {
    // shutdown the connection since we're done
    shutdown(ConnectSocket, SD_SEND);
    // cleanup
    closesocket(ConnectSocket);
    ReciveLoopThread.join();
  }
  if(!cleaned) {
    WSACleanup();
  }
}

void NetworkC::Loop()
{
  int iResult;
  // Receive until the peer shuts down the connection
  do {
    iResult = ReciveData();
    //NetLog.LogString("iResult: " + to_string(iResult));
    if (iResult <= 0) {
      if (iResult == 0) {
        closesocket(ConnectSocket);
        cleaned = true;
        WSACleanup();
      }
      else {
        NetworkError(NetworkErrorCodeClientReciveData, error, WSAGetLastError());
        closesocket(ConnectSocket);
        cleaned = true;
        WSACleanup();
        return;
      }
    }
  } while (iResult > 0 && Running);
  Running = false;
}

int NetworkC::SendData(char *Data, int Id, int DataLen) {
  char* SendRaw;
  SendRaw = new char[DataLen + 2 * PACKET_HEADER_LEN];
  for (int i = 0; i < DataLen; i++) {
    SendRaw[i + 2 * PACKET_HEADER_LEN] = Data[i];
  }

  Packet_Header_Convertor conv;
  conv.i = DataLen;
  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    SendRaw[i] = conv.chararr.chars[i];
  }

  conv.i = Id;
  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    SendRaw[i + PACKET_HEADER_LEN] = conv.chararr.chars[i];
  }

  int iSendResult = send(ConnectSocket, SendRaw, DataLen + 2 * PACKET_HEADER_LEN, 0);

  //delete Data;
  //delete SendRaw;
  
  delete[] SendRaw;
  delete[] Data;

  if (iSendResult != DataLen + 2 * PACKET_HEADER_LEN) {
    NetworkError(NetworkErrorCodeClientSendData, error, 0);
    closesocket(ConnectSocket);
    WSACleanup();
    return -1;
  }
  return iSendResult;
}

int NetworkC::SendData(unsigned char *Data, int Id, int DataLen) {
  return SendData(reinterpret_cast<char*>(Data), Id, DataLen);
}

int NetworkC::SendData(DataElement* Data, int Id) {
  int len = Data->getLen();
  unsigned char* datac = new unsigned char[len];
  int start = 0;
  Data->fill(datac, start);
  if (start != len) {
    throw 1;
  }

  delete Data;
  return SendData(datac, Id, len);
}

int NetworkC::ReciveData() {
  char len[PACKET_HEADER_LEN];
  Packet_Header_Convertor conv;
  PACKET_HEADER_TYPE dlen;
  PACKET_HEADER_TYPE pid;
  //DATA LENGTH
  int iRes = recv(ConnectSocket, len, PACKET_HEADER_LEN, 0);
  //NetLog.LogString("iRes: " + to_string(iRes));
  if (iRes != PACKET_HEADER_LEN) {
    return 0;
  }


  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    conv.chararr.chars[i] = len[i];
  }

  dlen = conv.i;
  //NetLog.LogString("dlen: " + to_string(dlen));
  if (dlen == 0) {
    return 0;
  }

  //PACKET ID
  iRes = recv(ConnectSocket, len, PACKET_HEADER_LEN, 0);
  //NetLog.LogString("iRes: " + to_string(iRes));
  if (iRes != PACKET_HEADER_LEN) {
    return 0;
  }

  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    conv.chararr.chars[i] = len[i];
  }

  pid = conv.i;

  //DATA

  netlock.lock();

  char* data;
  data = new char[dlen];
  iRes = 0;
  while (iRes != dlen) {
    iRes += recv(ConnectSocket, data + iRes, dlen - iRes, 0);
  }

  DataElement* datae = new DataElement();
  int start = 0;
  datae->empty(reinterpret_cast<unsigned char*>(data), start);
  if (start != dlen) {
    throw 1;
  }

  bool t = false;

  if (!datae->verify()) {
    LOG LERROR "COULD NOT DECODE PACKET" END;
    start = 0;
    delete datae;
    __debugbreak();
    datae->empty(reinterpret_cast<unsigned char*>(data), start);
    //try to re-decode (to check in debugger)
  } else {
    t = RecivePacket(datae, pid, this, ConnectedBinder);
  }

  delete[] data;
  delete datae;

  netlock.unlock();

  if (t) {
    return 0;
  }

  return dlen;
}

/*
void concat(vector<pair<unsigned char*, int> > in, unsigned char** C, int &lenC, bool destroy) {

  Packet_Header_Convertor conv;

  lenC = 0;

  for (int i = 0; i<in.size(); i++) {

    lenC += PACKET_HEADER_LEN + in[i].second;
  }

  *C = new unsigned char[lenC];

  int prevSector = 0;

  for (int k = 0; k<in.size(); k++) {
    conv.i = in[k].second;

    for (int i = 0; i < PACKET_HEADER_LEN; i++) {
      (*C)[prevSector + i] = conv.chararr.chars[i];
    }

    prevSector += PACKET_HEADER_LEN;

    for (int i = 0; i < in[k].second; i++) {
      (*C)[prevSector + i] = in[k].first[i];
    }

    prevSector += in[k].second;

    if (destroy) {
      delete in[k].first;
    }
  }


}

void split(unsigned char* data, int dataLen, vector<pair<unsigned char*, int> > &split, bool destroy) {
  Packet_Header_Convertor conv;

  int till = 0;
  while (till < dataLen) {
    for (int i = 0; i < PACKET_HEADER_LEN; i++) {
      conv.chararr.chars[i] = data[till + i];
    }

    till += PACKET_HEADER_LEN;

    unsigned char *text = new unsigned char[conv.i];

    for (int i = 0; i < conv.i; i++) {
      text[i] = data[till + i];
    }

    till += conv.i;

    split.push_back({ text, conv.i });
  }

  if(destroy) {
    delete data;
  }
}*/