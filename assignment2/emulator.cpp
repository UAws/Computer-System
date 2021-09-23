// Author : Akide Liu
// Date : 21/9/21
// Student ID : A1743748
// Description :
// emulate executing Hack CPU instructions
#include "iobuffer.h"
#include "symbols.h"
#include "hack_computer.h"
#include <bitset>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>

// to make out programs a bit neater
using namespace std;

using namespace CS_IO_Buffers;
using namespace CS_Symbol_Tables;
using namespace Hack_Computer;

// This program must emulate the execution of Hack CPU instructions
//
// To read from registers A, D or PC, use read_A(), read_D() or read_PC()
// To write to registers A, D or PC, use write_A(value), write_D(value) or write_PC(value)
//
// To read from ROM use read_ROM(address)
// To read from RAM use read_RAM(address)
// To write to RAM use write_RAM(address,value)
//
// All values returned by a read_*() function are 16-bit unsigned integers
// All parameters passed to a write_*() function are 16-bit unsigned integers
//

/*****************   REPLACE THE FOLLOWING CODE  ******************/

// commnet
inline uint16_t to_value(string input) { return (uint16_t)bitset<16>(input).to_ulong(); }

class ToolBox
{

public:
    // helpers:
    char local_and(char a, char b);

    // commnet
    char local_or(char a, char b);

    // commnet
    char local_not(char a);

    // commnet
    char local_xor(char a, char b);

    // commnet
    basic_string<char, char_traits<char>, allocator<char>>
    local_not16(basic_string<char, char_traits<char>, allocator<char>> &a);

    // commnet
    basic_string<char, char_traits<char>, allocator<char>>
    local_and16(basic_string<char, char_traits<char>, allocator<char>> &a, basic_string<char, char_traits<char>, allocator<char>> &b);

    // commnet
    basic_string<char, char_traits<char>, allocator<char>>
    local_mux16(basic_string<char, char_traits<char>, allocator<char>> a, basic_string<char, char_traits<char>, allocator<char>> b, char load);
    
    // commnet
    char local_Or8Way(const basic_string<char, char_traits<char>, allocator<char>> &in);

    // commnet
    basic_string<char, char_traits<char>, allocator<char>>

    // commnet
    local_add16(const string &a, const string &b);

    // sum , carry
    pair<int, int> local_add(char a, char b);
};

// commnet
char ToolBox::local_and(char a, char b)
{
    return (char)((int)a & (int)b);
}

// commnet
char ToolBox::local_or(char a, char b)
{
    return (char)((int)a | (int)b);
}

// commnet
char ToolBox::local_not(char a)
{
    return (char)(1 - ((int)a - '0')) + '0';
}

// commnet
char ToolBox::local_xor(char a, char b)
{
    return (char)((int)a ^ (int)b);
}

// commnet
string ToolBox::local_not16(string &a)
{
    string result;
    for_each(a.begin(), a.end(), [&](char var)
             { result.push_back(local_not(var)); });
    return result;
}

// commnet
string ToolBox::local_and16(string &a, string &b)
{
    string result;
    for (int i = 0; i < a.length(); ++i)
        result.push_back(local_and(a[i], b[i]));
    return result;
}

// commnet
string ToolBox::local_mux16(string a, string b, char load)
{
    return load == '0' ? a : b;
}

// commnet
char ToolBox::local_Or8Way(const string &in)
{
    char result = in[0];
    for_each(in.begin() + 1, in.end(), [&](char var)
             { result = local_or(result, var); });
    return result;
}

// commnet
string ToolBox::local_add16(const string &a, const string &b)
{
    unsigned int temp = bitset<16>(a).to_ulong() + bitset<16>(b).to_ulong();

    // if overflow (temp > 16 bits binary max -> 2 ^ 16 - 1  ) then temp - 2 ^ 16
    return bitset<16>((temp > (pow(2, 16) - 1)) ? temp - (int)pow(2, 16) : temp).to_string();
}

// commnet
pair<int, int> ToolBox::local_add(char a, char b)
{
    return make_pair((int)local_xor(a, b), (int)local_and(a, b));
}

class ALU_Result
{
public:
    string out;
    char zr, ng;
};

class ALU : public ToolBox
{
public:

    // ALU
    ALU();

    // commnet
    ALU(string instruction, const string &x, const string &y);

