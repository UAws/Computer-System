#ifndef EXAM_PARSER_AST_H
#define EXAM_PARSER_AST_H

#include <string>
#include <vector>
#include "tokeniser.h"

// Abstract Syntax Tree
// All AST nodes have a private implementation
// - the AST values passed around are indexes into a private data structure
// - vector like nodes, AST nodes are immutable
// - every AST node has a 'create' function, eg 'create_while(_condition,_statements)'
// - every field of an AST node has a 'get' function, eg, 'get_while_statements(_while_node)'
// - vector like AST nodes have a 'size_of' function, eg 'size_of_statements(_statements_node);'
// - vector like AST nodes have a 'get' function, eg 'get_statements(_statements_node,2);'
// - nullptr will be interpreted as an empty node

// all errors encountered by AST functions will result in calls to fatal_error() defined in iobuffer.h
// an additional error message will be printed to standard error
// the running program will terminate with an exit status of -1
// errors include
// - passing an ast value that does not identify an AST node
// - passing an ast value that refers to an inappropriate kind of AST node
// - tree traversal errors when print a tree as XML
// - parsing errors when constructing an XML tree

// An Abstract Sytax Tree is language specific and has its own private namespace, eg Workshop_Compiler

namespace Workshop_Compiler
{
    // we use 64-bit pointer (really a 64-bit int) to identify AST nodes in the public interface
    class _ast ;
    typedef _ast *ast ;

    // shorthand to avoid std::
    using namespace std ;

    // enumeration for AST node classes
    // ast_alpha & ast_omega describe the maximum range of ast_kind values
    // ast_oops is for error reporting
    enum ast_kind
    {
        ast_alpha = 3000,       // keep enum ranges unique


        ast_empty,              // an empty node

                                // exam language - one node kind per grammar rule
        ast_program,            // program

        ast_declarations,       // variable or function declarations
        ast_declaration,        // declaration of variable or function
        ast_variable,           // variable - identifier with segment, offset and type information
        ast_function,           // function declaration
        ast_function_body,      // function body

        ast_statements,         // statements
        ast_statement,          // contains one of case, default, do, if, if_else, let, return, return expr, statements, switch, throw, try or while
        ast_case,               // switch statement case label
        ast_default,            // switch statement default label
        ast_do,                 // do statement
        ast_if,                 // if then statement
        ast_if_else,            // if then else statement
        ast_let,                // let statement - variable assignment
        ast_return,             // return statement
        ast_return_expr,        // return expression statement
        ast_switch,             // switch statement
        ast_throw,              // throw statement
        ast_try,                // try catch statement
        ast_while,              // while statement

        ast_expressions,        // expressions
        ast_expression,         // contains one of infix_op or term
        ast_infix_op,           // infix op and operands

        ast_term,               // contains one of bool, call, expression, int, string or variable
        ast_bool,               // boolean constant
        ast_call,               // a function call
        ast_int,                // integer constant
        ast_string,             // string constant

        ast_oops,               // for error reporting
        ast_omega               // largest ast_kind value
    };


    /***** AST Functions *****
      * IMPORTANT:
      * all errors encountered by these functions will result in calls to fatal_error()
      * an additional error message will be printed to standard error
      * the running program will terminate with an exit status of -1
    */

    //***** AST node kind functions *****//

    extern string   to_string(ast_kind k) ;                 // returns string representation of ast kind k
    extern ast_kind ast_node_kind(ast t) ;                  // what kind of ast node is t?

                                                            // the next two functions recognise grouped kinds
    extern bool     ast_have_kind(ast t,ast_kind k) ;       // can node t be safely used as a node of kind k?
    extern void     ast_mustbe_kind(ast t,ast_kind k) ;     // call fatal_error() if node t cannot be safely used as a node of kind k

    // print tree t as XML, an indent_by > 0 causes pretty printing
    extern void ast_print_as_xml(ast t,int indent_by) ;

    // construct a Workshop_Compiler abstract syntax tree by parsing XML from standard input,
    // any pretty printed indents are ignored,
    // the result will be either an ast_tokens node or an ast_program node
    extern ast ast_parse_xml() ;

    //***** AST nodes to represent exam language programs *****//
    // program ::= declarations statement
    // ast_program - a workshop example language program
    // declarations must be an ast_declarations or empty node
    // statement must be an ast_statement node
    extern ast create_program(ast declarations,ast statement) ;
    extern ast get_program_declarations(ast program) ;
    extern ast get_program_statement(ast program) ;


