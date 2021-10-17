//
//
// Author : Akide Liu
// Date : 12/10/21
// Student ID : A1743748
//

#include <string>
#include <vector>
#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

// to shorten our code:
using namespace std;
using namespace CS_IO_Buffers;
using namespace CS_Symbol_Tables;
using namespace Jack_Compiler;

#pragma clang diagnostic ignored "-Wunused-function"

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to parse a Jack class and construct the equivalent abstract syntax tree, ast.
//        The main function will print the ast formatted as XML
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
//
// NOTE: use the fatal_token_context() function to print fatal error messages with the current
//       input file position marked.
//
// The skeleton code has one function per non-terminal in the Jack grammar
//  - comments describing how to create relevant ast nodes prefix most functions
//  - ast nodes are immutable - you must create all the sub-trees before creating a new node
//
// The structure of the skeleton code is just a suggestion
//  - it matches the grammar but does not quite match the ast structure
//  - you will need to change the parameters / results / functions to suit your own logic
//  - you can change it as much as you like
//
// NOTE: the best place to start is to write code to parse the input without attempting to create
//       any ast nodes. If correct this parser will report errors when it finds syntax errors but
//       correct programs will fail because the tree printer will not be provided with a valid ast.
//       Once the parsing works modify your code to create the ast nodes.
//
// NOTE: the return create_empty() ; statements are only present so that this skeleton code will compile.
//       these statements must be replaced by code that creates the correct ast nodes.
//

// ***** SYMBOL TABLES *****
//
// Separate symbol tables are required to hold all static and field declarations. Appropriate calls
// have been included in the skeleton code to create and destory these symbol tables.
//
// Each constructor, function and method (cfm) requires its own separate symbol table to hold all
// its own parameter and local declarations. Appropriate calls have been included that will create
// a new symbol table when parsing a new cfm and to delete it at the end of parsing the cfm.
// For methods, the argument segment offsets will start at 1.
//
// The names of constructors, functions and methods are never checked and must NOT be stored in a symbol table.
//
// When a new variable must be declared, use the declare_variable() function. This will add the new
// variable to the current symbol table and return the appropriate tree node to represent the declaration.
// The symbol table will automatically manage allocating offsets within each segment.
//
// When details of a variable are required, use the lookup_variable() function to locate it. This will
// search the symbol tables and return a tree node to represent the variable. If the variable cannot
// be found nullptr will be returned. If a fatal error must be reported use lookup_variable_fatal() instead.
//
// Variables in the "this" segment are not visible when a lookup is called while parsing a function.
// To accommodate this a global variable, now_parsing, is set to Constructor, Function or Method to
// enable lookup_variable() and lookup_variable_fatal() to implement this. The variable can also
// be used to implement other constructor, function or method specific parsing.
//

// what kind of subroutinue is being parsed?
enum Now_Parsing
{
    Class,
    Constructor,
    Function,
    Method
};
static Now_Parsing now_parsing;

// symbol tables
static void push_symbol_table();
static void pop_symbol_table();
static ast declare_variable(Token name, Token type, string segment);
static ast lookup_variable(Token name);
static ast lookup_variable_fatal(Token name);

// ***** The Jack grammar to be recognised *****
//
// program          ::= One or more classes, each class in a separate file named <class_name>'.Jack'
// class            ::= tk_class tk_identifier tk_lcb static_var_decs field_var_decs subr_decs tk_rcb
// static_var_decs  ::= tk_static_var_dec*
// static_var_dec   ::= tk_static tg_type tk_identifier (tk_comma tk_identifier)* tk_semi
// field_var_decs   ::= tk_field_var_dec*
// field_var_dec    ::= tk_field tg_type tk_identifier (tk_comma tk_identifier)* tk_semi
// subr_decs        ::= (constructor | function | method)*
// constructor      ::= tk_constructor tk_identifier tk_identifier tk_lrb param_list tk_rrb subr_body
// function         ::= tk_function tg_vtype tk_identifier tk_lrb param_list tk_rrb subr_body
// method           ::= tk_method tg_vtype tk_identifier tk_lrb param_list tk_rrb subr_body
// param_list       ::= ((tg_type tk_identifier) (tk_comma tg_type tk_identifier)*)?
// subr_body        ::= tk_lcb var_decs statements tk_rcb
// var_decs         ::= var_dec*
// var_dec          ::= tk_var tg_type tk_identifier (tk_comma tk_identifier)* tk_semi