    // commnet
    const ALU_Result &getAluResult() const;

private:
    string instruction, x, y; // 16-bit inputs
    char zx, nx, zy, ny, f, no;

    ALU_Result aluResult;
};

// commnet
ALU::ALU(string instruction, const string &x, const string &y) : instruction(instruction), x(x), y(y)
{

    reverse(instruction.begin(), instruction.end());

    zx = instruction[11];
    nx = instruction[10];
    zy = instruction[9];
    ny = instruction[8];
    f = instruction[7];
    no = instruction[6];

    const string local_false = "0000000000000000";

    string outOfzx = local_mux16(x, local_false, zx);

    string outOfzy = local_mux16(y, local_false, zy);

    string outX = local_not16(outOfzx);

    string outY = local_not16(outOfzy);

    string outOfNx = local_mux16(outOfzx, outX, nx);

    string outOfNy = local_mux16(outOfzy, outY, ny);

    string outOfAdd = local_add16(outOfNx, outOfNy);

    string outOfAnd = local_and16(outOfNx, outOfNy);

    string outOfMuxFirst = local_mux16(outOfAnd, outOfAdd, f);
    string outOfMuxFirstNot = local_not16(outOfMuxFirst);

    aluResult.out = local_mux16(outOfMuxFirst, outOfMuxFirstNot, no);

    aluResult.ng = aluResult.out[0];

    aluResult.zr = local_not(
        local_or(
            local_Or8Way(aluResult.out.substr(0, 8)),
            local_Or8Way(aluResult.out.substr(8, 8))));
}

// commnet
const ALU_Result &ALU::getAluResult() const
{
    return aluResult;
}

// commnet
ALU::ALU() = default;

class CPU_Result
{
public:
    string outM, addressM, pc, ARegister, DRegister;
    char writeM;

    // commnet
    friend ostream &operator<<(ostream &os, const CPU_Result &result)
    {
        os << "outM: " << result.outM << " addressM: " << result.addressM << " pc: " << result.pc << " ARegister: "
           << result.ARegister << " DRegister: " << result.DRegister << " writeM: " << result.writeM;
        return os;
    }
};

class CPU : public ToolBox
{
public:

    // commnet
    CPU();

    // commnet
    CPU(uint16_t inM, string instruction, uint16_t pc, uint16_t ARegister, uint16_t dRegister);
    
    // commnet
    const CPU_Result &getCpuResult() const;

private:

    // commnet
    CPU(const string &inM, string instruction, const string &pc, const string &aRegister,
        const string &dRegister);

    // commnet
    string inM, instruction, pc, ARegister, DRegister;

    // commnet
    CPU_Result cpuResult;
};

// commnet
CPU::CPU(const string &inM, string instruction, const string &pc, const string &aRegister,
         const string &dRegister) : inM(inM), instruction(instruction), pc(pc), ARegister(aRegister),
                                    DRegister(dRegister)
{

    const string instruction_original = instruction; // deep copy
    reverse(instruction.begin(), instruction.end());

    const string local_false = "0000000000000000";

    string ALUInputY = local_mux16(aRegister, inM, instruction[12]);

    string ALUInputX = dRegister;

    auto alu = make_shared<ALU>(instruction_original, ALUInputX, ALUInputY);

    ALU_Result aluResult = alu->getAluResult();

    cpuResult.outM = aluResult.out;

    cpuResult.ARegister = local_or(instruction[5], local_not(instruction[15])) == '1' ? aluResult.out : aRegister;

    if (to_value(cpuResult.ARegister) >= 24576 || to_value(cpuResult.ARegister) <= 65535)
    {
        cpuResult.addressM = aRegister;
    }
    else if (to_value(cpuResult.ARegister) >= 0)
    {
        cpuResult.addressM = cpuResult.ARegister;
    }

    cpuResult.DRegister = local_and(instruction[15], instruction[4]) == '1' ? aluResult.out : dRegister;

    cpuResult.writeM = local_and(instruction[15], instruction[3]);

    // program counter

    char pc_control = local_and(
        local_or(
            local_or(
                local_and(instruction[2], aluResult.ng), local_and(aluResult.zr, instruction[1])),
            local_and(
                local_and(local_not(aluResult.ng), local_not(aluResult.zr)), instruction[0])),
        instruction[15]);

    cpuResult.pc = pc_control == '1' ? aRegister : local_add16(pc, "0000000000000001");
}

