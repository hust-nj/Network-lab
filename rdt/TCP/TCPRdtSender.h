#pragma once
#include "..\\RdtSender.h"
#include<memory>
class TCPRdtSender :public RdtSender
{
private:
	int base = 0;	//���ڿ�ʼ���к�
	int nextseqnum = 0;//��һ��Ҫ���͵����к�
	const static int N = 4;//���ڴ�С
	const static int seqnum = N * 100;//��Ÿ���
	std::shared_ptr<Packet> sndpkt[seqnum];//����
	int cnt = 0;//������

public:

	bool getWaitingState();
	bool send(Message &message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(Packet &ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����

public:
	TCPRdtSender();
	virtual ~TCPRdtSender();

	friend class TCPRdtReceiver;
};
