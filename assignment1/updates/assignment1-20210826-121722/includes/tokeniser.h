#ifndef ASSIGNMENT_TOKENISER_H
#define ASSIGNMENT_TOKENISER_H

#include <string>

// Interface to the assignment tokeniser
namespace Assignment_Tokeniser
{
    // a shorthand for string
    typedef std::string string ;

    // the ID used to reference a Token - the implementation is hidden
    class _Token ;
    typedef _Token *Token ;

    // ***** BNF *****
    //
    // *** SYNTAX
    // * ASCII literals inside ' '
    // * grouping ( )
    // * ASCII ranges -
    // * alternatives |
    // * 0 or 1 ?
    // * 1 or more +
    // * 0 or more *
    //
    // *** TOKEN RULES
    // * Grammar rule ::= Definition
    //
    // * input ::= token* eoi
    //
    // * token ::= wspace | identifier | number | string | keyword | symbol | eol_comment | hash_comment
    //
    // * wspace ::= '\t' | '\n' | '\r' | ' '
    //
    // * identifier ::= ('a'-'z'|'A'-'Z'|'$') letter*
    //   - letter ::= 'a'-'z'|'A'-'Z'|'0'-'9'|'-'|'$'|'.'
    //
    // * number ::= integer | scientific
    //
    // * integer   ::= '0' | (digit19 digit*)
    //   - digit   ::= '0'-'9'
    //   - digit19 ::= '1'-'9'
    //
    // * scientific ::= integer fraction exponent?
    //   - fraction ::= '.' digit*
    //   - exponent ::= eee sign? integer?
    //   - eee      ::= 'e'|'E'
    //   - sign     ::= '+'|'-'
    //
    // * string ::= '"' instring* '"'
    //   - instring ::= ' '|'!'|'#'-'~'|telegu
    //   - telegu ::= all unicode code-points from 0x0C00 to 0x0C7F
    //
    // * symbol ::= '@'|'~'|'='|'-'|'+'|'/'|'*='|'~='|'=='|'<<<'|'<<'|'>>>'|'>>'|'{'|'}'|'('|')'|'['|']'|'.'
    //   - symbols each have their own TokenKind
    //
    // * hash_comment   ::= '#' comment_char* '\n'
    // * eol_comment    ::= '/' eol_suffix
    //   - eol_suffix   ::= '/' comment_char* '\n'
    //   - comment_char ::= '\t'|'\r'|' '-'~'|armenian
    //   - armenian     ::= all unicode code-points from 0x0530 to 0x058F and 0xFB13 to 0xFB17
    //
    // * keyword ::= 'constructor'|'function'|'if-goto'|'procedure'|'this'
    //   - keywords each have their own TokenKind
    //
    //
    // ** PARSING
    // * the functions read_next_char(), next_char_isa() and next_char_mustbe() must be used to parse tokens
    // * all input must be read using the functions read_next_char() or next_char_mustbe()
    // * the function did_not_find_char() must be used to report not finding an expected character or group
    // * do not attempt to parse keywords, they are identified later by the function classify_token()
    // 
    // * PARSING AMBIGUITIES
    // * '/' can start 2 rules: symbol and eol_comment
    //   . after parsing the '/' the next character will tell you which of 2 cases you are parsing
    //   . '/' its an eol_comment
    //   . anything else its '/'
    //
    // ** CHANGES TO TOKEN SPELLING by the function token_spelling()
    // * no spelling changes are required by the MILESTONE TESTS
    // * after parsing and creating a token, modify the token's spelling as follows:
    // * tk_string:
    //    . the '"' and '"' enclosing a string must be removed from the token's spelling
    // * tk_eol_comment:
    //    . the '//' and '\n' enclosing an eol_comment must be removed from the token's spelling
    // * tk_hash_comment:
    //    . the '#' and '\n' enclosing an hash_comment must be removed from the token's spelling
    // * tk_scientific:
    //   . if there is no exponent add one
    //   . the exponent must start with "e" and have a sign
    //   . the spelling for all representations of 0 must be "0"
    //   . scientific numbers must be normalised to have the first non-0 digit before the '.'
    //   . after normalisation, all trailing 0s must be removed from a fraction
    //   . if a fraction has no digits, after removing trailing 0s, the '.' is removed too
    //
    // ** SPECIFIC REQUIREMENTS
    // * there must be a single call to the function keyword_or_identifier()
    // * there must be a single call to the function new_token()
    //
    // ** EXPECTED TEST OUTPUTS
    // * you should consult the expected test outputs if you need help interpreting the token definitions
    //   . 'make Show' can be used to see the names of all files used by each test
    //


