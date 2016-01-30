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


#include<type_traits>
#include<ratio>

namespace ns1 {


#define __EXPR_STREAM__ \
/*{{{*/ \
'1', '+', '2'
/*}}}*/


enum etokens: char
{
    tok_none,
    tok_num,
    tok_plus,
    tok_minus,
    tok_mul,
    tok_div,
    tok_dot,
    tok_pow,
    tok_exclmk,
    tok_lparen,
    tok_rparen,
    tok_lbrace,
    tok_rbrace,
    tok_lsqbrac,
    tok_rsqbrac,
};

struct stat_begin;
struct stat_end;
struct stat_in_num;

//------------------------------------------------------------------------

template< char _Ch >
struct is_digit
{
    static constexpr bool value = (0 <= _Ch - '0' && _Ch - '0' <= 9);
};

//------------------------------------------------------------------------

template< char... _Strm >
struct stream
{
    static constexpr char LA = '\0';
    using Strm = stream;
};

template< char _LA, char... _Strm >
struct stream<_LA, _Strm...>
{
    static constexpr char LA = _LA;
    using Strm = stream<_Strm...>;
};

template< char... _Acc >
struct acc;

//------------------------------------------------------------------------

template< char... _Strm >
struct skip_spaces
{
    using type = stream<_Strm...>;
};

template< char... _Strm >
struct skip_spaces<' ', _Strm...> : skip_spaces<_Strm...>
{};

template< char... _Strm >
struct skip_spaces<'\t', _Strm...> : skip_spaces<_Strm...>
{};

//------------------------------------------------------------------------

template<char _LA>
struct map_tok
{ static constexpr etokens value = tok_none; };
template<>
struct map_tok<'+'>
{ static constexpr etokens value = tok_plus; };
template<>
struct map_tok<'-'>
{ static constexpr etokens value = tok_minus; };
template<>
struct map_tok<'*'>
{ static constexpr etokens value = tok_mul; };
template<>
struct map_tok<'/'>
{ static constexpr etokens value = tok_div; };
template<>
struct map_tok<'.'>
{ static constexpr etokens value = tok_dot; };
template<>
struct map_tok<'^'>
{ static constexpr etokens value = tok_pow; };
template<>
struct map_tok<'!'>
{ static constexpr etokens value = tok_exclmk; };
template<>
struct map_tok<'('>
{ static constexpr etokens value = tok_lparen; };
template<>
struct map_tok<')'>
{ static constexpr etokens value = tok_rparen; };
template<>
struct map_tok<'{'>
{ static constexpr etokens value = tok_lbrace; };
template<>
struct map_tok<'}'>
{ static constexpr etokens value = tok_rbrace; };
template<>
struct map_tok<'['>
{ static constexpr etokens value = tok_lsqbrac; };
template<>
struct map_tok<']'>
{ static constexpr etokens value = tok_rsqbrac; };

//------------------------------------------------------------------------

template< etokens _Tok, typename _AccTy, typename _StrmTy >
struct match_tuple
{
    static constexpr etokens token = _Tok;
    using acc_type = _AccTy;
    using strm_type = _StrmTy;
};

template< typename _St, etokens _Tok, typename _StrmTy, typename _AccTy >
struct automata;

//------------------------------------------------------------------------

template< etokens _Tok, char _Ch, char... _Strm, char... _Acc >
struct automata<stat_begin, _Tok, stream<_Ch, _Strm...>, acc<_Acc...>>
{
    using Strm1 = typename skip_spaces<_Ch, _Strm...>::type;