// statements       ::= statement*
// statement        ::= let | if | while | do | return
// let              ::= tk_let tk_identifier index? tk_assign expr tk_semi
// if               ::= tk_if tk_lrb expr tk_rrb tk_lcb statements tk_rcb (tk_else tk_lcb statements tk_rcb)?
// while            ::= tk_while tk_lrb expr tk_rrb tk_lcb statements tk_rcb
// do               ::= tk_do (void_var_call | void_this_call) tk_semi
// void_var_call    ::= tk_identifier (fn_call | method_call)
// void_this_call   ::= tk_this method_call
// return           ::= tk_return expr? tk_semi

// expr             ::= term (tg_infix_op expr)?
// term             ::= tk_integer | tk_string | tk_true | tk_false | tk_null | (tk_lrb expr tk_rrb) | (tg_unary_op term) | var_term | this_term
// var_term         ::= tk_identifier (index | fn_call | method_call)?
// this_term        ::= tk_this method_call?
// index            ::= tk_lsb expr tk_rsb
// fn_call          ::= tk_fn tk_identifier tk_lrb expr_list tk_rrb
// method_call      ::= tk_stop tk_identifier tk_lrb expr_list tk_rrb
// expr_list        ::= (expr (tk_comma expr)*)?

// forward declarations of one function per non-terminal rule in the Jack grammar - except for program
//
static ast parse_class();
static ast parse_static_var_decs();
static ast parse_static_var_dec();
static ast parse_field_var_decs();
static ast parse_field_var_dec();
static ast parse_subr_decs();
static ast parse_constructor();
static ast parse_function();
static ast parse_method();
static ast parse_param_list();
static ast parse_subr_body();
static ast parse_var_decs();
static ast parse_var_dec();

static ast parse_statements();
static ast parse_statement();
static ast parse_let();
static ast parse_if();
static ast parse_while();
static ast parse_do();
static ast parse_void_var_call();
static ast parse_void_this_call();
static ast parse_return();

static ast parse_expr();
static ast parse_term();
static ast parse_var_term();
static ast parse_this_term();
static ast parse_index();
static ast parse_fn_call();
static ast parse_method_call();
static ast parse_expr_list();

static string local_class_name;

static void local_fatal_error(string message)
{
    fatal_error(0, "***** Error:\n" + token_context() + message);
}

// class ::= tk_class tk_identifier tk_lcb static_var_decs field_var_decs subr_decs tk_rcb
// create_class(myclassname,statics,fields,class_subrs)
static ast parse_class()
{
    push_error_context("parse_class()");

    // the class needs new symbol tables for its static and field variables
    now_parsing = Class;
    push_symbol_table();

    // add code here ...
    mustbe(tk_class);
    local_class_name = token_spelling(mustbe(tk_identifier));
    mustbe(tk_lcb);
    ast static_vars = parse_static_var_decs();
    ast field_vars = parse_field_var_decs();

    if (static_vars == nullptr && field_vars == nullptr)
    {
        did_not_find(tg_class_var);
    }

    ast subr_decs = parse_subr_decs();
    mustbe(tk_rcb);

    // delete the statics and fields symbol tables
    pop_symbol_table();
    pop_symbol_table();

    ast ret = create_class(local_class_name, static_vars, field_vars, subr_decs);
    pop_error_context();
    // fatal_error(0,"123");
    return ret;
}

// static_var_decs ::= static_var_dec*
// returns ast_class_var_decs: create_class_var_decs(vector<ast> decs)
// create_class_var_decs must be passed a vector of ast_var_decs
//
static ast parse_static_var_decs()
{
    push_error_context("parse_static_var_decs()");

    // add code here ...
    vector<ast> var_decs;

    while (have(tk_static))
    {
        var_decs.push_back(parse_static_var_dec());
    }

    ast ret = create_class_var_decs(var_decs);
    pop_error_context();
    return ret;
}

// static_var_dec ::= tk_static tg_type tk_identifier (tk_comma tk_identifier)* tk_semi
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
// ast_var_dec: declare_variable(Token name,Token type,string segment)
//
static ast parse_static_var_dec()
{
    push_error_context("parse_static_var_dec()");

    // add code here ...
    vector<ast> decs;

    mustbe(tk_static);
    Token type = mustbe(tg_type);
    Token name = mustbe(tk_identifier);
    decs.push_back(declare_variable(name, type, "static"));

    while (have(tk_comma))
    {
        mustbe(tk_comma);
        name = mustbe(tk_identifier);
        decs.push_back(declare_variable(name, type, "static"));
    }

    mustbe(tk_semi);

    ast ret = create_var_decs(decs);
    pop_error_context();
    return ret;
}

