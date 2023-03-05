#pragma once

#include "SockAddr.h"

namespace sge {


	class Socket : public NonCopyable 
	{
	public:

	#ifdef SGE_OS_WINDOWS
		using NativeHandle = SOCKET;
		static const NativeHandle kInvalidHandle = INVALID_SOCKET;
	#else
		using NativeHandle = int;
		static const NativeHandle kInvalidHandle = -1;
	#endif

		static void platformInit();

		~Socket() { close(); }

		bool isValid() const { return m_sock != kInvalidHandle; }


		void createUDP();
		void createTCP();

		void setNonBlocking(bool b);

		void close();

		NativeHandle sock() { return m_sock; }

		void bind(StrView hostname, u16 port);
		void bind(const SockAddr& addr);

		void listen(int backlog = 64);

		bool connect(const SockAddr& addr);
		bool connect(StrView hostname, u16 port);

		bool accept(Socket& acceptedSock);

		size_t availableBytesToRead();

		int sendto(const SockAddr& addr, ByteSpan data);
		int sendto(const SockAddr& addr, StrView  data) { return sendto(addr, ByteSpan(reinterpret_cast<const u8*>(data.data()), data.size())); }

		int send(ByteSpan data);
		int send(StrView  data) { return send(ByteSpan(reinterpret_cast<const u8*>(data.data()), data.size())); }

		int recv(u8* buf, size_t bytesToRecv, int flags);
		int recvfrom(const SockAddr& addr, u8* buf, size_t bytesToRecv);

		template<class CONTAINER>
		int recv(CONTAINER& buf, size_t bytesToRecv, int flags);
		
		template<class CONTAINER>
		int appendRecv(CONTAINER& buf, size_t bytesToRecv, int flags);

		int getSockError();


	private:

		void _throwError(StrView msg);


		NativeHandle m_sock = kInvalidHandle;


		class PlatformInit {
		public:
			 PlatformInit();
			~PlatformInit();
		};



	};

	template<class CONTAINER>
	int Socket::recv(CONTAINER& buf, size_t bytesToRecv, int flags) {
		buf.clear();
		return appendRecv(buf, bytesToRecv, flags);
	}


	template<class CONTAINER>
	int Socket::appendRecv(CONTAINER& buf, size_t bytesToRecv, int flags) {
		auto oldSize = buf.size();
		auto newSize = oldSize + bytesToRecv;
	
		if (newSize < oldSize)
			throw SGE_ERROR("appendRecv : newSize < oldSize");
	
		buf.resize(newSize);
		int ret = 0;
	
		try {
			ret = recv(reinterpret_cast<u8*>(&buf[oldSize]), bytesToRecv, flags);
			if (ret < 0) {
				buf.resize(oldSize);
				return ret;
			}
		}
		catch(...) {
			buf.resize(oldSize);
			throw;
		}
	
		buf.resize(oldSize + ret);
		return ret;
	}
}