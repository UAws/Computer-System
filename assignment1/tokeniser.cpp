// Author : Akide Liu
// Date : 30/8/21
// Student ID : A1743748
// Description :
// tokeniser implementation for the workshop example language

#include "iobuffer.h"
#include "tokeniser-extras.h"

// to shorten the code
using namespace std;
using namespace CS_IO_Buffers;

// we are extending the Assignment_Tokeniser namespace
namespace Assignment_Tokeniser
{

    // comment
    static void parse_comment_char()
    {
        while (next_char_isa(cg_comment_char))
            next_char_mustbe(cg_comment_char);
    }

    //   - eol_suffix   ::= '/' comment_char* '\n'
    static void parse_eol_suffix()
    {
        next_char_mustbe('/');
        while (next_char_isa(cg_comment_char))
            parse_comment_char();
        next_char_mustbe('\n');
    }

    // * eol_comment    ::= '/' eol_suffix
    //   - eol_suffix   ::= '/' comment_char* '\n'
    //   - comment_char ::= '\t'|'\r'|' '-'~'|armenian
    //   - armenian     ::= all unicode code-points from 0x0530 to 0x058F and 0xFB13 to 0xFB17
    static void parse_eol_comment()
    {
        next_char_mustbe('/');

        if (next_char_isa('/'))
            parse_eol_suffix();
    }

    // identifier ::= ('a'-'z'|'A'-'Z'|'$') letter*
    static void parse_identifier()
    {
        next_char_mustbe(cg_identifier);
        while (next_char_isa(cg_letter))
            next_char_mustbe(cg_letter);
    }

    // integer ::= '0' | (digit19 digit*)
    static void parse_integer()
    {
        if (next_char_isa('0'))
        {
            next_char_mustbe('0');
            return;
        }
        else
        {
            if (next_char_isa(cg_digit19))
                next_char_mustbe(cg_digit19);
            while (next_char_isa(cg_digit))
                next_char_mustbe(cg_digit);
        }
    }

    // assign_eq ::= '=' | '=='
    static void parse_assign_eq()
    {
        next_char_mustbe('=');
        if (next_char_isa('='))
            next_char_mustbe('=');
    }

    // tk_mult,    // '*='
    static void parse_tk_mult()
    {
        next_char_mustbe('*');
        next_char_mustbe('=');
    }

    // tk_ne,      // '~='
    static void parse_tk_ne()
    {
        next_char_mustbe('~');
        if (next_char_isa('='))
            next_char_mustbe('=');
    }

    // tk_ll_shift,        // '<<<'
    // tk_la_shift,        // '<<'
    static void parse_tk_ll_shift()
    {
        next_char_mustbe('<');
        next_char_mustbe('<');
        if (next_char_isa('<'))
            next_char_mustbe('<');
    }

    // tk_rl_shift,        // '>>>'
    // tk_ra_shift,        // '>>'
    static void parse_tk_rl_shift()
    {
        next_char_mustbe('>');
        next_char_mustbe('>');
        if (next_char_isa('>'))
            next_char_mustbe('>');
    }

    // symbol ::= '@'|'~'|'='|'-'|'+'|'/'|'*='|'~='|'=='|'<<<'|'<<'|'>>>'|'>>'|'{'|'}'|'('|')'|'['|']'|'.'
    static void parse_symbol()
    {
        if (next_char_isa('@'))
            next_char_mustbe('@');
        else if (next_char_isa('-'))
            next_char_mustbe('-');
        else if (next_char_isa('+'))
            next_char_mustbe('+');
        else if (next_char_isa('/'))
            parse_eol_comment();
        else if (next_char_isa('{'))
            next_char_mustbe('{');
        else if (next_char_isa('}'))
            next_char_mustbe('}');
        else if (next_char_isa('('))
            next_char_mustbe('(');
        else if (next_char_isa(')'))
            next_char_mustbe(')');
        else if (next_char_isa('['))
            next_char_mustbe('[');
        else if (next_char_isa(']'))
            next_char_mustbe(']');
        else if (next_char_isa('.'))
            next_char_mustbe('.');
        else if (next_char_isa('='))
            parse_assign_eq();
        else if (next_char_isa('*'))
            parse_tk_mult();
        else if (next_char_isa('~'))
            parse_tk_ne();
        else if (next_char_isa('<'))
            parse_tk_ll_shift();
        else if (next_char_isa('>'))
            parse_tk_rl_shift();
        else
            did_not_find_start_of_token();
    }

    //   - exponent ::= eee sign? integer?
    //   - eee      ::= 'e'|'E'
    //   - sign     ::= '+'|'-'
    static void parse_exponent()
    {
        next_char_mustbe(cg_eee);
        if (next_char_isa(cg_sign))
            next_char_mustbe(cg_sign);
        if (next_char_isa(cg_integer))
            parse_integer();
    }

    //   - fraction ::= '.' digit*
    static void parse_fraction()
    {
        next_char_mustbe('.');
        while (next_char_isa(cg_digit))
            next_char_mustbe(cg_digit);
        if (next_char_isa(cg_eee))
            parse_exponent();
    }

    // * scientific ::= integer fraction exponent?
    static void parse_scientific()
    {
        parse_integer();
        if (next_char_isa('.'))
            parse_fraction();
    }

    // * hash_comment   ::= '#' comment_char* '\n'
    static void parse_hash_comment()
    {
        next_char_mustbe('#');
        while (next_char_isa(cg_comment_char))
            parse_comment_char();
        next_char_mustbe('\n');
    }

    // * string ::= '"' instring* '"'
    //   - instring ::= ' '|'!'|'#'-'~'|telegu
    //   - telegu ::= all unicode code-points from 0x0C00 to 0x0C7F
    static void parse_string()
    {
        next_char_mustbe('"');
        while (next_char_isa(cg_instring))
            next_char_mustbe(cg_instring);
        next_char_mustbe('"');
    }

    // token ::= ...
    static void parse_token()
    {
        if (next_char_isa(cg_wspace))
            read_next_char();
        else if (next_char_isa('/'))
            parse_eol_comment();
        else if (next_char_isa('#'))
            parse_hash_comment();
        else if (next_char_isa('"'))
            parse_string();
        else if (next_char_isa(cg_scientific))
            parse_scientific();
        else if (next_char_isa(cg_symbol))
            parse_symbol();
        else if (next_char_isa(cg_identifier))
            parse_identifier();
        else if (next_char_isa(EOF))
            ;
        else
            did_not_find_start_of_token();
    }

    // parse the next token in the input and return a new
    // Token object that describes its kind and spelling
    // Note: you must not call new_token() anywhere else in your program
    // Note: you should not modify this function
    Token read_next_token()
    {
        parse_token();
        return new_token();
    }
}