// field_var_decs ::= field_var_dec*
// returns ast_class_var_decs: create_class_var_decs(vector<ast> decs)
// create_class_var_decs must be passed a vector of ast_var_decs
//
static ast parse_field_var_decs()
{
    push_error_context("parse_field_var_decs()");

    // create a new symbol table for the fields - delete at the end of parsing the class
    push_symbol_table();

    // add code here ...

    vector<ast> decs;
    while (have(tk_field))
    {
        decs.push_back(parse_field_var_dec());
    }

    ast ret = create_class_var_decs(decs);
    pop_error_context();
    return ret;
}

// field_var_dec ::= tk_field tg_type tk_identifier (tk_comma tk_identifier)* tk_semi
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
// ast_var_dec: declare_variable(Token name,Token type,string segment)
//
static ast parse_field_var_dec()
{
    push_error_context("parse_field_var_dec()");

    // add code here ...
    vector<ast> decs;
    mustbe(tk_field);
    Token type = mustbe(tg_type);
    Token name = mustbe(tk_identifier);
    decs.push_back(declare_variable(name, type, "this"));

    while (have(tk_comma))
    {
        mustbe(tk_comma);
        name = mustbe(tk_identifier);
        decs.push_back(declare_variable(name, type, "this"));
    }

    mustbe(tk_semi);

    ast ret = create_var_decs(decs);
    pop_error_context();
    return ret;
}

// subr_decs ::= (constructor | function | method)*
// returns ast_subr_decs: create_subr_decs(vector<ast> subrs)
// create_subr_decs must be passed an vector of ast_subr
//
// ast_subr: create_subr(ast subr)
// create_subr must be passed one of: ast_constructor, ast_function or ast_method
//
static ast parse_subr_decs()
{
    push_error_context("parse_subr_decs()");

    vector<ast> subrs;
    // add code here ...
    if (!have(tg_subroutine))
    {
        return create_subr_decs(subrs);
    }

    while (have(tg_subroutine))
    {
        switch (token_kind())
        {
        case tk_constructor:
            subrs.push_back(create_subr(parse_constructor()));
            break;
        case tk_function:
            subrs.push_back(create_subr(parse_function()));
            break;
        case tk_method:
            subrs.push_back(create_subr(parse_method()));
            break;

        default:
            did_not_find(tg_subroutine);
            break;
        }
    }

    ast ret = create_subr_decs(subrs);
    pop_error_context();
    return ret;
}

// constructor ::= tk_constructor tk_identifier tk_identifier tk_lrb param_list tk_rrb subr_body
// returns ast_constructor: create_constructor(string vtype,string name,ast params,ast body)
// . vtype: the constructor's return type, this must be it's class name
// . name: the constructor's name within its class
// . params: ast_param_list - the constructor's parameters
// . body: ast_subr_body - the constructor's body
//
static ast parse_constructor()
{
    push_error_context("parse_constructor()");

    // the constructor needs a new symbol table for its parameters and local variables
    now_parsing = Constructor;
    push_symbol_table();

    // add code here ...
    mustbe(tk_constructor);
    string type = token_spelling(mustbe(tk_identifier));
    string name = type;
    if (name != local_class_name)
    {
        fatal_token_context("constructor return type must be its own class\n");
    }
    string constructor_return = token_spelling(mustbe(tk_identifier));

    mustbe(tk_lrb);
    ast params = parse_param_list();
    mustbe(tk_rrb);
    ast subr_body = parse_subr_body();

    // delete the constructor's symbol table
    pop_symbol_table();

    ast ret = create_constructor(type, "new", params, subr_body);
    pop_error_context();
    return ret;
}

static string current_function_type;

// function ::= tk_function tg_vtype tk_identifier tk_lrb param_list tk_rrb subr_body
// returns ast_function: create_function(string vtype,string name,ast params,ast body)
// . vtype: the function's return type
// . name: the function's name within its class
// . params: ast_param_list - the function's parameters
// . body: ast_subr_body - the function's body
// TODO fatal_token_context("a function cannot access this\n") ;
static ast parse_function()
{
    push_error_context("parse_function()");

    // the function needs a new symbol table for its parameters and local variables
    now_parsing = Function;
    push_symbol_table();

    // add code here ...
    mustbe(tk_function);
    string type = token_spelling(mustbe(tg_vtype));
    current_function_type = type;
    string name = token_spelling(mustbe(tk_identifier));
    mustbe(tk_lrb);
    ast params = parse_param_list();
    mustbe(tk_rrb);
    ast subr_body = parse_subr_body();

    // delete the function's symbol table
    pop_symbol_table();

    ast ret = create_function(type, name, params, subr_body);
    pop_error_context();
    return ret;
}

