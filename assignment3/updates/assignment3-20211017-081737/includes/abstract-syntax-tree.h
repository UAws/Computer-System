#ifndef JACK_AST_H
#define JACK_AST_H

#include <string>
#include <vector>
#include "tokeniser.h"

// Abstract Syntax Tree
// All AST nodes have a private implementation
// - the AST values passed around are indexes into a private data structure
// - the AST values are given a pointer type to an undefined class so they have unique type of size 64 bits
// - nullptr will be interpreted as being the index of the unique empty AST node
// - all AST nodes, including vector like nodes, are immutable
// - all AST nodes have an Annotation node
// - every AST node has a 'create' function, eg 'create_while(_condition,_statements)'
// - every field of an AST node has a 'get' function, eg, 'get_while_statements(_while_node)'
// - vector like AST nodes have a 'size_of' function, eg 'size_of_statements(_statements_node)'
// - vector like AST nodes have a 'get' function, eg 'get_statements(_statements_node,2)'
// - the first element in a vector is at index 0

// most errors encountered by AST functions will result in calls to fatal_error() defined in iobuffer.h
// an additional error message will be printed to standard error
// the running program will terminate with an exit status of -1
// errors include
// - passing an ast value that does not identify an AST node
// - passing an ast value that refers to an inappropriate kind of AST node
// - tree traversal errors when print a tree as XML
// - parsing errors when constructing an XML tree
// when printing a tree as XML some most tree traversal errors are included in the XML output

// All Annotation nodes have a private implementation
// - the Annotation values passed around are indexes into a private data structure
// - the Annotation values are given a pointer type to an undefined class so they have unique type of size 64 bits
// - nullptr will be interpreted as being the index of the unique empty annotation node
// - Annotation nodes:
//   - are immutable
//   - contain a vector of comments
//   - contain a vector of warning messages
//   - contain a vector of error messages
// - each vector has a 'size_of' function, eg 'size_of_ann_comments(_ann)'
// - each vector has a 'get' function, eg 'get_ann_errors(_ann,1)'
// - the first element in a vector is at index 0
// - each vector has an 'add' function, eg 'add_ann_comments(_ann,"hello")'
// - each vector has a 'delete' function, eg 'delete_ann_warnings(_ann,2)'

// An Abstract Sytax Tree is language specific and has its own private namespace, eg Jack_Compiler

// Jack Compiler
namespace Jack_Compiler
{
    // we use pointers to identify AST and Annotation nodes in the public interface
    class _ast ;
    typedef _ast *ast ;
    class _ann ;
    typedef _ann *ann ;

    // shorthand to avoid std::
    using namespace std ;

    // enumeration for AST node classes
    // ast_alpha & ast_omega describe the maximum range of ast_kind values
    // ast_oops is for error reporting
    enum ast_kind
    {
        ast_alpha = 4000,       // keep enum ranges unique

        ast_empty,              // an empty node

        ast_class,              // class
        ast_class_var_decs,     // vector of class variable declarations
        ast_var_dec,            // single variable declaration
        ast_subr_decs,          // vector of subroutine declarations

        ast_subr,               // subroutine declaration
        ast_constructor,        // constructor declaration
        ast_function,           // function declaration
        ast_method,             // method declaration
        ast_param_list,         // vector of subroutine parameter declarations
        ast_subr_body,          // body of a subroutine
        ast_var_decs,           // vector of subroutine local var declarations

        ast_statements,         // statements
        ast_statement,          // statement
        ast_let,                // let statement - variable assignment
        ast_let_array,          // let statement - array element assignment
        ast_if,                 // if then statement
        ast_if_else,            // if then else statement
        ast_while,              // while statement
        ast_do,                 // do statement
        ast_return,             // return statement - nothing returned
        ast_return_expr,        // return statement - expression value returned

