#include "server.h"

string root;

/* ������Ӧ�߳� */
void sub_thread(void *t)
{
	printf("\n----------------------��ǰ���̺߳�: %d--------------------\n",
		GetCurrentThreadId());
	STCP &s = *(STCP *)t;
	s.printIP();
	string request = s.recv();
	s.send(request);
	s.close();
	printf("\n------------------------------------------------\n���߳�%d����\n\n",
		GetCurrentThreadId());
	delete &s;
	_endthread();
}


/* ���ڼ����̰����߳� */
void esc(void *)
{
	while (1)
	{
		if (GetKeyState(0x1B) & 0x8000)
		{
			cout << "�ɹ��رշ�����" << endl;
			exit(0);
		}
	}
}


int main(int argc, char *argv[])
{
	try{
		if (_beginthread(esc, 0, 0) == -1L)
			throw runtime_error("create thread error");

		cout << "��������������esc�رշ�����" << endl;
		root = _getcwd(0, 0);
		cout << "��������Ŀ¼: ";
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
		cout << "����������õĶ˿ں�  ";
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
	//	cout << "�ɹ��رշ�����...\n" << endl;
	//}
	//for (auto i = hdq.front(); !hdq.empty(); hdq.pop())//�ȴ����100������
	//	WaitForSingleObject(i, INFINITE);
	return 0;
}
