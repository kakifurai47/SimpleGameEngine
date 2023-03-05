#include "SMTPServer.h"

namespace sge
{


	void SMTPServer::run()
	{
		m_listenSock.createTCP();
		m_listenSock.setNonBlocking(true);

		SockAddr addr;
		addr.setIPv4(0, 0, 0, 0);
		addr.setPort(25);

		m_listenSock.bind(addr);
		m_listenSock.listen();
	}

	void SMTPServer::removeClosedClients() 
	{
		for (int i = 0; i < m_clients.size(); ) {
			
			auto& c = m_clients[i];

			if (c && c->sock.isValid()) {
				i++;
				continue;
			}

			swap(c, m_clients.back());
			m_clients.resize(m_clients.size() - 1);
		}
	}

}