    using type = std::conditional_t<
                    is_digit<Strm1::LA>::value,
                    typename automata<stat_in_num, tok_num, Strm1, acc<_Acc...>>::type,
                    typename automata<stat_end, map_tok<Strm1::LA>::value, typename Strm1::Strm, acc<Strm1::LA>>::type
                >;
};

template< etokens _Tok, typename _AccTy >
struct automata<stat_begin, _Tok, stream<>, _AccTy>
{
    using type = match_tuple<tok_none, _AccTy, stream<>>;
};

//------------------------------------------------------------------------

template< char _N, char _LA, char... _Strm, char... _Acc >
struct automata<stat_in_num, tok_num, stream<_N, _LA, _Strm...>, acc<_Acc...>>
{
    using type = typename automata<
                        std::conditional_t<is_digit<_LA>::value, stat_in_num, stat_end>,
                        tok_num,
                        stream<_LA, _Strm...>,
                        acc<_N, _Acc...>
                    >::type;
};

template< char _N, char... _Acc >
struct automata<stat_in_num, tok_num, stream<_N>, acc<_Acc...>>
{
    using type = typename automata<stat_end, tok_num, stream<>, acc<_N, _Acc...>>::type;
};

//------------------------------------------------------------------------

template< char... _Chs >
struct rev_acc;

template< typename _RevAccTy, char... _Acc >
struct rev_impl;

template< char... _RevChs, char _Ch, char... _Acc >
struct rev_impl<rev_acc<_RevChs...>, _Ch, _Acc...>
    : rev_impl<rev_acc<_Ch, _RevChs...>, _Acc...>
{};

template< char... _RevChs, char _Ch >
struct rev_impl<rev_acc<_RevChs...>, _Ch>
{
    using type = acc<_Ch, _RevChs...>;
};

template< char... _Acc >
struct reverse_seq : rev_impl<rev_acc<>, _Acc...>
{};

//------------------------------------------------------------------------

template< etokens _Tok, typename _StrmTy, char... _Acc >
struct automata<stat_end, _Tok, _StrmTy, acc<_Acc...>>
{
    using type = match_tuple<_Tok, typename reverse_seq<'\0', _Acc...>::type, _StrmTy>;
};

//------------------------------------------------------------------------

template< etokens _Tok, typename _StrmTy >
struct consume_tok : automata<stat_begin, _Tok, _StrmTy, acc<>>
{};

//------------------------------------------------------------------------

template< etokens _CTL, etokens _CTR, typename _StrmTy >
struct match
{
    using type = match_tuple<tok_none, acc<>, _StrmTy>;
};

template< etokens _CurrTok, typename _StrmTy >
struct  match<_CurrTok, _CurrTok, _StrmTy>
{
    using type = typename consume_tok<_CurrTok, _StrmTy>::type;
};

//------------------------------------------------------------------------

template< bool _DblFac, intmax_t _Val >
struct factorial
{
    static constexpr intmax_t value = _Val * factorial<_DblFac, _Val - (_DblFac ? 2 : 1)>::value;
};

template< bool _DblFac >
struct factorial<_DblFac, 1> : factorial<_DblFac, 0>
{};

template< bool _DblFac >
struct factorial<_DblFac, 0>
{
    static constexpr intmax_t value = 1;
};

//------------------------------------------------------------------------

template< intmax_t _Base, intmax_t _Expo >
struct power_int
{
    static constexpr intmax_t value = _Base * power_int<_Base, _Expo - 1>::value;
};

template< intmax_t _Base >
struct power_int<_Base, 0>
{
    static constexpr intmax_t value = 1;
};

template< typename _ResTyBase, typename _ResTyExpo >
struct power // TODO: now only support integral exponent.
{
    static constexpr intmax_t IntegExpo = _ResTyExpo::num / _ResTyExpo::den; // to integral !
    static constexpr intmax_t PosIntExpo = IntegExpo >= 0 ? IntegExpo : -IntegExpo;

    using type = std::conditional_t<
                    IntegExpo >= 0,
                    std::ratio<
                        power_int<_ResTyBase::num, PosIntExpo>::value,
                        power_int<_ResTyBase::den, PosIntExpo>::value
                    >,
                    std::ratio<
                        power_int<_ResTyBase::den, PosIntExpo>::value,
                        power_int<_ResTyBase::num, PosIntExpo>::value
                    >
                >;
                     
};

//------------------------------------------------------------------------
/*
Grammar synopsis:

    expr --> ['+' | '-'] term {('+' | '-') term};
    term --> factor<true> {('*' | '/') factor<true>};
    factor<power?> if power --> factor<false> ['^' factor<false>];
    factor<power?> if !power --> (number | '(' expr ')') ['!' ['!']];
    number --> digits ['.' digits].
*/

template< etokens _Tok, typename _Res, typename _StrmTy >
struct result_tuple
{
    static constexpr etokens token = _Tok;
    using res_type = _Res;
    using strm_type = _StrmTy;
};

template< typename, etokens, typename >
struct expr;
template< etokens, typename, typename >
struct term;
template< bool, etokens, typename, typename >
struct factor;

//------------------------------------------------------------------------

template< etokens _Tok, typename _ResTy, typename _StrmTy >
struct pow_helper
{
    using MchTup = typename match<_Tok, tok_pow, _StrmTy>::type;
    using FactResTup = typename factor<false, MchTup::token, typename MchTup::strm_type, typename MchTup::acc_type>::type;

