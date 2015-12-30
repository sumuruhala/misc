#if 1
#   define TEST_ENTRY  main
#else
#	define TEST_ENTRY  test_entry
#endif
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

#include<cmath>
#include<string>
#include<memory>
#include<unordered_map>

#include<exception>
#include<iomanip>
#include<strstream>

namespace ns1 {


/****power enable ?****/
// #define _POWER_ENABLED_		1
// #define _BRACES_ENABLED_	1


enum
{
	ALLOW_FREE_SIGN = true,	/*"free sign": the explicit sign charactor that can be "freely" put at the front of a number, that is, whitout parentheses surrounding the sign charactor and the number.*/
	FORBID_FREE_SIGN = false,
	ALLOW_POWER = true,
	FORBID_POWER = false,
};

constexpr size_t MAX_STR_SIZE = 200;

char cMathExpr[MAX_STR_SIZE] =
	// #if _POWER_ENABLED_ && _BRACES_ENABLED_
		"-1.0+1.0+123-(123^1)^1*{20^(20-[19*1])-(212-211*1)*1*[19]}/123-122+[[-123.3748]-{2.0}^(-1.0)]+1^(([1^1]^0)^(1^1))+(-(0.0))-(-(-(1.0)))";
	// #elif _POWER_ENABLED_
	// 	"-1.0+1.0+123-(123^1)^1*(20^(20-19*1)-(212-211*1)*1*19)/123-122+(-123.3748-(2.0)^(-1.0))+1^(((1^1)^0)^(1^1))+(-(0.0))-(-(-(1.0)))";
	// #else
	// 	"123-123*(20-(212-211*1)*1*19)/123-122+123";
	// #endif

template<typename T>
std::strstream&& operator<<(std::strstream&& str, T&& e)
{
	str << std::forward<T>(e);
	return std::move(str);
}

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

#define DLLEXPORT //__declspec(dllexport)

// ----------------------------------------------------------------------------
// Simple Calculator
// ----------------------------------------------------------------------------

class DLLEXPORT CSimpleCalculator
{
public:
	explicit CSimpleCalculator(std::string pstrExpression = "\0")
		: m_strExp(pstrExpression), m_tokBuff(new char[MAX_BUF_SIZE])
		, m_strIdx{}, m_bufIdx{}, m_currTok(NONE)
	{
	}

	CSimpleCalculator(CSimpleCalculator&&) = default;
	CSimpleCalculator& operator=(CSimpleCalculator&&) = default;
	
	~CSimpleCalculator() = default;

	double  Calculate();

public:
	enum Token
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

private:

	char consumeChar()
	{
		return m_strExp[m_strIdx++];
	}

	char lookAhead() const
	{
		return m_strExp[m_strIdx];
	}

	Token consumeToken();
	void match(Token);

	void notifErr(Token) const;

	double factor(bool, bool);
	double term(bool);
	double expr();

	static double s_assertNotZero(double);
	static Token s_getCounterpart(Token);

private:
	enum status
	{
		IN_NUM,
		BEGIN,
		END
	};

	std::string m_strExp;

	std::unique_ptr<char[]> m_tokBuff; // move only.
	static const size_t MAX_BUF_SIZE = 50;

	std::string::size_type m_strIdx;
	std::string::size_type m_bufIdx;

    Token m_currTok;