// method ::= tk_method tg_vtype tk_identifier tk_lrb param_list tk_rrb subr_body
// returns ast_method: create_method(string vtype,string name,ast params,ast body)
// . vtype: the method's return type
// . name: the method;s name within its class
// . params: ast_param_list - the method's explicit parameters
// . body: ast_subr_body - the method's body
//
static ast parse_method()
{
    push_error_context("parse_method()");

    // the method needs a new symbol table for its parameters and local variables
    now_parsing = Method;
    push_symbol_table();

    // add code here ...

    mustbe(tk_method);
    string type = token_spelling(mustbe(tg_vtype));
    current_function_type = type;
    string name = token_spelling(mustbe(tk_identifier));
    mustbe(tk_lrb);
    ast params = parse_param_list();
    mustbe(tk_rrb);
    ast body = parse_subr_body();

    // delete the method's symbol table
    pop_symbol_table();

    ast ret = create_method(type, name, params, body);
    pop_error_context();
    return ret;
}

// param_list ::= ((tg_type tk_identifier) (tk_comma tg_type tk_identifier)*)?
// returns ast_param_list: create_param_list(vector<ast> params)
// create_param_list must be passed a vector of ast_var_dec
//
// ast_var_dec: declare_variable(Token name,Token type,string segment)
//
static ast parse_param_list()
{
    push_error_context("parse_param_list()");

    // add code here ...
    vector<ast> params;
    Token name, type;
    string segment = "argument";

    if (have(tg_type))
    {
        type = mustbe(tg_type);
        name = mustbe(tk_identifier);
        params.push_back(declare_variable(name, type, segment));

        while (have(tk_comma))
        {
            mustbe(tk_comma);
            type = mustbe(tg_type);
            name = mustbe(tk_identifier);
            params.push_back(declare_variable(name, type, segment));
        }
    }

    ast ret = create_param_list(params);
    pop_error_context();
    return ret;
}

// subr_body ::= tk_lcb var_decs statements tk_rcb
// returns ast_subr_body: create_subr_body(ast decs,ast body)
// create_subr_body must be passed:
// . decs: ast_var_decs - the subroutine's local variable declarations
// . body: ast_statements - the statements within the body of the subroutinue
//
static ast parse_subr_body()
{
    push_error_context("parse_subr_body()");

    // add code here ...

    mustbe(tk_lcb);
    ast var_decs = parse_var_decs();
    ast statements = parse_statements();

    if (size_of_statements(statements) == 0)
    {
        local_fatal_error("subroutine must finish with a return statement");
    }

    mustbe(tk_rcb);

    ast ret = create_subr_body(var_decs, statements);
    pop_error_context();
    return ret;
}

// var_decs ::= var_dec*
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_decs
//
static ast parse_var_decs()
{
    push_error_context("parse_var_decs()");

    // add code here ...
    vector<ast> decs;
    while (have(tk_var))
    {
        decs.push_back(parse_var_dec());
    }

    ast ret = create_var_decs(decs);
    pop_error_context();
    return ret;
}

// var_dec ::= tk_var tg_type tk_identifier (tk_comma tk_identifier)* tk_semi
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
// ast_var_dec: declare_variable(Token name,Token type,string segment)
//
static ast parse_var_dec()
{
    push_error_context("parse_var_dec()");

    // add code here ...

    vector<ast> decs;
    string segment = "local";
    Token name, type;

    mustbe(tk_var);
    type = mustbe(tg_type);
    name = mustbe(tk_identifier);
    decs.push_back(declare_variable(name, type, segment));
    while (have(tk_comma))
    {
        mustbe(tk_comma);
        name = mustbe(tk_identifier);
        decs.push_back(declare_variable(name, type, segment));
    }
    mustbe(tk_semi);

    ast ret = create_var_decs(decs);
    pop_error_context();
    return ret;
}

// statements ::= statement*
// create_statements(vector<ast> statements)
// create_statements must be passed a vector of ast_statement
//
static ast parse_statements()
{
    push_error_context("parse_statements()");

    // add code here ...

    vector<ast> statements;
    while (have(tg_statement))
    {
        statements.push_back(parse_statement());
    }

    ast ret = create_statements(statements);
    pop_error_context();
    return ret;
}

