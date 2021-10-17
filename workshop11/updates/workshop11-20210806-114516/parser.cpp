#include "iobuffer.h"
#include "symbols.h"
#include "tokeniser.h"
#include "abstract-syntax-tree.h"

using namespace std ;

using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Workshop_Compiler ;

// ignore unused-function warnings in this source file
#pragma clang diagnostic ignored "-Wunused-function"

// the grammar we are recognising
// rules containing text literals are written using the matching tk_* or tg_* names
//
// TERM: DEFINITION
// program: declarations statement tk_eoi
// declarations: declaration*
// declaration: tk_var tg_type tk_identifier tk_semi
// statement:  while | if | let | sequence
// while: tk_while tk_lrb condition tk_rrb statement
// if: tk_if tk_lrb condition tk_rrb statement (tk_else statement)?
// let: tk_let tk_identifier tk_assign expression tk_semi
// sequence: tk_lcb statement* tk_rcb
// expression: term (tg_infix_op term)?
// condition: term tg_relop term
// term: tk_identifier | tk_integer
//
// Token groups for use with have()/have_next()/mustbe()/did_not_find():
// tg_statement - matches any token that can start a statement
// tg_term      - matches any token that can start a term
// tg_infix_op  - matches any token that can be used as an infix_op
// tg_relop     - matches any token that can be used as a relop
// tg_type      - matches any token that can be used as a type


// every parse function returns an abstract syntax tree representing what it has parsed
static ast parse_program() ;
static ast parse_declarations() ;
static ast parse_declaration() ;
static ast parse_statement() ;
static ast parse_while() ;
static ast parse_if() ;
static ast parse_let() ;
static ast parse_sequence() ;
static ast parse_expression() ;
static ast parse_condition() ;
static ast parse_term() ;

// note: we have added push/pop_error_context() calls so that
//       you can see part of the call chain when an error occurs
// note: conditions are represented as an infix expression in the abstract syntax tree

// *****           DO NOT EDIT THE CODE ABOVE           *****



/////////////////////////////////////////////////////////////////////////////
// ***** COMPLETE THE CODE BELOW TO COMPLETE THE PARSER *****

// In each parse_*() function write code to correctly initialise
// the variables passed to the tree node create*() function



// program: declarations statement tk_eoi
static ast parse_program()
{
    push_error_context("parse_program()") ;

    // a declaration node and a statement node
    ast decls = nullptr ;
    ast stat = nullptr ;

    // add parsing code here ...
    decls = parse_declarations() ;
    stat = parse_statement() ;
    mustbe(tk_eoi) ;

    // return a program node
    ast ret = create_program(decls,stat) ;
    pop_error_context() ;
    return ret ;
}

// declarations: declaration*
static ast parse_declarations()
{
    push_error_context("parse_declarations()") ;

    // a vector of declaration nodes
    vector<ast> decls ;

    // add parsing code here ...
    while ( have(tk_var) )
    {
        decls.push_back(parse_declaration()) ;
    }

    // return a declarations node
    ast ret = create_declarations(decls) ;
    pop_error_context() ;
    return ret ;
}

// declare a variable of the given type and return the variable's tree node
static ast declare_variable(Token identifier,string vartype,string varsegment) ;

// declaration: tk_var tg_type tk_identifier tk_semi
// the first identifier is the type
static ast parse_declaration()
{
    push_error_context("parse_declaration()") ;

    // a variable node returned by declare_variable()
    ast var = nullptr ;

    // NOTES:
    // * the identifier is the name of a variable
    // * declare the variable as being of type "int" in segment "static"

    // add parsing code here ...
    mustbe(tk_var) ;
    string type = token_spelling(mustbe(tg_type)) ;
    Token id = mustbe(tk_identifier) ;
    mustbe(tk_semi) ;
    var = declare_variable(id,type,"static") ;

    // return a declaration node
    ast ret = create_declaration(var) ;
    pop_error_context() ;
    return ret ;
}

