// convert Text into Tokens 
#include "tokeniser.h"
#include "iobuffer.h"
#include <iostream>

using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Workshop_Tokeniser ;

// this main program tokenises standard input and
// prints the tokens as they are found
int main(int argc,char **argv)
{
    Token token ;
    int count = 0 ;

    // configure io buffers to output immediately - ignored by tests scripts
    config_output(iob_immediate) ;
    config_errors(iob_immediate) ;

    // count all tokens read but only display non whitespace
    try
    {
        token = read_next_token() ;
        while ( token_kind(token) != tk_eoi )
        {
            write_to_output(to_string(token)+"\n") ;
            token = read_next_token() ;
            count++ ;
        }
        write_to_output("read " + to_string(count) + " tokens\n") ;
    }
    catch ( string e )
    {
        // ignore in the hope if was thrown by did_not_find_char()
    }

    print_output() ;
    print_errors() ;
    return 0 ;
}
