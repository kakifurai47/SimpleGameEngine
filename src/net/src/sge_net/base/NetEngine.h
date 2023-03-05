#pragma once

#include "NESocket.h"

namespace sge
{


	class NetEngine : public NonCopyable 
	{
	public:


		void listen(const SockAddr& addr, int backlog = 64);
		void listen(StrView hostname, u16 port, int backlog = 64);

		void update();

		virtual void onRecvLine(Socket& client, ByteSpan data) {}

	private:
		Socket m_listenSock;


		Vector<UPtr<Socket>> m_sockets;
		Vector<u8> m_recvBuf;

		u8* m_recvPtr = nullptr;
	};

}