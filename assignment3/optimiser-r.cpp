//
//
// Author : Akide Liu
// Date : 12/10/21
// Student ID : A1743748
//

#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"
#include <bits/stdc++.h>

// to shorten our code:
using namespace std;
using namespace CS_IO_Buffers;
using namespace CS_Symbol_Tables;
using namespace Jack_Compiler;

#pragma clang diagnostic ignored "-Wunused-function"

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to make an optimised copy an abstract syntax tree, ast, of a Jack class.
//
// NOTE: abstract syntax trees are immutable - they cannot change - and cannot contain cycles so sub-trees
//       can be safely shared across multiple trees. As a result copying a node only requires the creation
//       of a new tree node if one of its fields changes or one of its sub-trees changes.
//
// The skeleton code has one function per node in the abstract tree
//  - they each extract all fields into local variables
//  - they each call the appropriate prune_* function to copy any sub-trees
//
// The structure of the skeleton code is just a suggestion
//  - you may want to change the parameters / results to suit your own logic
//  - you can change it as much as you like

// forward declarations of one function per node in the abstract syntax tree
static ast prune_class(ast t);
static ast prune_class_var_decs(ast t);
static ast prune_var_dec(ast t);
static ast prune_subr_decs(ast t);
static ast prune_subr(ast t);
static ast prune_constructor(ast t);
static ast prune_function(ast t);
static ast prune_method(ast t);
static ast prune_param_list(ast t);
static ast prune_subr_body(ast t);
static ast prune_var_decs(ast t);
static ast prune_statements(ast t);
static ast prune_statement(ast t);
static ast prune_let(ast t);
static ast prune_let_array(ast t);
static ast prune_if(ast t);
static ast prune_if_else(ast t);
static ast prune_while(ast t);
static ast prune_do(ast t);
static ast prune_return(ast t);
static ast prune_return_expr(ast t);
static ast prune_expr(ast t);
static ast prune_term(ast t);
static ast prune_int(ast t);
static ast prune_string(ast t);
static ast prune_bool(ast t);
static ast prune_null(ast t);
static ast prune_this(ast t);
static ast prune_unary_op(ast t);
static ast prune_var(ast t);
static ast prune_array_index(ast t);
static ast prune_call_as_function(ast t);
static ast prune_call_as_method(ast t);
static ast prune_subr_call(ast t);
static ast prune_expr_list(ast t);
static ast prune_infix_op(ast t);

struct Global
{
    bool while_condition = true;
    vector<string> static_vars;
    vector<string> local_vars;
    bool is_return = false;
    bool is_if_true_return = false;
    bool is_if_else_return = false;
} global;

enum NOW_PRUNE
{
    FUNCTION,
    IF_ELSE,
    WHILE
};

NOW_PRUNE now_parsing;

// copy an ast class node with fields:
// class_name - a string
// var_decs   - ast vector of variable declarations
// subr_decs  - ast vector of subroutine declarations
//
static ast prune_class(ast t)
{
    string myclassname = get_class_class_name(t);
    ast statics = get_class_statics(t);
    ast fields = get_class_fields(t);
    ast subr_decs = get_class_subr_decs(t);

    subr_decs = prune_subr_decs(subr_decs);

    statics = prune_class_var_decs(statics);
    fields = prune_class_var_decs(fields);

    return create_class(get_ann(t), myclassname, statics, fields, subr_decs);
}

// copy an ast class var decs node
// it is an ast vector of variable declarations
//
static ast prune_class_var_decs(ast t)
{
    vector<ast> decs;

    int ndecs = size_of_class_var_decs(t);
    for (int i = 0; i < ndecs; i++)
    {
        ast deci = get_class_var_decs(t, i);
        string name = get_var_dec_name(deci);
        string type = get_var_dec_segment(deci);

        if (type == "static")
        {
            if (find(global.static_vars.begin(), global.static_vars.end(), name) != global.static_vars.end())
            {
                decs.push_back(prune_var_dec(deci));
            }
        }
        else
        {
            decs.push_back(prune_var_dec(deci));
        }
    }

    return create_class_var_decs(get_ann(t), decs);
}

