#if 1
#   define TEST_ENTRY  main
#else
#   define TEST_ENTRY  test_entry
#endif
#define TEST_RETURN(_Ret)   return _Ret

#if 1

template<typename T>
struct XXXXXXXXXXXXXXXXXXXX__deduced_type_is_;

#   define SHOW_T(TYPE)\
        XXXXXXXXXXXXXXXXXXXX__deduced_type_is_<TYPE>{}

#   define EXPR_T(EXPR)\
        XXXXXXXXXXXXXXXXXXXX__deduced_type_is_<decltype(EXPR)>{}

#else

#   define SHOW_T(TYPE)
#   define EXPR_T(EXPR)

#endif

#include<iostream>
#include<cstdio>
#if 0
#   include"header.h"
#endif

using namespace std;
//--------------------------------------------------------

#include<cmath>
#include<string>
#include<memory>
#include<unordered_map>

#include<exception>
#include<iomanip>
#include<strstream>

namespace ns1 {


// #define _BRACT_ENABLED_ 1


enum
{
    ALLOW_POWER = true,
    FORBID_POWER = false,
};

constexpr size_t MAX_STR_SIZE = 200;

static char TEST_EXPR[MAX_STR_SIZE] =
    // #if _BRACT_ENABLED_
        /*-123.875 ==*/"-1.0+ 1 . 0!+123-(123^1)^1 !! *{20^(20-[19*1!])-(212-211*1)*1*[19]}/123-122+[[-123.3748]-{2.0}!^(-1.0)]+1^(([1^1]^0)^(1^1))+(-(0.0))-(-(-(1.0)))!";
    // #endif

template<typename T>
std::strstream&& operator<<(std::strstream&& str, T&& e)
{
    str << std::forward<T>(e);
    return std::move(str);
}

// ----------------------------------------------------------------------------
// Exception
// ----------------------------------------------------------------------------

class MyException : public std::exception
{
public:
    MyException(std::string msg);
    MyException(std::strstream&& msg);

    const char* what() const noexcept override;

private:
    std::string my_strMsg;
};

MyException::MyException(std::string msg)
    : my_strMsg(std::move(msg))
{
}

MyException::MyException(std::strstream&& msgstrm)
    : MyException(msgstrm.str())
{
}

const char* MyException::what() const noexcept
{
    return my_strMsg.c_str();
}

// ----------------------------------------------------------------------------
// Parser
// ----------------------------------------------------------------------------

class CParser
{
public:
    CParser(std::string strExpr);

    /* tell the compiler that we do want move semantics,
        and the default impl. is acceptable:    */
    CParser(CParser&&) = default;
    CParser& operator=(CParser&&) = default;

    /* also, tell the compiler that we do accept default copy semantics,
        coz our move semantics are nothing special (i.e. = default).  */
    CParser(const CParser&) = default;
    CParser& operator=(const CParser&) = default;

    virtual ~CParser() = default;

public:
    enum EToken: unsigned char
    {
        NONE,
        CARET,
        // #if _BRACT_ENABLED_
        LBRACT,
        RBRACT,
        LSQBRACT,
        RSQBRACT,
        // #endif // _BRACT_ENABLED_
        LPAREN,
        RPAREN,
        NUM,
        DOT,
        PLUS,
        MINUS,
        ASTER,
        SLASH,
        EXCLMK,
    };

    static const std::unordered_map<char, EToken> TokenTable;

protected:
    char lookAheadChar() const
    {
        return my_strExp[my_idxExp];
    }

    EToken lookAheadToken() const
    {
        return my_nextTok;
    }

    char consumeChar()
    {
        return my_strExp[my_idxExp++];
    }

    virtual void consumeToken() = 0;
    void consume(EToken);

    void setToken(EToken t)
    {
        my_nextTok = t;
    }

protected:
    std::unique_ptr<char[]> my_upNextTokBuf; // move only.
    std::string::size_type my_idxBuf;
    static const size_t MAX_BUF_SIZE = 50;

private:
    EToken my_nextTok;