    // The kinds of token that are recognised
    enum TokenKind
    {
        // MILESTONE TESTS Tokens to be recognised:

        tk_tab,             // '\t'
        tk_newline,         // '\n'
        tk_carriage_return, // '\r'
        tk_space,           // ' '

        tk_identifier,      // identifier
        tk_integer,         // integer
        tk_scientific,      // scientific

                            // the single character symbol tokens
        tk_at,              // '@'
        tk_not,             // '~'
        tk_assign,          // '='
        tk_sub,             // '-'
        tk_add,             // '+'
        tk_div,             // '/'
        tk_lcb,             // '{'
        tk_rcb,             // '}'
        tk_lrb,             // '('
        tk_rrb,             // ')'
        tk_lsb,             // '['
        tk_rsb,             // ']'
        tk_dot,             // '.'

        ////////////////////

        // FINAL TESTS Tokens to be recognised: all of the above and

        tk_hash_comment,    // '#' comment_char* '\n', '#' and '\n' are not part of the spelling
        tk_eol_comment,     // '//' comment_char* '\n', '//' and '\n' are not part of the spelling

        tk_string,          // '"' instring* '"', '"' and '"' are not part of the spelling

        tk_mult,            // '*='
        tk_ne,              // '~='
        tk_eq,              // '=='
        tk_ll_shift,        // '<<<'
        tk_la_shift,        // '<<'
        tk_rl_shift,        // '>>>'
        tk_ra_shift,        // '>>'

        tk_constructor,     // 'constructor'
        tk_function,        // 'function'
        tk_if_goto,         // 'if-goto'
        tk_procedure,       // 'procedure'
        tk_this,            // 'this'

        tk_extra_01,        // extra token kinds for testing / internal use
        tk_extra_02,
        tk_extra_03,
        tk_extra_04,
        tk_extra_05,
        tk_extra_06,
        tk_extra_07,
        tk_extra_08,
        tk_extra_09,
        tk_extra_10,
        tk_extra_11,
        tk_extra_12,
        tk_extra_13,
        tk_extra_14,
        tk_extra_15,
        tk_extra_16,
        tk_extra_17,
        tk_extra_18,
        tk_extra_19,
        tk_extra_20,

        ////////////////////

        tk_eoi,             // end of input reached

        tk_oops,            // for error handling

    } ;

    // ***** the following are implemented in the pre-compiled library files *****

    // return a string representation of the given token kind
    // returns "" if the kind is not recognised
    extern string to_string(TokenKind kind) ;

    // return the TokenKind for the given token
    // it is a fatal error if the token was not returned by a call to next_token()
    extern TokenKind token_kind(Token token) ;

    // return the characters that make up the given token
    // it is a fatal error if the token was not returned by a call to next_token()
    extern string token_spelling(Token token) ;

    // return a string representation of the given token
    // it is a fatal error if the token was not returned by a call to next_token()
    extern string to_string(Token token) ;

    // return a verbose string representation of the given token with additional details
    // it is a fatal error if the token was not returned by a call to next_token()
    extern string to_string_verbose(Token token) ;

    // return a context string for the given token showing its position in the input
    // it is a fatal error if the token was not returned by a call to next_token()
    extern string token_context(Token token) ;

    // to disable use of token_context in tokens tests
    extern void disable_error_context() ;

    // ***** the assignment involves implementing the following functions by completing tokeniser.cpp *****

    // read the next token from standard input
    extern Token read_next_token() ;

}
#endif //ASSIGNMENT_TOKENISER_H