// copy an ast variable declaration with fields
// name - a string
// type - a string - "int", "char", "boolean" or a class name
// segment - a string
// offset - an int
// this is used for statics, fields, parameters and local variables
//
static ast prune_var_dec(ast t)
{
    // string name = get_var_dec_name(t) ;
    // string type = get_var_dec_type(t) ;
    // string segment = get_var_dec_segment(t) ;
    // int offset = get_var_dec_offset(t) ;

    return t;
}

// copy an ast class var decs node
// it is an ast vector of subroutine declarations
//
static ast prune_subr_decs(ast t)
{
    vector<ast> decs;

    int size = size_of_subr_decs(t);
    for (int i = 0; i < size; i++)
    {
        ast deci = get_subr_decs(t, i);
        decs.push_back(prune_subr(deci));
    }

    return create_subr_decs(get_ann(t), decs);
}

// copy an ast subroutine node with a single field
// subr - an ast constructor, ast function or ast method node
//
static ast prune_subr(ast t)
{
    ast subr = get_subr_subr(t);

    switch (ast_node_kind(subr))
    {
    case ast_constructor:
        subr = prune_constructor(subr);
        break;
    case ast_function:
        subr = prune_function(subr);
        break;
    case ast_method:
        subr = prune_method(subr);
        break;
    default:
        fatal_error(0, "bad subroutine dec found");
        break;
    }

    return create_subr(get_ann(t), subr);
}

// copy an ast constructor node with fields
// vtype - a string - the name of the class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
static ast prune_constructor(ast t)
{
    string vtype = get_constructor_vtype(t);
    string name = get_constructor_name(t);
    ast param_list = get_constructor_param_list(t);
    ast subr_body = get_constructor_subr_body(t);

    param_list = prune_param_list(param_list);
    subr_body = prune_subr_body(subr_body);

    return create_constructor(get_ann(t), vtype, name, param_list, subr_body);
}

// copy an ast function node with fields
// vtype - a string - one of "void", "int", "char", "boolean" or the name of a class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
static ast prune_function(ast t)
{

    now_parsing = FUNCTION;

    string vtype = get_function_vtype(t);
    string name = get_function_name(t);
    ast param_list = get_function_param_list(t);
    ast subr_body = get_function_subr_body(t);

    param_list = prune_param_list(param_list);
    subr_body = prune_subr_body(subr_body);

    return create_function(get_ann(t), vtype, name, param_list, subr_body);
}

// copy an ast method node with fields
// vtype - a string - one of "void", "int", "char", "boolean" or the name of a class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
static ast prune_method(ast t)
{
    string vtype = get_method_vtype(t);
    string name = get_method_name(t);
    ast param_list = get_method_param_list(t);
    ast subr_body = get_method_subr_body(t);

    param_list = prune_param_list(param_list);
    subr_body = prune_subr_body(subr_body);

    return create_method(get_ann(t), vtype, name, param_list, subr_body);
}

// copy an ast param list node
// it is an ast vector of variable declarations
//
static ast prune_param_list(ast t)
{
    vector<ast> decs;

    int size = size_of_param_list(t);
    for (int i = 0; i < size; i++)
    {
        ast deci = get_param_list(t, i);
        decs.push_back(prune_var_dec(deci));
    }

    return create_param_list(get_ann(t), decs);
}

// copy an ast subr body node with fields
// decs - an ast vector of variable declarations
// body - an ast vector of statement nodes
//
static ast prune_subr_body(ast t)
{
    ast decs = get_subr_body_decs(t);
    ast body = get_subr_body_body(t);

    body = prune_statements(body);
    decs = prune_var_decs(decs);
    global.local_vars.clear();

    int numbers = size_of_statements(body);
    if (numbers == 1)
    {
        decs = create_empty();
    }

    return create_subr_body(get_ann(t), decs, body);
}

// copy an ast param list node
// it is an ast vector of variable declarations
//
static ast prune_var_decs(ast t)
{
    vector<ast> decs;

    int size = size_of_var_decs(t);
    for (int i = 0; i < size; i++)
    {
        ast deci = get_var_decs(t, i);
        string name = get_var_dec_name(deci);
        string type = get_var_dec_segment(deci);

        if (type == "local")
        {
            if (find(global.local_vars.begin(), global.local_vars.end(), name) != global.local_vars.end())
            {
                decs.push_back(prune_var_dec(deci));
            }
        }
        else
        {
            decs.push_back(prune_var_dec(deci));
        }
    }

    return create_var_decs(get_ann(t), decs);
}

