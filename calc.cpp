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


// #define _POWER_ENABLED_      1
// #define _BRACES_ENABLED_ 1


enum
{
    ALLOW_FREE_SIGN = true, /*"free sign": the explicit sign charactor that can be "freely" put at the front of a number, that is, whitout parentheses surrounding the sign charactor and the number.*/
    FORBID_FREE_SIGN = false,
    ALLOW_POWER = true,
    FORBID_POWER = false,
};

constexpr size_t MAX_STR_SIZE = 200;

char MathExpr[MAX_STR_SIZE] =
    // #if _POWER_ENABLED_ && _BRACES_ENABLED_
        /*-123.875 ==*/"-1.0+ 1 . 0+123-(123^1)^1*{20^(20-[19*1])-(212-211*1)*1*[19]}/123-122+[[-123.3748]-{2.0}^(-1.0)]+1^(([1^1]^0)^(1^1))+(-(0.0))-(-(-(1.0)))";
    // #elif _POWER_ENABLED_
    //  "-1.0+1.0+123-(123^1)^1*(20^(20-19*1)-(212-211*1)*1*19)/123-122+(-123.3748-(2.0)^(-1.0))+1^(((1^1)^0)^(1^1))+(-(0.0))-(-(-(1.0)))";
    // #else
    //  "123-123*(20-(212-211*1)*1*19)/123-122+123";
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
        // #if _POWER_ENABLED_
        POWER,
        // #endif
        // #if _BRACES_ENABLED_
        LBRACE,
        RBRACE,
        LSQRBR,
        RSQRBR,
        // #endif
        LPARE,
        RPARE,
        NUM,
        DOT,
        ADD,
        PLUS = ADD,
        SUB,
        MINUS = SUB,
        MUL,
        DIV,
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
    {'+', ADD}, // or PLUS
    {'-', SUB}, // or MINUS
    {'*', MUL},
    {'/', DIV},
    {'.', DOT},
// #if _POWER_ENABLED_
    {'^', POWER},
// #endif // _POWER_ENABLED_
    {'(', LPARE},
    {')', RPARE},
// #if _BRACES_ENABLED_
    {'{', LBRACE},
    {'}', RBRACE},
    {'[', LSQRBR},
    {']', RSQRBR},
// #endif // _BRACES_ENABLED_
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

    void notifErr(EToken) const;

    double factor(bool, bool);
    double term(bool);
    double expr();

    static double s_assertNotZero(double);
    static EToken s_getCounterpart(EToken);

private:
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

/* Grammer synopsis:
    expr --> term<true> { ('+'|'-') term<false>};
    term<sign?> --> factor<sign, true> {('*'|'/') factor<false, true>};
    factor<sign?, power?> if sign && power --> ['+'|'-'] factor<false, true>;
    factor<sign?, power?> if !sign && power --> factor<false, false> ['^' factor<false, false>];
    factor<sign?, power?> if !sign && !power --> number | '(' expr ')';
    number --> digits['.' digits].
*/

double CSimpleCalculator::factor(bool isStartOfExpr, bool allowPower)
{
    double res = NAN; // Important: NAN!

    if( isStartOfExpr && allowPower )
    {
        bool negate = false;

        if( PLUS == lookAheadToken() || MINUS == lookAheadToken() ) // explicitly signed number
        {
            negate = (MINUS == lookAheadToken());
            consumeToken();
        }

        res = factor(FORBID_FREE_SIGN, ALLOW_POWER);

        if( negate )
            res = -res;
    }
    else if( !isStartOfExpr && allowPower )
    {
        res = factor(FORBID_FREE_SIGN, FORBID_POWER);

        if( POWER == lookAheadToken() )
        {
            consumeToken();
            res = pow(res, factor(FORBID_FREE_SIGN, FORBID_POWER));
        }
    }
    else if( !isStartOfExpr && !allowPower )
    {
// #if _BRACES_ENABLED_
        if( LPARE == lookAheadToken()
            || LBRACE == lookAheadToken()
            || LSQRBR == lookAheadToken()
        )
// #else
//      if( LPARE == lookAheadToken() )
// #endif
        {
// #if _BRACES_ENABLED_
            EToken rightTok = s_getCounterpart(lookAheadToken());

            consumeToken();
            res = expr();
            consume(rightTok);
// #else
//          consume(LPARE);
//          res = expr();
//          consume(RPARE);
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
    }

    return res;
}

double CSimpleCalculator::term(bool isStartOfExpr)
{
    double res = factor(isStartOfExpr, ALLOW_POWER);

    while( true )
    {
        switch( lookAheadToken() )
        {
        case MUL:
            consume(MUL);
            res *= factor(FORBID_FREE_SIGN, ALLOW_POWER);
            break;

        case DIV:
            consume(DIV);
            res /= s_assertNotZero(factor(FORBID_FREE_SIGN, ALLOW_POWER));
            break;

        default:
            return res;
        }
    }
}

double CSimpleCalculator::expr()
{
    double res = term(ALLOW_FREE_SIGN);

    while( true )
    {
        switch( lookAheadToken() )
        {
        case ADD:
            consume(ADD);
            res += term(FORBID_FREE_SIGN);
            break;

        case SUB:
            consume(SUB);
            res -= term(FORBID_FREE_SIGN);
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

void CSimpleCalculator::notifErr(EToken currTok) const
{
    if( SUB == currTok )
        throw MyException{ std::strstream{} << "Illegal negative sign!" << '\n' };
    else
        throw MyException{ std::strstream{} << "Illegal positive sign!" << '\n' };
}

double CSimpleCalculator::s_assertNotZero(double dbl)
{
    if( dbl != 0.0 )
        return dbl;
    else
    {
        throw MyException{ std::strstream{} << "Error: Divided by Zero!" << '\n' };
    }
    //return 1.0;
}

// #if _BRACES_ENABLED_
EToken CSimpleCalculator::s_getCounterpart(EToken t)
{
    switch( t )
    {
    case LPARE:
        return RPARE;
    case LBRACE:
        return RBRACE;
    case LSQRBR:
        return RSQRBR;
    default:
        break;
    }
    return NONE;
}
// #endif



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



int TEST_ENTRY(int ret = 0)
{
    MyCalculator sc(MathExpr);
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
