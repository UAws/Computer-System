#ifndef HACK_COMPUTER_H
#define HACK_COMPUTER_H

// to get specific sized integer types
#include <cstdint>

namespace Hack_Computer
{
    // *********** reading / writing registers / RAM / ROM ***********
    //
    // read register A, D or PC and return an unsigned 16-bit integer
    extern uint16_t read_A() ;
    extern uint16_t read_D() ;
    extern uint16_t read_PC() ;

    // write an unsigned 16-bit integer to register A, D or PC
    extern void write_A(uint16_t new_value) ;
    extern void write_D(uint16_t new_value) ;
    extern void write_PC(uint16_t new_value) ;

    // read an unsigned 16-bit integer from an address in ROM
    // attempts to read from addresses in the range 32768 to 65535 will return 0 as the result
    extern uint16_t read_ROM(uint16_t address) ;

    // read an unsigned 16-bit integer from an address in RAM
    // attempts to read from addresses in the range 24577 to 65535 will return 0 as the result
    extern uint16_t read_RAM(uint16_t address) ;

    // write an unsigned 16-bit integer to an address in RAM
    // attempts to write to addresses in the range 24576 to 65535 will not change the RAM
    extern void write_RAM(uint16_t address,uint16_t new_value) ;

    // initialise the next CPU test and return true
    // if there are no more tests to be run return false
    // output the results of the previous test
    // disassemble is called to display the instruction being emulated in symbolic form
    extern bool more_cpu_tests(std::string(*disassemble)(uint16_t)) ;

    // *********** disassembler support  ***********
    //
    // assembler equivalent of destination, aluop and jump components of a C-instruction
    extern std::string destination(uint16_t destination_bits) ;
    extern std::string aluop(uint16_t aluop_bits) ;
    extern std::string jump(uint16_t jump_bits) ;

}
#endif //HACK_COMPUTER_H