    using type = result_tuple<
                    FactResTup::token,
                    typename power<_ResTy, typename FactResTup::res_type>::type,
                    typename FactResTup::strm_type
                >;
};



template< bool _Power, etokens _CurrTok, typename _StrmTy, typename _AccTy >
struct factor
{
    using FactResTup = typename factor<false, _CurrTok, _StrmTy, _AccTy>::type;

    using type = std::conditional_t<
                    tok_pow == FactResTup::token,
                    typename pow_helper<FactResTup::token, typename FactResTup::res_type, typename FactResTup::strm_type>::type,
                    FactResTup
                >;
};



template< etokens _CurrTok, typename _StrmTy >
struct fb_false_impl_subexpr
{
    static constexpr etokens RhsTok = _CurrTok == tok_lparen
                                        ? tok_rparen
                                        : _CurrTok == tok_lbrace
                                            ? tok_rbrace : tok_rsqbrac;
    using MchTup1 = typename match<_CurrTok, _CurrTok, _StrmTy>::type;
    using ExprResTup = typename expr<typename MchTup1::strm_type, MchTup1::token, typename MchTup1::acc_type>::type;
    using MchTup2 = typename match<ExprResTup::token, RhsTok, typename ExprResTup::strm_type>::type;

    using type = result_tuple<MchTup2::token, typename ExprResTup::res_type, typename MchTup2::strm_type>;
};



template< typename _AccTy, typename _HelperTy >
struct droplast_impl;
template< char... _Acc >
struct droplast_helper;

template< typename _AccTy >
struct droplast;
template< char... _Acc >
struct droplast<acc<_Acc...>> : droplast_impl<acc<_Acc...>, droplast_helper<>>
{};

template< char _Ch, char... _Acc1, char... _Acc2 >
struct droplast_impl<acc<_Ch, _Acc1...>, droplast_helper<_Acc2...>>
    : droplast_impl<acc<_Acc1...>, droplast_helper<_Acc2..., _Ch>>
{};
template< char _Ch, char... _Acc >
struct droplast_impl<acc<_Ch>, droplast_helper<_Acc...>>
{
    using type = acc<_Acc...>;
};


template< size_t _N, typename _AccRawTy >
struct append0;
template< size_t _N, char... _AccRaw >
struct append0<_N, acc<_AccRaw...>> : append0<_N - 1, acc<_AccRaw..., '0'>>
{};
template< char... _AccRaw >
struct append0<0, acc<_AccRaw...>>
{
    using type = acc<_AccRaw...>;
};


template< typename _AccRawTy >
struct to_integ;
template< char _Ch, char... _AccRaw >
struct to_integ<acc<_Ch, _AccRaw...>>
{
    static constexpr intmax_t value = (_Ch - '0') * power_int<10, sizeof...(_AccRaw)>::value + to_integ<acc<_AccRaw...>>::value;
};
template< char _Ch >
struct to_integ<acc<_Ch>>
{
    static_assert(_Ch != '\0', "to_integ: not raw Acc!");
    static constexpr intmax_t value = _Ch - '0';
};

template< typename _IntAccRawTy, typename _FraAccRawTy >
struct to_float;
template< typename _IntAccRawTy, char... _FraAccRaw >
struct to_float<_IntAccRawTy, acc<_FraAccRaw...>>
{
    using Int0sAccRaw = typename append0<sizeof...(_FraAccRaw) , _IntAccRawTy>::type;
    static constexpr intmax_t Int0s = to_integ<Int0sAccRaw>::value;
    static constexpr intmax_t Fra = to_integ<acc<_FraAccRaw...>>::value;

    using type = std::ratio<Int0s + Fra, power_int<10, sizeof...(_FraAccRaw)>::value>;
};

template< etokens _CurrTok, typename _StrmTy, typename _AccTy >
struct fb_false_impl_number_impl
{
    using IntAccRaw = typename droplast<_AccTy>::type;

