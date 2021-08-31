#ifndef WORKSHOP_TOKENISER_H
#define WORKSHOP_TOKENISER_H

#include <string>

// Interface for the workshop tokeniser
namespace Workshop_Tokeniser
{
    // shorthand name for the string type
    typedef std::string string ;

    // the ID used to reference a Token - the implementation is hidden
    class _Token ;
    typedef _Token *Token ;

    // The kinds of token that are recognised?
    enum TokenKind : unsigned int
    {
        // *** BNF syntax for tokens
        // * literals inside ' '
        // * grouping ( )
        // * ASCII ranges -
        // * alternatives |
        // * 0 or 1 ?
        // * 0 or more *
        //

        // Workshop Example Language Tokeniser Tokens
        // Grammar rule:    Definition
        // input:           token* eoi
        // token:           wspace | identifier | label | integer | op | varop | symbol
        // wspace:          '\t' | '\n' | '\r' | ' '
        // identifier:      letter alnum*
        // label:           '(' identifier ')'
        // letter:          ('a'-'z'|'A'-'Z')
        // alnum:           letter | digit
        // integer:         '0' | (digit19 digit*)
        // digit19:         ('1'-'9')
        // digit:           ('0'-'9')
        // op:              '+' | '-' | '*' | '/'
        // varop:           lt_le | assign_eq | not_ne | gt_gte
        // lt_le:           '<' | '<='
        // assign_eq:       '=' | '=='
        // not_ne:          '!' | '!='
        // gt_gte:          '>' | '>='
        // symbol:          '@' | '{' | '}' | ':' | ';' | '.' | ',' | '"'
        // eoi:             the end of the input
        //
        // * all input after returning an eoi token is ignored
        // * the only acceptable characters in the input are those used in the grammar
        // * once an identifier is found it is checked to see if its a keyword
        // * the keyword in the workshop are:
        // keyword:         'var' | 'while' | 'if' | 'else' | 'let'
        //
        // * the token returned for a label has the '(' and ')' removed from its spelling
        //

                            // the following token kinds are returned by the tokeniser
                            // the wpsace tokens
        tk_tab,             // '\t'
        tk_newline,         // '\n'
        tk_carriage_return, // '\r'
        tk_space,           // ' '

        tk_identifier,      // any identifier

        tk_label,           // any label

        tk_integer,         // any integer

                            // the op tokens
        tk_add,             // '+'
        tk_sub,             // '-'
        tk_times,           // '*'
        tk_divide,          // '/'

                            // the varop tokens
        tk_lt,              // '<'
        tk_le,              // '<='
        tk_assign,          // '='
        tk_eq,              // '=='
        tk_not,             // '!'
        tk_ne,              // '!='
        tk_ge,              // '>='
        tk_gt,              // '>'

                            // the symbol tokens
        tk_at,              // '@'
        tk_lcb,             // '{'
        tk_rcb,             // '}'
        tk_colon,           // ':'
        tk_semi,            // ';'
        tk_dot,             // '.'
        tk_comma,           // ','
        tk_dquote,          // '"'

                            // the keyword tokens
        tk_var,             // 'var'
        tk_while,           // 'while'
        tk_if,              // 'if'
        tk_else,            // 'else'
        tk_let,             // 'let'

        tk_eoi,             // end of input reached
        tk_oops             // just in case
    } ;

    // ***** the following are implemented in the pre-compiled lib/libcs_w05.a object file *****

    // return a string representation of the given token kind or ""
    extern string to_string(TokenKind kind) ;

    // return the TokenKind for the given token
    extern TokenKind token_kind(Token token) ;

    // return the spelling for the given token
    extern string token_spelling(Token token) ;

    // return a string representation of the given token
    extern string to_string(Token token) ;

    // ***** the workshop involves implementing the following functions by completing tokeniser.cpp *****

    // read the next token from standard input - the first call initialises the tokeniser
    extern Token read_next_token() ;
}

#endif //WORKSHOP_TOKENISER_H
