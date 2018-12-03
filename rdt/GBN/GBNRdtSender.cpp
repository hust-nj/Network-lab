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
	p->acknum = -1;//���Ը��ֶ�
	p->seqnum = nextseqnum;
	p->checksum = 0;
	memcpy(p->payload, message.data, sizeof(message.data));
	p->checksum = pUtils->calculateCheckSum(*p);
	if (base == nextseqnum)//��������Ǵ��ڵĿ�ʼ
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	pUtils->printPacket("���ͷ����ͱ���", *p);
	pns->sendToNetworkLayer(RECEIVER, *p);
	nextseqnum = (nextseqnum + 1) % seqnum;

	//��ӡ�����еİ�
	printf("\n-----------------------------------------------\n");
	printf("�����еİ�Ϊ:\n");
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
		base = (ackPkt.acknum + 1) % seqnum;//�����ۻ�ȷ��
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		pns->stopTimer(SENDER, 0);
		if (base != nextseqnum)//������ڷǿգ�������ʱ
			pns->startTimer(SENDER, Configuration::TIME_OUT,0);

		//��ӡ�����еİ�
		printf("\n-----------------------------------------------\n");
		printf("�����еİ�Ϊ:\n");
		for (int i = base; i != nextseqnum; i = (1 + i) % seqnum)
			pUtils->printPacket("", *sndpkt[i]);
		printf("------------------------------------------------\n\n");

	}
	else
	{
		pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
	}
}

void GBNRdtSender::timeoutHandler(int)
{
	printf("���ͷ���ʱ��ʱ�䵽���ط������ڷ���");
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
	return nextseqnum == (base - 1) % seqnum;//��������
}