    // declarations ::= ('var' type variable ';')*
    // ast_declarations - a vector of ast_declaration nodes
    // declarations must be a vector of ast_declaration nodes, it may be empty
    extern ast create_declarations(vector<ast> declarations) ;
    extern int size_of_declarations(ast t) ;
    extern ast get_declarations(ast t,int i) ;


    // ast_declaration - an ast_variable or ast_function
    // declaration must be an ast_variable or ast_function node
    extern ast create_declaration(ast declaration) ;
    extern ast get_declaration_declaration(ast declaration) ;


    // ast_variable node - an identifier with segment, offset and type informationode
    // it records the variable's name, it's segment name, it's segment offset and it's type
    // the first variable has offset 0, the second offset 1 and so on
    extern ast create_variable(string name,string segment,int offset,string type) ;
    extern string get_variable_segment(ast variable) ;
    extern string get_variable_name(ast variable) ;
    extern int get_variable_offset(ast variable) ;
    extern string get_variable_type(ast variable) ;


    // function ::= rtype fname '(' parameters? ')' functionBody
    // ast_function node - a function declaration
    // parameters must be an ast_declarations or ast_empty node
    // body must be an ast_function_body, ast_statements, ast_statement or ast_empty node
    extern ast create_function(string rtype,string name,ast parameters,ast body) ;
    extern string get_function_rtype(ast function) ;
    extern string get_function_name(ast function) ;
    extern ast get_function_parameters(ast function) ;
    extern ast get_function_body(ast function) ;


    // functionBody ::= '{' declarations statements '}'
    // ast_function_body node - a function body
    // declarations must be an ast_declarations or ast_empty node
    // statements must be an ast_statements, ast_statement or ast_empty node
    extern ast create_function_body(ast declarations,ast statements) ;
    extern ast get_function_body_declarations(ast function) ;
    extern ast get_function_body_statements(ast function) ;


    // statements ::= statement*
    // ast_statements node - a statement sequence
    // statements must be a vector of ast_statement nodes, it may be empty
    extern ast create_statements(vector<ast> statements) ;
    extern int size_of_statements(ast statements) ;
    extern ast get_statements(ast statements,int index) ;


    // ast_statement - a single statement
    // statement must be an ast_case, ast_default, ast_do, ast_if, ast_if_else, ast_let, ast_return,
    //                      ast_return_expr, ast_statements, ast_switch, ast_switch, ast_throw, ast_try or ast_while node
    extern ast create_statement(ast statement) ;
    extern ast get_statement_statement(ast statement) ;


    // case label ::= 'case' expression ':'
    // ast_case node - a case label in a switch
    // expression must be an ast_expression node
    extern ast create_case(ast expression) ;
    extern ast get_case_expression(ast _case) ;


    // default label ::= 'default' ':'
    // ast_default node - a default label in a switch
    extern ast create_default() ;


    // do ::= 'do' body 'while' '(' condition ')' ';'
    // ast_do node - a do loop
    // condition must be an ast_condition node
    // body must be an ast_statements, ast_statement or ast_empty node
    extern ast create_do(ast condition,ast body) ;
    extern ast get_do_body(ast _do) ;
    extern ast get_do_condition(ast _do) ;


    // if ::= 'if' '(' condition ')' '{' statements '}'
    // ast_if node - an if then statement
    // condition must be an ast_condition node
    // if_true must be an ast_statements, ast_statement or ast_empty node
    extern ast create_if(ast condition,ast if_true) ;
    extern ast get_if_condition(ast _if) ;                  // also accepts an ast_if_else node
    extern ast get_if_if_true(ast _if) ;                    // also accepts an ast_if_else node


    // if ::= 'if' '(' condition ')' '{' statements '}' 'else' '{' statements '}'
    // ast_if_else node - an if then else statement
    // condition must be an ast_condition node
    // if_true must be an ast_statements, ast_statement or ast_empty node
    // if_false must be an ast_statements, ast_statement or ast_empty node
    extern ast create_if_else(ast condition,ast if_true,ast if_false) ;
    extern ast get_if_else_condition(ast _if) ;             // also accepts an ast_if node
    extern ast get_if_else_if_true(ast _if) ;               // also accepts an ast_if node
    extern ast get_if_else_if_false(ast _if) ;


