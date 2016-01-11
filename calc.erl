-module(calc).
%-include("global_ds.hrl").
-export([start/0, start/1]).

%-------------------------------------------------------------

start(Stream)
->
    {CurrTok, Acc, S2} = match(tok_None, tok_None, Stream),
    {_, Res, _} = expr(S2, CurrTok, Acc),
    io:format("Result == ~p .~n", [Res])
.

start()
->
    TestStream = "-1.0 + 1 . 0 +123-(123^1)^1*{20^(20-[19*1])-(212-211*1)*1*[19]}/123-122+[[-123.3748]-{2.0}^(-1.0)]+1^(([1^1]^0)^(1^1))+(-(0.0))-(-(-(1.0)))",
    start(TestStream)
.

%-------------------------------------------------------------

is_digit(Ch)
->
    Val = Ch - $0,
    0 =< Val andalso Val =< 9
.

skip_spaces([32|Str])
->
    skip_spaces(Str)
;
skip_spaces([$\t|Str])
->
    skip_spaces(Str)
;
skip_spaces(Str)
->
    Str
.

automata(stat_begin, _T, [C|S], Acc)
->
    [LA|Stream] = skip_spaces([C|S]),
    IsDigit = is_digit(LA),
    if
        IsDigit
        -> automata(stat_in_num, tok_Num, [LA|Stream], Acc);
        true
        ->
            Tok2 =  case [LA] of
                        "+" -> tok_Plus; % or Add
                        "-" -> tok_Minus; % or Sub
                        "*" -> tok_Mul;
                        "/" -> tok_Div;
                        "." -> tok_Dot;
                        "^" -> tok_Pow;
                        "(" -> tok_LParen;
                        ")" -> tok_RParen;
                        "{" -> tok_LBrace;
                        "}" -> tok_RBrace;
                        "[" -> tok_LSqrBrace;
                        "]" -> tok_RSqrBrace;
                        _ -> tok_None
                    end,
            automata(stat_end, Tok2, Stream, [LA])
    end
;

automata(stat_begin, Tok, [], Acc)
->
    {Tok, Acc, []}
;

automata(stat_in_num, tok_Num, [N, LA|Stream], Acc)
->
    IsDigit = is_digit(LA),
    Stat =  if
                IsDigit
                -> stat_in_num;
                true
                -> stat_end
            end,
    automata(Stat, tok_Num, [LA|Stream], [N|Acc])
;

automata(stat_in_num, tok_Num, [N], Acc)
->
    automata(stat_end, tok_Num, [], [N|Acc])
;

automata(stat_end, Tok, Stream, Acc)
->
    {Tok, lists:reverse([0|Acc]), Stream}
.

consume_tok(Tok, Stream)
->
    automata(stat_begin, Tok, Stream, [])
.

%-------------------------------------------------------------

match(CurrTok, CurrTok, Stream)
->
    consume_tok(CurrTok, Stream)
;

match(_CTL, _CTR, Stream)
->
    {tok_None, [], Stream}
.

%-------------------------------------------------------------

% Grammer synopsis:
%     expr --> term<true> { ('+'|'-') term<false>};
%     term<sign?> --> factor<sign, true> {('*'|'/') factor<false, true>};
%     factor<sign?, power?> if sign && power --> ['+'|'-'] factor<false, true>;
%     factor<sign?, power?> if !sign && power --> factor<false, false> ['^' factor<false, false>];
%     factor<sign?, power?> if !sign && !power --> number | '(' expr ')';
%     number --> digits['.' digits].



expr(Stream, Tok, Acc)
->
    {CurrTok, Res, S2} = term(true, Tok, Stream, Acc),
    expr_loop(Res, CurrTok, S2)
.

