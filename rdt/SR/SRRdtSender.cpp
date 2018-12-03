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
	sndpkt[nextseqnum].second = false;//��δ��ȷ��
	auto p = sndpkt[nextseqnum].first;
	p->acknum = -1;//���Ը��ֶ�
	p->seqnum = nextseqnum;
	p->checksum = 0;
	memcpy(p->payload, message.data, sizeof(message.data));
	p->checksum = pUtils->calculateCheckSum(*p);
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);
	pUtils->printPacket("���ͷ����ͱ���", *p);
	pns->sendToNetworkLayer(RECEIVER, *p);
	nextseqnum = (nextseqnum + 1) % seqnum;


	//��ӡ�����еİ�
	printf("\n-----------------------------------------------\n");
	printf("�����еİ�Ϊ:\n");
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
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		if (sndpkt[ackPkt.acknum].second == false)
		{
			sndpkt[ackPkt.acknum].second = true;//ȷ��
			pns->stopTimer(SENDER, ackPkt.acknum);
			while (base != nextseqnum && sndpkt[base].second)
			{//��ǰ�ƶ�base
				base = (base + 1) % seqnum;
			}

			//��ӡ�����еİ�
			printf("\n-----------------------------------------------\n");
			printf("�����еİ�Ϊ:\n");
			for (int i = base; i != nextseqnum; i = (1 + i) % seqnum)
				pUtils->printPacket("", *sndpkt[i].first);
			printf("------------------------------------------------\n\n");

		}
	}
}

void SRRdtSender::timeoutHandler(int num)
{
	pns->stopTimer(SENDER, num);
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽�� �ط������ڷ���", *sndpkt[num].first);
	pns->startTimer(SENDER, Configuration::TIME_OUT, num);
	pns->sendToNetworkLayer(RECEIVER, *sndpkt[num].first);
}


bool SRRdtSender::getWaitingState()
{
	return nextseqnum == (base + N) % seqnum;
}