    // let ::= 'let' variable '=' expression ';'
    // ast_let node - a variable assignment statement
    // variable must be an astiable_var node
    // expression must be an ast_expression node
    // an ast_expression node is one of ast_int, ast_variable or ast_infix_op
    extern ast create_let(ast variable,ast expression) ;
    extern ast get_let_variable(ast _let) ;
    extern ast get_let_expression(ast _let) ;


    // return ::= 'return' ';'
    // ast_return node - void return statement
    extern ast create_return() ;


    // return expr ::= 'return' expression ';'
    // ast_return_expr node
    // expression must be an ast_expression node
    extern ast create_return_expr(ast expression) ;
    extern ast get_return_expr_expression(ast _return) ;


    // switch ::= 'switch' '(' expression ')' '{' labelled* '}'
    // ast_switch node
    // expression must be an ast_expression node
    // statements must be an ast_statements, ast_statement or ast_empty node
    extern ast create_switch(ast expression,ast statements) ;
    extern ast get_switch_expression(ast _switch) ;
    extern ast get_switch_statements(ast _switch) ;


    // throw ::= 'throw' expression ';'
    // ast_throw node
    // expression must be an ast_expression node
    extern ast create_throw(ast expression) ;
    extern ast get_throw_expression(ast _throw) ;


    // try ::= 'try' tryStatement 'catch' '(' identifier identifier ')' catchStatement
    // ast_try node - a try catch block
    // tryStatement and catchStatement must be an ast_statement or ast_empty node
    extern ast create_try(ast tryStatement,ast var,ast catchStatement) ;
    extern ast get_try_tryStatement(ast _try) ;
    extern ast get_try_var(ast _try) ;
    extern ast get_try_catchStatement(ast _try) ;


    // while ::= 'while' '(' condition ')' '{' statements '}'
    // ast_while node - a while loop
    // condition must be an ast_condition node
    // body must be an ast_statements, ast_statement or ast_empty node
    extern ast create_while(ast condition,ast body) ;
    extern ast get_while_condition(ast _while) ;
    extern ast get_while_body(ast _while) ;



    // expressions - arguments to a function call?
    // ast_expressions_node - a vector of ast_expression nodes
    // expressions must be a vector of ast_expression nodes, if may be empty
    extern ast create_expressions(vector<ast> expressions) ;
    extern int size_of_expressions(ast expressions) ;
    extern ast get_expressions(ast expressions,int index) ;



    // expression - a single expression
    // ast_expression_node 
    // expression must be ast_term or  ast_infix_op
    extern ast create_expression(ast expression) ;
    extern ast get_expression_expression(ast expression) ;



    // ast_infix_op node - an infix op expression, ie lhs op rhs
    // lhs must be an ast_term node
    // op must be an infix op, ie '+', '-', '*' or '/'
    // rhs must be an ast_term node
    // an ast_term node is one of ast_int or ast_variable
    extern ast create_infix_op(ast lhs,string op,ast rhs) ;
    extern ast get_infix_op_lhs(ast infix_op) ;
    extern string get_infix_op_op(ast infix_op) ;
    extern ast get_infix_op_rhs(ast infix_op) ;



    // term - a single expression term
    // ast_term_node 
    // term must be one of ast_bool, ast_call, ast_expression, ast_int, ast_string or ast_variable
    extern ast create_term(ast term) ;
    extern ast get_term_term(ast term) ;



    // bool ::= 'true' | 'false'
    // ast_bool node - a boolean constant
    extern ast create_bool(bool _constant) ;
    extern bool get_bool_constant(ast _bool) ;


    // ast_call node - a function call
    // fn is the name of the function being called
    // args must be an an ast_expressions node
    extern ast create_call(string fn,ast args) ;
    extern string get_call_fn(ast _call) ;
    extern ast get_call_args(ast _call) ;


    // int ::= '0' | ('1'-'9')('0'-'9')*
    // ast_int node - an integer constant
    extern ast create_int(int _constant) ;
    extern int get_int_constant(ast _int) ;


    // string ::= '"' (' ' | '!' | '#'-'~')* '"'
    // ast_string node - a string constant
    extern ast create_string(string _constant) ;
    extern string get_string_constant(ast _string) ;

}

#endif //EXAM_PARSER_AST_H