// copy an ast statements node
// it is an ast vector of statement nodes
//
static ast prune_statements(ast t)
{
    vector<ast> decs;

    global.is_return = false;

    int size = size_of_statements(t);
    for (int i = 0; i < size; i++)
    {
        ast deci = get_statements(t, i);
        decs.push_back(prune_statement(deci));

        // if there is else statement or if true statement
        // no more process the following lines
        if (global.is_if_else_return && global.is_if_true_return && now_parsing == IF_ELSE)
        {
            break;
        }

        // if there is a return to the function body
        // no more process the following lines
        if (global.is_return)
        {
            break;
        }
    }
    if (decs.empty())
    {
        return create_empty();
    }

    return create_statements(get_ann(t), decs);
}

enum Conditional_result
{
    TRUE,
    FALSE,
    NOT_FOUND
};

static bool do_not_evaluate = false;

// condition is the expr
static int evaluate_conditional(ast condition){

    for (int i = 0; i < size_of_expr(condition); ++i) {
        if (ast_have_kind(get_expr(condition, i), ast_term)) {
            ast term = get_term_term(get_expr(condition, i));
            if(ast_have_kind(term,ast_var)) {
                do_not_evaluate = true;
                return numeric_limits<int>::min();
            }
        }
    }

    if (size_of_expr(condition) == 1) {
        if (ast_have_kind(get_expr(condition, 0), ast_term)) {
            ast term = get_term_term(get_expr(condition, 0));
            if (ast_have_kind(term, ast_int)) {
                return get_int_constant(term);
            }

            if (ast_have_kind(term, ast_unary_op)) {
                ast unary_term = get_unary_op_term(term);
                if (ast_have_kind(get_term_term(unary_term), ast_expr)) {
                    int result = evaluate_conditional(get_term_term(unary_term));
                    return 0 - result;
                }

                if (ast_have_kind(get_term_term(unary_term), ast_int)) {
                    return 0 - get_int_constant(get_term_term(unary_term));
                }

            }
        }
    }

    if (size_of_expr(condition) == 3) {
        if (ast_have_kind(get_expr(condition, 0), ast_term)) {

            ast LHS = get_term_term(get_expr(condition, 0));
            ast OP = get_expr(condition, 1);
            ast RHS = get_term_term(get_expr(condition, 2));

            int lhs,rhs; string op;

            if (ast_have_kind(LHS, ast_int)) {
                lhs = get_int_constant(LHS);
            } else if (ast_have_kind(LHS, ast_expr)) {
                lhs = evaluate_conditional(LHS);
            } else {
                do_not_evaluate = true;
                return numeric_limits<int>::min();
            }

            // op - a string - one of "+", "-", "*", "/", "&", "|", "<", ">", "
            if (ast_have_kind(OP, ast_infix_op)) {
                op = get_infix_op_op(OP);
            }

            if (ast_have_kind(RHS, ast_expr)) {
                rhs = evaluate_conditional(RHS);
            }

            int result = numeric_limits<int>::min();
            if (op == "+") {
                result = lhs + rhs;
            } else if (op == "-") {
                result = lhs - rhs;
            } else if (op == "*") {
                result = lhs * rhs;
            } else if (op == "/") {
                // divider can not be 0
                if (rhs == 0){
                    do_not_evaluate = true;
                    return numeric_limits<int>::min();
                }
                result = lhs / rhs;
            } else if (op == "&") {
                result = lhs & rhs;
            } else if (op == "|") {
                result = lhs | rhs;
            } else if (op == "<") {
                result = lhs < rhs;
            } else if (op == ">") {
                result = lhs > rhs;
            } else if (op == "=") {
                result = lhs == rhs;
            }

            return result;
        }
    }

    return numeric_limits<int>::min();
}