// statement:  while | if | let | sequence
static ast parse_statement()
{
    push_error_context("parse_statement()") ;

    // a statement node
    ast stat = nullptr ;

    // add parsing code here ...
    switch(token_kind())
    {
    case tk_while:
        stat = parse_while() ;
        break ;
    case tk_if:
        stat = parse_if() ;
        break ;
    case tk_let:
        stat = parse_let() ;
        break ;
    case tk_lcb:
        stat = parse_sequence() ;
        break ;
    default:
        did_not_find(tg_statement) ;
        break ;
    }

    // return a statement node
    stat = create_statement(stat) ;
    pop_error_context() ;
    return stat ;
}

// while: tk_while tk_lrb condition tk_rrb statement
static ast parse_while()
{
    push_error_context("parse_while()") ;

    // an expression node and a statement node
    ast cond = nullptr ;
    ast stat = nullptr ;

    // add parsing code here ...
    mustbe(tk_while) ;
    mustbe(tk_lrb) ;
    cond = parse_condition() ;
    mustbe(tk_rrb) ;
    stat = parse_statement() ;

    // return a while node
    ast ret = create_while(cond,stat) ;
    pop_error_context() ;
    return ret ;
}

// if: tk_if tk_lrb condition tk_rrb statement (tk_else statement)?
static ast parse_if()
{
    push_error_context("parse_if()") ;

    // an expression node and statement nodes
    ast cond = nullptr ;
    ast then_stat = nullptr ;
    ast else_stat = nullptr ;

    // add parsing code here ...
    mustbe(tk_if) ;
    mustbe(tk_lrb) ;
    cond = parse_condition() ;
    mustbe(tk_rrb) ;
    then_stat = parse_statement() ;

    // if there is an else statement
    if ( have_next(tk_else) )
    {
        else_stat = parse_statement() ;

        // return an if else node
        ast ret = create_if_else(cond,then_stat,else_stat) ;
        pop_error_context() ;
        return ret ;
    }

    // return an if node
    ast ret = create_if(cond,then_stat) ;
    pop_error_context() ;
    return ret ;
}

// return the variable's tree node or report a fatal error if it was not previously declared
static ast lookup_variable(Token identifier) ;

// let: tk_let tk_identifier tk_assign expression tk_semi
static ast parse_let()
{
    push_error_context("parse_let()") ;

    // a variable node and an expression node
    ast id = nullptr ;
    ast expr = nullptr ;

    // add parsing code here ...
    mustbe(tk_let) ;
    id = lookup_variable(mustbe(tk_identifier)) ;
    mustbe(tk_assign) ;
    expr = parse_expression() ;
    mustbe(tk_semi) ;

    // return a let node
    ast ret = create_let(id,expr) ;
    pop_error_context() ;
    return ret ;
}

// sequence: tk_lcb statement* tk_rcb
static ast parse_sequence()
{
    push_error_context("parse_sequence()") ;

    // a vector of statemnt nodes
    vector<ast> seq ;

    // add parsing code here ...
    mustbe(tk_lcb) ;
    while ( have(tg_statement) )
    {
        seq.push_back(parse_statement()) ;
    }
    mustbe(tk_rcb) ;

    // return a statements node
    ast ret = create_statements(seq) ;
    pop_error_context() ;
    return ret ;
}

// expression: term (tg_infix_op term)?
static ast parse_expression()
{
    push_error_context("parse_expression()") ;

    // an expression node, two term nodes and a Token
    ast lhs = nullptr ;
    ast rhs = nullptr ;
    Token infix_op = nullptr ;

    // add parsing code here ...
    lhs = parse_term() ;

    // if the expression has an infix operator
    if ( have(tg_infix_op) )
    {
        infix_op = mustbe(tg_infix_op) ;
        rhs = parse_term() ;

        // return an infix expression node
        ast ret = create_expression(create_infix_op(lhs,token_spelling(infix_op),rhs)) ;
        pop_error_context() ;
        return ret ;
    }

    // return a single term expression node
    ast ret = create_expression(lhs) ;
    pop_error_context() ;
    return ret ;
}

// condition: term tg_relop term
static ast parse_condition()
{
    push_error_context("parse_condition()") ;

    // two term nodes and a Token
    ast lhs = nullptr ;
    Token relop = nullptr ;
    ast rhs = nullptr ;

    // add parsing code here ...
    lhs = parse_term() ;
    relop = mustbe(tg_relop) ;
    rhs = parse_term() ;

    // return the parsed condition as an expression node
    ast ret = create_expression(create_infix_op(lhs,token_spelling(relop),rhs)) ;
    pop_error_context() ;
    return ret ;
}