    using type = result_tuple<_CurrTok, std::ratio<to_integ<IntAccRaw>::value>, _StrmTy>;
};

template< typename _StrmTy, typename _AccTy >
struct fb_false_impl_number_impl<tok_dot, _StrmTy, _AccTy>
{
    using MchTup1 = typename match<tok_dot, tok_dot, _StrmTy>::type;
    using IntAccRaw = typename droplast<_AccTy>::type;
    using FraAccRaw = typename droplast<typename MchTup1::acc_type>::type;

    using Res = typename to_float<IntAccRaw, FraAccRaw>::type;

    using MchTup2 = typename match<MchTup1::token, tok_num, typename MchTup1::strm_type>::type;

    using type = result_tuple<MchTup2::token, Res, typename MchTup2::strm_type>;
};

template< etokens _CurrTok, typename _StrmTy, typename _AccTy >
struct fb_false_impl_number
{
    using MchTup = typename match<_CurrTok, tok_num, _StrmTy>::type;

    using type = typename fb_false_impl_number_impl<MchTup::token, typename MchTup::strm_type, _AccTy>::type;
};



template< etokens _CurrTok, typename _StrmTy, typename _AccTy >
struct fb_false_impl
{
    using type = result_tuple<_CurrTok, std::ratio<0>, _StrmTy>;
};
template< typename _StrmTy, typename _AccTy >
struct fb_false_impl<tok_lparen, _StrmTy, _AccTy> : fb_false_impl_subexpr<tok_lparen, _StrmTy>
{};
template< typename _StrmTy, typename _AccTy >
struct fb_false_impl<tok_lbrace, _StrmTy, _AccTy> : fb_false_impl_subexpr<tok_lbrace, _StrmTy>
{};
template< typename _StrmTy, typename _AccTy >
struct fb_false_impl<tok_lsqbrac, _StrmTy, _AccTy> : fb_false_impl_subexpr<tok_lsqbrac, _StrmTy>
{};
template< typename _StrmTy, typename _AccTy >
struct fb_false_impl<tok_num, _StrmTy, _AccTy> : fb_false_impl_number<tok_num, _StrmTy, _AccTy>
{};


template< typename _MchTy, bool _DblFac >
struct impl_2_tuple
{
    using type = _MchTy;
    static constexpr bool value = _DblFac;
};


template< typename _ResTupTy, bool = _ResTupTy::token == tok_exclmk >
struct fb_false_impl_2
{
    using type = _ResTupTy;
};

template< typename _ResTupTy >
struct fb_false_impl_2<_ResTupTy, true>
{
    using _E1 = std::enable_if_t<!(_ResTupTy::res_type::num < 0 || _ResTupTy::res_type::den < 0), int>;

    using Mch = typename match<tok_exclmk, tok_exclmk, typename _ResTupTy::strm_type>::type;

    using Tup = std::conditional_t<
                    Mch::token == tok_exclmk,
                    impl_2_tuple<typename match<tok_exclmk, tok_exclmk, typename Mch::strm_type>::type, true>,
                    impl_2_tuple<Mch, false>
                >;

    static constexpr intmax_t RoundedInt = _ResTupTy::res_type::num / _ResTupTy::res_type::den;

    using _E2 = std::enable_if_t<RoundedInt < 20, int>;

    using FactlRes = std::ratio<factorial<Tup::value, RoundedInt>::value>;

    using type = result_tuple<Tup::type::token, FactlRes, typename Tup::type::strm_type>;
};



template< etokens _CurrTok, typename _StrmTy, typename _AccTy >
struct factor<false, _CurrTok, _StrmTy, _AccTy>
{
    using ResTup = typename fb_false_impl<_CurrTok, _StrmTy, _AccTy>::type;

    using type = typename fb_false_impl_2<ResTup>::type;
};

//------------------------------------------------------------------------

template< typename _ResTy, etokens _CurrTok, typename _StrmTy >
struct term_loop
{
    using type = result_tuple<_CurrTok, _ResTy, _StrmTy>;
};

template< typename _ResTy, etokens _CurrTok, typename _StrmTy >
struct term_loop_continue
{
    using MchTup = typename match<_CurrTok, _CurrTok, _StrmTy>::type;

    using FactResTup = typename factor<true, MchTup::token, typename MchTup::strm_type, typename MchTup::acc_type>::type;