// statement ::= let | if | while | do | return
// create_statement(ast statement)
// create_statement initialiser must be one of: ast_let, ast_let_array, ast_if, ast_if_else, ast_while, ast_do, ast_return or ast_return_expr
//
static ast parse_statement()
{
    push_error_context("parse_statement()");

    if (now_parsing == Function && token_context().find("this") != std::string::npos)
    {
        fatal_token_context("a function cannot access this\n");
    }

    // add code here ...

    ast statement = nullptr;
    switch (token_kind())
    {
    case tk_let:
        statement = parse_let();
        break;
    case tk_if:
        statement = parse_if();
        break;
    case tk_while:
        statement = parse_while();
        break;
    case tk_do:
        statement = parse_do();
        break;
    case tk_return:
        statement = parse_return();
        break;
    default:
        did_not_find(tg_statement);
        break;
    }

    ast ret = create_statement(statement);
    pop_error_context();
    return ret;
}

// let ::= tk_let tk_identifier index? tk_assign expr tk_semi
// return one of: ast_let or ast_let_array
//
// create_let(ast var,ast expr)
// . var: ast_var - the variable being assigned to
// . expr: ast_expr - the variable's new value
//
// create_let_array(ast var,ast index,ast expr)
// . var: ast_var - the variable for the array being assigned to
// . index: ast_expr - the array index
// . expr: ast_expr - the array element's new value
//
static ast parse_let()
{
    push_error_context("parse_let()");

    // add code here ...

    ast index = nullptr;

    mustbe(tk_let);
    Token var = mustbe(tk_identifier);
    ast ast_var = lookup_variable_fatal(var);
    if (have(tk_lsb))
    {
        index = parse_index();
    }
    mustbe(tk_assign);
    ast expr = parse_expr();
    mustbe(tk_semi);

    ast ret = index == nullptr ? create_let(ast_var, expr) : create_let_array(ast_var, index, expr);
    pop_error_context();
    return ret;
}

// if ::= tk_if tk_lrb expr tk_rrb tk_lcb statements tk_rcb (tk_else tk_lcb statements tk_rcb)?
// return one of: ast_if or ast_if_else
//
// create_if(ast condition,ast if_true)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
//
// create_if_else(ast condition,ast if_true,ast if_false)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
// . if_false: ast_statements - the if false branch
//
static ast parse_if()
{
    push_error_context("parse_if()");

    // add code here ...

    ast inner_statements = nullptr;

    mustbe(tk_if);
    mustbe(tk_lrb);
    ast expr = parse_expr();
    mustbe(tk_rrb);
    mustbe(tk_lcb);
    ast statements = parse_statements();
    mustbe(tk_rcb);

    if (have(tk_else))
    {
        mustbe(tk_else);
        mustbe(tk_lcb);
        inner_statements = parse_statements();
        mustbe(tk_rcb);
    }

    ast ret = inner_statements == nullptr ? create_if(expr, statements) : create_if_else(expr, statements, inner_statements);
    pop_error_context();
    return ret;
}

// while ::= tk_while tk_lrb expr tk_rrb tk_lcb statements tk_rcb
// returns ast_while: create_while(ast condition,ast body)
// . condition: ast_expr - the loop condition
// . body: ast_statements - the loop body
//
static ast parse_while()
{
    push_error_context("parse_while()");

    // add code here ...

    mustbe(tk_while);
    mustbe(tk_lrb);
    ast expr = parse_expr();
    mustbe(tk_rrb);
    mustbe(tk_lcb);
    ast statements = parse_statements();
    mustbe(tk_rcb);

    ast ret = create_while(expr, statements);
    pop_error_context();
    return ret;
}

// do ::= tk_do (void_var_call | void_this_call) tk_semi
// returns ast_do: create_do(ast call)
// create_do must be passed one of: ast_call_as_function or ast_call_as_method
//
static ast parse_do()
{
    push_error_context("parse_do()");

    // add code here ...

    ast call = nullptr;

    mustbe(tk_do);
    if (have(tk_identifier))
    {
        // void_var_call

        call = parse_void_var_call();
    }
    else if (have(tk_this))
    {
        // void_this_call

        call = parse_void_this_call();
    }
    else
    {
        did_not_find(tg_void_call);
    }
    mustbe(tk_semi);

    ast ret = create_do(call);
    pop_error_context();
    return ret;
}