    std::string my_strExp;
    std::string::size_type my_idxExp;
};

using EToken = CParser::EToken;

const size_t CParser::MAX_BUF_SIZE;

const std::unordered_map<char, EToken> CParser::TokenTable =
{
    {'+', PLUS},
    {'-', MINUS},
    {'*', ASTER},
    {'/', SLASH},
    {'.', DOT},
    {'^', CARET},
    {'(', LPAREN},
    {')', RPAREN},
// #if _BRACT_ENABLED_
    {'{', LBRACT},
    {'}', RBRACT},
    {'[', LSQBRACT},
    {']', RSQBRACT},
    {'!', EXCLMK},
// #endif // _BRACT_ENABLED_
};

CParser::CParser(std::string strExpr)
    : my_nextTok(NONE), my_upNextTokBuf(new char[MAX_BUF_SIZE])
    , my_idxBuf(), my_strExp(strExpr), my_idxExp()
{
}

// virtual
void CParser::consumeToken() // = 0
{
    setToken(NONE);
    throw MyException{ std::strstream{} << "\"virtual void consumeToken() = 0\" is not implemented correctly!\n" };
}

void CParser::consume(EToken t)
{
    if( t == lookAheadToken() )
    {
        consumeToken();
        //std::cout<<my_upNextTokBuf.get()<<std::endl;
    }
    else
    {
        throw MyException{ std::strstream{} << "Can't match current EToken!" << "\t;\t"
            << "details>>>>>\n"
            << "matching: " << std::setw(MAX_BUF_SIZE) << my_upNextTokBuf.get() << " (my_nextTok = " << lookAheadToken() << ") ," << "\t;\t"
            << "but t = " << t << '\n' };
    }
}

// ----------------------------------------------------------------------------
// Simple Calculator
// ----------------------------------------------------------------------------

class CSimpleCalculator : public CParser
{
public:
    explicit CSimpleCalculator(std::string strExpr = "");

    double  Calculate();

private:
    void consumeToken() override;

    double factor(bool);
    double term();
    double expr();

    unsigned long long factorial(bool, char);

    static double s_assertNotZero(double);
    static EToken s_getCounterpart(EToken);

private:
    static constexpr char MAX_FACTORIAL_NUM = 20; // 20! < max(ui64)

