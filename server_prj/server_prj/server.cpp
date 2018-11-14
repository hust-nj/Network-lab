#include "server.h"

extern string root;

SSocket::SSocket(int type, int proto)
{
	WSADATA wsadata;
	if (WSAStartup(0x0002, &wsadata))//version 2.0
		errexit("WSAStartup failed.\n");
	if (wsadata.wVersion != 0x0002)
		errexit("version 2.0 needed.\n");

	/* 分配socket */
	if ((s = socket(AF_INET, type, proto)) == INVALID_SOCKET)
		errexit("cannont create socket: %d\n", WSAGetLastError());
}

/* 绑定套接字 */
void SSocket::bind(const char *port)
{
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons((u_short)atoi(port));
	if (::bind(s, (sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
		errexit("can't bind to port %s: %d\n", port,
			WSAGetLastError());
}

void SSocket::printIP()
{
	const auto &s = addr.sin_addr.S_un.S_un_b;
	printf("客户端IP地址 %d.%d.%d.%d\n", s.s_b1, s.s_b2, s.s_b3, s.s_b4);
	printf("客户端端口号 %d\n", htons(addr.sin_port));
}

void SSocket::errexit(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	WSACleanup();
	exit(1);
}



void STCP::listen()
{
	if (::listen(s, 2) == SOCKET_ERROR)
		errexit("listen error: %d\n", WSAGetLastError());
}

/* 连接会话 */
STCP STCP::accept()
{
	sockaddr_in addr;
	int alen = sizeof(addr);
	SOCKET newsocket = ::accept(s, (sockaddr *)&addr, &alen);
	if (newsocket == INVALID_SOCKET)
	{
		fprintf(stderr, "accept error: %d\n", WSAGetLastError());
	}
	STCP ns(newsocket);
	ns.addr = addr;
	return ns;
}


string STCP::recv()
{
	string rec;
	constexpr int buflen = 1023;
	char buf[buflen + 1];
	int n = ::recv(s, buf, buflen, 0);
	if (n != SOCKET_ERROR && n >= 0)
	{
		cout << "\n接收到的报文:\n";
		buf[n] = '\0';
		puts(buf);
		//n = ::recv(s, buf, buflen, 0);
	}
	if (n == SOCKET_ERROR)
	{
		fprintf(stderr, "recv failed: recv() error %d\n", WSAGetLastError());
	}
	return buf;
}

void STCP::send(const string & msg)
{
	stringstream str(msg);
	string method, url, protocal;
	str >> method >> url >> protocal;
	string path;
	string suffix;
	string content_type;
	ifstream in;
	string status;

	long long filesize = 0;
	if (method == "GET")
	{
		path = root + url;
		size_t pos = url.rfind('.');
		if (pos != string::npos)
			suffix = url.substr(pos);
		else
		{
			suffix = "html";
			path = root + "/index.html";
		}//默认访问index.html

		if (suffix == "html")
			content_type = "text/html";
		else if (suffix == "jpg")
			content_type = "image/jpg";
		else if (suffix == "png")
			content_type = "image/png";
		else if (suffix == "ico")
		{
			content_type = "x-ico";
			path = root + "/google.ico";
		}
		else if (suffix == "m4a")
			content_type = "m4a";

		in.open(path.c_str(), ios_base::in | ios_base::binary);
		cout << "GET 目标文件: " << path << endl;
		if (in.fail())
		{
			cout << "文件未找到 返回404 Not Found" << endl;
			status = "HTTP/1.1 404 Not Found\n";
			//in.open((root + "/404.html").c_str(),
			//ios_base::in | ios_base::binary);
		}
		else
		{
			cout << "文件成功找到 返回200 OK" << endl;
			status = "HTTP/1.1 200 OK\n";
			filesize = in.seekg(0, ios_base::end).tellg();
		}
	}
	cout << status << endl;

	//得到response header
	string rh;
	rh += status;
	rh += ("Content-Type: " + content_type + "\n");
	rh += ("Content-Length: " + to_string(filesize) + "\n");
	rh += "Connection: keep-alive\n";
	rh += "Server: NINJA\n";
	rh += "Cache-control: no-cache\n\n";


	//发送头部
	if (::send(s, rh.c_str(), rh.size(), 0) == SOCKET_ERROR)
	{
		fprintf(stderr, "cannot send the message: %d\n", WSAGetLastError());
	}


	//分段发送对象
	if (in.good())
	{
		in.seekg(0, ios_base::beg);
		constexpr int buflen = 1024 * 4;
		char buffer[buflen];

		while (filesize > 0)
		{
			in.read(buffer, buflen);
			int extract_num = static_cast<int>(in.gcount());
			if (::send(s, buffer, extract_num, 0) == SOCKET_ERROR)
			{
				fprintf(stderr, "Send buffer Failed\n");
			}

			filesize -= extract_num;
		}
	}
	in.close();
	//cout << rh << endl;
	return;
}
