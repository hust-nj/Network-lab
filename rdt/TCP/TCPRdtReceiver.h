#pragma once
#include "..\\RdtReceiver.h"
#include "TCPRdtSender.h"
class TCPRdtReceiver :public RdtReceiver
{
private:
	int expecktedseqnum = 0;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	TCPRdtReceiver();
	virtual ~TCPRdtReceiver();

public:

	void receive(Packet &packet);	//���ձ��ģ�����NetworkService����
};
