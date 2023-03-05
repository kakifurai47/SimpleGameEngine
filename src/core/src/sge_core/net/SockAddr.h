#pragma once

namespace sge
{

	enum class SockAddrFamily 
	{
		Unknown = AF_UNSPEC,
		IPv4	= AF_INET,
		IPv6	= AF_INET6,
	};

	class IPv4 {
	public:
		using Addr = decltype(sockaddr_in::sin_addr);
		static constexpr size_t	kAddrBufSize = 4;

		IPv4() { m_data32[0] = 0; }
		IPv4(u8 v0, u8 v1, u8 v2, u8 v3);

		void	resolve		(StrView hostname);
		bool	tryResolve	(StrView hostname);

		union {
			Addr	m_addr;
			u8		m_data[kAddrBufSize];
			u32		m_data32[1];
		};
	};

	class SockAddr {

	public:
		using Family = SockAddrFamily;

		void reset() {
			setFamily(Family::Unknown);
		}

	#if SGE_OS_WINDOWS
		using SockLen = int;
	#else
		using SockLen = socklen_t;
	#endif

		IPv4	ipv4	() const;
		u16		port	() const;

		Family	family	() const;
		void	setFamily(Family f);

		void setIPv4	(u8 v0, u8 v1, u8 v2, u8 v3);
		void setPort	(u16 port);

		void resolve	(StrView hostname, u16 port, Family family = Family::IPv4);
		bool tryResolve	(StrView hostname, u16 port, Family family = Family::IPv4);

		static constexpr size_t kAddrBufSize = 32;

	friend class Socket;
	protected:

		bool _setByAddrInfo(const addrinfo& v, u16 port);

		union {
			sockaddr		m_addr;
			sockaddr_in		m_addr_in;
			sockaddr_in6	m_addr_in6;
			u8				m_data[kAddrBufSize];
		};
	};


}