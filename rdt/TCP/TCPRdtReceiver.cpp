//#include "..\\stdafx.h"
//#include "..\\Global.h"
//#include "TCPRdtReceiver.h"
//
//TCPRdtReceiver::TCPRdtReceiver()
//{
//	lastAckPkt.acknum = -1;//初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接收的数据包出错时该确认报文的确认号为-1
//	lastAckPkt.checksum = 0;
//	lastAckPkt.seqnum = -1;	//忽略该字段
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
//		pUtils->printPacket("接收方收到未损报文", packet);
//		if (base + N <= seqnum && (packet.seqnum >= base || packet.seqnum < base + N)
//			|| base + N > seqnum && (packet.seqnum >= base || packet.seqnum < (base + N) % seqnum))
//		{
//			pUtils->printPacket("发送方的报文在窗口内", packet);
//			sndpkt[packet.seqnum].second = true;
//			sndpkt[packet.seqnum].first = std::make_shared<Packet>(packet);//缓存packet
//			Message msg;
//			for (; sndpkt[base].second; base = (base + 1) % seqnum)
//			{//递交给上层应用层
//				memcpy(msg.data, sndpkt[base].first->payload, sizeof(packet.payload));
//				pns->delivertoAppLayer(RECEIVER, msg);
//				sndpkt[base].second = false;
//			}
//
//		}
//	}
//	else
//	{
//		pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
//	}
//	lastAckPkt.acknum = (base + seqnum - 1) % seqnum;
//	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
//	pUtils->printPacket("接收方发送确认报文", lastAckPkt);
//	pns->sendToNetworkLayer(SENDER, lastAckPkt);
//}



#include "..\\stdafx.h"
#include "..\\Global.h"
#include "TCPRdtReceiver.h"

TCPRdtReceiver::TCPRdtReceiver()
{
	lastAckPkt.acknum = -1;//初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接收的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
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
		pUtils->printPacket("接收方正确收到发送方的报文", packet);

		//取出Message, 向上递交给应用层
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);

		lastAckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("接收方发送确认报文", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方

		expecktedseqnum = (expecktedseqnum + 1) % TCPRdtSender::seqnum;

	}
	else
	{
		if (checkSum != packet.checksum) {
			pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
		}
		else {
			pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
		}
		pUtils->printPacket("接收方重新发送上次的确认报文", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送上次的确认报文
	}
}

