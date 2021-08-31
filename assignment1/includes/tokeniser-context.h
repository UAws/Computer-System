#ifndef ASSIGNMENT_TOKENISER_CONTEXT_H
#define ASSIGNMENT_TOKENISER_CONTEXT_H

#include <string>

// interface to the input memory required to build tokens and token context strings
namespace Assignment_Tokeniser
{
    // a shorthand for string
    typedef std::string string ;

    // the line number of the next character to be remembered, starts at 1
    extern int get_line() ;

    // the column number of the next character to be remembered, starts at 1
    // column numbers are in bytes not characters
    extern int get_column() ;

    // remember the UTF-8 encoding of a unicode character in the range 0 to 0x10FFFF
    // you may assume that only legal code points will be passed to this function
    extern void remember(int ch) ;

    // return a remembered line with trailing newline character
    // the last line may or may not have a trailing newline character
    // returns "" if line_number is less than 1 or line_numbern is greater than the last remembered line
    // the string is encoded in UTF-8
    extern string remembered_line(int line_number) ;

}
#endif //ASSIGNMENT_TOKENISER_CONTEXT_H
