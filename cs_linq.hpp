#ifndef CS_LINQ_HPP
#   define CS_LINQ_HPP

#include<iostream>

#include<type_traits>
#include<iterator>

#include<vector>


namespace details_cs_linq {
    
template<typename Iter>
class select_clause
{
    std::vector<Iter> m_iters;

public:
    select_clause(std::vector<Iter>&& v) : m_iters(std::move(v)) {}

    template<template<typename... Ts> class Cont, typename Inv>
    auto select(Inv&& inv)
    {
        using inv_t = decltype(std::invoke(std::forward<Inv>(inv), *m_iters[0]));

        Cont<std::decay_t<inv_t>> coll;
        auto bins = std::back_inserter(coll);

        for( auto iter : m_iters )
            bins = std::invoke(std::forward<Inv>(inv), *iter);

        return coll;
    }
};


template<typename Coll>
class where_clause
{
    using Iter = decltype(std::begin(std::declval<Coll>()));
    Coll&& m_coll;

public:
    where_clause(Coll&& c) : m_coll(std::forward<Coll>(c)) {}

    template<typename Pred>
    auto where(Pred&& pred)
    {
        std::vector<Iter> iters;

        for( auto iter = std::begin(m_coll); iter != std::end(m_coll); ++iter )
        {
            if( std::forward<Pred>(pred)(*iter) )
                iters.push_back(iter);
        }

        return select_clause<Iter>(std::move(iters));
    }
};

} // details_cs_linq


namespace xutils {

template<typename Coll>
auto from(Coll&& c)
{
    return details_cs_linq::where_clause<Coll>(std::forward<Coll>(c));
}

}; // xutils


namespace xu_example {
namespace example_cs_linq {
    
struct A
{
    int i;

    int mul(int k) const
    {
        return i * k;
    }
};


int mul2(const A& a)
{
    return a.mul(2);
}


void run()
{
    std::vector<A> coll = { A{ 1 }, A{ 2 }, A{ 3 } };

    auto res =  xutils::from(coll)
                .where([](auto&& e) { return e.i % 2 == 1; })
                .select<std::vector>([](auto&& e) { return e.mul(2); }); // or .select(mul2), .select(&A::i), etc.

    for( const auto& e : res )
        std::cout << e << std::endl;
}

} // example_cs_linq
} // xu_example

#endif // CS_LINQ_HPP
