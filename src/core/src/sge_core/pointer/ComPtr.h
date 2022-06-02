#pragma once

namespace sge {

	template<class T>
	class ComPtr : NonCopyable {
	public:
		ComPtr() = default;

		ComPtr(const ComPtr& r) { reset(r.m_p); }
		ComPtr(ComPtr&& r)	    { m_p = r.m_p; r.m_p = nullptr; }

		void operator=(const ComPtr& r) { reset(m_p); }
		void operator=(ComPtr&& r)		{ reset(nullptr); m_p = r.m_p; r.m_p = nullptr; }

		~ComPtr() noexcept { reset(nullptr); }

			  T* operator->()		noexcept { return m_p; }
		const T* operator->() const noexcept { return m_p; }

		operator	   T* ()	   noexcept { return m_p; }
		operator const T* () const noexcept { return m_p; }

				T* ptr()		 noexcept { return m_p; }
		const	T* ptr() const	 noexcept { return m_p; }

		void reset(T* p) {
			if (p == m_p) return;
			if (m_p) {
				m_p->Release();
			}
			m_p = p;
			if (m_p) {
				m_p->AddRef();
			}
		}

		T** ptrForInit() noexcept { reset(nullptr); return &m_p; }
		T*  detach()	 noexcept { T* o = m_p;  m_p = nullptr;  return o; }
	private:
		T* m_p = nullptr;
	};


}
