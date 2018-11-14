#include "server.h"

string root;

/* 用于相应线程 */
void sub_thread(void *t)
{
	printf("\n----------------------当前子线程号: %d--------------------\n",
		GetCurrentThreadId());
	STCP &s = *(STCP *)t;
	s.printIP();
	string request = s.recv();
	s.send(request);
	s.close();
	printf("\n------------------------------------------------\n子线程%d结束\n\n",
		GetCurrentThreadId());
	delete &s;
	_endthread();
}


/* 用于检测键盘按键线程 */
void esc(void *)
{
	while (1)
	{
		if (GetKeyState(0x1B) & 0x8000)
		{
			cout << "成功关闭服务器" << endl;
			exit(0);
		}
	}
}


int main(int argc, char *argv[])
{
	try{
		if (_beginthread(esc, 0, 0) == -1L)
			throw runtime_error("create thread error");

		cout << "服务器启动，按esc关闭服务器" << endl;
		root = _getcwd(0, 0);
		cout << "请输入主目录: ";
		string main_root;
		cin >> main_root;
		root += main_root;
		if (root.empty())
		{
			perror("_getcwd error");
			exit(1);
		}
		STCP main_thread;
		string port_num;
		cout << "请输入待配置的端口号  ";
		cin >> port_num;
		main_thread.bind(port_num.c_str());
		main_thread.listen();
		int cnt = 0;
		queue<HANDLE> hdq;

		while (1)
		{
			STCP *p = new STCP(main_thread.accept());
			long int n = 0;
			if ((n = _beginthread(sub_thread, 0, (void*)p)) == -1L)
				cerr << "Create thread error" << endl;
			else
			{
				hdq.push((HANDLE)n);
				cnt++;
			}
			if (cnt-- == 100)
				hdq.pop();
		}
	}
	catch (runtime_error err)
	{
		cout << err.what() << endl;
	}
	//catch (exception)
	//{
	//	cout << "成功关闭服务器...\n" << endl;
	//}
	//for (auto i = hdq.front(); !hdq.empty(); hdq.pop())//等待最近100个进程
	//	WaitForSingleObject(i, INFINITE);
	return 0;
}
