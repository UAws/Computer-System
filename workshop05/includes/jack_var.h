#ifndef LIBRARIES_JACK_VAR_H
#define LIBRARIES_JACK_VAR_H

#include <string>

namespace CS_Symbol_Tables
{
    // to shorten the code
    typedef std::string string ;

    // *********** Jack Variable Objects ***********
    //
    // If an error occurs fatal_error will be called with an exit status of -1 and
    // the associated error message cannot be suppressed
    //
    // Errors include
    // - insufficient memory to create a new variable
    // - passing an invalid variable to a function
    //

    // a jack_var object represents a variable with a name, a type, a segment and an offset
    class _jack_var ;
    typedef _jack_var *jack_var ;

    extern jack_var jack_var_create(string name,string type,string segment,int offset) ;
    extern string   jack_var_name(jack_var variable) ;
    extern string   jack_var_type(jack_var variable) ;
    extern string   jack_var_segment(jack_var variable) ;
    extern int      jack_var_offset(jack_var variable) ;

}
#endif //LIBRARIES_JACK_VAR_H

