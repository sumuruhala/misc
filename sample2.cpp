#include<iostream>
#include<cmath>
#include<string>
#include<memory>
//#include<sstream>
//using namespace std;

/****power enable ?****/
#define _POWER_ENABLED_		1
#define _BRACES_ENABLED_	1

#define MAX_STR_SIZE		200
#define MAX_BUF_SIZE		50

#define ALLOW_FREE_SIGN		true	/*"free sign": the explicit sign charactor that can be "freely" put at the front of a number, that is, whitout parentheses surrounding the sign charactor and the number.*/
#define FORBID_FREE_SIGN	false
#define ALLOW_POWER			true
#define FORBID_POWER		false


char cMathExpr[MAX_STR_SIZE] =
	#if _POWER_ENABLED_ && _BRACES_ENABLED_
		"-1.0+1.0+123-(123^1)^1*{20^(20-[19*1])-(212-211*1)*1*[19]}/123-122+[[-123.3748]-{2.0}^(-1.0)]+1^(([1^1]^0)^(1^1))+(-(0.0))-(-(-(1.0)))";
	#elif _POWER_ENABLED_
		"-1.0+1.0+123-(123^1)^1*(20^(20-19*1)-(212-211*1)*1*19)/123-122+(-123.3748-(2.0)^(-1.0))+1^(((1^1)^0)^(1^1))+(-(0.0))-(-(-(1.0)))";
	#else
		"123-123*(20-(212-211*1)*1*19)/123-122+123";
	#endif

#define DLLEXPORT //__declspec(dllexport)

class DLLEXPORT CSimpleCalculator
{
public:
	explicit CSimpleCalculator(std::string pstrExpression = "\0")
		: m_strExp(pstrExpression), m_tokBuff(new char[MAX_BUF_SIZE])
		, m_strIdx(0), m_bufIdx(0), m_currTok(VOID)
	{
	}

	~CSimpleCalculator()
	{
	}

	bool Calculate();

public:
	enum Token
	{
		VOID,
#if _POWER_ENABLED_
		POWER,
#endif
#if _BRACES_ENABLED_
		LBRACE,
		RBRACE,
		LSQRBR,
		RSQRBR,
#endif
		LPARE,
		RPARE,
		NUM,
		DOT,
		ADD,
		PLUS = ADD,
		SUB,
		MINUS = SUB,
		MUL,
		DIV
	};

private:

	char getChar() { return m_strExp[m_strIdx++]; }
	char lookAhead() { return m_strExp[m_strIdx]; }
	Token getToken();
	void match(Token);
	void notifErr(Token) const;
	double factor(bool, bool);
	double term(bool);
	double expr();

	//static unsigned int _str2uint(char*);
	//static double _combineAsDouble(double, char*);
	static double _assertNotZero(double);
	static Token _getCounterpart(Token);

private:
	enum status
	{
		IN_NUM, BEGIN, END
	};

private:
	std::string m_strExp;
	std::unique_ptr<char[]> m_tokBuff;
	std::string::size_type m_strIdx;
	std::string::size_type m_bufIdx;
	Token m_currTok;
};

CSimpleCalculator::Token CSimpleCalculator::getToken()
{
	Token t;
	status s = BEGIN;
	m_bufIdx = 0;

	while( true )
	{
		switch( s )
		{
		case BEGIN:
			if( isdigit(lookAhead()) )
			{
				s = IN_NUM;
				t = NUM; // and we also need m_tokBuff to get the actual number.
				break;
			}

			switch( lookAhead() )
			{
			case '+':
				t = ADD;
				break;
			case '-':
				t = SUB;
				break;
			case '*':
				t = MUL;
				break;
			case '/':
				t = DIV;
				break;
			case '.':
				t = DOT;
				break;
#if _POWER_ENABLED_
			case '^':
				t = POWER;
				break;
#endif // _POWER_ENABLED_
			case '(':
				t = LPARE;
				break;
			case ')':
				t = RPARE;
				break;
#if _BRACES_ENABLED_
			case '{':
				t = LBRACE;
				break;
			case '}':
				t = RBRACE;
				break;
			case '[':
				t = LSQRBR;
				break;
			case ']':
				t = RSQRBR;
				break;
#endif // _BRACES_ENABLED_
			default:
				t = VOID;
			} // end switch

			s = END;
			break;

		case IN_NUM:
			if( isdigit(lookAhead()) )
				break;
			goto $END;

		case END:
		$END :
			m_tokBuff[m_bufIdx] = '\0';
			 return t;
		} // end switch

		m_tokBuff[m_bufIdx++] = getChar();
	} // end while
}

void CSimpleCalculator::match(Token t)
{
	if( t == m_currTok )
	{
		m_currTok = getToken();
		//std::cout<<m_tokBuff<<std::endl;
	}
	else
	{
		std::cerr << "Can't match current Token!" << std::ends
			<< "details>>>>>\n"
			<< "matching: " << m_tokBuff.get() << " (m_currTok = " << m_currTok << ") ," << std::ends
			<< "but t = " << t << std::endl;
		exit(EXIT_FAILURE);
	}
}

double CSimpleCalculator::factor(bool isStartOfExpr, bool allowPower) // ---> NUM { . NUM } | ([+|-|ε]Expr) | factor^factor
{
	double res = 0;

#if _BRACES_ENABLED_
	if( LPARE == m_currTok || LBRACE == m_currTok || LSQRBR == m_currTok )
#else
	if(LPARE == m_currTok)
#endif
	{
#if _BRACES_ENABLED_
		Token rightTok = _getCounterpart(m_currTok); // save m_currTok

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
			std::cerr << "Error: can't use power operator continuously!" << std::ends << "Calculating halted." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
#endif

	return res;
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
			res /= _assertNotZero(factor(FORBID_FREE_SIGN, ALLOW_POWER));
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

bool CSimpleCalculator::Calculate()
{
	// string ---> [+|-|ε]Expr
	match(VOID);
	std::cout << expr() << std::endl;
	return true;
}

//unsigned int CSimpleCalculator::_str2uint(char* buf)
//{
//	unsigned int uint = 0;
//	for( int i = 0; isdigit(buf[i]) && i < MAX_BUF_SIZE; i++ )
//	{
//		uint = (uint * 10 + (buf[i] - '0'));
//	}
//	return uint;
//}

//double CSimpleCalculator::_combineAsDouble(double itgr, char* fracBuf)
//{
//	int r = 1;
//	for( int i = 0; (fracBuf[i] != '\0') && i < MAX_BUF_SIZE; i++ )
//		r *= 10;
//	return itgr + (static_cast<double>(_str2uint(fracBuf)) / r);
//}

void CSimpleCalculator::notifErr(Token m_currTok) const
{
	if( SUB == m_currTok )
		std::cerr << "Illegal negative sign!" << std::endl;
	else
		std::cerr << "Illegal positive sign!" << std::endl;
	exit(EXIT_FAILURE);
}

double CSimpleCalculator::_assertNotZero(double dbl)
{
	if( dbl != 0.0 )
		return dbl;
	else
	{
		std::cerr << "Error: Divided by Zero!" << std::endl;
		exit(EXIT_FAILURE);
	}
	return 1.0;
}

#if _BRACES_ENABLED_
CSimpleCalculator::Token CSimpleCalculator::_getCounterpart(Token t)
{
	switch( t )
	{
	case LPARE:
		return RPARE;
	case LBRACE:
		return RBRACE;
	case LSQRBR:
		return RSQRBR;
	}
	return VOID;
}
#endif



int main()
{
	CSimpleCalculator sc(cMathExpr);
	sc.Calculate();
	return 0;
}