// the function to check conditions statement
// support optimize if and while statement
//
static Conditional_result local_prune_conditional_statement(ast condition)
{

    if (size_of_expr(condition) == 1)
    {
        if (ast_have_kind(get_expr(condition, 0), ast_term))
        {
            // if it is boolean constants
            ast term = get_term_term(get_expr(condition, 0));
            if (ast_have_kind(term, ast_bool))
            {
                bool _condition = get_bool_t_or_f(get_term_term(get_expr(condition, 0)));

                if (_condition)
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            // if it is an integer
            if (ast_have_kind(term, ast_int))
            {
                if (get_int_constant(term) == 0)
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
            // if it have a unary_op
            if (ast_have_kind(term, ast_unary_op)) {

                ast unary_term = get_unary_op_term(term);
                int result = evaluate_conditional(get_term_term(unary_term));

                if (do_not_evaluate) { return NOT_FOUND; }

                return TRUE;

            }
        }
    }

    if (size_of_expr(condition) == 3) {
        int result = evaluate_conditional(condition);

        if (do_not_evaluate) { return NOT_FOUND; }

        if (result == 0) {
            return FALSE;
        }else {
            return TRUE;
        }
    }

    return NOT_FOUND;
};

// copy an ast statement node with a single field
// statement - one of the following ast nodes, let, let array, if, if else, while, do, return, return expr or statements
//
static ast prune_statement(ast t)
{
    ast statement = get_statement_statement(t);

    switch (ast_node_kind(statement))
    {
    case ast_let:
        statement = prune_let(statement);
        break;
    case ast_let_array:
        statement = prune_let_array(statement);
        break;
    case ast_if:
    {
        // in if statement, if condition from local_prune_conditional_statement is true
        // recreate statement by if true part
        // if it is false
        // make the statement empty
        statement = prune_if(statement);
        ast condition = get_if_condition(statement);
        auto result = local_prune_conditional_statement(condition);
        if (result == TRUE)
        {
            return get_if_if_true(statement);
        }
        else if (result == FALSE)
        {
            return create_empty();
        }
        break;
    }
    case ast_if_else:
    {
        now_parsing = IF_ELSE;

        // in if statement, if condition from local_prune_conditional_statement is true
        // recreate statement by if true part
        // if it is false
        // make the statement if false part
        statement = prune_if_else(statement);
        ast condition = get_if_else_condition(statement);

        auto result = local_prune_conditional_statement(condition);
        if (result == TRUE)
        {
            return get_if_else_if_true(statement);
        }
        else if (result == FALSE)
        {
            return get_if_else_if_false(statement);
        }
        break;
    }

    case ast_while:
    {
        // in if statement, if condition from local_prune_conditional_statement is true
        // recreate statement by an empty body
        // if it is false
        // make the statement by while body
        now_parsing = WHILE;
        statement = prune_while(statement);
        ast condition = get_while_condition(statement);
        auto result = local_prune_conditional_statement(condition);
        if (result == FALSE)
        {
            return create_empty();
        }

        if (result == TRUE && global.is_return)
        {
            return get_while_body(statement);
        }

        break;
    }

    case ast_do:
        statement = prune_do(statement);
        break;
    case ast_return:
        statement = prune_return(statement);
        break;
    case ast_return_expr:
        statement = prune_return_expr(statement);
        break;
    case ast_statements:
        statement = prune_statements(statement);
        break;
    default:
        fatal_error(0, "Unexpected statement kind");
        break;
    }

    return create_statement(get_ann(t), statement);
}

// copy an ast let node with fields
// var  - an ast variable
// expr - an ast expr node
//
static ast prune_let(ast t)
{
    ast var = get_let_var(t);
    ast expr = get_let_expr(t);

    var = prune_var(var);
    expr = prune_expr(expr);

    return create_let(get_ann(t), var, expr);
}

// copy an ast let array node with fields
// var    - an ast variable
// index  - an ast expr node
// expr   - an ast expr node
//
static ast prune_let_array(ast t)
{
    ast var = get_let_array_var(t);
    ast index = get_let_array_index(t);
    ast expr = get_let_array_expr(t);

    var = prune_var(var);
    index = prune_expr(index);
    expr = prune_expr(expr);

    return create_let_array(get_ann(t), var, index, expr);
}

// copy an ast if node with fields
// condition - an ast expr node
// if true   - an ast statements node
//
static ast prune_if(ast t)
{
    ast condition = get_if_condition(t);
    ast if_true = get_if_if_true(t);

    condition = prune_expr(condition);
    if_true = prune_statements(if_true);

    return create_if(get_ann(t), condition, if_true);
}

// copy an ast if else node with fields
// condition - an ast expr node
// if true   - an ast statements node
// if else   - an ast statements node
//
static ast prune_if_else(ast t)
{
    ast condition = get_if_else_condition(t);
    ast if_true = get_if_else_if_true(t);
    ast if_false = get_if_else_if_false(t);

    condition = prune_expr(condition);
    if_true = prune_statements(if_true);
    if (global.is_return && now_parsing == IF_ELSE)
    {
        global.is_if_true_return = true;
    }
    if_false = prune_statements(if_false);
    if (global.is_return && now_parsing == IF_ELSE)
    {
        global.is_if_else_return = true;
    }

    return create_if_else(get_ann(t), condition, if_true, if_false);
}

// copy an ast while node with fields
// condition - an ast expr node
// body      - an ast statements node
//
static ast prune_while(ast t)
{
    ast condition = get_while_condition(t);
    ast body = get_while_body(t);

    condition = prune_expr(condition);
    body = prune_statements(body);

    return create_while(get_ann(t), condition, body);
}

// copy an ast do node with a single field
// call - an ast call as function node or an ast call as method node
//
static ast prune_do(ast t)
{
    ast call = get_do_call(t);

    switch (ast_node_kind(call))
    {
    case ast_call_as_function:
        call = prune_call_as_function(call);
        break;
    case ast_call_as_method:
        call = prune_call_as_method(call);
        break;
    default:
        fatal_error(0, "Unexpected call kind\n");
        break;
    }

    return create_do(get_ann(t), call);
}

// copy an ast return node, it has not fields
// nothing to do ...
//
static ast prune_return(ast t)
{
    global.is_return = true;
    return t;
}

// copy an ast return expr node with a single field
// expr - an ast expr node
//
static ast prune_return_expr(ast t)
{
    ast expr = get_return_expr(t);

    expr = prune_expr(expr);

    global.is_return = true;

    return create_return_expr(get_ann(t), expr);
}

// copy an ast param list node
// it is an ast vector of ast term and infix op nodes
// it must be of odd length > 0, ie 1, 3, 5, 7, ...
// all elements at even indices are an ast term node
// all elements at odd indices are an ast infix op
//
static ast prune_expr(ast t)
{
    vector<ast> terms;

    int size = size_of_expr(t);
    for (int i = 0; i < size; i++)
    {
        ast termop = get_expr(t, i);
        termop = i % 2 == 0 ? prune_term(termop) : prune_infix_op(termop);
        terms.push_back(termop);
    }

    return create_expr(get_ann(t), terms);
}

// copy an ast term node with a single field
// term - one of the following ast nodes:
//        int, string, bool, null, this, expr, unary op,
//        variable, array index, call as function or call as method
//
static ast prune_term(ast t)
{
    ast term = get_term_term(t);

    switch (ast_node_kind(term))
    {
    case ast_int:
        term = prune_int(term);
        break;
    case ast_string:
        term = prune_string(term);
        break;
    case ast_bool:
        term = prune_bool(term);
        break;
    case ast_null:
        term = prune_null(term);
        break;
    case ast_this:
        term = prune_this(term);
        break;
    case ast_expr:
        term = prune_expr(term);
        break;
    case ast_unary_op:
        term = prune_unary_op(term);
        break;
    case ast_var:
        term = prune_var(term);
        break;
    case ast_array_index:
        term = prune_array_index(term);
        break;
    case ast_call_as_function:
        term = prune_call_as_function(term);
        break;
    case ast_call_as_method:
        term = prune_call_as_method(term);
        break;
    default:
        fatal_error(0, "Unexpected term kind\n");
        break;
    }

    return create_term(get_ann(t), term);
}

// copy an ast int node with a single field
// constant - an integer in the range -32,768 to 32,767
//
static ast prune_int(ast t)
{
    // int _constant = get_int_constant(t) ;

    return t;
}

// copy an ast string node with a single field
// constant - a string
//
static ast prune_string(ast t)
{
    // string _constant = get_string_constant(t) ;

    return t;
}

// copy an ast bool node with a single field
// constant - either true or false
//
static ast prune_bool(ast t)
{
    // bool _constant = get_bool_t_or_f(t) ;

    return t;
}

// copy an ast null node, it has not fields
// nothing to do ...
//
static ast prune_null(ast t)
{
    return t;
}

// copy an ast this node, it has not fields
// nothing to do ...
//
static ast prune_this(ast t)
{
    return t;
}

// copy an ast unary op node with fields
// op   - a string, either "-" or "~"
// term - an ast term node
//        int, string, bool, null, this, expr, unary op,
//        variable, array index, call as function or call as method
//
static ast prune_unary_op(ast t)
{
    string uop = get_unary_op_op(t);
    ast term = get_unary_op_term(t);

    term = prune_term(term);

    return create_unary_op(get_ann(t), uop, term);
}

// copy an ast variable node with fields
// name - a string
// type - a string - "int", "char", "boolean" or a class name
// segment - a string
// offset - an int
//
static ast prune_var(ast t)
{
    string name = get_var_name(t);
    // string type = get_var_type(t) ;
    string segment = get_var_segment(t);
    // int offset = get_var_offset(t) ;
    if (segment == "static")
    {
        global.static_vars.push_back(name);
    }

    if (segment == "local")
    {
        global.local_vars.push_back(name);
    }

    return t;
}

// copy an ast array index node with fields
// var   - an ast variable node
// index - an ast expr node
//
static ast prune_array_index(ast t)
{
    ast var = get_array_index_var(t);
    ast index = get_array_index_index(t);

    var = prune_var(var);
    index = prune_expr(index);

    return create_array_index(get_ann(t), var, index);
}

// copy an ast subr call as method with fields
// class name - a string
// call       - an ast subr call node
//
static ast prune_call_as_function(ast t)
{
    string class_name = get_call_as_function_class_name(t);
    ast subr_call = get_call_as_function_subr_call(t);

    subr_call = prune_subr_call(subr_call);

    return create_call_as_function(get_ann(t), class_name, subr_call);
}

// copy an ast subr call as method with fields
// class name - a string
// var        - an ast variable or ast this node, hidden first parameter of the call
// call       - an ast subr call node
//
static ast prune_call_as_method(ast t)
{
    string class_name = get_call_as_method_class_name(t);
    ast var = get_call_as_method_var(t);
    ast subr_call = get_call_as_method_subr_call(t);

    var = ast_node_kind(var) == ast_var ? prune_var(var) : prune_this(var);
    subr_call = prune_subr_call(subr_call);

    return create_call_as_method(get_ann(t), class_name, var, subr_call);
}

// copy an ast subr call node with fields
// name      - a string
// expr list - a vector of ast expr nodes
//
static ast prune_subr_call(ast t)
{
    string subr_name = get_subr_call_subr_name(t);
    ast expr_list = get_subr_call_expr_list(t);

    expr_list = prune_expr_list(expr_list);

    return create_subr_call(get_ann(t), subr_name, expr_list);
}

// copy an ast expr list node
// it is an ast vector of ast expr nodes
//
static ast prune_expr_list(ast t)
{
    vector<ast> exprs;

    int size = size_of_expr_list(t);
    for (int i = 0; i < size; i++)
    {
        ast expri = get_expr_list(t, i);
        exprs.push_back(prune_expr(expri));
    }

    return create_expr_list(get_ann(t), exprs);
}

// copy an ast infix op node with a single field
// op - a string - one of "+", "-", "*", "/", "&", "|", "<", ">", "="
//
static ast prune_infix_op(ast t)
{
    // string op = get_infix_op_op(t) ;

    return t;
}

// main program
int main(int argc, char **argv)
{
    // walk an AST in XML and print VM code
    ast_print_as_xml(prune_class(ast_parse_xml()), 3);

    // flush the output and any errors
    print_output();
    print_errors();
}
