
c_type = {}
arith_dict = {}
pushpop_dict = {}
branch_dict = {}
function_dict = {}


def dict_initializer():

    # defines the command types
    c_type.update([ ('add', 'c_arith'), ('sub', 'c_arith'), ('neg', 'c_arith'), ('eq', 'c_arith'),
    ('gt', 'c_arith'), ('lt', 'c_arith'), ('and', 'c_arith'), ('or', 'c_arith'), ('not', 'c_arith'),
    ('pop', 'c_pop'), ('push', 'c_push'), ('label', 'c_label'), ('goto', 'c_goto'), ('if-goto', 'c_if'),
    ('function', 'c_function'), ('call', 'c_call'), ('return', 'c_return')])

    arith_dict['add'] = """@SP
M=M-1
A=M
D=M
A=A-1
M=D+M
"""

    arith_dict['sub'] = """@SP
M=M-1
A=M
D=M
A=A-1
M=M-D
"""

    arith_dict['neg'] = """@SP
A=M-1
M=-M
"""

# $$TRUE$$ will be replaced when translating the code

    arith_dict['eq'] = """@SP
M=M-1
A=M
D=M
A=A-1
D=D-M
M=-1
@$$TRUE$$
D;JEQ
@SP
A=M-1
M=0
($$TRUE$$)
"""

    arith_dict['gt'] = """@SP
M=M-1
A=M
D=M
A=A-1
D=M-D
M=-1
@$$TRUE$$
D;JGT
@SP
A=M-1
M=0
($$TRUE$$)
"""

    arith_dict['lt'] = """@SP
M=M-1
A=M
D=M
A=A-1
D=M-D
M=-1
@$$TRUE$$
D;JLT
@SP
A=M-1
M=0
($$TRUE$$)
"""

    arith_dict['and'] = """@SP
M=M-1
A=M
D=M
A=A-1
M=D&M
"""

    arith_dict['or'] = """@SP
M=M-1
A=M
D=M
A=A-1
M=D|M
"""

    arith_dict['not'] = """@SP
A=M-1
M=!M
"""

# template push/pop commands
    pushpop_dict['pop'] = """
@R13
M=D
@SP
M=M-1
A=M
D=M
@R13
A=M
M=D
"""

    pushpop_dict['push'] = """
D=M
@SP
M=M+1
A=M-1
M=D
"""

    pushpop_dict['local'] = """@$i$
D=A
@LCL
AD=D+M"""

    pushpop_dict['argument'] = """@$i$
D=A
@ARG
AD=D+M"""

    pushpop_dict['this'] = """@$i$
D=A
@THIS
AD=D+M"""

    pushpop_dict['that'] = """@$i$
D=A
@THAT
AD=D+M"""

    pushpop_dict['static'] = """@$DUMMY$"""

    pushpop_dict['temp'] = """@$DUMMY$
"""

    pushpop_dict['pointer'] = """@$DUMMY$
D=A
"""

    pushpop_dict['constant'] = """@$i$
D=A
@SP
M=M+1
A=M-1
M=D
"""

    branch_dict['label'] = """($LABEL$)
"""

    branch_dict['goto'] = """@$LABEL$
0;JMP
"""

    branch_dict['if-goto'] = """@SP
M=M-1
A=M
D=M
@$LABEL$
D;JNE
"""

    function_dict['call_addpush'] = """@$RETADD$
D=A
@SP
M=M+1
A=M-1
M=D
"""

    function_dict['call_segsaver'] = """@LCL
D=M
@SP
M=M+1
A=M-1
M=D
@ARG
D=M
@SP
M=M+1
A=M-1
M=D
@THIS
D=M
@SP
M=M+1
A=M-1
M=D
@THAT
D=M
@SP
M=M+1
A=M-1
M=D
"""

    function_dict['arg_set_1'] = """@SP
A=M
"""

    function_dict['arg_set_2'] = """A=A-1
"""

    function_dict['arg_set_3'] = """D=A
@ARG
M=D
"""

    function_dict['lcl_set'] = """@SP
D=M
@LCL
M=D
"""

    function_dict['endframe_store'] = """@LCL
D=M
@R14
M=D
"""

    function_dict['retadd_store'] = """@LCL
D=M
@5
A=D-A
D=M
@R15
M=D
"""

    function_dict['stack_restore'] = """// SP = ARG +1
@ARG
D=M+1
@SP
M=D
// THAT = endframe - 1
@R14
AM=M-1
D=M
@THAT
M=D
// THIS = endframe - 2
@R14
AM=M-1
D=M
@THIS
M=D
// ARG = endframe - 3
@R14
AM=M-1
D=M
@ARG
M=D
// LCL = endframe - 4
@R14
AM=M-1
D=M
@LCL
M=D
"""

    function_dict['retadd_jump'] = """@R15
A=M
0;JMP
"""


    function_dict['boot_code'] ="""@256
D=A
@SP
M=D
"""

