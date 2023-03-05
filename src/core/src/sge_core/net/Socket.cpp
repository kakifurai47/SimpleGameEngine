#include "Socket.h"

namespace sge 
{
	Socket::PlatformInit::PlatformInit() 
	{
	#if SGE_OS_WINDOWS
		WSADATA wsad;
		if (::WSAStartup(WINSOCK_VERSION, &wsad) != 0)
			throw SGE_ERROR("WSAStartup");
	#endif
	}

	Socket::PlatformInit::~PlatformInit() {
	#if SGE_OS_WINDOWS
		if (::WSACleanup() != 0) {
			SGE_ASSERT(false);
		}
	#endif
	}

	void Socket::platformInit() {
		static PlatformInit s;
	}

	void Socket::createUDP() {
		close();
		platformInit();

		m_sock = ::socket(AF_INET, SOCK_DGRAM, 0);

		if (m_sock == kInvalidHandle) {
			_throwError("createUDP");
		}
	}

	void Socket::createTCP() {
		close();
		platformInit();

		m_sock = ::socket(AF_INET, SOCK_STREAM, 0);
		if (m_sock == kInvalidHandle) {
			_throwError("createTCP");
		}
	}

	void Socket::setNonBlocking(bool b) {
	#if SGE_OS_WINDOWS
		u_long v = b ? 1 : 0;

		int ret = ::ioctlsocket(m_sock, FIONBIO, &v);
		if (ret != 0)
			_throwError("setNonBlocking");
	#elif
		int v = b ? 1 : 0;
		int ret = ::ioctl(m_sock, FIONBIO, &v);
		if (ret < 0)
			_throwError("setNonBlocking");
	#endif
	}

	void Socket::bind(StrView hostname, u16 port) {
		SockAddr addr;
		addr.resolve(hostname, port);
		bind(addr);
	}

	void Socket::bind(const SockAddr& addr) {
		if (!isValid()) return;

		int ret = ::bind(m_sock, &addr.m_addr, sizeof(addr.m_addr));
		if (ret != 0) {
			_throwError("bind");
		}
	}

	void Socket::listen(int backlog) {
		if (!isValid()) return;

		int ret = ::listen(m_sock, backlog);
		if (ret != 0) {
			_throwError("listen");
		}
	}

	bool Socket::connect(const SockAddr& addr)
	{
		if (!isValid()) return false;

		int ret = ::connect(m_sock, &addr.m_addr, sizeof(addr.m_addr));
		if (ret < 0) {
		#ifdef SGE_OS_WINDOWS
			int e = ::WSAGetLastError();
			if (e == WSAEWOULDBLOCK)
				return false;
		#else
			int e = errno;
			if (e == EINPROGRESS)
				return false;
		#endif
			_throwError("connect");
		}

		return true;
	}

	bool Socket::connect(StrView hostname, u16 port) {
		SockAddr addr;
		addr.resolve(hostname, port);
		return connect(addr);
	}

	bool Socket::accept(Socket& acceptedSock) {
		acceptedSock.close();

		auto c = ::accept(m_sock, nullptr, nullptr);
		if (c == kInvalidHandle) {
			return false;
		}
		acceptedSock.m_sock = c;
		return true;
	}

	size_t Socket::availableBytesToRead() {
		if (!isValid()) return 0;

	#if SGE_OS_WINDOWS
		u_long n = 0;
		if (0 != ::ioctlsocket(m_sock, FIONREAD, &n)) {
			_throwError("availableBytesToRead");
		}
	#else
		int n = 0;
		if (0 != ::ioctl(m_sock, FIONREAD, &n)) {
			_throwError("availableBytesToRead");
		}
	#endif
		return static_cast<size_t>(n);
	}

	int Socket::sendto(const SockAddr& addr, ByteSpan data) {
		if (!isValid()) return 0;

		if (data.size() > Math::max<int>())
			_throwError("send : dataSize > max value");

		int ret = ::sendto(m_sock, reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()), 0, &addr.m_addr, sizeof(addr.m_addr));
		return ret;
	}

	int Socket::send(ByteSpan data) {
		if (!isValid()) return 0;

		if (data.size() > Math::max<int>())
			_throwError("send : data.size() > max value");

		int ret = ::send(m_sock, reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()), 0);
		return ret;
	}

	int Socket::recv(u8* buf, size_t bytesToRecv, int flags)
	{
		if (!isValid()) return 0;

		if (bytesToRecv > Math::max<int>())
			bytesToRecv = Math::max<int>();

		int ret = ::recv(m_sock, reinterpret_cast<char*>(buf), static_cast<int>(bytesToRecv), flags);
		return ret;

	}

	int Socket::recvfrom(const SockAddr& addr, u8* buf, size_t bytesToRecv) {
		if (!isValid()) return 0;

		if (bytesToRecv > Math::max<int>())
			bytesToRecv = Math::max<int>();
		
		socklen_t addrLen = sizeof(addr.m_addr);
		int ret = ::recvfrom(m_sock, reinterpret_cast<char*>(buf), static_cast<int>(bytesToRecv), 0, constCast(&addr.m_addr), &addrLen);
		return ret;
	}

	int Socket::getSockError() {
	#if SGE_OS_WINDOWS
		int ret = WSAGetLastError();
	#else
		int ret = errno;
	#endif
		return ret;
	}

	void Socket::_throwError(StrView msg) {
		throw SGE_ERROR("[{}] : {}", getSockError(), msg);
	}

	void Socket::close()
	{
		if (!isValid()) return;
	#if SGE_OS_WINDOWS
		::closesocket(m_sock);
	#else
		::close(m_sock);
	#endif
		m_sock = kInvalidHandle;
	}

}