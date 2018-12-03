//#include "..\\stdafx.h"
//#include "..\\Global.h"
//#include "TCPRdtReceiver.h"
//
//TCPRdtReceiver::TCPRdtReceiver()
//{
//	lastAckPkt.acknum = -1;//��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����յ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
//	lastAckPkt.checksum = 0;
//	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
//	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
//		lastAckPkt.payload[i] = '.';
//	}
//	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
//}
//
//TCPRdtReceiver::~TCPRdtReceiver() {}
//
//void TCPRdtReceiver::receive(Packet &packet)
//{
//	int N = TCPRdtSender::N;
//	int seqnum = TCPRdtSender::seqnum;
//	int checkSum = pUtils->calculateCheckSum(packet);
//
//	if (checkSum == packet.checksum)//not corrupt
//	{
//		pUtils->printPacket("���շ��յ�δ����", packet);
//		if (base + N <= seqnum && (packet.seqnum >= base || packet.seqnum < base + N)
//			|| base + N > seqnum && (packet.seqnum >= base || packet.seqnum < (base + N) % seqnum))
//		{
//			pUtils->printPacket("���ͷ��ı����ڴ�����", packet);
//			sndpkt[packet.seqnum].second = true;
//			sndpkt[packet.seqnum].first = std::make_shared<Packet>(packet);//����packet
//			Message msg;
//			for (; sndpkt[base].second; base = (base + 1) % seqnum)
//			{//�ݽ����ϲ�Ӧ�ò�
//				memcpy(msg.data, sndpkt[base].first->payload, sizeof(packet.payload));
//				pns->delivertoAppLayer(RECEIVER, msg);
//				sndpkt[base].second = false;
//			}
//
//		}
//	}
//	else
//	{
//		pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
//	}
//	lastAckPkt.acknum = (base + seqnum - 1) % seqnum;
//	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
//	pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
//	pns->sendToNetworkLayer(SENDER, lastAckPkt);
//}



#include "..\\stdafx.h"
#include "..\\Global.h"
#include "TCPRdtReceiver.h"

TCPRdtReceiver::TCPRdtReceiver()
{
	lastAckPkt.acknum = -1;//��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����յ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

TCPRdtReceiver::~TCPRdtReceiver() {}

void TCPRdtReceiver::receive(Packet &packet)
{
	int checkSum = pUtils->calculateCheckSum(packet);

	if (checkSum == packet.checksum && expecktedseqnum == packet.seqnum)
	{
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);

		//ȡ��Message, ���ϵݽ���Ӧ�ò�
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);

		lastAckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�

		expecktedseqnum = (expecktedseqnum + 1) % TCPRdtSender::seqnum;

	}
	else
	{
		if (checkSum != packet.checksum) {
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
		}
		else {
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,������Ų���", packet);
		}
		pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢���ϴε�ȷ�ϱ���
	}
}

