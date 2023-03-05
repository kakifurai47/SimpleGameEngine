#include "NetEngine.h"

namespace sge
{

	void NetEngine::listen(const SockAddr& addr, int backlog) {
		try {
			m_listenSock.createTCP();
			m_listenSock.bind(addr);
			m_listenSock.listen(backlog);
		}
		catch(...) {
			m_listenSock.close();
		}
	}

	void NetEngine::listen(StrView hostname, u16 port, int backlog) {
		SockAddr addr;
		addr.tryResolve(hostname, port);
		listen(addr, backlog);
	}

	void NetEngine::update() {

		auto& s = m_sockets.emplace_back(new Socket());
		m_listenSock.accept (*s);
		SGE_DUMP_VAR("accepted");

		for(;;) {

			size_t n = s->availableBytesToRead();
			if (n <= 0) {
				sge_sleep(0);
				continue;
			}

			n = s->appendRecv(m_recvBuf, n, 0);

			SGE_LOG("engine recv : [{}]", StrView_make(m_recvBuf));

			auto  recvStr = StrView_make(m_recvBuf.subspan(m_recvBuf.size() - n));
			auto* endline = StringUtil::findStr(recvStr, "\r\n");

			if (!endline)
			{
				SGE_LOG("!endline");
				continue;
			}

			auto* start = m_recvBuf.begin();
			auto* end   = reinterpret_cast<const u8*>(endline);

			if (end <= start) { // only \r\n
				SGE_LOG("end <= start before	: [{}]", StrView_make(m_recvBuf));
				m_recvBuf.erase(start, start + 2);
				SGE_LOG("end <= start after		: [{}]", StrView_make(m_recvBuf));
				continue;
			}
			
			onRecvLine(*s, ByteSpan(start, end));
			m_recvBuf.erase(start, end + 2); // TODO : circular buffer
		}
	}

}