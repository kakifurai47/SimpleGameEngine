#pragma once

namespace sge
{

	template<class T>
	class WPtr : public NonCopyable {
	public:

		using SPtr = WPtr<T>;

		WPtr() = default;

		WPtr(T* p)				 noexcept	 { reset(p); }
		WPtr(WPtr&& r)			 noexcept	 { m_p = r; r.m_p = nullptr; }

		void operator=(T* p)	 noexcept	 { reset(p); }
		void operator=(WPtr&& r) noexcept	 { reset(nullptr); m_p = r.m_p; r.m_p = nullptr; }

		~WPtr()					 noexcept	 { reset(nullptr); }

				T* operator->()		  noexcept { return m_p; }
		const	T* operator->() const noexcept { return m_p; }

		operator		T*() 	   noexcept { return m_p; }
		operator const  T*() const noexcept { return m_p; }

			  T* ptr()		 noexcept { return m_p; }
		const T* ptr() const noexcept { return m_p; }

			  SPtr toSPtr()		   noexcept { return m_p && m_p->m_refCount > 0 ? SPtr(m_p) : nullptr; }
		const SPtr toSPtr() const  noexcept { return m_p && m_p->m_refCount > 0 ? SPtr(m_p) : nullptr; }

		void reset(T* p) noexcept {
			static_assert(std::is_base_of<RefCountBase, T>::value, "");
			if (p == m_p) return;
			if (m_p) {
				auto c = --m_p->m_weakCount;
				if  (c <= 0 && m_p->m_refCount <= 0) 
				{
					sge_release(p);
				}
			}
			m_p = p;
			if (m_p) {
				++m_p->m_weakCount;
			}
		}

		T* detach() noexcept { T* o = m_p; m_p = nullptr; return o; }

	private:
		T* m_p = nullptr;
	};	



}
