                            //
                            // The following tokens are keywords
        tk_bool,            // 'bool'
        tk_else,            // 'else'
        tk_if,              // 'if'
        tk_int,             // 'int'
        tk_let,             // 'let'
        tk_var,             // 'var'
        tk_while,           // 'while'

                            // The following tokens are not produced by the tokeniser
                            // These are provided to describe groups of tokens
        tg_statement,       // one of tk_while, tk_if, tk_else, tk_let or tk_lcb
        tg_term,            // one of tk_identifier or tk_integer
        tg_infix_op,        // one of tk_add, tk_sub, tk_times or tk_divide, 
        tg_relop,           // one of tk_lt, tk_le, tk_eq, tk_ne, tk_ge, tk_gt,
        tg_type,            // one of tk_identifier or tk_integer

        tk_last             // the last token value