// the following helper functions can be used in parse_term()

// turn an integer token into an ast node
static ast integer_to_ast(Token integer)
{
    return create_int(token_ivalue(integer)) ;
}

// To return the variable's tree node or report a fatal error if it was not previously declared, use:
// ast lookup_variable(Token identifier) ;

// term: tk_identifier | tk_integer
static ast parse_term()
{
    push_error_context("parse_term()") ;

    // a variable or integer node
    ast term = nullptr ;

    // add parsing code here ...
    switch(token_kind())
    {
    case tk_identifier:
        term = lookup_variable(mustbe(tk_identifier)) ;
        break ;
    case tk_integer:
        term = integer_to_ast(mustbe(tk_integer)) ;
        break ;
    default:
        did_not_find(tg_term) ;
        break ;
    }

    // return the parsed term in a term node
    ast ret = create_term(term) ;
    pop_error_context() ;
    return ret ;
}


/////////////////////////////////////////////////////////////////////////////
// ***** DO NOT CHANGE CODE AFTER THIS POINT *****


// ****  SYMBOL TABLES  ****

// scope levels each have their own symbol tables, segment and next free location
static jsymbols symbol_tables ;

// initialise the symbol tables
static void initialise_symbol_tables()
{
    symbol_tables = nullptr ;
}

// push a new symbol table onto the stack of symbol tables
// in a Jack compiler you would call this at the start of parsing a class
// to get a symbol table to put static and field declarations in
// you would also call this at the start of parsing a new constructor, function or method
// to get a new empty symbol table to put the argument and local variable declarations in
static void push_symbol_table()
{
    symbol_tables = jsymbols_push(symbol_tables) ;
}

// pop the top scope from the stack of symbol tables and delete it
// in a Jack compiler you would call this at the end of parsing a class
// since the symbol tables are no longer required
// you would also call this at the end of parsing a new constructor, function or method
// because the arguments and local variables in the top symbol table are no longer required
static void pop_symbol_table()
{
    symbol_tables = jsymbols_pop(symbol_tables) ;
}

// this function adds an identifier to the top symbol table on the symbol table stack
// it allocates the variable the next free offset in its segment
// it returns a tree node representing the variable
static ast declare_variable(Token identifier,string vartype,string varsegment)
{
    string varname = token_spelling(identifier) ;
    int varoffset = jsymbols_offset(symbol_tables,varsegment) ;

    if ( !jsymbols_insert(symbol_tables,varname,vartype,varsegment,varoffset) ) // it is an error to declare something twice
    {
        fatal_error(0,"\n" + token_context(identifier) + "Variable:  " + varname +  " has already been declared") ;
    }

    return create_variable(varname,varsegment,varoffset,vartype) ;
}

// lookup the identifier, it is an error if it was not previously declared
// it returns a tree node representing the variable
static ast lookup_variable(Token identifier)
{
    // search symbol tables from top to bottom of the symbol table stack
    string varname = token_spelling(identifier) ;
    jack_var var = jsymbols_lookup(symbol_tables,varname) ;
    if ( var != nullptr )
    {
        return create_variable(varname,jack_var_segment(var),jack_var_offset(var),jack_var_type(var)) ;
    }

    // variables not found - report a fatal error - the return is just so that the function compiles
    fatal_error(0,"\n" + token_context(identifier) + "Found undeclared variable:  " + varname) ;
    return nullptr ;
}

// **** MAIN PROGRAM ****

// main program for workshop 10/11 parser to XML
int main(int argc,char **argv)
{
    config_output(iob_immediate) ;      // make all output and errors appear immediately
    config_errors(iob_immediate) ;

    initialise_symbol_tables() ;        // initialise symbol tables
    push_symbol_table() ;               // push a new symbol table to hold declarations

    next_token() ;                      // read first token to initialise tokeniser
    ast program = parse_program() ;     // parse a Program to get a parse tree

    pop_symbol_table() ;                // delete the symbol table

    ast_print_as_xml(program,3) ;       // write the tree to standard output formatted as XML

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

