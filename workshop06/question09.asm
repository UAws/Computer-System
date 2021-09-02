// test header so that variables named in workshop
// questions will have known addresses we can test
	@x
	@y
	@sum
	@ptr
	@numbers
	@arr
// add your instructions after this line
	@x
	M=0 // x = 0
	@sum
	M=0 // sum = 0
(LOOP)
	@x
	D=M
	@10
	D=D-A // (x - 10)
	@END
	D;JGE  // if (x - 10) >= 10 got END
	@numbers
	D=M
	@x
	A=D+M
	D=M
	@sum
	M=M+D
	@x
	M=M+1
	@LOOP
	0;JMP
(END)
	@END
	0;JMP
	