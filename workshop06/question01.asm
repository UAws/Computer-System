// Adds 1 + ... + 100
       @i                               // 0000000000010000
       M=1    // i=1                    // 1110111111001000
       @sum                             // 0000000000010001
       M=0    // sum=0                  // 1110101010001000
(LOOP)
       @i                               // 0000000000010000
       D=M    // D=i                    // 1111110000010000
       @100                             // 0000000001100100
       D=D-A  // D=i-100                // 1110010011010000
       @END                             // 0000000000010010
       D;JGT  // if (i-100)>0 goto END  // 1110001100000001
       @i                               // 0000000000010000
       D=M    // D=i                    // 1111110000010000
       @sum                             // 0000000000010001
       M=D+M  // sum=sum+i              // 1111000010001000
       @i                               // 0000000000010000
       M=M+1  // i=i+1                  // 1111110111001000
       @LOOP                            // 0000000000000100
       0;JMP  // goto LOOP              // 1110101010000111
 (END)
       @END                             // 0000000000010010
       0;JMP  // infinite loop          // 1110101010000111

