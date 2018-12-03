#pragma once
#include "..\\RdtSender.h"
#include<memory>
class GBNRdtSender :public RdtSender
{
private:
	int base = 0;	//���ڿ�ʼ���к�
	int nextseqnum = 0;//��һ��Ҫ���͵����к�
	const static int N = 100;//���ڴ�С
	const static int seqnum = N + 1;//��Ÿ���
	std::shared_ptr<Packet> sndpkt[seqnum];//����

public:

	bool getWaitingState();
	bool send(Message &message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(Packet &ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����

public:
	GBNRdtSender();
	virtual ~GBNRdtSender();
	
	friend class GBNRdtReceiver;
};
