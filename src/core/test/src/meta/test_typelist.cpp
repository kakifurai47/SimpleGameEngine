#include <sge_core/base/UnitTest.h>
#include <sge_render/vertex/Vertex.h>

namespace sge {
	//template<bool COND, class T, class F> struct IfElse;
	//template<class T, class F> struct IfElse<true,  T, F> { using Type = T; };
	//template<class T, class F> struct IfElse<false, T, F> { using Type = F; };
	//template<bool COND, class T, class F> using IfElse_t = typename IfElse<COND, T, F>::Type;

	//template<class T> struct AddConst { using Type = const T; };
	//template<class T> using AddConst_t = typename AddConst<T>::Type;

	//template<class R> struct Convert { using Type = R; };
	//template<class R> using  Convert_t = typename Convert<R>::Type;

	//template<class X, class Y>
	//struct Largest_2 {
	//	using Type = typename IfElse<(sizeof(X) > sizeof(Y)), X, Y>::Type;
	//};

	//template<class... Ts> struct TypeList {};

	//template<class L> struct IsEmpty			 { static constexpr bool value = false; };
	//template<>		  struct IsEmpty<TypeList<>> { static constexpr bool value = true;  };

	//template<class L>			   struct Front {};
	//template<class H, class ...Ts> struct Front<TypeList<H, Ts...>> { using Type = H; };
	//template<class L>			   using  Front_t = typename Front<L>::Type;

	//template<class L>				struct PopFront{};
	//template<class H, class... Ts>	struct PopFront<TypeList<H, Ts...>> { using Type = TypeList<Ts...>; };
	//template<class L>				using  PopFront_t = typename PopFront<L>::Type;

	//template<class L,	  class E> struct PushFront{};
	//template<class... Ts, class E> struct PushFront<TypeList<Ts...>, E> { using Type = TypeList<E, Ts...>; };
	//template<class L,	  class E> using  PushFront_t = typename PushFront<L, E>::Type;

	//
	//template<class L, size_t N> struct getNth		: getNth<PopFront_t<L>, N - 1> {};
	//template<class L>			struct getNth<L, 0> : Front<L> {};
	//template<class L, size_t N> using  getNth_t = typename getNth<L, N>::Type;

	////==========================
	//template<class L, bool Empty = IsEmpty<L>::value> struct LargestType;
	//template<class L> struct LargestType<L, false> {
	//private:
	//	using Contender = Front_t<L>;
	//	using Best = typename LargestType<PopFront_t<L>>::Type;
	//public:
	//	using Type = IfElse_t<(sizeof(Contender) >= sizeof(Best)), Contender, Best>;
	//};

	//template<class L> struct LargestType<L, true> { using Type = char; };
	//template<class L> using  LargestType_t = typename LargestType<L>::Type;

	//template<class L, class E> struct PushBack;
	//template<class... Ts, class E> struct PushBack<TypeList<Ts...>, E> { using Type = TypeList<Ts..., E>; };
	//template<class L, class E> using PushBack_t = typename PushBack<L, E>::Type;

	//template<class L, class E, bool = IsEmpty<L>::value> struct PushBackRec;
	//template<class L, class E> 
	//struct PushBackRec<L, E, false> {
	//private:
	//	using Head = Front_t<L>;
	//	using Tail = typename PushBackRec<PopFront_t<L>, E>::Type;
	//public:
	//	using Type = PushFront_t<Tail, Head>;
	//};
	//template<class L, class E> struct PushBackRec<L, E, true> { using Type = PushFront_t<L, E>; };
	//template<class L, class E> using PushBackRec_t = typename PushBackRec<L, E>::Type;

	//template<class L, bool = IsEmpty<L>::value> struct Reverse;
	//template<class L> struct Reverse<L, false> {
	//private:
	//	using Head = Front_t<L>;
	//	using Tail = typename Reverse<PopFront_t<L>>::Type;
	//
	//public:
	//	using Type = PushBackRec_t<Tail, Head>;
	//
	//};
	//
	//template<class L> struct Reverse<L, true> { using Type = L; };
	//template<class L> using Reverse_t = typename Reverse<L>::Type;

	//template<class L> struct PopBack { using Type = Reverse_t<PopFront_t<Reverse_t<L>>>; };
	//template<class L> using  PopBack_t = typename PopBack<L>::Type;

	//
	//template<class L, template<class T> class FUN, bool Empty = IsEmpty<L>::value>
	//struct Transform;

	//template<class L, template<class T> class FUN>
	//struct Transform<L, FUN, true> {
	//	using Type = L; 
	//};