    enum status
    {
        IN_NUM,
        BEGIN,
        END
    };
};

CSimpleCalculator::CSimpleCalculator(std::string strExpr)
    : CParser(std::move(strExpr))
{
}

void CSimpleCalculator::consumeToken() // override
{
    EToken t = NONE;
    status s = BEGIN;
    my_idxBuf = 0;

    while( true )
    {
        switch( s )
        {
        case BEGIN:
            {
                while( isspace(lookAheadChar()) )
                    consumeChar();

                auto chLA = lookAheadChar();
                if( char{} == chLA )
                    goto __END;

                if( isdigit(chLA) )
                {
                    t = NUM;
                    s = IN_NUM;
                }
                else
                {
                    auto ptr = TokenTable.find(chLA);
                    t = ptr != TokenTable.end() ? ptr->second : NONE;
                    s = END;
                }
            }
            break;

        case IN_NUM:
            if( isdigit(lookAheadChar()) )
                break;
            goto __END;

        case END:
        default:
        __END:
            my_upNextTokBuf[my_idxBuf] = char{};
            setToken(t);
            return;
        } // switch

        my_upNextTokBuf[my_idxBuf++] = consumeChar();
    } // while
}

/*
Grammar synopsis:

    expr --> ['+' | '-'] term {('+' | '-') term};
    term --> factor<true> {('*' | '/') factor<true>};
    factor<power?> if power --> factor<false> ['^' factor<false>];
    factor<power?> if !power --> (number | '(' expr ')') ['!' ['!']];
    number --> digits ['.' digits].
*/

double CSimpleCalculator::factor(bool allowPower)
{
    double res = NAN; // Important: NAN!

    if( allowPower )
    {
        res = factor(FORBID_POWER);

        if( CARET == lookAheadToken() )
        {
            consumeToken();
            res = pow(res, factor(FORBID_POWER));
        }
    }
    else if( !allowPower )
    {
        if( LPAREN == lookAheadToken()
// #if _BRACT_ENABLED_
            || LBRACT == lookAheadToken()
            || LSQBRACT == lookAheadToken()
// #endif // _BRACT_ENABLED_
        )
        {
// #if _BRACT_ENABLED_
            EToken rightTok = s_getCounterpart(lookAheadToken());

            consumeToken();
            res = expr();
            consume(rightTok);
// #else
//          consumeToken();
//          res = expr();
//          consume(RPAREN);
// #endif
        }
        else if( NUM == lookAheadToken() )
        {
            std::string strInteg(my_upNextTokBuf.get());
            consumeToken();

            if( DOT == lookAheadToken() ) // fraction number
            {
                consumeToken();
                //std::istringstream(strInteg + "." + my_upNextTokBuf.get()) >> res;
                res = std::stod(strInteg + "." + my_upNextTokBuf.get());
                consume(NUM);
            }
            else
            {
                res = std::stod(strInteg);
            }
        }
        else
        {
            throw MyException{ std::strstream{} << "Rule: Factor<false, false> not matched !" << '\n' };
        }

        if( EXCLMK == lookAheadToken() )
        {
            if( res < 0.0 )
                throw MyException{ std::strstream{} << "factorial: Minus number!.\n" };

            consumeToken();

            bool bDblFac = false;
            if( EXCLMK == lookAheadToken() )
            {
                bDblFac = true;
                consumeToken();
            }

            auto ullRes = static_cast<unsigned long long>(res); // to integral
            if( MAX_FACTORIAL_NUM < ullRes )
                throw MyException{ std::strstream{} << "factorial: Too large base number! (Max limit == " << std::to_string(MAX_FACTORIAL_NUM) << "!).\n" };

            res = static_cast<double>(factorial(bDblFac, ullRes));
        }
    }

    return res;
}

double CSimpleCalculator::term()
{
    double res = factor(ALLOW_POWER);

    while( true )
    {
        switch( lookAheadToken() )
        {
        case ASTER:
            consumeToken();
            res *= factor(ALLOW_POWER);
            break;

        case SLASH:
            consumeToken();
            res /= s_assertNotZero(factor(ALLOW_POWER));
            break;

        default:
            return res;
        }
    }
}

double CSimpleCalculator::expr()
{
    bool bNegFirstTerm = false;
    if( PLUS == lookAheadToken() || MINUS == lookAheadToken() )
    {
        bNegFirstTerm = (MINUS == lookAheadToken());
        consumeToken();
    }

    double res = term();
    if( bNegFirstTerm )
        res = -res;

    while( true )
    {
        switch( lookAheadToken() )
        {
        case PLUS:
            consumeToken();
            res += term();
            break;

        case MINUS:
            consumeToken();
            res -= term();
            break;

        default:
            return res;
        }
    }
}

double CSimpleCalculator::Calculate()
{
    consume(NONE);
    return expr();
}

double CSimpleCalculator::s_assertNotZero(double dbl)
{
    if( dbl != 0.0 )
        return dbl;
    else
    {
        throw MyException{ std::strstream{} << "Error: Divided by Zero!" << '\n' };
    }
}

// #if _BRACT_ENABLED_
EToken CSimpleCalculator::s_getCounterpart(EToken t)
{
    switch( t )
    {
    case LPAREN:
        return RPAREN;
    case LBRACT:
        return RBRACT;
    case LSQBRACT:
        return RSQBRACT;
    default:
        break;
    }
    return NONE;
}
// #endif // _BRACT_ENABLED_

unsigned long long CSimpleCalculator::factorial(bool bDblFac, char val)
{
    using ull = unsigned long long;
    if( 1 >= val )
        return ull(1);
    return factorial(bDblFac, val - (bDblFac ? 2 : 1)) * ull(val);
}

// ----------------------------------------------------------------------------
// Calculator Interface
// ----------------------------------------------------------------------------

class MyCalculator
{
public:
    MyCalculator(std::string s);

    double Calculate();

private:
    std::unique_ptr<CSimpleCalculator> my_upImpl;
};

MyCalculator::MyCalculator(std::string s)
    : my_upImpl(new CSimpleCalculator(std::move(s)))
{
}

double MyCalculator::Calculate()
{
    return my_upImpl->Calculate();
}

// ----------------------------------------------------------------------------
// Test
// ----------------------------------------------------------------------------

int TEST_ENTRY(int ret = 0)
{
    MyCalculator sc(TEST_EXPR);
    cout<< sc.Calculate() <<endl;

    TEST_RETURN(ret);
}

int TEST_ENTRY(int argc, const char* args[])
{
    TEST_RETURN(TEST_ENTRY());
}

} // namespace

int TEST_ENTRY(int argc, const char* args[])
{
    namespace TEST_NS = ns1;

    return TEST_NS::TEST_ENTRY(argc, args);
}
