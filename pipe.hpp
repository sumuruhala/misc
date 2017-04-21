#ifndef PIPE_HPP
#   define PIPE_HPP

#include<vector>

namespace details_pipe {
    
template<typename T>
struct xu_view_base
{};

struct reversed_tag
    : xu_view_base<reversed_tag>
{};

template<int N>
struct trim_n_tag
    : xu_view_base<trim_n_tag<N>>
{};

template<typename>
struct sel_view;

template<>
struct sel_view<reversed_tag>
{
    template<typename E>
    static auto apply(const std::vector<E>& vec)
    {
        std::vector<E> v2(vec);
        std::reverse(v2.begin(), v2.end());
        return v2;
    }
};

template<int N>
struct sel_view<trim_n_tag<N>>
{
    template<typename E>
    static auto apply(std::vector<E>& vec)
    {
        if( vec.size() <= N )
            return std::vector<E>{};

        for( int i = 0; i < N; i++ )
            vec.pop_back();

        return vec;
    }
};

} // details_pipe


template<typename Coll, typename VTag>
auto operator|(Coll&& vec, details_pipe::xu_view_base<VTag>)
{
    return details_pipe::sel_view<VTag>::apply(std::forward<Coll>(vec));
}


namespace xutils {

namespace view {

details_pipe::reversed_tag reversed;

template<int N>
details_pipe::trim_n_tag<N> trim;

} // view

} // xutils

namespace xu_example {
namespace example_pipe {

using namespace xutils;

void test()
{
    std::vector<int> v{ 1,2,3,4,5,6,7,8 };

    auto ls = v | view::trim<3> | view::reversed;
    for( auto i : ls )
        cout << i << endl; // 5 4 3 2 1
}

} // example_pipe
} // xu_example

#endif // PIPE_HPP
