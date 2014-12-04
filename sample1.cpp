#include <iostream>
#include <string>
#include <vector>

template<typename...>
struct __idx_seq_for_impl;

template<typename T, typename... Ts>
struct __idx_seq_for_impl<T, Ts...>
{
    template<bool, size_t... Ss>
    struct __append_idx : __append_idx<false, sizeof...(Ts), Ss...>
    {
    };
	
    template<size_t... Ss>
    struct __append_idx<false, Ss...>
    {
        typedef __append_idx	res_type;
    };

    template<typename>
    struct __fwd_type;
    
    template<size_t... ttSs>
    using __type_forwarded = typename __fwd_type<
    						typename __append_idx<true, ttSs...>::res_type
						>::type;
    
    template<size_t... Ss>
    struct __fwd_type<__append_idx<false, Ss...>>
    {
        using type = typename __idx_seq_for_impl<Ts...>::template __type_forwarded<Ss...>;
    };
};

template<size_t... Ss>
struct idx_seq
{
};

template<typename T>
struct __idx_seq_for_impl<T>
{
    template<size_t... Ss>
    using __type_forwarded = idx_seq<(Ss - 1)...>;
};

template<typename... T>
struct idx_seq_for
{
	using idx_seq_t = typename __idx_seq_for_impl<T...>::template __type_forwarded<sizeof...(T)>;
    
    explicit operator idx_seq_t()
    {
        return idx_seq_t{};
    }
};

///////////////////////////////////////////////////////

template<size_t... I>
void test_idx(idx_seq<I...>&&)
{
	int ary[sizeof...(I)] = { I... };
    for(auto idx : ary)
        std::cout << idx << std::endl;
}

int main()
{
    using isf_t = idx_seq_for<int, double, short, char, float, std::string, std::vector<int>, unsigned, long, long long>;
    test_idx((isf_t::idx_seq_t)isf_t{});
}
