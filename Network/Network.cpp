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

NetworkS::NetworkS(string port, RecivePacketFunc recivePacketFunc) : Network(recivePacketFunc){
  _port = port;
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

NetworkC::NetworkC(string IP, string port, RecivePacketFunc recivePacketFunc) : Network(recivePacketFunc) {
  _port = port;
  _IP = IP;
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
    ClientSocket = socket(ptr->ai_family, ptr->ai_socktype,
      ptr->ai_protocol);
    if (ClientSocket == INVALID_SOCKET) {
      NetworkError(NetworkErrorCodeCreateClientSocket, error, 0);
      cleaned = true;
      WSACleanup();
      return;
    }

    // Connect to server.
    iResult = connect(ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
      closesocket(ClientSocket);
      ClientSocket = INVALID_SOCKET;
      continue;
    }
    break;
  }

  freeaddrinfo(result);

  if (ClientSocket == INVALID_SOCKET) {
    NetworkError(NetworkErrorCodeConnectServer, error, 0);
    cleaned = true;
    WSACleanup();
    return;
  }

  ReciveLoopThread = thread(&NetworkC::Loop, this);

}
NetworkC::~NetworkC() {
  Running = false;
  if (ClientSocket != SOCKET_ERROR) {
    // shutdown the connection since we're done
    shutdown(ClientSocket, SD_SEND);
    // cleanup
    closesocket(ClientSocket);
    ReciveLoopThread.join();
  }
  if(!cleaned) {
    WSACleanup();
  }
}

void Network::Loop()
{
  int iResult;
  // Receive until the peer shuts down the connection
  do {
    iResult = ReciveData();
    if (iResult <= 0) {
      if (iResult == 0) {
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

int Network::SendData(char *Data, int Id, int DataLen) {
  char* SendRaw;
  SendRaw = new char[DataLen + 2 * PACKET_HEADER_LEN];
  for (int i = 0; i < DataLen; i++) {
    SendRaw[i + 2 * PACKET_HEADER_LEN] = Data[i];
  }

  PACKET_HEADER_TYPE conv;
  conv = DataLen;
  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    SendRaw[i] = reinterpret_cast<unsigned char*>(&conv)[i];
  }

  conv = Id;
  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    SendRaw[i + PACKET_HEADER_LEN] = reinterpret_cast<unsigned char*>(&conv)[i];
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

int Network::SendData(DataElement* Data, int Id) {
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

int Network::SendData(unsigned char *Data, int Id, int DataLen) {
  return SendData(reinterpret_cast<char*>(Data), Id, DataLen);
}

int Network::ReciveData() {
  char len[PACKET_HEADER_LEN];
  PACKET_HEADER_TYPE dlen;
  PACKET_HEADER_TYPE pid;
  //DATA LENGTH
  int iRes = recv(ClientSocket, len, PACKET_HEADER_LEN, 0);
  if (iRes != PACKET_HEADER_LEN) {
    return 0;
  }


  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    reinterpret_cast<unsigned char*>(&dlen)[i] = len[i];
  }

  if (dlen == 0) {
    return 0;
  }

  //PACKET ID
  iRes = recv(ClientSocket, len, PACKET_HEADER_LEN, 0);
  if (iRes != PACKET_HEADER_LEN) {
    return 0;
  }

  for (int i = 0; i < PACKET_HEADER_LEN; i++) {
    reinterpret_cast<unsigned char*>(&pid)[i] = len[i];
  }

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
    t = RecivePacket(datae, pid, this, _binder);
  }

  delete[] data;
  delete datae;

  netlock.unlock();

  if (t) {
    return 0;
  }

  return dlen;
}
