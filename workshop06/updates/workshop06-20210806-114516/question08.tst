// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/07/MemoryAccess/BasicTest/BasicTest.tst

load question08.asm,
output-file question08.out,
compare-to question08.cmp,
output-list RAM[16]%D1.6.1 RAM[17]%D1.6.1 RAM[18]%D1.6.1 RAM[19]%D1.6.1 RAM[20]%D1.6.1 ;

set RAM[16] 8384,	// x
set RAM[17] 7,		// y
set RAM[18] 9925,	// sum
set RAM[19] 9922,	// ptr
set RAM[20] 9925,	// numbers
set RAM[21] 9929,	// arr
set RAM[9920] 98,
set RAM[9921] 34,
set RAM[9922] 230,
set RAM[9923] 17,
set RAM[9924] 83,
set RAM[9925] 1,
set RAM[9926] 5,
set RAM[9927] 3,
set RAM[9928] 46,
set RAM[9929] 999,
set RAM[9930] 11,
set RAM[9931] 34,
set RAM[9932] 230,
set RAM[9933] 17,
set RAM[9934] 83,
set RAM[9935] 1,
set RAM[9936] 5,
set RAM[9937] 3,
set RAM[9938] 46,
set RAM[9939] 999,
set RAM[9940] 11,

output;

repeat 600 {
  ticktock;
}

output;