// void_var_call    ::= tk_identifier (fn_call | method_call)
// returns one of: ast_call_as_function or ast_call_as_method
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_var - the variable the method is applied to
// . subr_call: ast_subr_call - the method's name within the variable's class and it's explicit arguments
//
static ast parse_void_var_call()
{
    push_error_context("parse_void_var_call()");

    // add code here ...

    string current_class_name;
    Token name = mustbe(tk_identifier);
    ast var;
    if (have(tk_fn)) {
        current_class_name = token_spelling(name);
    } else {
        var = lookup_variable(name);
    }

    ast call = nullptr;
    ast ret = create_empty();

    if (have(tk_fn))
    {
        // fn_call

        call = parse_fn_call();
        ret = create_call_as_function(current_class_name, call);
    }
    else if (have(tk_stop))
    {
        // method_call

        if (var == nullptr)
        {
            local_fatal_error(token_spelling(name) + " must be a declared variable");
        }
        call = parse_method_call();
        ret = create_call_as_method(get_var_type(var), var, call);
    }
    else
    {
        did_not_find(Jack_Compiler::tg_stop_fn);
    }

    pop_error_context();
    return ret;
}

// void_this_call ::= tk_this method_call
// returns: ast_call_as_method
//
// ast_this: create_this()
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_this - the method is applied to the current object
// . subr_call: ast_subr_call - the method's name and it's explicit arguments
//
static ast parse_void_this_call()
{
    push_error_context("parse_void_this_call()");

    // add code here ...

    mustbe(tk_this);
    ast method_call = parse_method_call();

    ast ret = create_call_as_method(local_class_name, create_this(), method_call);
    pop_error_context();
    return ret;
}

// return ::= tk_return expr? tk_semi
// returns one of: ast_return or ast_return_expr
//
// ast_return: create_return()
//
// ast_return_expr: create_return_expr(ast expr)
// create_return_expr must be passed an ast_expr
//
static ast parse_return()
{
    push_error_context("parse_return()");

    // add code here ...

    ast expr = nullptr;

    mustbe(tk_return);

    if (have(tg_expression) && current_function_type == "void")
    {
        fatal_token_context("returning a value from a void function or method\n");
    }

    if (!have(tg_expression) && current_function_type != "void")
    {
        fatal_token_context("not returning a value from a non-void function or method\n");
    }

    if (now_parsing == Constructor)
    {
        mustbe(tk_this);
        expr = create_expr(create_term(create_this()));
    }
    else if (have(tg_expression))
    {
        expr = parse_expr();
    }
    mustbe(tk_semi);

    ast ret = expr == nullptr ? create_return() : create_return_expr(expr);
    pop_error_context();
    return ret;
}

// expr ::= term (tg_infix_op expr)?
// returns ast_expr: create_expr(term) OR create_expr(lhs,op,rhs)
// term, lhs and rhs must be ast_term nodes
// op must be an ast_infix_op node
//
// ast_infix_op: create_infix_op(string infix_op)
// create_infix_op must be passed:
// . infix_op: the infix op
//
static ast parse_expr()
{
    push_error_context("parse_expr()");

    // add code here ...

    ast op = nullptr, rhs = nullptr;

    ast lhs = parse_term();
    if (have(tg_infix_op))
    {
        op = create_infix_op(token_spelling(mustbe(tg_infix_op)));
        rhs = create_term(parse_expr());
    }

    ast ret = op == nullptr ? create_expr(lhs) : create_expr(lhs, op, rhs);
    pop_error_context();
    return ret;
}

