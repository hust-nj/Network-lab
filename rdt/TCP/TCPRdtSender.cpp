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

void TCPRdtSender::receive(Packet &ackPkt)
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);

	if (checkSum == ackPkt.checksum)//not corrupt
	{
		
		int tmp = ackPkt.acknum;//�����ۻ�ȷ��

		if (base + N <= seqnum && (tmp >= base && tmp < base + N)
			|| base + N > seqnum && (tmp >= base || tmp < (base + N) % seqnum))
		{//����ڴ�����
			cnt = 0;
			base = (tmp + 1) % seqnum;//�ƶ�����
		}
		else//�������
			cnt++;

		printf("\ncnt = %d\n", cnt);
		printf("base = %d\n", base);
		if (cnt == 3)
		{//�����ش�
			cnt = 0;
			pns->stopTimer(SENDER, 0);
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
			printf("���ͷ���ʼ�����ش�\n");
			pUtils->printPacket("", *sndpkt[base]);
			pns->sendToNetworkLayer(RECEIVER, *sndpkt[base]);
		}
		else if(cnt == 0)
		{
			pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
			pns->stopTimer(SENDER, 0);
			if (base != nextseqnum)//������ڷǿգ�������ʱ
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);

			//��ӡ�����еİ�
			printf("\n-----------------------------------------------\n");
			printf("�����еİ�Ϊ:\n");
			for (int i = base; i != nextseqnum; i = (1 + i) % seqnum)
				pUtils->printPacket("", *sndpkt[i]);
			printf("------------------------------------------------\n\n");

		}
		else
		{ }
	}
	else
	{
		pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
	}
}

void TCPRdtSender::timeoutHandler(int)
{
	printf("���ͷ���ʱ��ʱ�䵽���ط�base����");
	pns->stopTimer(SENDER, 0);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	pUtils->printPacket("", *sndpkt[base]);
	pns->sendToNetworkLayer(RECEIVER, *sndpkt[base]);
}

bool TCPRdtSender::getWaitingState()
{
	return nextseqnum == (base + N - 1) % seqnum;//��������
}