// commnet
const CPU_Result &CPU::getCpuResult() const
{
    return cpuResult;
}

// commnet
CPU::CPU(uint16_t inM, string instruction, uint16_t pc, uint16_t ARegister, uint16_t dRegister)
{
    const string _inM = std::bitset<16>(inM).to_string();

    const string _readPC = std::bitset<16>(pc).to_string();

    const string _ARegister = std::bitset<16>(ARegister).to_string();
    
    const string _DRegister = std::bitset<16>(dRegister).to_string();

    auto nested = CPU(_inM, std::move(instruction), _readPC, _ARegister, _DRegister);
    this->cpuResult = nested.cpuResult;
}

CPU::CPU() = default;

// A-instruction, simply copy the instruction to the A register and add 1 to the PC register.
void emulate_A_instruction(string instruction)
{
    // write_to_traces("A");
    bitset<16> decimal(instruction);
    write_A((uint16_t)decimal.to_ulong());

    write_PC(read_PC() + 1);
}

// commnet
void emulate_C_instruction(string instruction)
{
    auto cpu = make_shared<CPU>(read_RAM(read_A()), instruction, read_PC(), read_A(), read_D());
    CPU_Result cpuResult = cpu->getCpuResult();

    write_A(to_value(cpuResult.ARegister));

    if (cpuResult.writeM == '1')
    {
        write_RAM(to_value(cpuResult.addressM), to_value(cpuResult.outM));
    }

    write_D(to_value(cpuResult.DRegister));
    write_PC(to_value(cpuResult.pc));
}

class Disassembler
{
public:
    // commnet
    Disassembler(const string &instruction);

    // commnet
    string disassemble_A_instruction(const string &instruction);

    // commnet
    string disassemble_C_instruction(const string &instruction);

    // commnet
    string errorReporter();
    
    // commnet
    void identify_instruction(const string &instruction);

    // commnet
    const string &getResult() const;

private:
    enum INSTRUCTION_TYPE_C
    {
        A,
        C,
        FXXK
    };

    INSTRUCTION_TYPE_C INSTRUCTION_TYPE;

    string instruction;
    string result;

    const unordered_map<string, string> compareMapLeft = {
        {"101010", "0"},
        {"111111", "1"},
        {"111010", "-1"},
        {"001100", "D"},
        {"110000", "A"},
        {"001101", "!D"},
        {"110001", "!A"},
        {"001111", "-D"},
        {"110011", "-A"},
        {"011111", "D+1"},
        {"110111", "A+1"},
        {"001110", "D-1"},
        {"110010", "A-1"},
        {"000010", "D+A"},
        {"010011", "D-A"},
        {"000111", "A-D"},
        {"000000", "D&A"},
        {"010101", "D|A"}};

    const unordered_map<string, string> compareMapRight = {
        {"110000", "M"},
        {"110001", "!M"},
        {"110011", "-M"},
        {"110111", "M+1"},
        {"110010", "M-1"},
        {"000010", "D+M"},
        {"010011", "D-M"},
        {"000111", "M-D"},
        {"000000", "D&M"},
        {"010101", "D|M"}};

    const unordered_map<string, string> destMap = {
        {"000", "null"},
        {"001", "M"},
        {"010", "D"},
        {"011", "DM"},
        {"100", "A"},
        {"101", "AM"},
        {"110", "AD"},
        {"111", "AMD"}};

    const unordered_map<string, string> jumpMap = {
        {"000", "null"},
        {"001", "JGT"},
        {"010", "JEQ"},
        {"011", "JGE"},
        {"100", "JLT"},
        {"101", "JNE"},
        {"110", "JLE"},
        {"111", "JMP"}};
};

// commnet
void Disassembler::identify_instruction(const string &instruction)
{

    if (instruction.length() != 16)
    {
        INSTRUCTION_TYPE = FXXK;
        return;
    }

    if (instruction[0] == '0')
    {
        INSTRUCTION_TYPE = A;
    }
    else if (instruction.substr(0, 3) == "111")
    {
        INSTRUCTION_TYPE = C;
    }
    else
    {
        INSTRUCTION_TYPE = FXXK;
    }
}