// term ::= tk_integer | tk_string | tk_true | tk_false | tk_null | (tk_lrb expr tk_rrb) | (tg_unary_op term) | var_term | this_term
// returns ast_term: create_term(ast term)
// create_term must be passed one of: ast_int, ast_string, ast_bool, ast_null, ast_this, ast_expr,
//                                    ast_unary_op, ast_var, ast_array_index, ast_call_as_function, ast_call_as_method
//
// ast_int: create_int(int _constant)
// create_int must be passed an integer value in the range 0 to 32767
//
// ast_string: create_string(string _constant)
// create_string must be passed any legal Jack string literal
//
// ast_bool: create_bool(bool t_or_f)
// create_bool must be passed true or false
//
// ast_null: create_null()
//
// ast_unary_op: create_unary_op(string op,ast term)
// create_unary_op must be passed:
// . op: the unary op
// . term: ast_term
//
static ast parse_term()
{
    push_error_context("parse_term()");

    // add code here ...

    ast ast_result = nullptr;

    switch (token_kind())
    {
    case tk_integer:
        ast_result = create_int(stoi(token_spelling(mustbe(tk_integer))));
        break;
    case tk_string:
        ast_result = create_string(token_spelling(mustbe(tk_string)));
        break;
    case tk_true:
        mustbe(tk_true);
        ast_result = create_bool(true);
        break;
    case tk_false:
        mustbe(tk_false);
        ast_result = create_bool(false);
        break;
    case tk_null:
        mustbe(tk_null);
        ast_result = create_null();
        break;
    case tk_lrb:
    {
        mustbe(tk_lrb);
        ast_result = parse_expr();
        mustbe(tk_rrb);
        break;
    }

    // tg_unary_op , no idea why switch case can not stop at tg_unary_op when input is tk_sub

    case tk_sub:
    case tk_not:
    {
        string op = token_spelling(mustbe(tg_unary_op));
        ast term = parse_term();
        ast_result = create_unary_op(op, term);
        break;
    }
    case tk_identifier:
        // var_term
        ast_result = parse_var_term();
        break;
    case tk_this:
        // parse_this_term
        ast_result = parse_this_term();
        break;
    default:
        did_not_find(tg_term);
        break;
    }

    ast ret = create_term(ast_result);
    pop_error_context();
    return ret;
}

// var_term ::= tk_identifier (index | fn_call | method_call)?
// returns one of: ast_var, ast_array_index, ast_call_as_function or ast_call_as_method
//
// ast_var: lookup_variable_fatal(Token name)
//
// ast_array_index: create_array_index(ast var,ast index)
// create_ast_array_index must be passed:
// . var: ast_var - the array variable
// . index: ast_expr - the index into the array
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the variable's class
// . object: ast_var - the variable the method is applied to
// . subr_call: ast_subr_call - the method's name within the variable's class and it's explicit arguments
//
static ast parse_var_term()
{
    push_error_context("parse_var_term()");

    // add code here ...

    string current_class_name;
    Token name = mustbe(tk_identifier);
    ast var;
    if (have(tk_fn)) {
        current_class_name = token_spelling(name);
    } else {
        var = lookup_variable_fatal(name);
    }

    // just a tk_identifier

    if (!(have(tk_lsb) || have(tk_fn) || have(tk_stop)))
    {
        return var;
    }

    ast temp = nullptr;
    switch (token_kind())
    {
    case tk_lsb:
    {
        temp = parse_index();
        return create_array_index(var, temp);
        break;
    }
    case tk_fn:
    {
        temp = parse_fn_call();
        return create_call_as_function(current_class_name, temp);
        break;
    }
    case tk_stop:
    {
        temp = parse_method_call();
        return create_call_as_method(get_var_type(var), var, temp);
        break;
    }

    default:
        break;
    }

    ast ret = create_empty();
    pop_error_context();
    return ret;
}

// this_term ::= tk_this method_call?
// returns one of: ast_this or ast_call_as_method
//
// ast_this: create_this()
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the class being parsed
// . object: ast_this - the method is applied to the current object
// . subr_call: ast_subr_call - the method's name and it's explicit arguments
//
static ast parse_this_term()
{
    push_error_context("parse_this_term()");

    // add code here ...

    ast subr_call = nullptr;

    mustbe(tk_this);

    if (have(tk_stop))
    {
        subr_call = parse_method_call();
    }

    ast ret = subr_call == nullptr ? create_this() : create_call_as_method(local_class_name, create_this(), subr_call);
    pop_error_context();
    return ret;
}

// index ::= tk_lsb expr tk_rsb
// returns ast_expr
static ast parse_index()
{
    push_error_context("parse_index()");

    // add code here ...

    mustbe(tk_lsb);
    ast expr = parse_expr();
    mustbe(tk_rsb);

    ast ret = expr;
    pop_error_context();
    return ret;
}

// fn_call ::= tk_fn tk_identifier tk_lrb expr_list tk_rrb
// returns ast_subr_call: create_subr_call(string subr_name,ast expr_list)
// create_subr_call must be passed:
// . subr_name: the constructor or function's name within its class
// . expr_list: ast_expr_list - the explicit arguments to the call
//
static ast parse_fn_call()
{
    push_error_context("parse_fn_call()");

    // add code here ...

    mustbe(tk_fn);
    string name = token_spelling(mustbe(tk_identifier));
    mustbe(tk_lrb);
    ast expr_list = parse_expr_list();
    mustbe(tk_rrb);

    ast ret = create_subr_call(name, expr_list);
    ;
    pop_error_context();
    return ret;
}