expr_loop(Res, CurrTok, Stream)
->
    if
        tok_Plus == CurrTok orelse tok_Minus == CurrTok
        ->
            {CurrTok2, Acc2, S2} = match(CurrTok, CurrTok, Stream),
            {T3, Rhs, S3} = term(false, CurrTok2, S2, Acc2),
            Res2 =  if
                        tok_Plus == CurrTok
                        -> Res + Rhs;
                        true
                        -> Res - Rhs
                    end,
            expr_loop(Res2, T3, S3);

        true
        ->
            {CurrTok, Res, Stream}
    end
.

%-------------------------------------------------------------

term(Sign, CurrTok, Stream, Acc)
->
    {CurrTok2, Res, S2} = factor(Sign, true, CurrTok, Stream, Acc),
    term_loop(Res, CurrTok2, S2)
.

term_loop(Res, CurrTok, Stream)
->
    if
        tok_Mul == CurrTok orelse tok_Div == CurrTok
        ->
            {CurrTok2, Acc2, S2} = match(CurrTok, CurrTok, Stream),
            {T3, Rhs, S3} = factor(false, true, CurrTok2, S2, Acc2),
            Res2 =  if
                        tok_Mul == CurrTok
                        -> Res * Rhs;
                        true
                        ->  case Rhs of
                                0 -> halt();
                                0.0 -> halt();
                                _ -> Res / Rhs
                            end
                    end,
            term_loop(Res2, T3, S3);

        true
        ->
            {CurrTok, Res, Stream}
    end
.

%-------------------------------------------------------------

factor(Sign, Power, CurrTok, Stream, Acc)
->
    factor_branch(Sign, Power, CurrTok, Stream, Acc)
.

factor_branch(true, true, CurrTok, Stream, Acc)
->
    {Neg, T, S, A} = if
                        tok_Plus == CurrTok orelse tok_Minus == CurrTok
                        ->
                            {CT2, Acc2, S2} = match(CurrTok, CurrTok, Stream),
                            Neg2 = (tok_Minus == CurrTok),
                            {Neg2, CT2, S2, Acc2};
                        true
                        ->
                            {false, CurrTok, Stream, Acc}
                     end,
    Ret = {T2, Res, S3} = factor(false, true, T, S, A),
    if
        Neg
        -> {T2, -Res, S3};
        true
        -> Ret
    end
;

factor_branch(false, true, CurrTok, Stream, Acc)
->
    Ret = {T2, Res, S2} = factor(false, false, CurrTok, Stream, Acc),
    if
        T2 == tok_Pow
        ->
            {T3, Acc2, S3} = match(T2, tok_Pow, S2),
            {T4, Rhs, S4} = factor(false, false, T3, S3, Acc2),
            {T4, math:pow(Res, Rhs), S4};

        true
        -> Ret
    end
;

factor_branch(false, false, CurrTok, Stream, Acc)
->
    if
        tok_LParen == CurrTok
        orelse tok_LBrace == CurrTok
        orelse tok_LSqrBrace == CurrTok
        ->
            Rhs =   case CurrTok of
                        tok_LParen -> tok_RParen;
                        tok_LBrace -> tok_RBrace;
                        tok_LSqrBrace -> tok_RSqrBrace;
                        _ -> tok_None
                    end,
            {T2, Acc2, S2} = match(CurrTok, CurrTok, Stream),
            {T3, Res, S3} = expr(S2, T2, Acc2),
            {T4, _, S4} = match(T3, Rhs, S3),
            {T4, Res, S4};

        tok_Num == CurrTok
        ->
            {T2, _, S2} = match(CurrTok, tok_Num, Stream),
            if
                tok_Dot == T2
                ->
                    {T3, Acc2, S3} = match(T2, tok_Dot, S2),
                    Int = lists:droplast(Acc),
                    Fra = lists:droplast(Acc2),
                    Str = Int ++ "." ++ Fra,
                    {Res, _} = string:to_float(Str),
                    {T4, _, S4} = match(T3, tok_Num, S3),
                    {T4, Res, S4};

                true
                ->
                    {Res, _} = string:to_integer(Acc),
                    {T2, Res, S2}
            end;

        true
        -> {CurrTok, 0.0, Stream}
    end
.
