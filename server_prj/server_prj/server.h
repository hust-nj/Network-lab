#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdarg.h>
#include <process.h>
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <direct.h>
#include <stdexcept>
#include <exception>
#include <queue>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

class SSocket {
public:
	SSocket(int type, int proto);
	SSocket(SOCKET s) : s(s) { }
	void bind(const char *port);
	void printIP();
	void close() { closesocket(s); }
protected:
	SOCKET s;
	sockaddr_in addr;
	void errexit(const char *format, ...);
};

class STCP : public SSocket {
public:
	STCP() : SSocket(SOCK_STREAM, IPPROTO_TCP) { }
	STCP(SOCKET s) : SSocket(s) { }
	void listen();
	STCP accept();
	string recv();
	void send(const string &response);
};
