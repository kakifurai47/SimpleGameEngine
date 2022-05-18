
namespace sge  {
namespace meta {
	template<class T> struct type_identity { using type = T; };
	
	template<bool COND, class T = void> using enif_t = std::enable_if_t<COND, T>;
}
}