    using type = typename term_loop<
                    std::conditional_t<
                        tok_mul == _CurrTok,
                        typename std::ratio_multiply<_ResTy, typename FactResTup::res_type>::type,
                        typename std::ratio_divide<
                            _ResTy,
                            std::conditional_t<
                                FactResTup::res_type::num != 0 && FactResTup::res_type::den != 0, /// TODO
                                typename FactResTup::res_type,
                                typename FactResTup::res_type /// TODO
                            >
                        >::type
                    >,
                    FactResTup::token,
                    typename FactResTup::strm_type
                >::type;
};

template< typename _ResTy, typename _StrmTy >
struct term_loop<_ResTy, tok_mul, _StrmTy> : term_loop_continue<_ResTy, tok_mul, _StrmTy>
{};
template< typename _ResTy, typename _StrmTy >
struct term_loop<_ResTy, tok_div, _StrmTy> : term_loop_continue<_ResTy, tok_div, _StrmTy>
{};

template< etokens _CurrTok, typename _StrmTy, typename _AccTy >
struct term
{
    using FactResTup = typename factor<true, _CurrTok, _StrmTy, _AccTy>::type;

    using type = typename term_loop<typename FactResTup::res_type, FactResTup::token, typename FactResTup::strm_type>::type;
};

//------------------------------------------------------------------------

template< bool _Neg, etokens _T, typename _S, typename _A >
struct expr_tuple
{
    static constexpr bool Neg = _Neg;
    static constexpr etokens token = _T;
    using strm_type = _S;
    using acc_type = _A;
};


template< typename _ResTy, etokens _CurrTok, typename _StrmTy >
struct expr_loop
{
    using type = result_tuple<_CurrTok, _ResTy, _StrmTy>;
};

template< typename _ResTy, etokens _CurrTok, typename _StrmTy >
struct expr_loop_continue
{
    using MchTup = typename match<_CurrTok, _CurrTok, _StrmTy>::type;

    using TermResTup = typename term<MchTup::token, typename MchTup::strm_type, typename MchTup::acc_type>::type;

    using type = typename expr_loop<
                    std::conditional_t<
                        tok_plus == _CurrTok,
                        typename std::ratio_add<_ResTy, typename TermResTup::res_type>::type,
                        typename std::ratio_subtract<_ResTy, typename TermResTup::res_type>::type
                    >,
                    TermResTup::token,
                    typename TermResTup::strm_type
                >::type;
};

template< typename _ResTy, typename _StrmTy >
struct expr_loop<_ResTy, tok_plus, _StrmTy> : expr_loop_continue<_ResTy, tok_plus, _StrmTy>
{};
template< typename _ResTy, typename _StrmTy >
struct expr_loop<_ResTy, tok_minus, _StrmTy> : expr_loop_continue<_ResTy, tok_minus, _StrmTy>
{};


template< typename _StrmTy, etokens _Tok, typename _AccTy >
struct expr
{
    using Neg_T_S_A = std::conditional_t<
                        tok_plus == _Tok || tok_minus == _Tok,
                        expr_tuple<
                            tok_minus == _Tok,
                            match<_Tok, _Tok, _StrmTy>::type::token,
                            typename match<_Tok, _Tok, _StrmTy>::type::strm_type,
                            typename match<_Tok, _Tok, _StrmTy>::type::acc_type
                        >,
                        expr_tuple<false, _Tok, _StrmTy, _AccTy>
                    >;

    using TermResTup = typename term<Neg_T_S_A::token, typename Neg_T_S_A::strm_type, typename Neg_T_S_A::acc_type>::type;

    using type = typename expr_loop<
                    std::conditional_t<
                        Neg_T_S_A::Neg,
                        std::ratio<-TermResTup::res_type::num, TermResTup::res_type::den>,
                        typename TermResTup::res_type
                    >,
                    TermResTup::token,
                    typename TermResTup::strm_type
                >::type;
};

//------------------------------------------------------------------------

int TEST_ENTRY(int ret = 0)
{
    using MchTup = typename match<tok_none, tok_none, stream<__EXPR_STREAM__>>::type;
    using ResTup = typename expr<typename MchTup::strm_type, MchTup::token, typename MchTup::acc_type>::type;
    //cout << (double) ResTup::res_type::num / (double) ResTup::res_type::den << endl;
    SHOW_T(ResTup::res_type);

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