        ast_expr_list,          // expression list - arguments to a subrountine call
        ast_expr,               // expression - alternating term infix_op term infix_op term ... ending with a term
        ast_term,               // expression term
        ast_int,                // integer constant
        ast_string,             // string constant
        ast_bool,               // boolean constant - false or true
        ast_null,               // null
        ast_this,               // this
        ast_unary_op,           // unary op and operand
        ast_var,                // variable
        ast_array_index,        // array index
        ast_call_as_function,   // constructor or function call
        ast_call_as_method,     // method call
        ast_subr_call,          // constructor, function or method name and explicit constructor, function or method call arguments
        ast_infix_op,           // an infix operator

        ast_annotation,         // an annotation node - for internal use only
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

    extern string   ast_kind_to_string(ast_kind k) ;        // returns string representation of ast kind k
    extern ast_kind ast_node_kind(ast t) ;                  // what kind of ast node is t?
    extern bool     ast_have_kind(ast t,ast_kind k) ;       // can node t be safely used as a node of kind k?
    extern void     ast_mustbe_kind(ast t,ast_kind k) ;     // call fatal_error() if node t cannot be safely used as a node of kind k

    //***** AST annotation functions *****//

    // annotation objects are attached to all AST nodes and contain 3 vectors of strings: comments, warnings and errors
    // an empty annotation object is added by default if not explicitly provided to a create_*() function

    // return the unique empty annotation object
    // nullptr will be interpreted as being the unique empty annotation object
    // nullptr will be accepted by functions that accept an empty annotation object
    // . add_ann_*() functions return a new annotation object with a single comment, warning or error
    // . size_of_ann_*() functions will return 0
    extern ann create_ann() ;

    // create a new annotation object, initialised by 3 vectors of strings
    // any "" are removed from the vectors before creating the object
    // if the 3 vectors become empty, the unique empty annotation object is returned
    extern ann create_ann(vector<string> comments,vector<string> warnings,vector<string> errors) ;

    // return the annotation object attached to AST node t
    extern ann get_ann(ast t) ;

    // the ann_add_*() and ann_delete_*() functions return a copy of the annotation object with the changes applied
    // ann_add_*(), adds the string to the end of the relevant vector
    // ann_delete_*(), deletes the string at the given index of the relevant vector, later strings move down one position
    // ann_size_of_*() returns the number of strings in the relevant vector
    // ann_get_*() returns the string at the given index of the relevant vector

    // comments vector
    extern ann add_ann_comments(ann a,string s) ;                       // will accept nullptr
    extern ann delete_ann_comments(ann a,int index) ;
    extern int size_of_ann_comments(ann a) ;                            // will accept nullptr
    extern string get_ann_comments(ann a,int index) ;

    // warnings vector
    extern ann add_ann_warnings(ann a,string s) ;                       // will accept nullptr
    extern ann delete_ann_warnings(ann a,int index) ;
    extern int size_of_ann_warnings(ann a) ;                            // will accept nullptr
    extern string get_ann_warnings(ann a,int index) ;

    // errors vector
    extern ann add_ann_errors(ann a,string s) ;                         // will accept nullptr
    extern ann delete_ann_errors(ann a,int index) ;
    extern int size_of_ann_errors(ann a) ;                              // will accept nullptr
    extern string get_ann_errors(ann a,int index) ;

    //***** XML printing / parsing functions for Jack AST nodes *****//

    // print tree t as XML, an indent_by > 0 causes pretty printing
    extern void ast_print_as_xml(ast t,int indent_by) ;

    // construct a Jack_Compiler abstract syntax tree by parsing XML from standard input,
    // any pretty printed indents are ignored,
    // the result will be either an ast_tokens node or an ast_class node
    extern ast ast_parse_xml() ;

    //***** AST nodes to represent Jack classes *****//

    // ast_empty - an empty ast node
    // there is a unique ast_empty node with no annotations
    // the unique ast_empty node is returned if no annotation object is provided or if it is empty
    // nullptr will be interpreted as being the unique ast_empty node with no annotations
    // nullptr and ast_empty will be accepted by functions that expect an ast vector node that can be empty
    // . size_of_*() functions will return 0
    // . ast_have_kind() will return true if the ast_kind is an ast vector node that can be empty
    // . ast_mustbe_kind() will not report an error if the ast_kind is an ast vector node that can be empty
    extern ast create_empty(ann annotation) ;
    extern ast create_empty() ;

