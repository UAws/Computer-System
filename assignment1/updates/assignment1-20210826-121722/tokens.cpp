// convert Text into Tokens 
#include "tokeniser.h"
#include "iobuffer.h"

using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Assignment_Tokeniser ;

// this main program tokenises standard input and
// prints the tokens as they are found
int main(int argc,char **argv)
{
    Token token ;
    int count = 0 ;

    // configure io buffers to output immediately
    config_output(iob_immediate) ;
    config_errors(iob_immediate) ;

    // disable error context in parsing errors
    disable_error_context() ;

    // wrap this all in a try catch - just in case
    try
    {
        // display each token as it is read
        token = read_next_token() ;
        while ( token_kind(token) != tk_eoi )
        {
            write_to_output(to_string(token)+"\n") ;
            count++ ;
            token = read_next_token() ;
        }
    }
    catch ( std::string e )
    {
        // silently ignore hoping it was thrown by did_not_find_char()
    }
    write_to_output("Read " + to_string(count) + " tokens\n") ;

    // flush output - the test scripts disable unbuffered output
    print_output() ;
    print_errors() ;

    return 0 ;
}
