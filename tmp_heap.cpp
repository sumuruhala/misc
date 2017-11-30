#define TEST_RETURN(_Ret)   return _Ret

#if 1
template<typename T>
struct XXXXXXXXXXXXXXXXXXXX__deduced_type_is_;
#	define SHOW_T(TYPE)\
		XXXXXXXXXXXXXXXXXXXX__deduced_type_is_<TYPE>{}
#	define EXPR_T(EXPR)\
		XXXXXXXXXXXXXXXXXXXX__deduced_type_is_<decltype(EXPR)>{}
#else
#	define SHOW_T(TYPE)
#	define EXPR_T(EXPR)
#endif

#include<iostream>
#include<cstdio>
#if 0
#	include"header.h"
#endif

using namespace std;
//--------------------------------------------------------

namespace ns1 {
using val_t = long long int;

template<val_t... _Vs>
struct seq;

template<int _I>
struct parent_idx
{
    static constexpr int value = (_I - 2 + (_I % 2)) / 2;
};

template<int _I>
constexpr int parent_idx_v = parent_idx<_I>::value;

template<>
struct parent_idx<0>
{
    static constexpr int value = 0;
};

template<>
struct parent_idx<1> : parent_idx<0>
{
};

template<>
struct parent_idx<2> : parent_idx<1>
{
};

template<typename _S, int _I, int _J>
struct val_at_impl;

template<val_t _V, val_t... _Vs, int _I, int _J>
struct val_at_impl<seq<_V, _Vs...>, _I, _J> : val_at_impl<seq<_Vs...>, _I + 1, _J>
{
};

template<val_t _V, val_t... _Vs, int _J>
struct val_at_impl<seq<_V, _Vs...>, _J, _J>
{
    static constexpr val_t value = _V;
};

template<typename _S, int _I>
struct val_at : val_at_impl<_S, 0, _I>
{
};

template<typename _S, int _I>
constexpr val_t val_at_v = val_at<_S, _I>::value;

template<val_t... _Vs>
struct acc;

template<typename _S, typename _A, int _M, int _B, int _E, bool _Lt = (_M < _E)>
struct slice_impl_acc;

template<val_t _V, val_t... _Vs, val_t... _As, int _M, int _B, int _E>
struct slice_impl_acc<seq<_V, _Vs...>, acc<_As...>, _M, _B, _E, true> : slice_impl_acc<seq<_Vs...>, acc<_As..., _V>, _M + 1, _B, _E>
{
};

template<typename _S, val_t... _As, int _B, int _E>
struct slice_impl_acc<_S, acc<_As...>, _E, _B, _E, false>
{
    using type = acc<_As...>;
};

template<typename _S, int _I, int _B, int _E, bool _Lt = (_I < _B)>
struct slice_impl_skip;

template<val_t _V, val_t... _Vs, int _I, int _B, int _E>
struct slice_impl_skip<seq<_V, _Vs...>, _I, _B, _E, true> : slice_impl_skip<seq<_Vs...>, _I + 1, _B, _E>
{
};

template<typename _S, int _B, int _E>
struct slice_impl_skip<_S, _B, _B, _E, false> : slice_impl_acc<_S, acc<>, _B, _B, _E>
{
};

template<typename _S, int _B, int _E>
struct slice : slice_impl_skip<_S, 0, _B, _E>
{
};

template<typename _S, int _B, int _E>
using slice_t = typename slice<_S, _B, _E>::type;

template<val_t _VL, val_t _VR, typename _AccL, typename _AccM, typename _AccR>
struct swap_impl;

template<val_t _VL, val_t _VR, val_t... _AccsL, val_t... _AccsM, val_t... _AccsR>
struct swap_impl<_VL, _VR, acc<_AccsL...>, acc<_AccsM...>, acc<_AccsR...>>
{
    using type = seq<_AccsL..., _VR, _AccsM..., _VL, _AccsR...>;
};

template<typename _S, int _I, int _J>
struct swap;

template<typename _S, int _I, int _J>
using swap_t = typename swap<_S, _I, _J>::type;

template<val_t... _Vs, int _I, int _J>
struct swap<seq<_Vs...>, _I, _J> : swap_impl<val_at_v<seq<_Vs...>, (_I < _J ? _I : _J)>, val_at_v<seq<_Vs...>, (_I < _J ? _J : _I)>, slice_t<seq<_Vs...>, 0, (_I < _J ? _I : _J)>, slice_t<seq<_Vs...>, (_I < _J ? _I : _J) + 1, (_I < _J ? _J : _I)>, slice_t<seq<_Vs...>, (_I < _J ? _J : _I) + 1, sizeof...(_Vs)>>
{
};

template<val_t... _Vs, int _I>
struct swap<seq<_Vs...>, _I, _I>
{
    using type = seq<_Vs...>;
};

template<typename _S, int _I, bool>
struct swim_impl;

template<val_t... _Vs, int _I>
struct swim_impl<seq<_Vs...>, _I, true> // do swim up
{
    static constexpr int value = parent_idx_v<_I>;
    using type = swap_t<seq<_Vs...>, _I, value>;
};

template<val_t... _Vs, int _I>
struct swim_impl<seq<_Vs...>, _I, false> // don't swim
{
    static constexpr int value = -1;
    using type = seq<_Vs...>;
};

template<typename _S, int _I>
struct swim;

template<typename _S, int _I>
using swim_t = typename swim<_S, _I>::type;

template<typename _S, int _I>
constexpr int swim_v = swim<_S, _I>::value;

template<val_t... _Vs, int _I>
struct swim<seq<_Vs...>, _I> : swim_impl<seq<_Vs...>, _I, (val_at_v<seq<_Vs...>, _I> < val_at_v<seq<_Vs...>, parent_idx_v<_I>>)>
{
};

template<typename _S, int _I>
struct push_heap_at_impl : push_heap_at_impl<swim_t<_S, _I>, swim_v<_S, _I>>
{
};

template<val_t... _Vs>
struct push_heap_at_impl<seq<_Vs...>, -1>
{
    using type = seq<_Vs...>;
};

template<typename _S, int _I>
struct push_heap_at : push_heap_at_impl<_S, _I>
{
};

template<typename _S, int _I>
using push_heap_at_t = typename push_heap_at<_S, _I>::type;

template<typename _S, int _I, int _N>
struct heapify_impl : heapify_impl<push_heap_at_t<_S, _I>, _I + 1, _N>
{
};

template<val_t... _Vs, int _N>
struct heapify_impl<seq<_Vs...>, _N, _N>
{
    using type = seq<_Vs...>;
};

template<typename _S>
struct heapify;

template<typename _S>
using heapify_t = typename heapify<_S>::type;

template<val_t... _Vs>
struct heapify<seq<_Vs...>> : heapify_impl<seq<_Vs...>, 0, sizeof...(_Vs)>
{
};

//-----------------------------------------------

template<typename _S, int _L, int _R, int _N, int _InBound = (_R < _N ? 1 : (_L < _N ? 2 : 3))>
struct idx_of_min;

template<typename _S, int _L, int _R, int _N>
constexpr int idx_of_min_v = idx_of_min<_S, _L, _R, _N>::value;

template<val_t... _Vs, int _L, int _R, int _N>
struct idx_of_min<seq<_Vs...>, _L, _R, _N, 1>
{
    static constexpr int value = (val_at_v<seq<_Vs...>, _L> <= val_at_v<seq<_Vs...>, _R>) ? _L : _R;
};

template<val_t... _Vs, int _L, int _R, int _N>
struct idx_of_min<seq<_Vs...>, _L, _R, _N, 2>
{
    static constexpr int value = _L;
};

template<val_t... _Vs, int _L, int _R, int _N>
struct idx_of_min<seq<_Vs...>, _L, _R, _N, 3>
{
    static constexpr int value = parent_idx_v<_L>;
};

template<typename _S, int _I, bool>
struct sink_impl;

template<val_t... _Vs, int _I>
struct sink_impl<seq<_Vs...>, _I, true> // do sink down
{
    static constexpr int value = idx_of_min_v<seq<_Vs...>, _I * 2 + 1, _I * 2 + 2, sizeof...(_Vs)>;
    using type = swap_t<seq<_Vs...>, _I, value>;
};

template<val_t... _Vs, int _I>
struct sink_impl<seq<_Vs...>, _I, false> // don't sink
{
    static constexpr int value = -1;
    using type = seq<_Vs...>;
};

template<typename _S, int _I>
struct sink;

template<typename _S, int _I>
using sink_t = typename sink<_S, _I>::type;

template<typename _S, int _I>
constexpr int sink_v = sink<_S, _I>::value;

template<val_t... _Vs, int _I>
struct sink<seq<_Vs...>, _I> : sink_impl<seq<_Vs...>, _I, (val_at_v<seq<_Vs...>, _I> > val_at_v<seq<_Vs...>, idx_of_min_v<seq<_Vs...>, _I * 2 + 1, _I * 2 + 2, sizeof...(_Vs)>>)>
{
};

template< typename _S, typename _A>
struct drop_last_impl;

template< val_t _V, val_t... _Vs, val_t... _As >
struct drop_last_impl<seq<_V, _Vs...>, acc<_As...>> : drop_last_impl<seq<_Vs...>, acc<_As..., _V>>
{
};

template< val_t _V, val_t... _As >
struct drop_last_impl<seq<_V>, acc<_As...>>
{
    using type = seq<_As...>;
};

template< typename _S >
struct drop_last : drop_last_impl<_S, acc<>>
{
};

template< typename _S >
using drop_last_t = typename drop_last<_S>::type;

template<typename _S, int _I>
struct pop_heap_impl : pop_heap_impl<sink_t<_S, _I>, sink_v<_S, _I>>
{
};

template<val_t... _Vs>
struct pop_heap_impl<seq<_Vs...>, -1>
{
    using type = seq<_Vs...>;
};

template<>
struct pop_heap_impl<seq<>, 0>
{
    using type = seq<>;
};

template<typename _S>
struct pop_heap;

template<typename _S>
using pop_heap_t = typename pop_heap<_S>::type;

template<val_t... _Vs>
struct pop_heap<seq<_Vs...>> : pop_heap_impl<drop_last_t<swap_t<seq<_Vs...>, 0, sizeof...(_Vs) - 1>>, 0>
{
};

template<typename _S, typename _A>
struct heap_sort_impl;

template<val_t _V, val_t... _Vs, val_t... _As>
struct heap_sort_impl<seq<_V, _Vs...>, acc<_As...>> : heap_sort_impl<pop_heap_t<seq<_V, _Vs...>>, acc<_As..., _V>>
{
};

template<val_t... _As>
struct heap_sort_impl<seq<>, acc<_As...>>
{
    using type = seq<_As...>;
};

template<typename _S>
struct heap_sort : heap_sort_impl<_S, acc<>>
{
};

template<typename _S>
using heap_sort_t = typename heap_sort<_S>::type;

//-----------------------------------------------

struct IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII; // display separator

int TEST_ENTRY(int ret = 0)
{
    using seq_t = seq<28, 38, 1, 21, 51, 17, 8, 0, 2, 11, 1, 33, 3, 51, 102, 9, 9, 84>;
    
    SHOW_T(IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII);
    SHOW_T(heapify_t<seq_t>);
    SHOW_T(IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII);
    SHOW_T(heap_sort_t<heapify_t<seq_t>>);

    TEST_RETURN(ret);
}

int TEST_ENTRY(int argc, const char* args[])
{
    TEST_RETURN(TEST_ENTRY());
}

} // namespace

int main(int argc, const char* args[])
{
    namespace TEST_NS = ns1;

    return TEST_NS::TEST_ENTRY(argc, args);
}
