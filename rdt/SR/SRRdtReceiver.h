#pragma once
#include "..\\RdtReceiver.h"
#include "SRRdtSender.h"
#include<memory>
class SRRdtReceiver :public RdtReceiver
{
private:
	int base = 0; //���ڿ�ʼ���к�
	std::pair<std::shared_ptr<Packet>, bool> sndpkt[SRRdtSender::seqnum]; //���մ���
	//(�������к�, �Ƿ�ȷ��)
	Packet lastAckPkt;
public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:

	void receive(Packet &packet);	//���ձ��ģ�����NetworkService����
};