	//template<class L, template<class T> class FUN>
	//struct Transform<L, FUN, false> :
	//	public PushFront<typename Transform<PopFront_t<L>, FUN>::Type,
	//					 typename FUN<Front_t<L>>::Type>
	//{
	//};
	//template<class L, template<class T> class FUN> using Transform_t = typename Transform<L, FUN>::Type;



	//
	//template<class L, template<class X, class Y> class FUN,
	//		 class I, bool = IsEmpty<L>::value>
	//struct Accumulate
	//{
	//};

	//template<class L, template<class X, class Y> class FUN, class I>
	//struct Accumulate<L, FUN, I, false> :
	//	public Accumulate<PopFront_t<L>, FUN,
	//					  typename FUN<I, Front_t<L>>::Type>
	//{
	//};

	//template<class L, template<class X, class Y> class FUN, class I>
	//struct Accumulate<L, FUN, I, true> {
	//	using Type = I;
	//};

	//template<class L, template<class X, class Y> class FUN, class I>
	//using Accumulate_t = typename Accumulate<L, FUN, I>::Type;

	//template<class L, class E, template<class X, class Y> class COMP, bool = IsEmpty<L>::value>
	//struct InsertSorted;

	//template<class L, class E, template<class X, class Y>class COMP>
	//struct InsertSorted<L, E, COMP, false>
	//{
	//	using Head = Front_t<L>;
	//	using Tail = PopFront_t<L>;
	//	using CompRslt = IfElse_t<COMP<E, Head>::value, E, Head>;

	//	using NewHead = CompRslt;
	//	using NewTail = IfElse_t < COMP<E, Head>::value, L, typename InsertSorted<Tail, E, COMP>::Type>;


	//	using Type = PushFront_t<NewTail, NewHead>;

	//};

	//template<class L, class E, template<class X, class Y>class COMP>
	//struct InsertSorted<L, E, COMP, true> : public PushFront<L, E>
	//{
	//};

	class Test_Typelist : public UnitTestBase {
	public:

		void test_typelist() {
			/*

			using ATypeList = TypeList<u8, u16, u32, u64>;
			using ETypeList = TypeList<>;
			{
				{//test if else
					using testType = IfElse_t<(1 > 2), u8, u32>;
					using rsltType = u32;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{//test front;
					using testType = Front_t<ATypeList>;
					using rsltType = u8;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{//test pop front;
					using testType = PopFront_t<ATypeList>;
					using rsltType = TypeList<u16, u32, u64>;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{//test pushfront;
					using testType = PushFront_t<ATypeList, u64>;
					using rsltType = TypeList<u64, u8, u16, u32, u64>;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{// test nth type
					using testType = getNth_t<ATypeList, 1>;
					using rsltType = u16;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{//test largest type
					using testType = LargestType_t<ATypeList>;
					using rsltType = u64;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{//test push back
					using testType = PushBack_t<ATypeList, u32>;
					using rsltType = TypeList<u8, u16, u32, u64, u32>;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{//test push back rec
					using BTypeList = TypeList<>;

					using testType = PushBackRec_t<ATypeList, u32>;
					using rsltType = TypeList<u8, u16, u32, u64, u32>;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{//test reverse
					using testType = Reverse_t<ATypeList>;
					using rsltType = TypeList<u64, u32, u16, u8>;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{
					using testType = PopBack_t<ATypeList>;
					using rsltType = TypeList<u8, u16, u32>;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
				{//test transform
					using testType = Transform_t<ATypeList, AddConst>;
					using rsltType = TypeList<const u8, const u16, const u32, const u64>;
					static_assert(std::is_same<testType, rsltType>::value, "");
				}
			}
			{//test accumulate 1
				using testType = Accumulate_t<ETypeList, Largest_2, TypeList<>>;
				using rsltType = u64;
				//static_assert(std::is_same<testType, rsltType>::value, "");
			}
			{//test accumulate 2
				using testType = Accumulate_t<ATypeList, PushFront, TypeList<>>;
				using rsltType = TypeList<u64, u32, u16, u8>;				
				static_assert(std::is_same<testType, rsltType>::value, "");
			}
			{
				using T1 = InsertSorted<ATypeList, u8, Larger>::Type;
				using T2 = TypeList<u8, u8, u16, u32, u64>;



				//static_assert(std::is_same<T1, T2>::value, "");


			}
			*/

		}


	};

} // namespace 

void test_typelist() {
	using namespace sge;
	SGE_TEST_CASE(Test_Typelist, test_typelist());
}