	static const std::unordered_map<char, Token> TokenTable;
};

using EToken = CSimpleCalculator::Token;

const size_t CSimpleCalculator::MAX_BUF_SIZE; // = 50

const std::unordered_map<char, EToken> CSimpleCalculator::TokenTable =
{
	{'+', ADD},
	{'-', SUB},
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

EToken CSimpleCalculator::consumeToken()
{
	Token t = NONE;
	status s = BEGIN;
	m_bufIdx = 0;

	while( true )
	{
		switch( s )
		{
		case BEGIN:
			{
				if( isdigit(lookAhead()) )
				{
					s = IN_NUM;
					t = NUM; // and we also need m_tokBuff to get the actual number.
					break;
				}

				auto chLA = lookAhead();
				auto ptr = TokenTable.find(chLA);
				if( ptr != TokenTable.end() )
					t = ptr->second;
				else
					t = NONE;
			}

			s = END;
			break;

		case IN_NUM:
			if( isdigit(lookAhead()) )
				break;
			goto __END;

		case END:
		__END:
			m_tokBuff[m_bufIdx] = '\0';
			return t;
		} // end switch

		m_tokBuff[m_bufIdx++] = consumeChar();
	} // end while
}

void CSimpleCalculator::match(Token t)
{
	if( t == m_currTok )
	{
		m_currTok = consumeToken();
		//std::cout<<m_tokBuff.get()<<std::endl;
	}
	else
	{
		throw MyException{ std::strstream{} << "Can't match current Token!" << "\t;\t"
			<< "details>>>>>\n"
			<< "matching: " << std::setw(MAX_BUF_SIZE) << m_tokBuff.get() << " (m_currTok = " << m_currTok << ") ," << "\t;\t"
			<< "but t = " << t << '\n' };
	}
}

double CSimpleCalculator::factor(bool isStartOfExpr, bool allowPower) // ---> NUM { . NUM } | ([+|-|¦Å]Expr) | factor^factor
{
	/*
	exp --> term<true> { ('+'|'-') term<false>};
	term<sign?> --> factor<sign, true> {('*'|'/') factor<false, true>};
	factor<sign?, power?> if sign && power --> ['+'|'-'] factor<false, true>;
	factor<sign?, power?> if !sign && power --> factor<false, false> ['^' factor<false, false>];
	factor<sign?, power?> if !sign && !power --> number | '(' exp ')';
	number --> digits['.' digits].
	*/

	double res = 0;

	if( isStartOfExpr && allowPower )
	{
		bool negate = false;

		if( PLUS == m_currTok || MINUS == m_currTok ) // explicitly signed number
		{
			negate = (MINUS == m_currTok);
			match(m_currTok);
		}

		res = factor(FORBID_FREE_SIGN, ALLOW_POWER); // i.e., res = factor(false, true); Here "(false, true)" means : forbids free sign , allows performing power calculation

		if( negate )
			res = -res;
	}
	else if( !isStartOfExpr && allowPower )
	{
		res = factor(FORBID_FREE_SIGN, FORBID_POWER);

		if( POWER == m_currTok )
		{
			match(POWER);
			res = pow(res, factor(FORBID_FREE_SIGN, FORBID_POWER));
		}
	}
	else if( !isStartOfExpr && !allowPower )
	{
// #if _BRACES_ENABLED_
		if( LPARE == m_currTok || LBRACE == m_currTok || LSQRBR == m_currTok )
// #else
// 		if( LPARE == m_currTok )
// #endif
		{
// #if _BRACES_ENABLED_
			Token rightTok = s_getCounterpart(m_currTok); // save m_currTok

			match(m_currTok); // match the left brace/sqr-brace/parenthese
			res = expr();
			match(rightTok); // get the corresponding right brace/sqr-brace/parenthese and match it.
// #else
// 			match(LPARE);
// 			res = expr();
// 			match(RPARE);
// #endif
		}
		else if( NUM == m_currTok )
		{
			std::string strInteg(m_tokBuff.get());
			match(NUM);

			if( DOT == m_currTok ) // fraction number
			{
				match(DOT);
				//std::istringstream(strInteg + "." + m_tokBuff.get()) >> res;
				res = std::stod(strInteg + "." + m_tokBuff.get());
				match(NUM);
			}
			else
			{
				res = std::stod(strInteg);
			}
		}
	}

	return res;

#if 0
	double res = 0;

#if _BRACES_ENABLED_
	if( LPARE == m_currTok || LBRACE == m_currTok || LSQRBR == m_currTok )
#else
	if(LPARE == m_currTok)
#endif
	{
#if _BRACES_ENABLED_
		Token rightTok = s_getCounterpart(m_currTok); // save m_currTok

		match(m_currTok); // match the left brace/sqr-brace/parenthese
		res = expr();
		match(rightTok); // get the corresponding right brace/sqr-brace/parenthese and match it.
#else
		match(LPARE);
		res = expr();
		match(RPARE);
#endif
	}
	else if( NUM == m_currTok )
	{
		std::string strInteg(m_tokBuff.get());
		match(NUM);

		if( DOT == m_currTok ) // fraction number
		{
			match(DOT);
			//std::istringstream(strInteg + "." + m_tokBuff.get()) >> res;
			res = std::stod(strInteg + "." + m_tokBuff.get());
			match(NUM);
		}
		else
			res = std::stod(strInteg);
	}
	else if( PLUS == m_currTok || MINUS == m_currTok ) // explicitly signed number
	{
		if( isStartOfExpr ) // if the leading sign(+ or -) is the start of the current expression, then accept it (as a free sign).
		{
			bool isNeg = (MINUS == m_currTok);

			match(m_currTok);
			res = factor(FORBID_FREE_SIGN, ALLOW_POWER); // i.e., res = factor(false, true); Here "(false, true)" means : forbids free sign , allows performing power calculation
			res = isNeg ? -res : +res;
		}
		else // if the sign is not at the begining of the expression, then syntax error occurs and reject it.
			notifErr(m_currTok); // there is no parentheses surrounding the explicitly signed number!
		// program halted.
	}

#if _POWER_ENABLED_
	if( POWER == m_currTok )
	{
		if( allowPower )
		{
			match(POWER);
			res = pow(res, factor(FORBID_FREE_SIGN, FORBID_POWER));
		}
		else
		{
			throw MyException{ std::strstream{} << "Error: can't use power operator continuously!" << "\t;\t" << "Calculating halted." << '\n' };
		}
	}
#endif

	return res;
#endif
}

double CSimpleCalculator::term(bool isStartOfExpr) // ---> factor { [*|/] factor }
{
	double res = factor(isStartOfExpr, ALLOW_POWER);

	while( true )
	{
		switch( m_currTok )
		{
		case MUL:
			match(MUL);
			res *= factor(FORBID_FREE_SIGN, ALLOW_POWER);
			break;

		case DIV:
			match(DIV);
			res /= s_assertNotZero(factor(FORBID_FREE_SIGN, ALLOW_POWER));
			break;

		default:
			return res;
		}
	}
}

double CSimpleCalculator::expr() // ---> term { [+|-] term }
{
	double res = term(ALLOW_FREE_SIGN); //term(true);//"true": it is the begining of a expression. In every recursion circle(from expr to factor), it is set "true" here.

	while( true )
	{
		switch( m_currTok )
		{
		case ADD:
			match(ADD);
			res += term(FORBID_FREE_SIGN); //term(false);//"false": now, it is NOT the begining of a expression.
			break;

		case SUB:
			match(SUB);
			res -= term(FORBID_FREE_SIGN);
			break;

		default:
			return res;
		}
	}
}

double CSimpleCalculator::Calculate()
{
	match(NONE);
	return expr();
}

void CSimpleCalculator::notifErr(Token currTok) const
{
	if( SUB == currTok )
		throw MyException{ std::strstream{} << "Illegal negative sign!" << '\n' };
	else
		throw MyException{std::strstream{} << "Illegal positive sign!" << '\n' };
}

double CSimpleCalculator::s_assertNotZero(double dbl)
{
	if( dbl != 0.0 )
		return dbl;
	else
	{
		throw MyException{std::strstream{} << "Error: Divided by Zero!" << '\n' };
	}
	//return 1.0;
}

// #if _BRACES_ENABLED_
EToken CSimpleCalculator::s_getCounterpart(Token t)
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

	double Calculate(); // Don't use auto type specifier for interface declaration.

	~MyCalculator();

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

MyCalculator::~MyCalculator() = default;




int TEST_ENTRY(int ret = 0)
{
	CSimpleCalculator sc(cMathExpr);
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
