#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

using namespace std ;

using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Workshop_Compiler ;

// ignore unused-function warnings in this source file
#pragma clang diagnostic ignored "-Wunused-function"

// keep global counters so we can create unique labels in while and if statements
static int while_counter = 0 ;
static int if_counter = 0 ;

// we have a legal infix operator, translate into VM command equivalent
static string translate_op(string op)
{
    int oplen = op.length() ;

    if ( oplen == 1 )
    {
        switch(op[0])
        {
        case '+': return "add" ;
        case '-': return "sub" ;
        case '*': return "call Math.multiply 2" ;
        case '/': return "call Math.divide 2" ;
        case '<': return "lt" ;
        case '>': return "gt" ;
        default:;
        }
    }
    else
    if ( oplen == 2 && op[1] == '=' )
    {
        switch(op[0])
        {
        case '<': return "gt\nnot" ;
        case '>': return "lt\nnot" ;
        case '=': return "eq" ;
        case '!': return "eq\nnot" ;
        default:;
        }
    }

    fatal_error(-1,"translate_op passed unknown op: " + op + "\n") ;
    return op ;
}

// the grammar we are recognising
// rules containing text literals are written using the matching tk_* or tg_* names
//
// TERM: DEFINITION
// program:         declarations statement tk_eoi
// declarations:    declaration*
// declaration:     tk_var tg_type tk_identifier tk_semi
// statement:       while | if | let | sequence
// while:           tk_while tk_lrb condition tk_rrb statement
// if:              tk_if tk_lrb condition tk_rrb statement (tk_else statement)?
// let:             tk_let tk_identifier tk_assign expression tk_semi
// sequence:        tk_lcb statement* tk_rcb
// expression:      term (tg_infix_op term)?
// condition:       term tg_relop term
// term:            tk_identifier | tk_integer
//
// Token groups for use with have()/have_next()/mustbe()/did_not_find():
// tg_statement - matches any token that can start a statement
// tg_term      - matches any token that can start a term
// tg_infix_op  - matches any token that can be used as an infix_op
// tg_relop     - matches any token that can be used as a relop
// tg_type      - matches any token that can be used as a type

// since parsing is recursive, forward declare one function to walk each non-terminal:
// note: conditions are represented by expressions
static void walk_program(ast) ;
static int  walk_declarations(ast) ;
static void walk_statement(ast) ;
static void walk_while(ast) ;
static void walk_if(ast) ;
static void walk_if_else(ast) ;
static void walk_let(ast) ;
static void walk_sequence(ast) ;
static void walk_expression(ast) ;
static void walk_term(ast) ;

// now implement the parsing functions
// ast create_program(ast declarations,ast statement)
static void walk_program(ast n)
{
    push_error_context("walk_program()") ;

    int nlocals = walk_declarations(get_program_declarations(n)) ;

    // if the programs starts with x variable declarations, we must start with:
    //     function Main.main x
    // nextlocal is effectively a variable count so ...
    write_to_output("function Main.main " + to_string(nlocals) + "\n") ;

    walk_statement(get_program_statement(n)) ;

    // always finish with return so the VM code is a complete void function
    write_to_output("push constant 0\n") ;
    write_to_output("return\n") ;

    pop_error_context() ;
}

// ast create_declarations(vector<ast> variables)
static int walk_declarations(ast n)
{
    push_error_context("walk_declarations()") ;

    int ndecls = size_of_declarations(n) ;

    pop_error_context() ;
    return ndecls ;
}

// statement nodes can contain one of ast_while, ast_if, ast_if_else, ast_let or ast_statements
static void walk_statement(ast n)
{
    push_error_context("walk_statement()") ;

    ast stat = get_statement_statement(n) ;

    switch(ast_node_kind(stat))
    {
    case ast_while:
        walk_while(stat) ;
        break ;
    case ast_if:
        walk_if(stat) ;
        break ;
    case ast_if_else:
        walk_if_else(stat) ;
        break ;
    case ast_let:
        walk_let(stat) ;
        break ;
    case ast_statements:
        walk_sequence(stat) ;
        break ;
    default:
        fatal_error(0,"Unknown kind of statement node found") ;
        break ;
    }

    pop_error_context() ;
}

// ast create_while(ast condition,ast body)
static void walk_while(ast n)
{
    push_error_context("walk_while()") ;

    string label = to_string(while_counter++) ;

    walk_expression(get_while_condition(n)) ;
    walk_sequence(get_while_body(n)) ;

    pop_error_context() ;
}

// ast create_if(ast condition,ast if_true)
static void walk_if(ast n)
{
    push_error_context("walk_if()") ;

    string label = to_string(if_counter++) ;

    walk_expression(get_if_condition(n)) ;
    walk_sequence(get_if_if_true(n)) ;

    pop_error_context() ;
}

// ast create_if_else(ast condition,ast if_true,ast if_false)
static void walk_if_else(ast n)
{
    push_error_context("walk_if_else()") ;

    string label = to_string(if_counter++) ;

    walk_expression(get_if_else_condition(n)) ;
    walk_sequence(get_if_else_if_true(n)) ;
    walk_sequence(get_if_else_if_false(n)) ;

    pop_error_context() ;
}

// ast create_let(ast variable,ast expression)
static void walk_let(ast n)
{
    push_error_context("walk_let()") ;

    //ast var = get_let_variable(n) ;
    walk_expression(get_let_expression(n)) ;

    pop_error_context() ;
}

// ast create_statements(vector<ast> statements) ;
static void walk_sequence(ast n)
{
    push_error_context("walk_sequence()") ;

    int children = size_of_statements(n) ;
    for ( int i = 0 ; i < children ; i++ )
    {
        walk_statement(get_statements(n,i)) ;
    }

    pop_error_context() ;
}

// there are no expression nodes, only ast_infix_op, ast_variable and ast_int nodes
// ast create_infix_op(ast lhs,string op,ast rhs)
static void walk_expression(ast n)
{
    push_error_context("walk_expression()") ;

    ast expr = get_expression_expression(n) ;

    if ( ast_have_kind(expr,ast_infix_op) )
    {
        walk_term(get_infix_op_lhs(expr)) ;
        walk_term(get_infix_op_rhs(expr)) ;
        string op = get_infix_op_op(expr) ;
    }
    else
    {
        walk_term(expr) ;
    }

    pop_error_context() ;
}

// there are no term nodes, only ast_variable and ast_int nodes
// ast create_variable(string name,string segment,int offset,string type)
static void walk_term(ast n)
{
    push_error_context("walk_term()") ;

    ast term = get_term_term(n) ;

    switch(ast_node_kind(term))
    {
    case ast_variable:
        break ;
    case ast_int:
        break ;
    default:
        fatal_error(0,"Unknown kind of term node found") ;
        break ;
    }

    pop_error_context() ;
}

// main program for workshop 11 XML to VM code translator
int main(int argc,char **argv)
{
    // make all output and errors appear immediately
    config_output(iob_immediate) ;
    config_errors(iob_immediate) ;

    walk_program(ast_parse_xml()) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}
