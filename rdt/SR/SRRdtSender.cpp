#include "..\\stdafx.h"
#include "..\\Global.h"
#include "SRRdtSender.h"

SRRdtSender::SRRdtSender() {}
SRRdtSender::~SRRdtSender() {}

bool SRRdtSender::send(Message &message)
{
	if (getWaitingState())
		return false;

	sndpkt[nextseqnum].first = std::make_shared<Packet>();
	sndpkt[nextseqnum].second = false;//还未被确认
	auto p = sndpkt[nextseqnum].first;
	p->acknum = -1;//忽略该字段
	p->seqnum = nextseqnum;
	p->checksum = 0;
	memcpy(p->payload, message.data, sizeof(message.data));
	p->checksum = pUtils->calculateCheckSum(*p);
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);
	pUtils->printPacket("发送方发送报文", *p);
	pns->sendToNetworkLayer(RECEIVER, *p);
	nextseqnum = (nextseqnum + 1) % seqnum;


	//打印窗口中的包
	printf("\n-----------------------------------------------\n");
	printf("窗口中的包为:\n");
	for (int i = base; i != nextseqnum; i = (1 + i) % seqnum)
		pUtils->printPacket("", *sndpkt[i].first);
	printf("------------------------------------------------\n\n");


	return true;
}

void SRRdtSender::receive(Packet &ackPkt)
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	if (checkSum == ackPkt.checksum)//not corrupt
	{
		pUtils->printPacket("发送方正确收到确认", ackPkt);
		if (sndpkt[ackPkt.acknum].second == false)
		{
			sndpkt[ackPkt.acknum].second = true;//确认
			pns->stopTimer(SENDER, ackPkt.acknum);
			while (base != nextseqnum && sndpkt[base].second)
			{//向前移动base
				base = (base + 1) % seqnum;
			}

			//打印窗口中的包
			printf("\n-----------------------------------------------\n");
			printf("窗口中的包为:\n");
			for (int i = base; i != nextseqnum; i = (1 + i) % seqnum)
				pUtils->printPacket("", *sndpkt[i].first);
			printf("------------------------------------------------\n\n");

		}
	}
}

void SRRdtSender::timeoutHandler(int num)
{
	pns->stopTimer(SENDER, num);
	pUtils->printPacket("发送方定时器时间到， 重发窗口内分组", *sndpkt[num].first);
	pns->startTimer(SENDER, Configuration::TIME_OUT, num);
	pns->sendToNetworkLayer(RECEIVER, *sndpkt[num].first);
}


bool SRRdtSender::getWaitingState()
{
	return nextseqnum == (base + N) % seqnum;
}