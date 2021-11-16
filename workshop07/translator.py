import sys
import os
import glob

from trans_dict import * # uses trans_dict to store dictionaries of hardcoded translation specifications

class line_elements:
    def __init__(self,line, type, arg1, arg2, arg3):
        self.line = line
        self.type = type
        self.arg1 = arg1
        self.arg2 = arg2
        self.arg3 = arg3


# counts number of relational operators and function calls in order to assign unique assembly labels
relation_count = 0
call_count = 0

def main():

    if len(sys.argv) != 2:
        sys.exit("input 1 argument: [program].vm or vm file directory")

    #initializes hard_coded translation libary
    dict_initializer()

    #determines if command line input is file or directory and puts .vm file names in a list
    vm_files = []
    if os.path.isdir(sys.argv[1]):
        dir_path = sys.argv[1]
        vm_files = glob.glob(dir_path + "/*.vm")
    else:
        vm_files.append(sys.argv[1])

    #asm_lines stores all translated code
    asm_lines = []

    for file in vm_files:

        # opens input .vm file and writes each line to a list
        vm_file = open(file, 'r')
        vm_lines = vm_file.readlines()
        vm_file.close()

        # parser cleans spaces/comments and parses the  vm line elements
        parsed_lines = []
        parser(vm_lines, parsed_lines)

        # file_name used for naming static variables per specification
        file_path = file.strip(".vm")
        file_name = file_path.split('/')[-1]

        # translates the parsed vm lines into asm code
        for i in range(len(parsed_lines)):
            code_writer(parsed_lines[i], asm_lines, file_name)


    # creates .asm output file and opens for writing
    if os.path.isdir(sys.argv[1]):
        dir_name = dir_path.split('/')[-1]
        file_path = dir_path + '/' + dir_name
        boot_code = bootstrapper(file_name)
        asm_lines.insert(0, boot_code )
    asm_file_name = file_path + ".asm"
    asm_file = open(asm_file_name, 'w')

    # writes translated code to output file
    asm_file.writelines(asm_lines)
    asm_file.close()


# parser cleans spaces/comments and parses the  vm line elements
def parser(vm_lines, parsed_lines):

    for i in range(len(vm_lines)):

        temp_line = vm_lines[i].split('//')[0].strip() + '\n'

        if len(temp_line) > 1:
            parsed_lines.append(line_elements(temp_line,'','','',''))
            line_parts = temp_line.split()
            parsed_lines[-1].arg1 = line_parts[0]
            parsed_lines[-1].type = c_type[line_parts[0]]

            if len(line_parts) >= 2:
                parsed_lines[-1].arg2 = line_parts[1]
            if len(line_parts) == 3:
                parsed_lines[-1].arg3 = line_parts[2]


# translates the parsed vm lines into asm code one line at a time
def code_writer(parsed_line, asm_lines, file_name):

    asm_lines.append('// ' + parsed_line.line)

    if parsed_line.type == "c_arith":
        arith_writer(parsed_line, asm_lines)

    if parsed_line.type in ("c_pop" , 'c_push'):
        pushpop_writer(parsed_line, asm_lines, file_name)

    if parsed_line.arg1 in ('label','goto','if-goto'):
        asm_lines.append(branch_dict[parsed_line.arg1])
        asm_lines[-1] = asm_lines[-1].replace('$LABEL$', parsed_line.arg2)

    if parsed_line.arg1 == 'function':
        function_writer(parsed_line, asm_lines, file_name)

    if parsed_line.arg1 == 'call':
        call_writer(parsed_line, asm_lines, file_name)

    if parsed_line.arg1 == 'return':
        return_writer(parsed_line, asm_lines, file_name)


