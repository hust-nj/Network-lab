#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#pragma comment(lib, "ws2_32.lib")//���� ws2_32.dll
#pragma warning(disable:4996)

//#define DEBUG

class CSocket {
public:
	CSocket(const char *transport, int type, int proto);
	SOCKET connectsock(const char *ihost, const char *iservice);
	void sendmsg(const char *msg);
	virtual void recvmsg() = 0;
	CSocket(const CSocket&) = delete;//�����������
	~CSocket() { closesocket(s); }
private:
	//;//����˵��ַ

	//�������Զ���ӿ�����
	const char *transport = nullptr;
	int proto;
	int type;

protected:
	SOCKET s;
	void errexit(const char *format, ...);
};

CSocket::CSocket(const char *transport, int type, int proto)
	:transport(transport), type(type), proto(proto)
{
	WSADATA wsadata;
	if (WSAStartup(0x0002, &wsadata))//version2.0
		errexit("WSAStartup failed\n");
	if (wsadata.wVersion != 0x0002)
		errexit("version 2.0 needed\n");

	/* ����socket */
	if ((s = socket(AF_INET, type, proto)) == INVALID_SOCKET)
		errexit("cannot create socket: %d\n", WSAGetLastError());
}

SOCKET CSocket::connectsock(const char *host, const char *service)
{
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;//internet

	/* ���� Ӧ�ò�Э��(����)[+�����Э��] �Ƶ� �˿ڵ�ַ */
	servent *pse = getservbyname(service, transport);
	if (pse)
		sin.sin_port = pse->s_port;
	else if ((sin.sin_port = htons((u_short)atoi(service))) == 0)//htons���������ֽ���ת��
		errexit("cannot get \"%s\" service entry\n", service);

	/* �Զ��������� */
	hostent *phe = gethostbyname(host);
	if (phe)
		memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
	else if ((sin.sin_addr.s_addr = inet_addr(host)) == (u_long)-1)
		errexit("cannot get \"s\" host entry\n", host);

	/* �������� */
	if (connect(s, (sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
		errexit("cannot connect to %s.%s: %d\n", host, service,
			WSAGetLastError());

#ifdef DEBUG//���sockaddr_in sin�е���Ϣת���Ƿ���ȷ
	printf("port: %hx\n", sin.sin_port);//����˿�(�����ֽڡ��������)
	printf("ip: %x\n", sin.sin_addr.s_addr);//���ip��ַ
#endif // !NDEBUG

	return s;
}

void CSocket::sendmsg(const char *msg)
{
	if (send(s, msg, strlen(msg), 0) == SOCKET_ERROR)
		errexit("cannot send the message: %d\n", WSAGetLastError());
}


void CSocket::errexit(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	WSACleanup();
	exit(1);//error
}


class CUDP : public CSocket{
public:
	CUDP() :CSocket("udp", SOCK_DGRAM, IPPROTO_UDP) {}
	void sendmsgto(const char *host, const char *service, const char *msg);
	void recvmsg() override;
private:
	static constexpr int buflen = 128;
	char buf[buflen + 1];
};

void CUDP::sendmsgto(const char *host, const char *service, const char *msg)
{
	connectsock(host, service);
	sendmsg(msg);
}

void CUDP::recvmsg()
{
	int n = recv(s, buf, buflen, 0);
	if (n == SOCKET_ERROR)
		errexit("recv failed: recv() error %d\n", WSAGetLastError());
	else
	{
		buf[n] = '\0';
		fputs(buf, stdout);
	}
}

class CTCP : public CSocket {
public:
	CTCP() :CSocket("tcp", SOCK_STREAM, IPPROTO_TCP) {}
	void recvmsg() override;
};

void CTCP::recvmsg()
{
	constexpr int buflen = 100000;
	char buf[buflen + 1];
	int n = recv(s, buf, buflen, 0);
#ifdef DEBUG
	cout << "call ones" << endl;
	cout << n << endl;
#endif // DEBUG
	int cnt = 0;
	if (n != SOCKET_ERROR && n > 0)
	{
		cout << "\n\n" << cnt++ << "\n\n";

		buf[n] = '\0';
		fputs(buf, stdout);
		n = recv(s, buf, buflen, 0);
#ifdef DEBUG
		cout << "call ones" << endl;
#endif // DEBUG
	}
	if (n == SOCKET_ERROR)
		errexit("recv failed: recv() error %d\n", WSAGetLastError());

	cout << "--------------------------------------------------------------------------------------------" << endl;
}

int main(int argc, char *argv[])
{
	//t.connectsock("www.cs.hust.edu.cn", "http");
	string s;
	ifstream request("request.txt");
	//freopen("response.txt", "w", stdout);
	if (request.fail())
	{
		cerr << "open fail";
		exit(1);
	}
	getline(request, s, (char)-1);
#ifdef DEBUG
	cout << s << endl;
#endif // DEBUG
	request.close();
	//t.sendmsg("GET / HTTP/1.1\nHost: www.baidu.com\nConnection: keep-alive\n\n");
	//CTCP t;
	//t.connectsock("10.14.116.230", "http");
	//t.sendmsg(s.data());
//	t.recvmsg();//���ܱ��Ĳ���ӡ
	long long cnt = 0;
	
	{
		cnt++;
		cout << cnt << endl;
		CTCP *pt = new CTCP;
		pt->connectsock("", "http");
		pt->sendmsg(s.data());
		pt->recvmsg();//���ܱ��Ĳ���ӡ
	}
	WSACleanup();
	return 0;
}
