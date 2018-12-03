#include "..\\stdafx.h"
#include "..\\Global.h"
#include "TCPRdtSender.h"

TCPRdtSender::TCPRdtSender() {}
TCPRdtSender::~TCPRdtSender() {}

bool TCPRdtSender::send(Message &message)
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

void TCPRdtSender::receive(Packet &ackPkt)
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	if (checkSum == ackPkt.checksum)//not corrupt
	{
		
		int tmp = ackPkt.acknum;//由于累积确认

		if (base + N <= seqnum && (tmp >= base && tmp < base + N)
			|| base + N > seqnum && (tmp >= base || tmp < (base + N) % seqnum))
		{//如果在窗口内
			cnt = 0;
			base = (tmp + 1) % seqnum;//移动窗口
		}
		else//否则计数
			cnt++;

		printf("\ncnt = %d\n", cnt);
		printf("base = %d\n", base);
		if (cnt == 3)
		{//快速重传
			cnt = 0;
			pns->stopTimer(SENDER, 0);
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
			printf("发送方开始快速重传\n");
			pUtils->printPacket("", *sndpkt[base]);
			pns->sendToNetworkLayer(RECEIVER, *sndpkt[base]);
		}
		else if(cnt == 0)
		{
			pUtils->printPacket("发送方正确收到确认", ackPkt);
			pns->stopTimer(SENDER, 0);
			if (base != nextseqnum)//如果窗口非空，继续计时
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);

			//打印窗口中的包
			printf("\n-----------------------------------------------\n");
			printf("窗口中的包为:\n");
			for (int i = base; i != nextseqnum; i = (1 + i) % seqnum)
				pUtils->printPacket("", *sndpkt[i]);
			printf("------------------------------------------------\n\n");

		}
		else
		{ }
	}
	else
	{
		pUtils->printPacket("发送方没有正确收到确认", ackPkt);
	}
}

void TCPRdtSender::timeoutHandler(int)
{
	printf("发送方定时器时间到，重发base分组");
	pns->stopTimer(SENDER, 0);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	pUtils->printPacket("", *sndpkt[base]);
	pns->sendToNetworkLayer(RECEIVER, *sndpkt[base]);
}

bool TCPRdtSender::getWaitingState()
{
	return nextseqnum == (base + N - 1) % seqnum;//窗口已满
}


