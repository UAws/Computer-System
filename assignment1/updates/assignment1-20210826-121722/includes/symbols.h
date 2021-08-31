#ifndef LIBRARIES_SYMBOLS_H
#define LIBRARIES_SYMBOLS_H

#include <string>
#include "jack_var.h"

namespace CS_Symbol_Tables
{
    // to shorten the code
    typedef std::string string ;

    // *********** Lookup Tables ***********
    //
    // A symbol table is a way of recording a mapping from a string key to a value
    // Three kinds of table are provided to allow mappings to strings, integers or variable descriptions
    //
    // Each table has a private implementation access via what appears to be a pointer
    //
    // Each table has the following functions:
    // *_create this creates a new empty table
    // *_delete this destroys an existing table
    // *_insert this inserts a new key to value mapping into the table or returns false if the key is already in the table
    // *_update this inserts a new key to value mapping into the table replacing an existing mapping if the key is already in the table
    // *_lookup this returns the value that the given key maps to or returns a specified value if the key is not in the table
    //
    // The jack_symbols table has some extra features
    // jsymbols_push   this creates a new symbol table and uses its parameter an alternate for failed lookups
    // jsymbols_pop    this deletes a symbol table and returns its alternate table or nullptr
    // jsymbols_offset this returns the next offset in a named segment
    //                 . the first request remembers the segment name, an offset of 0 and returns 0, 
    //                 . subsequent requests increment and return the named segment's offset
    //
    // If an error occurs fatal_error will be called with an exist status of -1 and the associated error message cannot be suppressed
    // Errors include
    // - insufficient memory to create a new table
    // - passing an invalid table to a function
    //

    // ***************  Symbol table mapping strings to int values  *********************

    // a symbol table from string to int, failed lookups return -1
    class _isymbols ;
    typedef _isymbols *isymbols ;

    extern isymbols isymbols_create() ;
    extern void     isymbols_delete(isymbols table) ;
    extern bool     isymbols_insert(isymbols table,string symbol,int value) ;
    extern void     isymbols_update(isymbols table,string symbol,int value) ;
    extern int      isymbols_lookup(isymbols table,string symbol) ;

    // ***************  Symbol table mapping strings to string values  *********************

    // a symbol table from string to string, failed lookups return ""
    class _symbols ;
    typedef _symbols *symbols ;

    extern symbols symbols_create() ;
    extern void     symbols_delete(symbols table) ;
    extern bool     symbols_insert(symbols table,string symbol,string value) ;
    extern void     symbols_update(symbols table,string symbol,string value) ;
    extern string   symbols_lookup(symbols table,string symbol) ;

    // ***************  Symbol table mapping strings to variable values  *********************

    // a symbol table from string to jsymbol, failed lookups return nullptr 
    class _jsymbols ;
    typedef _jsymbols *jsymbols ;

    extern jsymbols jsymbols_create() ;
    extern void     jsymbols_delete(jsymbols table) ;
    extern jsymbols jsymbols_push(jsymbols alternate) ;
    extern jsymbols jsymbols_pop(jsymbols table) ;
    extern bool     jsymbols_insert(jsymbols table,string symbol,string type,string segment,int offset) ;
    extern void     jsymbols_update(jsymbols table,string symbol,string type,string segment,int offset) ;
    extern jack_var jsymbols_lookup(jsymbols table,string symbol) ;
    extern int      jsymbols_offset(jsymbols table,string segment) ;


    // ***************   EXAMPLES OF HOW TO USE THE SYMBOL TABLES   *********************

    // ***************  Symbol table mapping strings to int values  *********************

    // Example of how to use an int symbol table
    //
    // Create a new empty table
    //     symbols mytable = isymbols_create() ;
    //
    // Now add a mapping from "bob" to 73, this will not report an error because "bob" is not in mytable
    //     if ( !isymbols_insert(mytable,"bob",73) ) /* report an error somehow? */
    //
    // Now lookup "bob"'s value, this will return 73
    //     x = isymbols_lookup(mytable,"bob") ;
    //
    // Now change "bob"'s 73 to 17 
    //     isymbols_update(mytable,"bob",17) ;
    //
    // Now lookup "bob"'s value, this will return 17
    //     x = isymbols_lookup(mytable,"bob") ;
    //
    // Now lookup "bill"'s value, this will return -1 because "bill" is not in mytable
    //     x = isymbols_lookup(mytable,"bill") ;
    //
    // Now delete the table
    //     isymbols_delete(mytable) ;
    //

    // ***************  Symbol table mapping strings to string values  *********************

    // Example of how to use a string symbol table
    //
    // Create a new empty table
    //     symbols mytable = symbols_create() ;
    //
    // Now add a mapping from "bob" to "hello mum", this will not report an error because "bob" is not in mytable
    //     if ( !symbols_insert(mytable,"bob","hello mum") ) { /* report an error somehow? */ }
    //
    // Now change "bob"'s "hello mum" to "bye bye mum"
    //     symbols_update(mytable,"bob","bye bye mum") ;
    //
    // Now lookup "bob"'s value, this will return "bye bye mum"
    //     y = symbols_lookup(mytable,"bob") ;
    //
    // Now lookup "bill"'s value, this will return "" because "bill" is not in mytable
    //     y = symbols_lookup(mytable,"bill") ;
    //
    // Now delete the table
    //     symbols_delete(mytable) ;
    //

    // ***********  Symbol table mapping strings to variable objects  *************

    // Example of how to use a variable symbol table
    //
    // Create a new table
    //     jsymbols mytable = jsymbols_create() ;
    //
    // Now add a jack variable named "bob" of type "int" in segment "local" at offset 2,
    // this will not report an error because "bob" is not in mytable
    //     if ( !jsymbols_insert(mytable,"bob","int","local",2) ) { /* report an error somehow? */ }
    //
    // Now change "bob"'s offset to 5
    //     jsymbols_update(mytable,"bob","int","local",5) )
    //
    // Now lookup "bob"'s offset, this will return 5
    //     jack_var temp = jsymbols_lookup(mytable,"bob") ;
    //     z = jack_var_offset(temp) ;
    //
    // Now lookup "bill"'s offset, this will result in nullptr because "bill" is not in mytable
    //     temp = jsymbols_lookup(mytable,"bill") ;
    //
    // Now create a new symbol table using the existing one as an alternate
    //     mytable = jsymbols_push(mytable) ;
    //
    // Now lookup "bob", "bob" is not in the table but failed lookups use the alternate so can still find "bob"
    //     temp = jsymbols_lookup(mytable,"bob") ;
    //
    // Now add a new "bob" of type "xyz" in segment "gerbil" at the next free offset in segment "gerbil"
    //     int offset = jsymbols_offset(mytable,"gerbil") ;
    //     if ( !jsymbols_insert(mytable,"bob","xyz","gerbil",offset) ) { /* report an error somehow? */ }
    //
    // Now lookup "bob", this will return the new "bob" of type "xyz"
    //     temp = jsymbols_lookup(mytable,"bob") ;
    //     string vt = jack_var_type(temp) ;
    //
    // Now delete the current symbol table and go back to using the previous one
    //     mytable = jsymbols_pop(mytable) ;
    //
    // Now lookup "bob", this will return the old "bob" of type "int"
    //     temp = jsymbols_lookup(mytable,"bob") ;
    //     vt = jack_var_type(temp) ;
    //
    // Now delete the table
    //     jsymbols_delete(mytable) ;
    //

}
#endif //LIBRARIES_SYMBOLS_H

