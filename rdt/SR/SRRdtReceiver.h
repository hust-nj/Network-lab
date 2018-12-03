#pragma once
#include "..\\RdtReceiver.h"
#include "SRRdtSender.h"
#include<memory>
class SRRdtReceiver :public RdtReceiver
{
private:
	int base = 0; //窗口开始序列号
	std::pair<std::shared_ptr<Packet>, bool> sndpkt[SRRdtSender::seqnum]; //接收窗口
	//(接收序列号, 是否确认)
	Packet lastAckPkt;
public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:

	void receive(Packet &packet);	//接收报文，将被NetworkService调用
};
