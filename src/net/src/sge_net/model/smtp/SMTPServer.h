#pragma once

#include <sge_net.h>
#include "SMTPClient.h"

namespace sge
{





	class SMTPServer : public NonCopyable {
	public:

		static const size_t kMaxClientCount = 64;

		void run();
		void removeClosedClients();
		

	private:
		Socket	m_listenSock;
		Vector<UPtr<SMTPClient>> m_clients;
	};


}