def return_writer(parsed_line, asm_lines, file_name):

    # stores the endframe and return address of caller in temporary variables
    asm_lines.append(function_dict['endframe_store'])
    asm_lines.append(function_dict['retadd_store'])

    # pushes return value back to top of stack of the caller
    new_vm_line = line_elements("pop argument 0\n","c_pop", 'pop', 'argument', '0')
    code_writer(new_vm_line, asm_lines, file_name)

    # restores caller's pointers for SP, LCL, ARG, THIS, THAT
    asm_lines.append(function_dict['stack_restore'])

    # returns to caller's return address
    asm_lines.append(function_dict['retadd_jump'])


def call_writer(parsed_line, asm_lines, file_name):

    # generates return address and pushes to stack
    global call_count
    asm_lines.append(function_dict['call_addpush'])
    return_address = '$RETADD$' + str(call_count)
    asm_lines[-1] = asm_lines[-1].replace('$RETADD$', return_address)
    call_count += 1

    # pushes caller state - LCL, ARG, THIS, THAT
    asm_lines += function_dict['call_segsaver']

    # specifies ARG pointer for callee  based on number of arguments
    asm_lines += function_dict['arg_set_1']
    asm_lines += function_dict['arg_set_2'] * (5+int(parsed_line.arg3))
    asm_lines += function_dict['arg_set_3']

     # specifies LCL pointer for callee function
    asm_lines += function_dict['lcl_set']

    # generates code to goto callee
    asm_lines.append(branch_dict['goto'])
    asm_lines[-1] = asm_lines[-1].replace('$LABEL$', parsed_line.arg2)

    # adds return address label
    asm_lines.append(branch_dict['label'])
    asm_lines[-1] = asm_lines[-1].replace('$LABEL$', return_address)


def function_writer(parsed_line, asm_lines, file_name):
    asm_lines.append(branch_dict['label'])
    asm_lines[-1] = asm_lines[-1].replace('$LABEL$', parsed_line.arg2)


    # pushes constant 0 for each local cell function needs, as indicated by arg3 of function command
    for i in range(int(parsed_line.arg3)):
        new_vm_line = line_elements("push constant 0\n","c_push", 'push', 'constant', '0')
        code_writer(new_vm_line, asm_lines, file_name)


def pushpop_writer(parsed_line, asm_lines, file_name):

    asm_lines.append(pushpop_dict[parsed_line.arg2])
    asm_lines[-1] = asm_lines[-1].replace('$i$', (parsed_line.arg3))

    if parsed_line.arg2 == 'static':
        if parsed_line.arg1 == 'pop':
            asm_lines[-1] += "\nD=A"
        asm_lines[-1] = asm_lines[-1].replace('$DUMMY$', file_name + '.' + str(int(parsed_line.arg3)))
    if parsed_line.arg2 == 'pointer':
        if int(parsed_line.arg3) == 0:
            asm_lines[-1] = asm_lines[-1].replace('$DUMMY$', 'THIS')
        else:
            asm_lines[-1] = asm_lines[-1].replace('$DUMMY$', 'THAT')
    if parsed_line.arg2 == 'temp':
        if parsed_line.arg1 == 'pop':
            asm_lines[-1] += "D=A"
        asm_lines[-1] = asm_lines[-1].replace('$DUMMY$', str(5+int(parsed_line.arg3)))

    if parsed_line.arg2 != 'constant':
        asm_lines[-1] += pushpop_dict[parsed_line.arg1]


def arith_writer(parsed_line, asm_lines):
    global relation_count
    asm_lines.append(arith_dict[parsed_line.arg1])
    if parsed_line.arg1 in ('gt', 'lt', 'eq'):
        asm_lines[-1] = asm_lines[-1].replace('$$TRUE$$', '$$TRUE$$' + str(relation_count)) # if relational operator, replaces template label with unique label
        relation_count += 1


def bootstrapper(file_name):
    sys_init_code = []
    new_vm_line = line_elements("call Sys.init 0\n","c_call", 'call', 'Sys.init', '0')
    code_writer(new_vm_line, sys_init_code, file_name)

    boot_code = function_dict['boot_code']
    for line in sys_init_code:
        boot_code += line

    return boot_code



main()