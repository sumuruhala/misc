#ifndef PY_RANGE_HPP
#   define PY_RANGE_HPP

#include<iostream>

//--------------------------------------------------------

#include<type_traits>

#if 0
template<int _B, int _E, int... _N>
struct _range_impl : _range_impl<_B + 1, _E, _N..., _B>
{

};

template<int _I, int... _N>
struct _range_impl<_I, _I, _N...>
{
    initializer_list<int> my_il{_N...};

    auto begin()
    {
        return my_il.begin();
    }

    auto end()
    {
        return my_il.end();
    }
};

template<int _B, int _E>
struct range_obj : _range_impl<_B, _E>
{
};

int test_range()
{
    for(auto i : range_obj<0, 4>{})
        cout << i << endl;
}
#endif


namespace details_py_range {

template<int...>
struct __rev_helper;

template<typename, int...>
struct __reverse;

template<int... Ns, int H, int... T>
struct __reverse<__rev_helper<Ns...>, H, T...> : __reverse<__rev_helper<H, Ns...>, T...>
{
};

template<int... Ns>
struct __reverse<__rev_helper<Ns...>>
{
    static constexpr std::initializer_list<int> value = { Ns... };
};
template<int... Ns>
constexpr std::initializer_list<int> __reverse<__rev_helper<Ns...>>::value;

//-------------------------------

template<bool B, int End, int S, int H, int... T>
struct __impl_inc : __impl_inc<(H + S) <= End, End, S, H + S, H + S, T...>
{
};

template<int End, int S, int H, int... Ns>
struct __impl_inc<false, End, S, H, H, Ns...> : __reverse<__rev_helper<>, Ns...>
{
};

//-------------------------------------

template<bool B, int End, int S, int H, int... T>
struct __impl_dec : __impl_dec<(H - S) >= End, End, S, H - S, H - S, T...>
{
};

template<int End, int S, int H, int... Ns>
struct __impl_dec<false, End, S, H, H, Ns...> : __reverse<__rev_helper<>, Ns...>
{
};

//-------------------------------------

struct __illegal_range;

template<int L, int R, int S>
struct __rng_impl
    : std::conditional_t
      <
        (L < R && S > 0),
        __impl_inc<(L < R), R - 1, S, L, L>,
        std::conditional_t
        <
            (L > R && S < 0),
            __impl_dec<(L > R), R + 1, -S, L, L>,
            std::conditional_t<(L == R), __reverse<__rev_helper<>>, __illegal_range>
        >
      >
{
};

} // details_py_range
//-------------------------------------


namespace xutils {

template<int N>
constexpr auto&& range()
{
    return details_py_range::__rng_impl<0, N, 1>::value;
}

template<int B, int E>
constexpr auto&& range()
{
    return details_py_range::__rng_impl<B, E, 1>::value;
}

template<int B, int E, int S>
constexpr auto&& range()
{
    return details_py_range::__rng_impl<B, E, S>::value;
}

} // xutils

namespace xu_example {
namespace example_py_range {

using namespace xutils;

void run_range_func()
{
    for( auto i : range<10, 1, -4>() )
        cout << i << endl;
    cout << "----------------" << endl;
    for( auto i : range<2, 10, 4>() )
        cout << i << endl;
    cout << "----------------" << endl;
    for( auto i : range<10>() )
        cout << i << endl;
    cout << "----------------" << endl;
    for( auto i : range<2, 5>() )
        cout << i << endl;
    cout << "----------------" << endl;
    for( auto i : range<2, 2>() )
        cout << i << endl;
    cout << "----------------" << endl;
}

} // example_py_range
} // xu_example



namespace xutils {

struct range_obj
{
    struct iter
    {
        using iterator_category = input_iterator_tag;
        using value_type = int;
        using difference_type = int;
        using pointer = iter;
        using reference = int;

    private:
        const int m_beg, m_end, m_step;
        int m_next;

        bool is_inc() const
        {
            return m_beg < m_end && m_step > 0;
        }

        bool is_dec() const
        {
            return m_beg > m_end && m_step < 0;
        }

        bool is_empty() const
        {
            return m_beg == m_end;
        }

    public:
        iter(int b, int e, int s) : m_beg(b), m_end(e), m_step(s), m_next(m_beg) {}

        bool operator!=(const iter& rhs) const
        {
            if( is_inc() )
                return m_next <= rhs.m_end - 1;
            if( is_dec() )
                return m_next >= rhs.m_end + 1;
            if( !is_empty() )
                std::cout << "range_obj::iter: bad range used!" << std::endl;
            return false;
        }

        iter& operator++()
        {
            if( is_inc() || is_dec() )
                m_next += m_step;

            return *this;
        }

        int operator*() const
        {
            return m_next;
        }
    };


    range_obj(int n) : range_obj(0, n, 1) {}
    range_obj(int beg, int end) : range_obj(beg, end, 1) {}
    range_obj(int beg, int end, int step)
        : m_beg(beg), m_end(end), m_step(step)
    {
    }

    iter begin() const
    {
        return iter{ m_beg, m_end, m_step};
    }

    iter end() const
    {
        return iter{ 0, m_end, 0};
    }

private:
    const int m_beg;
    const int m_end;
    const int m_step;
};

} // xutils


namespace xu_example {
namespace example_py_range {

using namespace xutils;

void run_range_obj()
{
    for( auto i : range_obj{ 2, 10, 4 } )
        cout << i << endl;

    for( auto i : range_obj{ 10, 2, -4 } )
        cout << i << endl;

    for( auto i : range_obj{ 10, 10 } )
        cout << i << endl;

    for( auto i : range_obj{ 1, 2 } )
        cout << i << endl;
}

} // example_py_range
} // xu_example

#endif // PY_RANGE_HPP