// method_call ::= tk_stop tk_identifier tk_lrb expr_list tk_rrb
// returns ast_subr_call: create_subr_call(string subr_name,ast expr_list)
// create_subr_call must be passed:
// . subr_name: the method's name within its class
// . expr_list: ast_expr_list - the explicit arguments to the call
//
static ast parse_method_call()
{
    push_error_context("parse_method_call()");

    // add code here ...

    mustbe(tk_stop);
    string name = token_spelling(mustbe(tk_identifier));
    ;
    mustbe(tk_lrb);
    ast expr_list = parse_expr_list();
    mustbe(tk_rrb);

    ast ret = create_subr_call(name, expr_list);
    pop_error_context();
    return ret;
}

// expr_list ::= (expr (tk_comma expr)*)?
// returns ast_expr_list: create_expr_list(vector<ast> exprs)
// create_expr_list must be pashsed: a vector of ast_expr
//
static ast parse_expr_list()
{
    push_error_context("parse_expr_list()");

    // add code here ...
    vector<ast> exprs;
    if (have(tg_term))
    {
        exprs.push_back(parse_expr());
        while (have(tk_comma))
        {
            mustbe(tk_comma);
            exprs.push_back(parse_expr());
        }
    }

    ast ret = create_expr_list(exprs);
    pop_error_context();
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
// ***** DO NOT CHANGE CODE AFTER THIS POINT *****

// ****  SYMBOL TABLES  ****

// scope levels each have their own symbol tables, segment and next free location
static jsymbols symbol_tables = nullptr;

// push a new symbol table onto the stack of symbol tables
// remember statics or fields tables when first created
static void push_symbol_table()
{
    symbol_tables = jsymbols_push(symbol_tables);

    switch (now_parsing)
    {
        // For Methods we need the argument segment offsets to start at 1
    case Method:
        jsymbols_offset(symbol_tables, "argument");
        break;

        // For Class, Constructor and Function there is nothing else to do
    case Class:
    case Constructor:
    case Function:
    default:
        break;
    }
}

// pop the top scope from the stack of symbol tables and delete it
static void pop_symbol_table()
{
    symbol_tables = jsymbols_pop(symbol_tables);
}

// this function adds an identifier to the top symbol table on the symbol table stack
// it allocates the variable the next free offset in its segment
// it returns a tree node representing the variable
// a fatal error is reported if the variable has already been declared
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
static ast declare_variable(Token identifier, Token type, string varsegment)
{
    string varname = token_spelling(identifier);
    string vartype = token_spelling(type);
    int varoffset = jsymbols_offset(symbol_tables, varsegment);

    if (!jsymbols_insert(symbol_tables, varname, vartype, varsegment, varoffset)) // it is an error to declare something twice
    {
        fatal_token_context(varname + " has already been declared\n");
    }

    return create_var_dec(varname, varsegment, varoffset, vartype);
}

// lookup the identifier and return a tree node representing the variable
// if the variable cannot be found, nullptr is returned
//
// ast_var: create_var(string name,string segment,int offset,string type)
// create_ast_var must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
static ast lookup_variable(Token identifier)
{
    // search symbol tables from top to bottom of the symbol table stack
    string varname = token_spelling(identifier);
    jack_var var = jsymbols_lookup(symbol_tables, varname);
    if (var != nullptr)
    {
        // functions have no object so they cannot access the "this" segment
        if (jack_var_segment(var) == "this" && now_parsing == Function)
            return nullptr;

        return create_var(varname, jack_var_segment(var), jack_var_offset(var), jack_var_type(var));
    }

    return nullptr;
}

// lookup variable but report a fatal error if not found
static ast lookup_variable_fatal(Token identifier)
{
    ast var = lookup_variable(identifier);

    // if not found report a fatal error
    if (var == nullptr)
    {
        string varname = token_spelling(identifier);
        fatal_token_context(varname + " must be a declared variable\n");
    }

    return var;
}

// **** MAIN PROGRAM ****

// main program
int main(int argc, char **argv)
{
    config_output(iob_immediate); // make all output and errors appear immediately
    config_errors(iob_immediate); // this will be overridden by the test scripts

    // read the first token to get the tokeniser initialised
    next_token();

    // parse a Jack class and print the abstract syntax tree as XML
    ast_print_as_xml(parse_class(), 3);

    // flush the output and any errors
    print_output();
    print_errors();
}
