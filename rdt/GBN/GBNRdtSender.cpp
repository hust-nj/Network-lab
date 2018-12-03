#include "..\\stdafx.h"
#include "..\\Global.h"
#include "GBNRdtSender.h"
GBNRdtSender::GBNRdtSender() {}
GBNRdtSender::~GBNRdtSender() {}

bool GBNRdtSender::send(Message &message)
{
	if (getWaitingState())
		return false;

	sndpkt[nextseqnum] = std::make_shared<Packet>();
	auto p = sndpkt[nextseqnum];
	p->acknum = -1;//忽略该字段
	p->seqnum = nextseqnum;
	p->checksum = 0;
	memcpy(p->payload, message.data, sizeof(message.data));
	p->checksum = pUtils->calculateCheckSum(*p);
	if (base == nextseqnum)//如果发送是窗口的开始
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	pUtils->printPacket("发送方发送报文", *p);
	pns->sendToNetworkLayer(RECEIVER, *p);
	nextseqnum = (nextseqnum + 1) % seqnum;

	//打印窗口中的包
	printf("\n-----------------------------------------------\n");
	printf("窗口中的包为:\n");
	for (int i = base; i != nextseqnum; i = (1 + i) % seqnum)
		pUtils->printPacket("", *sndpkt[i]);
	printf("------------------------------------------------\n\n");

	return true;
}

void GBNRdtSender::receive(Packet &ackPkt)
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	if (checkSum == ackPkt.checksum)//not corrupt
	{
		base = (ackPkt.acknum + 1) % seqnum;//由于累积确认
		pUtils->printPacket("发送方正确收到确认", ackPkt);
		pns->stopTimer(SENDER, 0);
		if (base != nextseqnum)//如果窗口非空，继续计时
			pns->startTimer(SENDER, Configuration::TIME_OUT,0);

		//打印窗口中的包
		printf("\n-----------------------------------------------\n");
		printf("窗口中的包为:\n");
		for (int i = base; i != nextseqnum; i = (1 + i) % seqnum)
			pUtils->printPacket("", *sndpkt[i]);
		printf("------------------------------------------------\n\n");

	}
	else
	{
		pUtils->printPacket("发送方没有正确收到确认", ackPkt);
	}
}

void GBNRdtSender::timeoutHandler(int)
{
	printf("发送方定时器时间到，重发窗口内分组");
	pns->stopTimer(SENDER, 0);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	for (int i = base; i != nextseqnum; i = (i + 1) % seqnum)
	{
		pUtils->printPacket("", *sndpkt[i]);
		pns->sendToNetworkLayer(RECEIVER, *sndpkt[i]);
	}
}

bool GBNRdtSender::getWaitingState()
{
	return nextseqnum == (base - 1) % seqnum;//窗口已满
}