    // class ::= 
    // ast_class - a Jack class
    // it has a class name, static (statics) and field declarations (fields) and subroutine declarations (subrs)
    // decs, statics and fields must be an ast_class_var_decs or ast_empty node
    // if only one decs is provided, the decs are assigned to fields, statics is nullptr
    // subrs must be an ast_subr_decs or ast_empty node
    extern ast create_class(ann annotation,string class_name,ast statics,ast fields,ast subrs) ;
    extern ast create_class(string class_name,ast statics,ast fields,ast subrs) ;
    extern string get_class_class_name(ast _class) ;
    extern ast get_class_statics(ast _class) ;
    extern ast get_class_fields(ast _class) ;
    extern ast get_class_subr_decs(ast _class) ;


    // class_var_decs ::= 
    // ast_class_var_decs - a vector of ast_var_dec nodes
    // vars must be a vector that is empty or where all nodes are ast_var_dec, ast_var_decs, ast_empty or nullptr
    // . ast_empty nodes and nullptrs are ignored
    // . ast_var_decs nodes are replaced by their contents or ignored if empty
    extern ast create_class_var_decs(ann annotation,vector<ast> vars) ;
    extern ast create_class_var_decs(vector<ast> vars) ;
    extern int size_of_class_var_decs(ast t) ;                          // will accept ast_empty or nullptr
    extern ast get_class_var_decs(ast t,int i) ;


    // var_decs ::= 
    // ast_var_decs - a vector of ast_var_dec nodes
    // vars must be a vector that is empty or where all nodes are ast_var_dec, ast_var_decs, ast_empty or nullptr
    // . ast_empty nodes and nullptrs are ignored
    // . ast_var_decs nodes are replaced by their contents or ignored if empty
    extern ast create_var_decs(ann annotation,vector<ast> vars) ;
    extern ast create_var_decs(vector<ast> vars) ;
    extern int size_of_var_decs(ast t) ;                                // will accept ast_empty or nullptr
    extern ast get_var_decs(ast t,int i) ;


    // var_dec ::= 
    // ast_var_dec node
    // it records the variable's name, it's segment name, it's segment offset and it's type
    extern ast create_var_dec(ann annotation,string name,string segment,int offset,string type) ;
    extern ast create_var_dec(string name,string segment,int offset,string type) ;
    extern string get_var_dec_segment(ast var_dec) ;
    extern string get_var_dec_name(ast var_dec) ;
    extern int get_var_dec_offset(ast var_dec) ;
    extern string get_var_dec_type(ast var_dec) ;


    // var ::= 
    // ast_var node
    // it records the variable's name, it's segment name, it's segment offset and it's type
    extern ast create_var(ann annotation,string name,string segment,int offset,string type) ;
    extern ast create_var(string name,string segment,int offset,string type) ;
    extern string get_var_segment(ast var_dec) ;
    extern string get_var_name(ast var_dec) ;
    extern int get_var_offset(ast var_dec) ;
    extern string get_var_type(ast var_dec) ;


    // ast_subr_decs node - a vector of ast_subr nodes
    // subrs must be a vector that is empty or where all nodes are ast_subr, ast_empty or nullptr
    // . ast_empty nodes and nullptrs are ignored
    extern ast create_subr_decs(ann annotation,vector<ast> subrs) ;
    extern ast create_subr_decs(vector<ast> subrs) ;
    extern int size_of_subr_decs(ast t) ;                               // will accept ast_empty or nullptr
    extern ast get_subr_decs(ast t,int i) ;


    // subr ::= 
    // ast_subr node - a subroutine declaration
    // subr must be one of ast_constructor, ast_function or ast_method
    extern ast create_subr(ann annotation,ast subr) ;
    extern ast create_subr(ast subr) ;
    extern ast get_subr_subr(ast subr) ;


