// test token groups and context strings
#include "tokeniser.h"
#include "iobuffer.h"
#include <vector>

using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Assignment_Tokeniser ;

// display token with context
void display_context(Token token,int count)
{
    // its string representation
    write_to_output("token: " + std::to_string(count)) ;
    write_to_output("\n    " + to_string_verbose(token) + "\n") ;

    // its context string
    write_to_output("token_context:\n") ;
    write_to_output(token_context(token)+"\n") ;
    write_to_output("----------------\n") ;
}

// this main program tokenises standard input and
// prints details of every 5th token in reverse order
int main(int argc,char **argv)
{
    Token token ;
    vector<Token> tokens ;
    int count = 0 ;

    // configure io buffers to output immediately
    config_output(iob_immediate) ;
    config_errors(iob_immediate) ;

    // wrap tokenising in a try catch - just in case
    try
    {
        // remember each token as it is read
        token = read_next_token() ;
        while ( token_kind(token) != tk_eoi )
        {
            tokens.push_back(token) ;
            count++ ;
            token = read_next_token() ;
        }
    }
    catch ( std::string e )
    {
        // silently ignore hoping it was thrown by did_not_find_char()
    }
    write_to_output("Read " + to_string(count) + " tokens\n") ;

    if ( count > 0 )
    {
        write_to_output("Token Contexts:\n\n") ;

        // display first token's context
        display_context(tokens[0],0) ;

        // display last token's context and every 5th before it in reverse order
        for ( int i = count - 1 ; i > 0 ; i -= 5 )
        {
            display_context(tokens[i],i) ;
        }
    }

    // flush output - the test scripts disable unbuffered output
    print_output() ;
    print_errors() ;

    return 0 ;
}
