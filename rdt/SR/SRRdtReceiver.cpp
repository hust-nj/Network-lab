#include "..\\stdafx.h"
#include "..\\Global.h"
#include "SRRdtReceiver.h"

SRRdtReceiver::SRRdtReceiver()
{
	lastAckPkt.acknum = -1;//��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����յ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}
SRRdtReceiver::~SRRdtReceiver() {}

void SRRdtReceiver::receive(Packet &packet)
{
	int N = SRRdtSender::N;
	int seqnum = SRRdtSender::seqnum;
	int checkSum = pUtils->calculateCheckSum(packet);

	if (checkSum == packet.checksum)//not corrupt
	{
		pUtils->printPacket("���շ��յ�δ����", packet);
		if (base + N <= seqnum && (packet.seqnum >= base || packet.seqnum < base + N)
			|| base + N > seqnum && (packet.seqnum >= base || packet.seqnum < (base + N) % seqnum))
		{
			pUtils->printPacket("���ͷ��ı����ڴ�����", packet);
			sndpkt[packet.seqnum].second = true;
			sndpkt[packet.seqnum].first = std::make_shared<Packet>(packet);//����packet
			Message msg;
			for (; sndpkt[base].second; base = (base + 1) % seqnum)
			{//�ݽ����ϲ�Ӧ�ò�
				memcpy(msg.data, sndpkt[base].first->payload, sizeof(packet.payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				sndpkt[base].second = false;
			}

		}
		
		lastAckPkt.acknum = packet.seqnum;
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);
	}
	else
	{
		pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
	}
}