    // ast_constructor_node - a Jack constructor
    // it records the return type, constructor's name, its parameters and its body
    // params must be an ast_param_list or ast_empty node or nullptr
    // body must be an ast_subr_body or ast_empty node or nullptr
    extern ast create_constructor(ann annotation,string vtype,string name,ast params,ast body) ;
    extern ast create_constructor(string vtype,string name,ast params,ast body) ;
    extern string get_constructor_vtype(ast constructor) ;
    extern string get_constructor_name(ast constructor) ;
    extern ast get_constructor_param_list(ast constructor) ;
    extern ast get_constructor_subr_body(ast constructor) ;

    // ast_function_node - a Jack function
    // it records the return type, function's name, its parameters and its body
    // params must be an ast_param_list or ast_empty node or nullptr
    // body must be an ast_subr_body or ast_empty node or nullptr
    extern ast create_function(ann annotation,string vtype,string name,ast params,ast body) ;
    extern ast create_function(string vtype,string name,ast params,ast body) ;
    extern string get_function_vtype(ast function) ;
    extern string get_function_name(ast function) ;
    extern ast get_function_param_list(ast function) ;
    extern ast get_function_subr_body(ast function) ;


    // ast_method_node - a Jack method
    // it records the return type, method's name, its parameters and its body
    // params must be an ast_param_list or ast_empty node or nullptr
    // body must be an ast_subr_body or ast_empty node or nullptr
    extern ast create_method(ann annotation,string vtype,string name,ast params,ast body) ;
    extern ast create_method(string vtype,string name,ast params,ast body) ;
    extern string get_method_vtype(ast method) ;
    extern string get_method_name(ast method) ;
    extern ast get_method_param_list(ast method) ;
    extern ast get_method_subr_body(ast method) ;


    // param_list ::= 
    // ast_param_list node - subroutine parameter declarations
    // params must be a vector that is empty or where all nodes are ast_var_dec, ast_empty or nullptr
    // . ast_empty nodes and nullptrs are ignored
    extern ast create_param_list(ann annotation,vector<ast> params) ;
    extern ast create_param_list(vector<ast> params) ;
    extern int size_of_param_list(ast param_list) ;                     // will accept ast_empty or nullptr
    extern ast get_param_list(ast param_list,int index) ;


    // subr_body ::= 
    // ast_subr_body node - body of a Jack subroutine, ie a constructor, function or method
    // decs must be an ast_var_decs or ast_empty node or nullptr
    // body must be an ast_statements or ast_empty node or nullptr
    extern ast create_subr_body(ann annotation,ast decs,ast body) ;
    extern ast create_subr_body(ast decs,ast body) ;
    extern ast get_subr_body_decs(ast subr_body) ;
    extern ast get_subr_body_body(ast subr_body) ;


    // statements ::= 
    // ast_statements node - a Jack statement sequence
    // statements must be a vector that is empty or where all nodes are ast_statement, ast_statements, ast_empty or nullptr
    // . ast_empty nodes and nullptrs are ignored
    // . ast_statments nodes are replaced by their contents or ignored if empty
    extern ast create_statements(ann annotation,vector<ast> statements) ;
    extern ast create_statements(vector<ast> statements) ;
    extern int size_of_statements(ast statements) ;                     // will accept ast_empty or nullptr
    extern ast get_statements(ast statements,int index) ;


    // statement ::= 
    // ast_statement node - a single statement
    // statement must be one of ast_let, ast_let_array, ast_if, ast_if_else, ast_while, ast_do, ast_return or ast_return_expr nodes
    extern ast create_statement(ann annotation,ast statement) ;
    extern ast create_statement(ast statement) ;
    extern ast get_statement_statement(ast statement) ;


    // let ::= 
    // ast_let node - a Jack variable assignment statement
    // var must be an ast_var node
    // index must be an ast_expr node
    // expr must be an ast_expr node
    extern ast create_let(ann annotation,ast var,ast expr) ;
    extern ast create_let(ast var,ast expr) ;
    extern ast get_let_var(ast _let) ;                                  // also accepts an ast_let_array node
    extern ast get_let_expr(ast _let) ;                                 // also accepts an ast_let_array node


