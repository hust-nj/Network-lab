#pragma once
#include "..\\RdtSender.h"
#include<memory>
class GBNRdtSender :public RdtSender
{
private:
	int base = 0;	//窗口开始序列号
	int nextseqnum = 0;//下一个要发送的序列号
	const static int N = 100;//窗口大小
	const static int seqnum = N + 1;//序号个数
	std::shared_ptr<Packet> sndpkt[seqnum];//窗口

public:

	bool getWaitingState();
	bool send(Message &message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(Packet &ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用

public:
	GBNRdtSender();
	virtual ~GBNRdtSender();
	
	friend class GBNRdtReceiver;
};