// commnet
string Disassembler::errorReporter()
{
    return "** illegal instruction **";
}

// commnet
string Disassembler::disassemble_A_instruction(const string &instruction)
{
    return '@' + to_string(bitset<16>(instruction).to_ulong());
}

// commnet
Disassembler::Disassembler(const string &instruction) : instruction(instruction)
{

    identify_instruction(instruction);

    switch (INSTRUCTION_TYPE)
    {
    case A:
        result = disassemble_A_instruction(instruction);
        break;
    case C:
        result = disassemble_C_instruction(instruction);
        break;
    case FXXK:
        result = errorReporter();
        break;
    }
}

// commnet
const string &Disassembler::getResult() const
{
    return result;
}

// commnet
string Disassembler::disassemble_C_instruction(const string &instruction)
{
    string _result;
    // process dest
    {
        auto find = destMap.find(instruction.substr(10, 3));
        if (find != destMap.end())
        {
            _result.append(find->second == "null" ? "" : find->second + "=");
        }
        else
        {
            return errorReporter();
        }
    }

    // process compare
    if (instruction[3] == '0')
    {
        auto find = compareMapLeft.find(instruction.substr(4, 6));
        if (find != compareMapLeft.end())
        {
            _result.append(find->second);
        }
        else
        {
            _result.append(aluop(bitset<7>(instruction.substr(3, 7)).to_ulong()));
        }
    }
    else
    {
        auto find = compareMapRight.find(instruction.substr(4, 6));
        if (find != compareMapRight.end())
        {

            _result.append(find->second);
        }
        else
        {
            // if the ALU op bits do not match an instruction that can be generated in Hack Assembly Language, aluop() returns an expression enclosed in dots, eg ".-D-M-1."
            _result.append(aluop(bitset<7>(instruction.substr(3, 7)).to_ulong()));
        }
    }

    // process jump
    {
        auto find = jumpMap.find(instruction.substr(13, 3));
        if (find != jumpMap.end())
        {
            _result.append(find->second == "null" ? "" : ";" + find->second);
        }
        else
        {
            return errorReporter();
        }
    }

    return _result;
}

// disassemble an instruction - convert binary to symbolic form
// A instructions should be "@" followed by the value in decimal
// C instructions should be dest=alu;jump
// omit dest= if all destination bits are 0
// omit ;jump if all jump bits are 0
string disassemble_instruction(uint16_t instruction)
{
    const string _instruction = std::bitset<16>(instruction).to_string();
    auto disassembler = make_shared<Disassembler>(_instruction);
    return disassembler->getResult();
}

// emulate a cpu instruction - the Hack Computer has been initialised
// the PC contains the address in ROM of the instruction to execute
// if you want to produce additional output, use write_to_traces()
static void emulate_instruction()
{
    // write_to_traces(std::bitset<16>(read_ROM(read_PC())).to_string());
    const string instruction = std::bitset<16>(read_ROM(read_PC())).to_string();

    if (instruction[0] == '0')
    {
        emulate_A_instruction(instruction);
    }
    else if (instruction.substr(0, 3) == "111")
    {
        emulate_C_instruction(instruction);
    }
}

/*****************        DOWN TO HERE         ******************/

// if the first command line argument is "D" the test output must display disassembled instructions
bool display_disassembled_instructions = false;

// this function only calls disassemble_instruction if required
// it is called from more_cpu_tests() and is passed the instruction being emulated
string disassemble(uint16_t instruction)
{
    if (display_disassembled_instructions)
        return disassemble_instruction(instruction);

    // default is to display instruction in binary
    return "\"" + std::bitset<16>(instruction).to_string() + "\"";
}

// main program
int main(int argc, char **argv)
{
    // force all output to be immediate - the test scripts will override this
    config_output(iob_immediate);
    config_errors(iob_immediate);

    // is the first command line argument "D", ie does the test output include disassembly of the instruction being emulated
    display_disassembled_instructions = argc > 1 && string(argv[1]) == "D";

    // more_cpu_tests() will initialise the next test and return true
    // if there are no more tests to be run, it returns false
    // more_cpu_tests() also outputs the results of the previous test
    while (more_cpu_tests(disassemble))
    {
        emulate_instruction();
    }

    // flush the output and any errors
    print_output();
    print_errors();
}
