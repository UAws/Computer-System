#include "iobuffer.h"
#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;
using namespace CS_IO_Buffers ;

namespace Assignment_Tokeniser
{
    // check if a char matches another char or is a member of a character group
    // eg char_isa('3',cg_digit) returns true
    bool char_isa(int ch,int cg)
    {
        // single character groups use their character as their value
        // do not add them to the switch statement

        if ( ch == cg ) return true ;

        switch(cg)
        {
        case cg_wspace:              // characters that start rule wspace
            switch(ch)
            {
            case '\t':
            case '\n':
            case '\r':
            case ' ':
                return true ;
            default:
                return false ;
            }
                                    // complete a switch statement for each of the following
        case cg_identifier:         // characters that start rule identifier
        case cg_letter:             // characters that start rule letter
        case cg_number:             // characters that start rule number
        case cg_integer:            // characters that start rule integer
        case cg_scientific:         // characters that start rule scientific
        case cg_digit:              // characters that start rule digit
        case cg_digit19:            // characters that start rule digit19
        case cg_exponent:           // characters that start rule exponent
        case cg_eee:                // characters that start rule eee
        case cg_sign:               // characters that start rule eee
        case cg_instring:           // characters that start rule instring
        case cg_telegu:             // characters that start rule telegu
        case cg_symbol:             // characters that start rule symbol
        case cg_comment_char:       // characters that start rule comment_char
        case cg_armenian:           // characters that start rule armenian

        default:
            return false ;
        }
    }

    // work out the kind of a parsed token, this may call keyword_or_identifier()
    // the spelling is a valid token or "" if at end of input
    TokenKind classify_spelling(string spelling)
    {
        if ( spelling == "" ) return tk_eoi ;

        return tk_identifier ;
    }

    // work out the correct spelling to use in the Token object being created by new_token()
    // the spelling is a valid token and kind is its kind
    string correct_spelling(TokenKind kind,string spelling)
    {
        if ( spelling == "" ) return "" ;

        return spelling ;
    }

}
