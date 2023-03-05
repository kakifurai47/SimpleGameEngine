#include "SockAddr.h"
#include "Socket.h"

namespace sge 
{
	IPv4::IPv4(u8 v0, u8 v1, u8 v2, u8 v3) {
		m_data[0] = v0; m_data[1] = v1; m_data[2] = v2; m_data[3] = v3;
	}

	void IPv4::resolve(StrView hostname) {
		if (!tryResolve(hostname))
			throw SGE_ERROR("resolve");
	}

	bool IPv4::tryResolve(StrView hostname) {
		SockAddr addr;
		if (!addr.tryResolve(hostname, 0, SockAddrFamily::IPv4))
			return false;
		*this = addr.ipv4();
		return false;
	}

	IPv4 SockAddr::ipv4() const {
		if (family() != Family::IPv4)
			return IPv4();

		static_assert(sizeof(IPv4) == sizeof(m_addr_in.sin_addr));
		return *reinterpret_cast<const IPv4*>(&m_addr_in.sin_addr);
	}

	u16 SockAddr::port() const {
		switch(family()) {
			case Family::IPv4: return ntohs(m_addr_in.sin_port);
			case Family::IPv6: return ntohs(m_addr_in6.sin6_port);
			default: throw SGE_ERROR("");
		}
	}

	SockAddr::Family SockAddr::family() const {
		switch(m_addr.sa_family) {
			case AF_INET:	return Family::IPv4;
			case AF_INET6:	return Family::IPv4;
			default:		return Family::Unknown;
		}
	}

	void SockAddr::setFamily(Family f) {
		m_addr.sa_family = static_cast<u16>(f);
	}

	void SockAddr::setIPv4(u8 v0, u8 v1, u8 v2, u8 v3) {
		Socket::platformInit();
		reset();

		setFamily(Family::IPv4);

		m_addr.sa_data[2] = v0;
		m_addr.sa_data[3] = v1;
		m_addr.sa_data[4] = v2;
		m_addr.sa_data[5] = v3;
	}

	void SockAddr::setPort(u16 port) {
		switch(family()) {
			case Family::IPv4: { m_addr_in.sin_port		= htons(port); } break;
			case Family::IPv6: { m_addr_in6.sin6_port	= htons(port); } break;
			default: throw SGE_ERROR("");
		}
	}

	void SockAddr::resolve(StrView hostname, u16 port, Family family) {
		if (!tryResolve(hostname, port, family))
			throw SGE_ERROR("resolve hostname");
	}

	bool SockAddr::tryResolve(StrView hostname, u16 port, Family family) {
		Socket::platformInit();
		reset();

		TempString hostnameStr(hostname);

		addrinfo hints {};
		hints.ai_family = enumInt(family);

		addrinfo* result = nullptr;

		if (::getaddrinfo(hostnameStr.c_str(), nullptr, &hints, &result) != 0) {
			return false;
		}

		auto ret = _setByAddrInfo(*result, port);
		::freeaddrinfo(result);
		return ret;
	}

	bool SockAddr::_setByAddrInfo(const addrinfo& v, u16 port) {
		setFamily(Family::Unknown);
		switch (v.ai_family) {
			case AF_INET: {
				m_addr_in = *reinterpret_cast<const sockaddr_in*>(v.ai_addr);
				m_addr_in.sin_port = htons(port);
				setFamily(Family::IPv4);
			} break;

			case AF_INET6: {
				m_addr_in6 = *reinterpret_cast<const sockaddr_in6*>(v.ai_addr);
				m_addr_in6.sin6_port = htons(port);
				setFamily(Family::IPv6);
			}break;

			default:
				return false;
		}
		return true;
	}
}