    // let_array ::= 
    // ast_let_array node - a Jack array assignment statement
    // var must be an ast_var node
    // index must be an ast_expr node
    // expr must be an ast_expr node
    extern ast create_let_array(ann annotation,ast var,ast index,ast expr) ;
    extern ast create_let_array(ast var,ast index,ast expr) ;
    extern ast get_let_array_var(ast _let) ;
    extern ast get_let_array_index(ast _let) ;
    extern ast get_let_array_expr(ast _let) ;


    // if ::= 
    // ast_if node - a Jack if then statement
    // condition must be an ast_expr node
    // if_true must be an ast_statements or ast_empty node or nullptr
    extern ast create_if(ann annotation,ast condition,ast if_true) ;
    extern ast create_if(ast condition,ast if_true) ;
    extern ast get_if_condition(ast _if) ;
    extern ast get_if_if_true(ast _if) ;


    // if_else ::= 
    // ast_if_else node - a Jack if then else statement
    // condition must be an ast_expr node
    // if_true must be an ast_statements or ast_empty node or nullptr
    // if_false must be an ast_statements or ast_empty node or nullptr
    extern ast create_if_else(ann annotation,ast condition,ast if_true,ast if_false) ;
    extern ast create_if_else(ast condition,ast if_true,ast if_false) ;
    extern ast get_if_else_condition(ast _if) ;                         // also accepts an ast_if node
    extern ast get_if_else_if_true(ast _if) ;                           // also accepts an ast_if node
    extern ast get_if_else_if_false(ast _if) ;


    // while ::= 
    // ast_while node - a Jack while loop
    // condition must be an ast_expr node
    // body must be an ast_statements or ast_empty node or nullptr
    extern ast create_while(ann annotation,ast condition,ast body) ;
    extern ast create_while(ast condition,ast body) ;
    extern ast get_while_condition(ast _while) ;
    extern ast get_while_body(ast _while) ;


    // do ::= 
    // ast_do node - a Jack void function call statement
    // call must be an ast_call node
    extern ast create_do(ann annotation,ast call) ;
    extern ast create_do(ast call) ;
    extern ast get_do_call(ast _do) ;


    // return ::= 
    // ast_return node - a Jack return statement with no result
    extern ast create_return(ann annotation) ;
    extern ast create_return() ;


    // return_expr ::= 
    // ast_return_expr node - a Jack return statement with a result
    // expr must be an ast_expr node
    extern ast create_return_expr(ann annotation,ast expr) ;
    extern ast create_return_expr(ast expr) ;
    extern ast get_return_expr(ast _return) ;


    // expr_list ::= 
    // ast_expr_list node - arguments to a Jack subroutine call
    // exprs must be a vector that is empty or where all nodes are ast_expr, ast_empty or nullptr
    // . ast_empty nodes and nullptrs are ignored
    extern ast create_expr_list(ann annotation,vector<ast> exprs) ;
    extern ast create_expr_list(vector<ast> exprs) ;
    extern int size_of_expr_list(ast expr_list) ;                       // will accept ast_empty or nullptr
    extern ast get_expr_list(ast expr_list,int index) ;


    // expression ::= 
    // ast_expr node - a Jack expression
    // expr must be a non-empty vector alternating between ast_term and ast_infix_op nodes
    // . the first node must be an ast_term node
    // . all even indices, ie 0, 2, 4, etc. must be an ast_term node
    // . all odd indices, ie 1, 3, 5, etc. must be an ast_infix_op node
    // . the last node must be an ast_term node
    extern ast create_expr(ann annotation,vector<ast> expr) ;
    extern ast create_expr(vector<ast> expr) ;
    // term must be an ast_term node
    extern ast create_expr(ann annotation,ast term) ;
    extern ast create_expr(ast term) ;
    // lhs and rhs must be ast_term nodes, op must be an ast_infix_op node
    extern ast create_expr(ann annotation,ast lhs,ast op,ast rhs) ;
    extern ast create_expr(ast lhs,ast op,ast rhs) ;
    extern int size_of_expr(ast expr) ;
    extern ast get_expr(ast expr,int index) ;


