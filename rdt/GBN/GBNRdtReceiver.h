#pragma once
#include "..\\RdtReceiver.h"
#include "GBNRdtSender.h"
class GBNRdtReceiver :public RdtReceiver
{
private:
	int expecktedseqnum = 0;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

public:

	void receive(Packet &packet);	//���ձ��ģ�����NetworkService����
};
