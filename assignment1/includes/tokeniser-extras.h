#ifndef ASSIGNMENT_TOKENISER_EXTRAS_H
#define ASSIGNMENT_TOKENISER_EXTRAS_H

#include "tokeniser.h"

// Interface to extra support function for the tokeniser
namespace Assignment_Tokeniser
{
    // character groupings to make parsing code simpler
    // for each grammar rule that can be started by more than one character we define a character group
    // but not for keywords which are not explicitly parsed, or input or token
    // for use with next_char_isa(), next_char_mustbe() and did_not_find_char()
    enum Char:int
    {
        cg_min = 0x70000000,    // make all character groups start outside the legal uni-code range

        cg_wspace,              // characters that start rule wspace
        cg_identifier,          // characters that start rule identifier
        cg_letter,              // characters that start rule letter
        cg_number,              // characters that start rule number
        cg_integer,             // characters that start rule integer
        cg_scientific,          // characters that start rule scientific
        cg_digit,               // characters that start rule digit
        cg_digit19,             // characters that start rule digit19
        cg_exponent,            // characters that start rule exponent
        cg_eee,                 // characters that start rule eee
        cg_sign,                // characters that start rule sign
        cg_instring,            // characters that start rule instring
        cg_telegu,              // characters that start rule telegu
        cg_symbol,              // characters that start rule symbol
        cg_comment_char,        // characters that start rule comment_char
        cg_armenian             // characters that start rule armenian
    } ;

    // ***** the following are implemented in the pre-compiled object files *****

    // ***** Input functions *****
 
    // read the next character if not at the end of input
    // all input must be read using this function
    // the first call initialises the tokeniser
    extern void read_next_char() ;

    // ***** Parsing functions *****
 
    // report an error and exit because the next character cannot start a token
    extern void did_not_find_start_of_token() ;

    // report an error and exit because the next character in the input is not expected?
    extern void did_not_find_char(int expected) ;

    // return true if the next character is expected, otherwise false
    // this is implemented by calling char_isa()
    extern bool next_char_isa(int expected) ;

    // if the next character is expected call read_next_char(),
    // otherwise call did_not_find_char()
    extern void next_char_mustbe(int expected) ;

    // ***** Token functions *****
 
    // if s is a keyword it returns its token kind otherwise it returns tk_identifier
    extern TokenKind keyword_or_identifier(string s) ;

    // create a new Token object of the given kind using all characters parsed since the last call
    // it calls classify_spelling() to work out the kind of token that was parsed
    // it calls correct_spelling() to work out the spelling of the token
    extern Token new_token() ;

    // ***** the following are implemented in the tokeniser-extras.cpp file *****

    // check if a char matches another char or is a member of a character group
    // eg char_isa(':',cg_symbol) returns true
    // eg char_isa('-',cg_identifier) returns false
    extern bool char_isa(int ch,int cg) ;

    // work out the kind of a parsed token, this calls keyword_or_identifier() if a token looks like an identifier
    extern TokenKind classify_spelling(string spelling) ;

    // work out the correct spelling to use in the Token object being created by new_token()
    extern string correct_spelling(TokenKind kind,string spelling) ;

}

#endif //ASSIGNMENT_TOKENISER_EXTRAS_H