    // term ::= 
    // ast_term node - a single expression term
    // term must be one of ast_int, ast_string, ast_bool, ast_null, ast_this, ast_expr, ast_var, ast_array_index, ast_unary_op or ast_call
    extern ast create_term(ann annotation,ast term) ;
    extern ast create_term(ast term) ;
    extern ast get_term_term(ast term) ;


    // ast_infix_op node - a single infix operator
    // op must be a legal infix operator
    extern ast create_infix_op(ann annotation,string op) ;
    extern ast create_infix_op(string op) ;
    extern string get_infix_op_op(ast op) ;


    // int ::= '0' to '32767'
    // ast_int node - a Jack integer constant
    // _constant should be in the range 0 up to +32,767 but values from -32,768 to +32,767 are acceptable
    extern ast create_int(ann annotation,int _constant) ;
    extern ast create_int(int _constant) ;
    extern int get_int_constant(ast _int) ;


    // string ::= 
    // ast_string node - a Jack string constant
    extern ast create_string(ann annotation,string _constant) ;
    extern ast create_string(string _constant) ;
    extern string get_string_constant(ast _string) ;


    // bool ::= 'true' | 'false'
    // ast_bool node - a Jack boolean constant, ie true or false
    extern ast create_bool(ann annotation,bool t_or_f) ;
    extern ast create_bool(bool t_or_f) ;
    extern bool get_bool_t_or_f(ast _bool) ;


    // null ::= 'null'
    // ast_null node - the Jack null value
    extern ast create_null(ann annotation) ;
    extern ast create_null() ;


    // this ::= 'this'
    // ast_this node - the object operated on by the current Jack method
    extern ast create_this(ann annotation) ;
    extern ast create_this() ;


    // array_index ::= 
    // ast_array_index node - a Jack array indexing expression
    // var must be an ast_var node
    // index must be an ast_expr node
    extern ast create_array_index(ann annotation,ast var,ast index) ;
    extern ast create_array_index(ast var,ast index) ;
    extern ast get_array_index_var(ast _array_index) ;
    extern ast get_array_index_index(ast _array_index) ;


    // unary_op ::= 
    // ast_unary_op node - a Jack unary op expression
    // op must be a unary op, ie '-' or '~'
    // index must be an ast_term node
    extern ast create_unary_op(ann annotation,string op,ast term) ;
    extern ast create_unary_op(string op,ast term) ;
    extern string get_unary_op_op(ast unary_op) ;
    extern ast get_unary_op_term(ast unary_op) ;


    // call ::= class_name.subr_name '(' expr_list ')'
    // ast_call_as_function node - a Jack function call
    // subr_call must be an ast_subr_call node
    extern ast create_call_as_function(ann annotation,string class_name,ast subr_call) ;
    extern ast create_call_as_function(string class_name,ast subr_call) ;
    extern string get_call_as_function_class_name(ast call) ;
    extern ast get_call_as_function_subr_call(ast call) ;


    // call ::= var_name.subr_name '(' expr_list ')'
    // ast_call_as_method node - a Jack method call
    // var must be a var node or a this node
    // subr_call must be a subr_call node
    extern ast create_call_as_method(ann annotation,string class_name,ast var,ast subr_call) ;
    extern ast create_call_as_method(string class_name,ast var,ast subr_call) ;
    extern string get_call_as_method_class_name(ast call) ;
    extern ast get_call_as_method_var(ast call) ;
    extern ast get_call_as_method_subr_call(ast call) ;

    // call ::= subr_name '(' expr_list ')'
    // ast_subr_call node - a Jack subroutine call
    // expr_list must be an ast_expr_list, ast_empty node or nullptr
    extern ast create_subr_call(ann annotation,string subr_name,ast expr_list) ;
    extern ast create_subr_call(string subr_name,ast expr_list) ;
    extern string get_subr_call_subr_name(ast call) ;
    extern ast get_subr_call_expr_list(ast call) ;

}

#endif //JACK_AST_H
